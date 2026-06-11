import os
import warnings

import joblib
import pandas as pd
import seaborn as sns

from data_utils import (
    analyze_data,
    analyze_outliers,
    create_directories,
    create_target_variable,
    load_and_inspect_data,
    preprocess_data,
    save_modeling_dataset,
    save_preprocessing_summary,
)
from model_train_utils import (
    FLOAT_FORMAT,
    ImbPipeline,
    RandomOverSampler,
    XGBClassifier,
    feature_importance_analysis,
    oversample_training_data,
    prepare_train_test_data,
    run_cross_validation,
    save_class_distribution,
    scale_features,
    train_and_evaluate_models,
    plot_results,
)
from model_tune_utils import (
    _select_best_row_by_metric,
    example_prediction,
    run_tuning_block,
    select_and_save_final_model,
)
from report_utils import write_final_results_markdown


def main():
    # Run the end-to-end training + reporting pipeline.
    warnings.filterwarnings("ignore")
    sns.set_theme(style="whitegrid")

    paths = create_directories()

    # Load the raw dataset.
    df_raw, _ = load_and_inspect_data("framingham.csv")

    # Preprocess and persist cleaned dataset + summary.
    df_clean, _, preprocess_summary = preprocess_data(df_raw, paths["data"])
    save_preprocessing_summary(preprocess_summary, paths["tables"], float_format=FLOAT_FORMAT)

    # Generate EDA plots.
    analyze_outliers(df_clean, paths["figures"])

    # Create target label and class distribution plot.
    df_target, _ = create_target_variable(df_clean, paths["figures"])

    # Build modeling dataset (drop leakage-prone columns and pick features).
    df_modeling, feature_list, _, _ = analyze_data(df_target, paths["figures"])

    save_modeling_dataset(df_modeling, feature_list, paths["data"])

    # Split, oversample train, and scale features.
    X_train, X_test, y_train, y_test = prepare_train_test_data(df_modeling, feature_list)

    X_train_bal, y_train_bal, _ = oversample_training_data(X_train, y_train, paths["data"])
    class_dist_csv = save_class_distribution(
        df_full=df_modeling,
        y_train=y_train,
        y_train_bal=y_train_bal,
        y_test=y_test,
        outputs_tables_dir=paths["tables"],
    )

    scaler, X_train_scaled, X_test_scaled = scale_features(X_train_bal, X_test)

    # Baseline training + evaluation.
    feature_importance_analysis(X_train_scaled, y_train_bal, feature_list, paths["figures"])

    baseline_perf_df, artifacts = train_and_evaluate_models(X_train_scaled, y_train_bal, X_test_scaled, y_test)
    baseline_perf_df.to_csv(os.path.join(paths["tables"], "model_performance.csv"), index=False, float_format=FLOAT_FORMAT)

    plot_results(artifacts, y_test, paths["figures"])

    # Cross-validation (5-fold and 10-fold).
    cv_df, _ = run_cross_validation(df_modeling[feature_list], df_modeling["BP_Disorder"].astype(int), paths["tables"])

    # Hyperparameter tuning.
    tuned_estimators, tuned_perf_df, _, _, _, gridsearch_failed = run_tuning_block(X_train, y_train, X_test, y_test, paths["tables"])

    best_tuned_row = _select_best_row_by_metric(tuned_perf_df, "F1-Score") if tuned_perf_df is not None else None

    # Select and persist final model (tuned only if strictly better).
    saved_final = select_and_save_final_model(
        baseline_perf_df=baseline_perf_df,
        artifacts=artifacts,
        tuned_perf_df=tuned_perf_df,
        tuned_estimators=tuned_estimators,
        scaler=scaler,
        feature_list=feature_list,
        outputs_models_dir=paths["models"],
        outputs_tables_dir=paths["tables"],
        selection_metric="F1-Score",
    )

    final_model = joblib.load(saved_final["model_path"])
    example_prediction(final_model, saved_final["model_source"], scaler, feature_list, paths["tables"])

    # Notes for the report.
    notes = []
    if (RandomOverSampler is None) or (ImbPipeline is None):
        notes.append("imbalanced-learn is not installed: GridSearch and cross-validation run without within-fold oversampling.")
    if gridsearch_failed:
        notes.append("GridSearch/tuned model generation failed. See outputs/tables/gridsearch_error.txt for details.")
    if tuned_perf_df is not None:
        notes.append("GridSearch scoring/refit metric: f1.")

    try:
        class_dist_df = pd.read_csv(class_dist_csv)
    except Exception:
        class_dist_df = None

    # Write final markdown report
    write_final_results_markdown(
        outputs_base_dir=paths["base"],
        cleaned_shape=df_clean.shape,
        balanced_shape=(int(len(y_train_bal)), int(len(feature_list) + 1)),
        preprocess_summary=preprocess_summary,
        feature_list=feature_list,
        baseline_perf_df=baseline_perf_df,
        tuned_perf_df=tuned_perf_df,
        cv_df=cv_df,
        best_baseline_row=_select_best_row_by_metric(baseline_perf_df, "F1-Score"),
        best_tuned_row=best_tuned_row,
        saved_final_model_info=saved_final,
        notes=notes,
        class_distribution_df=class_dist_df,
    )


if __name__ == "__main__":
    main()
