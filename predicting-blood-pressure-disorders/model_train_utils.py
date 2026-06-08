import os
import warnings

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import (
    accuracy_score,
    confusion_matrix,
    f1_score,
    precision_score,
    recall_score,
    roc_auc_score,
    roc_curve,
)
from sklearn.model_selection import StratifiedKFold, train_test_split
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import MinMaxScaler
from sklearn.tree import DecisionTreeClassifier
from sklearn.utils import resample

from xgboost import XGBClassifier

try:
    from imblearn.over_sampling import RandomOverSampler
    from imblearn.pipeline import Pipeline as ImbPipeline
except Exception as exc:
    RandomOverSampler = None
    ImbPipeline = None
    _IMBLEARN_IMPORT_ERROR = exc

RANDOM_STATE = 42
N_JOBS = 1
FLOAT_FORMAT = "%.5f"


def prepare_train_test_data(df, feature_list):
    # Split data into train/test with stratification.
    X = df[feature_list].copy()
    y = df["BP_Disorder"].astype(int).copy()

    X_train, X_test, y_train, y_test = train_test_split(
        X,
        y,
        test_size=0.2,
        random_state=RANDOM_STATE,
        stratify=y,
    )

    return X_train, X_test, y_train, y_test


def oversample_training_data(X_train, y_train, outputs_data_dir):
    # Simple upsampling of the minority class on the training split.
    counts = y_train.value_counts()

    if len(counts) != 2:
        raise ValueError("This project expects binary classification (BP_Disorder: 0/1).")

    majority_class = counts.idxmax()
    minority_class = counts.idxmin()
    if counts[majority_class] == counts[minority_class]:
        X_bal, y_bal = X_train.copy(), y_train.copy()
    else:
        df_train = X_train.copy()
        df_train["BP_Disorder"] = y_train.values
        df_major = df_train[df_train["BP_Disorder"] == majority_class]
        df_minor = df_train[df_train["BP_Disorder"] == minority_class]

        df_minor_upsampled = resample(
            df_minor,
            replace=True,
            n_samples=len(df_major),
            random_state=RANDOM_STATE,
        )

        df_bal = pd.concat([df_major, df_minor_upsampled], axis=0).sample(frac=1.0, random_state=RANDOM_STATE)
        X_bal = df_bal.drop(columns=["BP_Disorder"])
        y_bal = df_bal["BP_Disorder"].astype(int)

    balanced_csv = os.path.join(outputs_data_dir, "balanced_training_data.csv")
    pd.concat([X_bal, y_bal.rename("BP_Disorder")], axis=1).to_csv(balanced_csv, index=False)

    return X_bal, y_bal, balanced_csv


def save_class_distribution(df_full, y_train, y_train_bal, y_test, outputs_tables_dir):
    # Save class distribution stats for full/train/test stages.
    def _counts_and_pct(y):
        counts = y.value_counts().reindex([0, 1], fill_value=0)
        total = int(counts.sum())
        pct0 = (counts.loc[0] / total * 100) if total else 0.0
        pct1 = (counts.loc[1] / total * 100) if total else 0.0
        return int(counts.loc[0]), int(counts.loc[1]), float(pct0), float(pct1)

    rows = []
    c0, c1, p0, p1 = _counts_and_pct(df_full["BP_Disorder"].astype(int))
    rows.append({"stage": "full_dataset", "class_0_count": c0, "class_1_count": c1, "class_0_pct": p0, "class_1_pct": p1})
    c0, c1, p0, p1 = _counts_and_pct(y_train.astype(int))
    rows.append({"stage": "train_before_oversampling", "class_0_count": c0, "class_1_count": c1, "class_0_pct": p0, "class_1_pct": p1})
    c0, c1, p0, p1 = _counts_and_pct(y_train_bal.astype(int))
    rows.append({"stage": "train_after_oversampling", "class_0_count": c0, "class_1_count": c1, "class_0_pct": p0, "class_1_pct": p1})
    c0, c1, p0, p1 = _counts_and_pct(y_test.astype(int))
    rows.append({"stage": "test_set", "class_0_count": c0, "class_1_count": c1, "class_0_pct": p0, "class_1_pct": p1})

    out = pd.DataFrame(rows)
    path = os.path.join(outputs_tables_dir, "class_distribution.csv")
    out.to_csv(path, index=False, float_format=FLOAT_FORMAT)
    return path


def scale_features(X_train_bal, X_test):
    # Fit scaler on (oversampled) train, transform train and test.
    scaler = MinMaxScaler()
    X_train_scaled = scaler.fit_transform(X_train_bal)
    X_test_scaled = scaler.transform(X_test)
    return scaler, X_train_scaled, X_test_scaled


