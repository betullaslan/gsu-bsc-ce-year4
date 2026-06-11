import os

import numpy as np
import pandas as pd


def _fmt_float(x, digits=5):
    try:
        return f"{float(x):.{digits}f}"
    except Exception:
        return str(x)


def _df_to_markdown_table(df, float_digits=5):
    df2 = df.copy()
    for col in df2.columns:
        if pd.api.types.is_integer_dtype(df2[col]):
            df2[col] = df2[col].map(lambda v: str(int(v)) if pd.notna(v) else "")
        elif pd.api.types.is_numeric_dtype(df2[col]):
            df2[col] = df2[col].map(lambda v: _fmt_float(v, float_digits))

    cols = list(df2.columns)
    rows = df2.astype(str).values.tolist()

    def esc(val):
        s = str(val)
        return s.replace("|", "\\|")

    header = "| " + " | ".join(esc(c) for c in cols) + " |"
    sep = "| " + " | ".join(["---"] * len(cols)) + " |"
    body = ["| " + " | ".join(esc(v) for v in row) + " |" for row in rows]
    return "\n".join([header, sep] + body)


def write_final_results_markdown(
    outputs_base_dir,
    cleaned_shape,
    balanced_shape,
    preprocess_summary,
    feature_list,
    baseline_perf_df,
    tuned_perf_df,
    cv_df,
    best_baseline_row,
    best_tuned_row,
    saved_final_model_info,
    notes,
    class_distribution_df=None,
):

    md_path = os.path.join(outputs_base_dir, "summary.md")

    removed_vars = ["sysBP", "diaBP", "TenYearCHD", "education"]

    lines = []
    lines.append("# BP_Disorder Prediction Project")
    lines.append("")
    lines.append("## Main results")
    lines.append(f"- Cleaned dataset size: **{cleaned_shape[0]} x {cleaned_shape[1]}**")
    lines.append(f"- Balanced training dataset size: **{balanced_shape[0]} x {balanced_shape[1]}**")
    lines.append(f"- Best baseline (F1): **{best_baseline_row['Algorithm']}** (`{_fmt_float(best_baseline_row['F1-Score'], 5)}`)")
    if best_tuned_row is not None:
        lines.append(f"- Best tuned (F1): **{best_tuned_row['Algorithm']}** (`{_fmt_float(best_tuned_row['F1-Score'], 5)}`)")
    lines.append("")

    lines.append("## Selected feature list")
    lines.append("- " + ", ".join(f"`{f}`" for f in feature_list))
    lines.append("")

    lines.append("## Preprocessing summary")
    try:
        pre_df = pd.DataFrame([preprocess_summary])
        lines.append(_df_to_markdown_table(pre_df, float_digits=5))
    except Exception:
        lines.append("_Preprocessing summary is unavailable._")
    lines.append("")

    if class_distribution_df is not None and len(class_distribution_df) > 0:
        lines.append("## Class distribution")
        lines.append(_df_to_markdown_table(class_distribution_df, float_digits=5))
        lines.append("")

    lines.append("## Model performance (test)")
    lines.append("### Baseline")
    lines.append(_df_to_markdown_table(baseline_perf_df, float_digits=5))
    lines.append("")
    if tuned_perf_df is not None and len(tuned_perf_df) > 0:
        lines.append("### Tuned")
        lines.append(_df_to_markdown_table(tuned_perf_df, float_digits=5))
        lines.append("")
    else:
        lines.append("_Tuned models were not generated._")
        lines.append("")

    lines.append("## Cross-validation (Models)")
    cv_small = cv_df[
        [
            "Algorithm",
            "folds",
            "accuracy_mean",
            "accuracy_std",
            "f1_mean",
            "f1_std",
            "recall_mean",
            "recall_std",
            "precision_mean",
            "precision_std",
            "roc_auc_mean",
            "roc_auc_std",
        ]
    ].copy()
    lines.append(_df_to_markdown_table(cv_small, float_digits=5))
    lines.append("")

    lines.append("## Saved final model")
    lines.append(f"- Model name: **{saved_final_model_info['model_name']}**")
    lines.append(f"- Model source: **{saved_final_model_info['model_source']}**")
    lines.append(f"- Selection metric: **{saved_final_model_info['selection_metric']}**")
    lines.append(f"- Accuracy: `{_fmt_float(saved_final_model_info['metrics']['accuracy'], 5)}`")
    lines.append(f"- Recall: `{_fmt_float(saved_final_model_info['metrics']['recall'], 5)}`")
    lines.append(f"- Precision: `{_fmt_float(saved_final_model_info['metrics']['precision'], 5)}`")
    lines.append(f"- F1-score: `{_fmt_float(saved_final_model_info['metrics']['f1_score'], 5)}`")
    lines.append(f"- ROC-AUC: `{_fmt_float(saved_final_model_info['metrics']['roc_auc'], 5)}`")
    lines.append("")

    if notes:
        lines.append("## Limitations")
        lines.append("- Removed input variables to avoid leakage: " + ", ".join(f"`{v}`" for v in removed_vars))
        for n in notes:
            lines.append(f"- {n}")
        lines.append("")

    lines.append(
        "Inference note: Use outputs/models/final_bp_disorder_model.joblib directly for prediction (it includes preprocessing when needed)."
    )
    lines.append("")

    content = "\n".join(lines) + "\n"
    with open(md_path, "w", encoding="utf-8") as f:
        f.write(content)

    return md_path
