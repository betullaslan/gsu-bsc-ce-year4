import os
import traceback
import warnings

import joblib
import numpy as np
import pandas as pd
from sklearn.metrics import accuracy_score, f1_score, precision_score, recall_score, roc_auc_score
from sklearn.model_selection import GridSearchCV, StratifiedKFold
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import MinMaxScaler
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier

from model_train_utils import FLOAT_FORMAT, ImbPipeline, N_JOBS, RANDOM_STATE, RandomOverSampler, XGBClassifier


def grid_search_models(X_train, y_train, outputs_tables_dir):
    # GridSearchCV over model pipelines (scaler + optional oversampling + estimator).
    use_imblearn = (RandomOverSampler is not None) and (ImbPipeline is not None)
    cv = StratifiedKFold(n_splits=5, shuffle=True, random_state=RANDOM_STATE)

    def make_pipe(model):
        steps = [("scaler", MinMaxScaler())]
        if use_imblearn:
            steps.append(("oversample", RandomOverSampler(random_state=RANDOM_STATE)))
            return ImbPipeline(steps + [("model", model)])
        return Pipeline(steps + [("model", model)])

    searches = [
        (
            "Random Forest",
            make_pipe(RandomForestClassifier(random_state=RANDOM_STATE, n_jobs=N_JOBS)),
            {
                "model__n_estimators": [800, 1000, 1200],
                "model__max_depth": [20, 30, None],
                "model__min_samples_split": [2, 5],
                "model__min_samples_leaf": [1, 2],
                "model__criterion": ["gini"],
                "model__class_weight": [None, "balanced"],
            },
        ),
        (
            "Decision Tree",
            make_pipe(DecisionTreeClassifier(random_state=RANDOM_STATE)),
            {
                "model__max_depth": [5, 10, 15, 20, 30, None],
                "model__min_samples_split": [2, 5, 10],
                "model__min_samples_leaf": [1, 2, 4],
                "model__criterion": ["gini"],
                "model__class_weight": [None, "balanced"],
            },
        ),
    ]

    searches.append(
        (
            "XGBoost",
            make_pipe(
                XGBClassifier(
                    eval_metric="logloss",
                    random_state=RANDOM_STATE,
                    n_jobs=N_JOBS,
                    tree_method="hist",
                )
            ),
            {
                "model__max_depth": [2, 3, 4],
                "model__n_estimators": [800, 1000],
                "model__learning_rate": [0.03, 0.05, 0.1],
                "model__subsample": [0.9],
                "model__colsample_bytree": [0.9],
                "model__min_child_weight": [1, 3],
                "model__gamma": [0, 0.1],
            },
        )
    )

    tuned_results = []
    best_params_rows = []

    for name, pipe, grid in searches:
        gs = GridSearchCV(estimator=pipe, param_grid=grid, scoring="f1", cv=cv, n_jobs=N_JOBS, verbose=0)
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore", category=UserWarning)
            warnings.filterwarnings("ignore", category=FutureWarning)
            gs.fit(X_train, y_train)

        best_params_rows.append({"Algorithm": name, **gs.best_params_, "BestCVF1": float(gs.best_score_)})
        tuned_results.append((name, gs.best_estimator_))

    best_params_df = pd.DataFrame(best_params_rows)
    best_params_csv = os.path.join(outputs_tables_dir, "best_parameters_gridsearch.csv")
    best_params_df.to_csv(best_params_csv, index=False, float_format=FLOAT_FORMAT)

    return tuned_results, best_params_df, best_params_csv


def evaluate_tuned_models(tuned_estimators, X_test_raw, y_test, outputs_tables_dir):
    # Evaluate tuned estimators on the raw test features (pipeline handles scaling).
    rows = []
    for name, estimator in tuned_estimators:
        proba = estimator.predict_proba(X_test_raw)[:, 1]
        pred = estimator.predict(X_test_raw)
        rows.append(
            {
                "Algorithm": name,
                "Accuracy": float(accuracy_score(y_test, pred)),
                "Recall": float(recall_score(y_test, pred, zero_division=0)),
                "Precision": float(precision_score(y_test, pred, zero_division=0)),
                "F1-Score": float(f1_score(y_test, pred, zero_division=0)),
                "ROC-AUC": float(roc_auc_score(y_test, proba)),
            }
        )

    tuned_df = pd.DataFrame(rows).sort_values(["F1-Score", "ROC-AUC", "Accuracy"], ascending=False).round(5)
    tuned_csv = os.path.join(outputs_tables_dir, "tuned_model_performance.csv")
    tuned_df.to_csv(tuned_csv, index=False, float_format=FLOAT_FORMAT)
    return tuned_df, tuned_csv


def _select_best_row_by_metric(df, metric_col):
    if df is None or len(df) == 0:
        return None
    if metric_col not in df.columns:
        raise ValueError(f"Selection metric column not found: {metric_col}")
    return df.sort_values(metric_col, ascending=False, kind="mergesort").iloc[0]


