# Predicting Blood Pressure Disorders

This project develops an end-to-end machine learning pipeline for detecting blood-pressure disorder from the Framingham cardiovascular dataset. The workflow covers data cleaning, exploratory analysis, feature preparation, class balancing, model training, hyperparameter tuning, evaluation, and artifact generation.

---

## Project Info

- **Project Name:** Predicting Blood Pressure Disorders
- **Language:** Python
- **Author:** Betul Aslan

---

## Academic Info

- **Course:** Knowledge Discovery and Introduction to Data Mining
- **Institution:** Galatasaray University
- **Department:** Computer Engineering
- **Academic Year:** 2025–2026 Spring
- **Assignment:** Term Project

---

## Methodology

The pipeline performs the following operations:

1. Loads and validates the Framingham dataset.
2. Removes rows with missing clinical measurements and fills selected missing values.
3. Creates the `BP_Disorder` target from systolic and diastolic blood-pressure rules.
4. Removes target-defining variables (`sysBP` and `diaBP`) to prevent data leakage.
5. Splits the data into stratified training and test sets.
6. Balances the training set using oversampling and scales the features.
7. Trains baseline Random Forest, Decision Tree, and XGBoost classifiers.
8. Performs 5-fold and 10-fold cross-validation.
9. Tunes model hyperparameters with `GridSearchCV`.
10. Saves the best final model and evaluation artifacts under `outputs/`.

The pipeline evaluates the models using accuracy, recall, precision, F1-score, and ROC-AUC. Generated reports and figures are created locally when the pipeline is executed.

---

## Project Structure

```text
predicting-blood-pressure-disorders/
├── main.py                   # End-to-end pipeline entry point
├── data_utils.py             # Loading, cleaning, target creation, and EDA
├── model_train_utils.py      # Splitting, balancing, training, and evaluation
├── model_tune_utils.py       # GridSearchCV and final-model selection
└── report_utils.py           # Markdown report generation
```

---

## File Descriptions

- **`main.py`** – Runs the complete data preparation, model training, tuning, evaluation, and reporting workflow.
- **`data_utils.py`** – Loads and preprocesses the dataset, creates the `BP_Disorder` target, performs exploratory analysis, and prepares modeling data.
- **`model_train_utils.py`** – Splits the data, balances the training set, scales features, trains baseline models, performs cross-validation, and generates evaluation plots.
- **`model_tune_utils.py`** – Performs hyperparameter tuning with `GridSearchCV`, compares tuned models, selects the final model, and creates an example prediction.
- **`report_utils.py`** – Generates the final Markdown summary report containing preprocessing information, model metrics, cross-validation results, and final-model details.

---