def feature_importance_analysis(X_train_scaled, y_train_bal, feature_list, outputs_figures_dir):
    # Train a helper Random Forest for feature importance visualization.
    rf = RandomForestClassifier(n_estimators=500, random_state=RANDOM_STATE, n_jobs=N_JOBS)
    rf.fit(X_train_scaled, y_train_bal)

    importances = pd.DataFrame({"feature": feature_list, "importance": rf.feature_importances_}).sort_values("importance", ascending=False)
    importances = importances.round(5)

    plt.figure(figsize=(9, 5))
    sns.barplot(data=importances, x="importance", y="feature", color="#4C72B0")
    plt.title("Feature Importance (Random Forest)")
    plt.tight_layout()
    fig_path = os.path.join(outputs_figures_dir, "feature_importance_random_forest.png")
    plt.savefig(fig_path, dpi=200)
    plt.close()

    return importances, fig_path


def _evaluate_model(model, X_test_scaled, y_test, y_proba, algorithm_name):
    y_pred = model.predict(X_test_scaled)
    metrics = {
        "Algorithm": algorithm_name,
        "Accuracy": float(accuracy_score(y_test, y_pred)),
        "Recall": float(recall_score(y_test, y_pred, zero_division=0)),
        "Precision": float(precision_score(y_test, y_pred, zero_division=0)),
        "F1-Score": float(f1_score(y_test, y_pred, zero_division=0)),
        "ROC-AUC": float(roc_auc_score(y_test, y_proba)),
    }
    cm = confusion_matrix(y_test, y_pred)
    return metrics, cm


def train_and_evaluate_models(X_train_scaled, y_train_bal, X_test_scaled, y_test):
    # Train baseline models and compute test-set metrics.
    results = []
    artifacts = {}

    rf = RandomForestClassifier(
        n_estimators=1000,
        criterion="gini",
        max_depth=30,
        min_samples_split=2,
        min_samples_leaf=1,
        random_state=RANDOM_STATE,
        n_jobs=N_JOBS,
    )
    rf.fit(X_train_scaled, y_train_bal)
    rf_proba = rf.predict_proba(X_test_scaled)[:, 1]
    rf_metrics, rf_cm = _evaluate_model(rf, X_test_scaled, y_test, rf_proba, "Random Forest")
    results.append(rf_metrics)
    artifacts["Random Forest"] = {"model": rf, "proba": rf_proba, "cm": rf_cm}

    dt = DecisionTreeClassifier(
        criterion="gini",
        max_depth=15,
        min_samples_split=5,
        min_samples_leaf=2,
        random_state=RANDOM_STATE,
        class_weight="balanced",
    )
    dt.fit(X_train_scaled, y_train_bal)
    dt_proba = dt.predict_proba(X_test_scaled)[:, 1]
    dt_metrics, dt_cm = _evaluate_model(dt, X_test_scaled, y_test, dt_proba, "Decision Tree")
    results.append(dt_metrics)
    artifacts["Decision Tree"] = {"model": dt, "proba": dt_proba, "cm": dt_cm}

    with warnings.catch_warnings():
        warnings.filterwarnings("ignore", category=UserWarning)
        warnings.filterwarnings("ignore", category=FutureWarning)
        xgb = XGBClassifier(
            max_depth=3,
            n_estimators=1000,
            learning_rate=0.05,
            eval_metric="logloss",
            random_state=RANDOM_STATE,
            n_jobs=N_JOBS,
            subsample=0.9,
            colsample_bytree=0.9,
            tree_method="hist",
        )
        xgb.fit(X_train_scaled, y_train_bal)

    xgb_proba = xgb.predict_proba(X_test_scaled)[:, 1]
    xgb_metrics, xgb_cm = _evaluate_model(xgb, X_test_scaled, y_test, xgb_proba, "XGBoost")
    results.append(xgb_metrics)
    artifacts["XGBoost"] = {"model": xgb, "proba": xgb_proba, "cm": xgb_cm}

    performance_df = pd.DataFrame(results).sort_values(["F1-Score", "ROC-AUC", "Accuracy"], ascending=False)
    performance_df = performance_df.round(5)
    return performance_df, artifacts


