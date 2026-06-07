import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns


def create_directories(base_dir="outputs"):
    # Create the standard outputs directory layout.
    paths = {
        "base": base_dir,
        "figures": os.path.join(base_dir, "figures"),
        "tables": os.path.join(base_dir, "tables"),
        "models": os.path.join(base_dir, "models"),
        "data": os.path.join(base_dir, "data"),
    }

    for p in paths.values():
        os.makedirs(p, exist_ok=True)

    return paths


def load_and_inspect_data(csv_path):
    # Load CSV and compute missing-value stats (returned in-memory).
    if not os.path.exists(csv_path):
        raise FileNotFoundError(
            f"Dataset not found: {csv_path}\n"
            "Please make sure 'framingham.csv' is located in the project directory."
        )

    df = pd.read_csv(csv_path)

    missing_counts = df.isna().sum()
    missing_pct = (missing_counts / len(df)) * 100
    missing_table = (
        pd.DataFrame({"missing_count": missing_counts, "missing_pct": missing_pct})
        .sort_values("missing_count", ascending=False)
        .reset_index()
        .rename(columns={"index": "column"})
    )

    return df, missing_table


def _validate_columns(df, required_columns):
    missing = [c for c in required_columns if c not in df.columns]
    if missing:
        raise ValueError(f"Missing required column(s) in dataset: {missing}")


def preprocess_data(df, outputs_data_dir):
    # Clean the dataset and write `cleaned_framingham.csv`.
    required_cols = [
        "male",
        "age",
        "education",
        "currentSmoker",
        "cigsPerDay",
        "BPMeds",
        "prevalentStroke",
        "prevalentHyp",
        "diabetes",
        "totChol",
        "sysBP",
        "diaBP",
        "BMI",
        "heartRate",
        "glucose",
        "TenYearCHD",
    ]
    _validate_columns(df, required_cols)
    rows_before = len(df)

    clinical_cols = ["totChol", "BMI", "heartRate", "glucose"]
    # Drop rows where key clinical measurements are missing.
    nulls_in_clinical = df[clinical_cols].isna().any(axis=1).sum()
    df_clean = df.dropna(subset=clinical_cols).copy()

    bpm_missing = df_clean["BPMeds"].isna().sum()
    cigs_missing = df_clean["cigsPerDay"].isna().sum()
    df_clean["BPMeds"] = df_clean["BPMeds"].fillna(0)
    df_clean["cigsPerDay"] = df_clean["cigsPerDay"].fillna(0)

    edu_missing = df_clean["education"].isna().sum()
    if "education" in df_clean.columns:
        df_clean = df_clean.drop(columns=["education"])

    rows_after = len(df_clean)
    remaining_missing = df_clean.isna().sum().sum()

    cleaned_csv = os.path.join(outputs_data_dir, "cleaned_framingham.csv")
    df_clean.to_csv(cleaned_csv, index=False)

    summary = {
        "rows_before": int(rows_before),
        "rows_after": int(rows_after),
        "dropped_rows_due_to_clinical_missing": int(nulls_in_clinical),
        "bpm_missing_filled": int(bpm_missing),
        "cigs_missing_filled": int(cigs_missing),
        "education_missing_ignored": int(edu_missing),
        "remaining_missing_total": int(remaining_missing),
    }

    return df_clean, cleaned_csv, summary


def analyze_outliers(df, outputs_figures_dir):
    # IQR-based outlier summary + boxplot figure.
    numeric_vars = [
        "age",
        "cigsPerDay",
        "totChol",
        "sysBP",
        "diaBP",
        "BMI",
        "heartRate",
        "glucose",
    ]

    summary_rows = []
    for col in numeric_vars:
        series = df[col].dropna()
        q1 = series.quantile(0.25)
        q3 = series.quantile(0.75)
        iqr = q3 - q1
        lower = q1 - 1.5 * iqr
        upper = q3 + 1.5 * iqr
        outlier_count = ((series < lower) | (series > upper)).sum()
        summary_rows.append(
            {
                "variable": col,
                "q1": q1,
                "q3": q3,
                "iqr": iqr,
                "lower_bound": lower,
                "upper_bound": upper,
                "outlier_count": int(outlier_count),
            }
        )

    outlier_df = pd.DataFrame(summary_rows)

    fig, axes = plt.subplots(2, 4, figsize=(16, 7))
    axes = axes.flatten()
    for ax, col in zip(axes, numeric_vars):
        sns.boxplot(x=df[col].dropna(), ax=ax, color="#4C72B0")
        ax.set_title(col)
        ax.set_xlabel("")
    for ax in axes[len(numeric_vars):]:
        ax.axis("off")
    fig.suptitle("Boxplots - Numeric Features", y=1.02)
    fig.tight_layout()
    boxplots_path = os.path.join(outputs_figures_dir, "boxplots_numeric_features.png")
    plt.savefig(boxplots_path, dpi=200, bbox_inches="tight")
    plt.close()

    return outlier_df, boxplots_path