def select_and_save_final_model(
    baseline_perf_df,
    artifacts,
    tuned_perf_df,
    tuned_estimators,
    scaler,
    feature_list,
    outputs_models_dir,
    outputs_tables_dir,
    selection_metric="F1-Score",
):
    # Pick final model: tuned only if it strictly improves the selection metric.
    source = "baseline"
    selected_row = _select_best_row_by_metric(baseline_perf_df, selection_metric)
    selected_name = selected_row["Algorithm"] if selected_row is not None else None
    selected_model = artifacts[selected_name]["model"] if selected_name in artifacts else None

    tuned_best = _select_best_row_by_metric(tuned_perf_df, selection_metric)
    if tuned_best is not None and selected_row is not None:
        tuned_name = tuned_best["Algorithm"]
        tuned_model = None
        for name, est in tuned_estimators:
            if name == tuned_name:
                tuned_model = est
                break

        if tuned_model is not None:
            try:
                tuned_metric = float(tuned_best[selection_metric])
                baseline_metric = float(selected_row[selection_metric])
            except Exception:
                tuned_metric, baseline_metric = None, None

            if (
                tuned_metric is not None
                and baseline_metric is not None
                and np.isfinite(tuned_metric)
                and np.isfinite(baseline_metric)
                and tuned_metric > baseline_metric
            ):
                source = "tuned"
                selected_row = tuned_best
                selected_name = tuned_name
                selected_model = tuned_model

    model_path = os.path.join(outputs_models_dir, "final_bp_disorder_model.joblib")
    if source == "baseline":
        final_pipeline = Pipeline([("scaler", scaler), ("model", selected_model)])
        joblib.dump(final_pipeline, model_path)
    else:
        joblib.dump(selected_model, model_path)

    features_path = os.path.join(outputs_models_dir, "feature_list.txt")
    with open(features_path, "w", encoding="utf-8") as f:
        for feat in feature_list:
            f.write(f"{feat}\n")

    info = {
        "model_name": str(selected_name),
        "model_source": source,
        "selection_metric": "f1_score" if selection_metric == "F1-Score" else str(selection_metric),
        "accuracy": round(float(selected_row["Accuracy"]), 5),
        "recall": round(float(selected_row["Recall"]), 5),
        "precision": round(float(selected_row["Precision"]), 5),
        "f1_score": round(float(selected_row["F1-Score"]), 5),
        "roc_auc": round(float(selected_row["ROC-AUC"]), 5),
    }
    info_csv = os.path.join(outputs_tables_dir, "saved_final_model_info.csv")
    pd.DataFrame([info]).to_csv(info_csv, index=False, float_format=FLOAT_FORMAT)

    return {
        "model_name": selected_name,
        "model_source": source,
        "selection_metric": selection_metric,
        "model_path": model_path,
        "features_path": features_path,
        "saved_final_model_info_csv": info_csv,
        "metrics": info,
    }


def example_prediction(final_model, final_model_source, scaler, feature_list, outputs_tables_dir):
    # Save a single example prediction as a quick smoke-test artifact.
    example = {
        "male": 1,
        "age": 55,
        "cigsPerDay": 10,
        "BPMeds": 0,
        "prevalentStroke": 0,
        "prevalentHyp": 0,
        "diabetes": 0,
        "totChol": 220,
        "BMI": 28.0,
        "heartRate": 75,
        "glucose": 90,
    }

    row = pd.DataFrame([example])[feature_list]
    if hasattr(final_model, "named_steps"):
        proba = float(final_model.predict_proba(row)[:, 1][0])
        pred = int(final_model.predict(row)[0])
    else:
        row_scaled = scaler.transform(row)
        proba = float(final_model.predict_proba(row_scaled)[:, 1][0])
        pred = int(final_model.predict(row_scaled)[0])

    out = {"model_source": str(final_model_source), "prediction": int(pred), "probability": float(proba)}
    out.update({f"feature_{k}": v for k, v in example.items()})
    path = os.path.join(outputs_tables_dir, "example_prediction.csv")
    pd.DataFrame([out]).to_csv(path, index=False, float_format=FLOAT_FORMAT)
    return path


def run_tuning_block(X_train, y_train, X_test, y_test, outputs_tables_dir):
    # Run tuning end-to-end and capture failures into `gridsearch_error.txt`.
    tuned_perf_df = None
    tuned_estimators = []
    gridsearch_error_path = os.path.join(outputs_tables_dir, "gridsearch_error.txt")
    gridsearch_failed = False

    if os.path.exists(gridsearch_error_path):
        try:
            os.remove(gridsearch_error_path)
        except Exception:
            pass

    try:
        tuned_estimators, best_params_df, best_params_csv = grid_search_models(X_train, y_train, outputs_tables_dir)
        tuned_perf_df, tuned_perf_csv = evaluate_tuned_models(tuned_estimators, X_test, y_test, outputs_tables_dir)
    except Exception:
        tuned_perf_df = None
        tuned_estimators = []
        gridsearch_failed = True
        try:
            with open(gridsearch_error_path, "w", encoding="utf-8") as f:
                f.write(traceback.format_exc())
        except Exception:
            pass
        best_params_df, best_params_csv, tuned_perf_csv = None, None, None
    else:
        if os.path.exists(gridsearch_error_path):
            try:
                os.remove(gridsearch_error_path)
            except Exception:
                pass

    return tuned_estimators, tuned_perf_df, best_params_df, best_params_csv, tuned_perf_csv, gridsearch_failed