def plot_results(artifacts, y_test, outputs_figures_dir):
    # Save confusion matrices per model and a combined ROC plot.
    for algo, meta in artifacts.items():
        cm = meta["cm"]
        plt.figure(figsize=(5, 4))
        labels = ["No BP Disorder (0)", "BP Disorder (1)"]
        sns.heatmap(cm, annot=True, fmt="d", cmap="Blues", cbar=False, xticklabels=labels, yticklabels=labels)
        plt.title(f"Confusion Matrix - {algo}")
        plt.xlabel("Predicted")
        plt.ylabel("Actual")
        plt.tight_layout()
        mapping = {
            "Random Forest": "confusion_matrix_random_forest.png",
            "Decision Tree": "confusion_matrix_decision_tree.png",
            "XGBoost": "confusion_matrix_xgboost.png",
        }
        if algo in mapping:
            plt.savefig(os.path.join(outputs_figures_dir, mapping[algo]), dpi=200)
        plt.close()

    plt.figure(figsize=(7, 6))
    for algo, meta in artifacts.items():
        proba = meta["proba"]
        fpr, tpr, _ = roc_curve(y_test, proba)
        auc = roc_auc_score(y_test, proba)
        plt.plot(fpr, tpr, label=f"{algo} (AUC={auc:.3f})")
    plt.plot([0, 1], [0, 1], "k--", label="Random Classifier")
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.title("ROC Curves Comparison (BP_Disorder)")
    plt.legend(loc="lower right")
    plt.tight_layout()
    roc_path = os.path.join(outputs_figures_dir, "roc_curves_comparison.png")
    plt.savefig(roc_path, dpi=200)
    plt.close()

    return {"roc": roc_path}


def run_cross_validation(X, y, outputs_tables_dir):
    # Run 5-fold and 10-fold cross-validation for each model.
    from sklearn.model_selection import cross_validate

    scoring = {"accuracy": "accuracy", "f1": "f1", "recall": "recall", "precision": "precision", "roc_auc": "roc_auc"}

    def make_pipe(model):
        use_imblearn = (RandomOverSampler is not None) and (ImbPipeline is not None)
        if use_imblearn:
            return ImbPipeline([("scaler", MinMaxScaler()), ("oversample", RandomOverSampler(random_state=RANDOM_STATE)), ("model", model)])
        return Pipeline([("scaler", MinMaxScaler()), ("model", model)])

    models = [
        (
            "Random Forest",
            RandomForestClassifier(
                n_estimators=1000,
                criterion="gini",
                max_depth=30,
                min_samples_split=2,
                min_samples_leaf=1,
                random_state=RANDOM_STATE,
                n_jobs=N_JOBS,
            ),
        ),
        (
            "Decision Tree",
            DecisionTreeClassifier(
                criterion="gini",
                max_depth=15,
                min_samples_split=5,
                min_samples_leaf=2,
                random_state=RANDOM_STATE,
                class_weight="balanced",
            ),
        ),
    ]
    models.append(
        (
            "XGBoost",
            XGBClassifier(
                max_depth=3,
                n_estimators=1000,
                learning_rate=0.05,
                eval_metric="logloss",
                random_state=RANDOM_STATE,
                n_jobs=N_JOBS,
                subsample=0.9,
                colsample_bytree=0.9,
                tree_method="hist",
            ),
        )
    )

    results = []
    for algo_name, model in models:
        pipe = make_pipe(model)
        for k in [5, 10]:
            cv = StratifiedKFold(n_splits=k, shuffle=True, random_state=RANDOM_STATE)
            cv_out = cross_validate(pipe, X, y, cv=cv, scoring=scoring, n_jobs=N_JOBS)
            results.append(
                {
                    "Algorithm": algo_name,
                    "folds": int(k),
                    "accuracy_mean": float(np.mean(cv_out["test_accuracy"])),
                    "accuracy_std": float(np.std(cv_out["test_accuracy"])),
                    "f1_mean": float(np.mean(cv_out["test_f1"])),
                    "f1_std": float(np.std(cv_out["test_f1"])),
                    "recall_mean": float(np.mean(cv_out["test_recall"])),
                    "recall_std": float(np.std(cv_out["test_recall"])),
                    "precision_mean": float(np.mean(cv_out["test_precision"])),
                    "precision_std": float(np.std(cv_out["test_precision"])),
                    "roc_auc_mean": float(np.mean(cv_out["test_roc_auc"])),
                    "roc_auc_std": float(np.std(cv_out["test_roc_auc"])),
                }
            )

    cv_df = pd.DataFrame(results).round(5)
    cv_csv = os.path.join(outputs_tables_dir, "cv_results_by_model.csv")
    cv_df.to_csv(cv_csv, index=False, float_format=FLOAT_FORMAT)
    return cv_df, cv_csv