def save_preprocessing_summary(preprocess_summary, outputs_tables_dir, float_format):
    # Persist a one-row preprocessing summary table.
    path = os.path.join(outputs_tables_dir, "preprocessing_summary.csv")
    pd.DataFrame([preprocess_summary]).to_csv(path, index=False, float_format=float_format)
    return path


def save_modeling_dataset(df_modeling, feature_list, outputs_data_dir):
    # Persist the final modeling dataset used for training.
    path = os.path.join(outputs_data_dir, "modeling_dataset.csv")
    cols = list(feature_list) + ["BP_Disorder"]
    df_modeling[cols].to_csv(path, index=False)
    return path


def create_target_variable(df, outputs_figures_dir):
    # Create `BP_Disorder` target from sysBP/diaBP and save a class distribution plot.
    if "sysBP" not in df.columns or "diaBP" not in df.columns:
        raise ValueError("sysBP and diaBP columns are required to create BP_Disorder.")

    sysbp = df["sysBP"]
    diabp = df["diaBP"]

    disorder = ((sysbp >= 140) | (diabp >= 90) | (sysbp < 90) | (diabp < 60)).astype(int)
    df2 = df.copy()
    df2["BP_Disorder"] = disorder

    plt.figure(figsize=(6, 4))
    ax = sns.countplot(x="BP_Disorder", hue="BP_Disorder", data=df2, palette="Set2", legend=False)
    ax.set_title("BP_Disorder Class Distribution")
    ax.set_xlabel("BP_Disorder")
    ax.set_ylabel("Count")
    for container in ax.containers:
        ax.bar_label(container)
    plt.tight_layout()
    fig_path = os.path.join(outputs_figures_dir, "bp_disorder_class_distribution.png")
    plt.savefig(fig_path, dpi=200)
    plt.close()

    return df2, fig_path


def analyze_data(df, outputs_figures_dir):
    # Drop leakage-prone columns, compute correlation heatmap, and build feature list.
    df_work = df.copy()
    for col in ["sysBP", "diaBP", "TenYearCHD", "education"]:
        if col in df_work.columns:
            df_work = df_work.drop(columns=[col])

    feature_cols_for_corr = [c for c in df_work.columns if c != "BP_Disorder"]
    corr_df = df_work[feature_cols_for_corr].corr(numeric_only=True)

    plt.figure(figsize=(10, 8))
    sns.heatmap(corr_df, cmap="coolwarm", center=0, square=True, cbar_kws={"shrink": 0.8})
    plt.title("Correlation Heatmap (Independent Variables)")
    plt.tight_layout()
    heatmap_path = os.path.join(outputs_figures_dir, "correlation_heatmap.png")
    plt.savefig(heatmap_path, dpi=200)
    plt.close()

    pairs = []
    cols = corr_df.columns.tolist()
    for i in range(len(cols)):
        for j in range(i + 1, len(cols)):
            val = corr_df.iloc[i, j]
            if pd.notna(val) and abs(val) > 0.7:
                pairs.append(
                    {
                        "feature_1": cols[i],
                        "feature_2": cols[j],
                        "correlation": float(val),
                        "abs_correlation": float(abs(val)),
                    }
                )
    if pairs:
        high_pairs_df = pd.DataFrame(pairs).sort_values("abs_correlation", ascending=False)
    else:
        high_pairs_df = pd.DataFrame(columns=["feature_1", "feature_2", "correlation", "abs_correlation"])

    drop_current_smoker = False
    if not high_pairs_df.empty:
        mask = (
            ((high_pairs_df["feature_1"] == "currentSmoker") & (high_pairs_df["feature_2"] == "cigsPerDay"))
            | ((high_pairs_df["feature_1"] == "cigsPerDay") & (high_pairs_df["feature_2"] == "currentSmoker"))
        )
        if mask.any():
            drop_current_smoker = True

    if drop_current_smoker and "currentSmoker" in df_work.columns:
        df_work = df_work.drop(columns=["currentSmoker"])

    base_features = [
        "male",
        "age",
        "cigsPerDay",
        "BPMeds",
        "prevalentStroke",
        "prevalentHyp",
        "diabetes",
        "totChol",
        "BMI",
        "heartRate",
        "glucose",
    ]

    _validate_columns(df, base_features + ["BP_Disorder"])
    feature_list = [f for f in base_features if f in df_work.columns]

    forbidden = {"sysBP", "diaBP", "TenYearCHD", "education", "BP_Disorder"}
    if any(f in feature_list for f in forbidden):
        raise ValueError(f"Forbidden variable(s) found in feature list: {sorted(set(feature_list) & forbidden)}")
    leak_cols = [c for c in ["sysBP", "diaBP", "TenYearCHD", "education", "BP_Disorder"] if c in feature_list]
    if leak_cols:
        raise ValueError(f"Data leakage detected: forbidden columns found in X: {leak_cols}")

    return df_work, feature_list, heatmap_path, None
