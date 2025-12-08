#ifndef EMPLOYEE_UI_H
#define EMPLOYEE_UI_H

#include <gtk/gtk.h>


// UI Functions
void create_employee_ui(GtkWidget *container);
void refresh_employee_list();

// Button callbacks
void on_add_employee_clicked(GtkButton *button, gpointer user_data);
void on_save_employee_clicked(GtkButton *button, gpointer user_data);
void on_refresh_employee_clicked(GtkButton *button, gpointer user_data);
void on_search_employee_clicked(GtkButton *button, gpointer user_data);
void on_edit_employee_clicked(GtkButton *button, gpointer user_data);
void on_delete_employee_clicked(GtkButton *button, gpointer user_data);

#endif
// Optional - for more advanced features
void on_employee_selected(GtkTreeView *tree_view, gpointer user_data);
void show_employee_details(int emp_id);

/* ============================================================================
 * FILE: src/db/db_payroll.c
 * PURPOSE: Database operations for Payroll Module
 * FUNCTIONS: Create tables, CRUD operations, queries
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../../include/payroll.h"

/* ============================================================================
 * EXTERNAL VARIABLES (from database.c)
 * ============================================================================ */

extern sqlite3 *db;  // Global database connection

/* ============================================================================
 * STATIC VARIABLES
 * ============================================================================ */

static char payroll_error_msg[500] = "";

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ============================================================================ */

/**
 * Create payroll-related tables in database
 * Tables: employees, payroll, salary_slips
 * @return 1 on success, 0 on failure
 */
int db_create_payroll_tables() {
    if (db == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg), 
                 "[ERROR] Database not initialized");
        fprintf(stderr, "%s\n", payroll_error_msg);
        return 0;
    }

    printf("[INFO] Creating payroll-related tables...\n");

    // SQL statements for all three tables
    const char *sql_statements[] = {
        // EMPLOYEES TABLE - Extended with all required fields
        "CREATE TABLE IF NOT EXISTS employees ("
        "    emp_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    emp_no TEXT UNIQUE NOT NULL,"
        "    employee_name TEXT NOT NULL,"
        "    birth_date DATE,"
        "    department TEXT NOT NULL,"
        "    designation TEXT NOT NULL,"
        "    reporting_person TEXT,"
        "    email TEXT UNIQUE,"
        "    mobile_number TEXT,"
        "    base_salary REAL NOT NULL,"
        "    joining_date DATE,"
        "    status TEXT DEFAULT 'Active',"
        "    address TEXT,"
        "    bank_account TEXT,"
        "    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");",

        // PAYROLL TABLE
        "CREATE TABLE IF NOT EXISTS payroll ("
        "    payroll_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    emp_id INTEGER NOT NULL,"
        "    month_year TEXT NOT NULL,"
        "    basic_salary REAL NOT NULL,"
        "    house_rent REAL DEFAULT 0,"
        "    medical REAL DEFAULT 0,"
        "    conveyance REAL DEFAULT 0,"
        "    dearness_allowance REAL DEFAULT 0,"
        "    performance_bonus REAL DEFAULT 0,"
        "    other_allowances REAL DEFAULT 0,"
        "    total_allowances REAL DEFAULT 0,"
        "    income_tax REAL DEFAULT 0,"
        "    provident_fund REAL DEFAULT 0,"
        "    health_insurance REAL DEFAULT 0,"
        "    loan_deduction REAL DEFAULT 0,"
        "    other_deductions REAL DEFAULT 0,"
        "    total_deductions REAL DEFAULT 0,"
        "    gross_salary REAL DEFAULT 0,"
        "    net_salary REAL DEFAULT 0,"
        "    payment_date DATE,"
        "    payment_method TEXT,"
        "    status TEXT DEFAULT 'Pending',"
        "    remarks TEXT,"
        "    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY (emp_id) REFERENCES employees(emp_id),"
        "    UNIQUE(emp_id, month_year)"
        ");",

        // SALARY_SLIPS TABLE
        "CREATE TABLE IF NOT EXISTS salary_slips ("
        "    slip_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    payroll_id INTEGER NOT NULL,"
        "    emp_id INTEGER NOT NULL,"
        "    emp_no TEXT,"
        "    employee_name TEXT,"
        "    designation TEXT,"
        "    department TEXT,"
        "    from_date DATE,"
        "    to_date DATE,"
        "    slip_date DATE,"
        "    basic_salary REAL,"
        "    house_rent REAL,"
        "    medical REAL,"
        "    conveyance REAL,"
        "    dearness_allowance REAL,"
        "    performance_bonus REAL,"
        "    other_allowances REAL,"
        "    total_allowances REAL,"
        "    income_tax REAL,"
        "    provident_fund REAL,"
        "    health_insurance REAL,"
        "    loan_deduction REAL,"
        "    other_deductions REAL,"
        "    total_deductions REAL,"
        "    gross_salary REAL,"
        "    net_salary REAL,"
        "    payment_status TEXT,"
        "    FOREIGN KEY (payroll_id) REFERENCES payroll(payroll_id),"
        "    FOREIGN KEY (emp_id) REFERENCES employees(emp_id)"
        ");",

        NULL
    };

    // Execute all SQL statements
    for (int i = 0; sql_statements[i] != NULL; i++) {
        char *err = NULL;
        int rc = sqlite3_exec(db, sql_statements[i], NULL, NULL, &err);

        if (rc != SQLITE_OK) {
            snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                     "SQL error while creating table: %s", err);
            fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
            sqlite3_free(err);
            return 0;
        }
    }

    printf("[SUCCESS] All payroll tables created successfully\n");
    return 1;
}

/**
 * Add a new payroll record to database
 * @param payroll - Pointer to Payroll struct with data to insert
 * @return payroll_id on success, -1 on failure
 */
int db_add_payroll(const Payroll *payroll) {
    if (db == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Database not initialized");
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        return -1;
    }

    if (payroll == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Payroll struct is NULL");
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        return -1;
    }

    printf("[INFO] Adding payroll for emp_id: %d, month: %s\n",
           payroll->emp_id, payroll->month_year);

    // SQL INSERT statement
    const char *sql = "INSERT INTO payroll ("
        "emp_id, month_year, basic_salary, house_rent, medical, conveyance, "
        "dearness_allowance, performance_bonus, other_allowances, total_allowances, "
        "income_tax, provident_fund, health_insurance, loan_deduction, other_deductions, "
        "total_deductions, gross_salary, net_salary, payment_date, payment_method, "
        "status, remarks) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        return -1;
    }

    if (stmt == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Statement is NULL");
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        return -1;
    }

    // Bind parameters (22 parameters in total)
    sqlite3_bind_int(stmt, 1, payroll->emp_id);
    sqlite3_bind_text(stmt, 2, payroll->month_year, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, payroll->basic_salary);
    sqlite3_bind_double(stmt, 4, payroll->house_rent);
    sqlite3_bind_double(stmt, 5, payroll->medical);
    sqlite3_bind_double(stmt, 6, payroll->conveyance);
    sqlite3_bind_double(stmt, 7, payroll->dearness_allowance);
    sqlite3_bind_double(stmt, 8, payroll->performance_bonus);
    sqlite3_bind_double(stmt, 9, payroll->other_allowances);
    sqlite3_bind_double(stmt, 10, payroll->total_allowances);
    sqlite3_bind_double(stmt, 11, payroll->income_tax);
    sqlite3_bind_double(stmt, 12, payroll->provident_fund);
    sqlite3_bind_double(stmt, 13, payroll->health_insurance);
    sqlite3_bind_double(stmt, 14, payroll->loan_deduction);
    sqlite3_bind_double(stmt, 15, payroll->other_deductions);
    sqlite3_bind_double(stmt, 16, payroll->total_deductions);
    sqlite3_bind_double(stmt, 17, payroll->gross_salary);
    sqlite3_bind_double(stmt, 18, payroll->net_salary);
    sqlite3_bind_text(stmt, 19, payroll->payment_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 20, payroll->payment_method, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 21, payroll->status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 22, payroll->remarks, -1, SQLITE_STATIC);

    // Execute insert
    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to insert payroll: %s", sqlite3_errmsg(db));
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_int64 payroll_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    printf("[SUCCESS] Payroll added with ID: %lld\n", payroll_id);
    return (int)payroll_id;
}

/**
 * Get payroll record by ID
 * @param payroll_id - ID of payroll to retrieve
 * @param payroll - Pointer to Payroll struct to fill
 * @return 1 if found, 0 if not found, -1 on error
 */
int db_get_payroll(int payroll_id, Payroll *payroll) {
    if (db == NULL || payroll == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Database or payroll pointer is NULL");
        return -1;
    }

    const char *sql = "SELECT payroll_id, emp_id, month_year, basic_salary, "
        "house_rent, medical, conveyance, dearness_allowance, performance_bonus, "
        "other_allowances, total_allowances, income_tax, provident_fund, "
        "health_insurance, loan_deduction, other_deductions, total_deductions, "
        "gross_salary, net_salary, payment_date, payment_method, status, remarks "
        "FROM payroll WHERE payroll_id = ?;";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, payroll_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Extract data from result row
        payroll->payroll_id = sqlite3_column_int(stmt, 0);
        payroll->emp_id = sqlite3_column_int(stmt, 1);
        strncpy(payroll->month_year, (const char *)sqlite3_column_text(stmt, 2), 19);
        payroll->basic_salary = sqlite3_column_double(stmt, 3);
        payroll->house_rent = sqlite3_column_double(stmt, 4);
        payroll->medical = sqlite3_column_double(stmt, 5);
        payroll->conveyance = sqlite3_column_double(stmt, 6);
        payroll->dearness_allowance = sqlite3_column_double(stmt, 7);
        payroll->performance_bonus = sqlite3_column_double(stmt, 8);
        payroll->other_allowances = sqlite3_column_double(stmt, 9);
        payroll->total_allowances = sqlite3_column_double(stmt, 10);
        payroll->income_tax = sqlite3_column_double(stmt, 11);
        payroll->provident_fund = sqlite3_column_double(stmt, 12);
        payroll->health_insurance = sqlite3_column_double(stmt, 13);
        payroll->loan_deduction = sqlite3_column_double(stmt, 14);
        payroll->other_deductions = sqlite3_column_double(stmt, 15);
        payroll->total_deductions = sqlite3_column_double(stmt, 16);
        payroll->gross_salary = sqlite3_column_double(stmt, 17);
        payroll->net_salary = sqlite3_column_double(stmt, 18);
        strncpy(payroll->payment_date, (const char *)sqlite3_column_text(stmt, 19), 19);
        strncpy(payroll->payment_method, (const char *)sqlite3_column_text(stmt, 20), 49);
        strncpy(payroll->status, (const char *)sqlite3_column_text(stmt, 21), 19);
        strncpy(payroll->remarks, (const char *)sqlite3_column_text(stmt, 22), 199);

        sqlite3_finalize(stmt);
        printf("[SUCCESS] Payroll found: ID=%d, Emp=%d\n", payroll_id, payroll->emp_id);
        return 1;
    }

    sqlite3_finalize(stmt);
    printf("[WARNING] Payroll not found: ID=%d\n", payroll_id);
    return 0;
}

/**
 * Get all payroll records as SQLite statement
 * Caller must finalize the returned statement
 * @return sqlite3_stmt pointer, NULL on error
 */
sqlite3_stmt* db_get_all_payroll() {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return NULL;
    }

    const char *sql = "SELECT payroll_id, emp_id, month_year, basic_salary, "
        "house_rent, medical, conveyance, dearness_allowance, performance_bonus, "
        "other_allowances, total_allowances, income_tax, provident_fund, "
        "health_insurance, loan_deduction, other_deductions, total_deductions, "
        "gross_salary, net_salary, payment_date, payment_method, status, remarks "
        "FROM payroll ORDER BY payroll_id DESC;";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        return NULL;
    }

    printf("[INFO] Retrieved all payroll records\n");
    return stmt;
}

/**
 * Get payroll for specific employee in specific month
 * @param emp_id - Employee ID
 * @param month_year - Month and year (e.g., "Dec-2025")
 * @param payroll - Pointer to Payroll struct to fill
 * @return 1 if found, 0 if not found, -1 on error
 */
int db_get_payroll_by_emp_month(int emp_id, const char *month_year, Payroll *payroll) {
    if (db == NULL || payroll == NULL || month_year == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Invalid parameters");
        return -1;
    }

    const char *sql = "SELECT payroll_id, emp_id, month_year, basic_salary, "
        "house_rent, medical, conveyance, dearness_allowance, performance_bonus, "
        "other_allowances, total_allowances, income_tax, provident_fund, "
        "health_insurance, loan_deduction, other_deductions, total_deductions, "
        "gross_salary, net_salary, payment_date, payment_method, status, remarks "
        "FROM payroll WHERE emp_id = ? AND month_year = ? LIMIT 1;";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, emp_id);
    sqlite3_bind_text(stmt, 2, month_year, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        payroll->payroll_id = sqlite3_column_int(stmt, 0);
        payroll->emp_id = sqlite3_column_int(stmt, 1);
        strncpy(payroll->month_year, (const char *)sqlite3_column_text(stmt, 2), 19);
        payroll->basic_salary = sqlite3_column_double(stmt, 3);
        payroll->house_rent = sqlite3_column_double(stmt, 4);
        payroll->medical = sqlite3_column_double(stmt, 5);
        payroll->conveyance = sqlite3_column_double(stmt, 6);
        payroll->dearness_allowance = sqlite3_column_double(stmt, 7);
        payroll->performance_bonus = sqlite3_column_double(stmt, 8);
        payroll->other_allowances = sqlite3_column_double(stmt, 9);
        payroll->total_allowances = sqlite3_column_double(stmt, 10);
        payroll->income_tax = sqlite3_column_double(stmt, 11);
        payroll->provident_fund = sqlite3_column_double(stmt, 12);
        payroll->health_insurance = sqlite3_column_double(stmt, 13);
        payroll->loan_deduction = sqlite3_column_double(stmt, 14);
        payroll->other_deductions = sqlite3_column_double(stmt, 15);
        payroll->total_deductions = sqlite3_column_double(stmt, 16);
        payroll->gross_salary = sqlite3_column_double(stmt, 17);
        payroll->net_salary = sqlite3_column_double(stmt, 18);
        strncpy(payroll->payment_date, (const char *)sqlite3_column_text(stmt, 19), 19);
        strncpy(payroll->payment_method, (const char *)sqlite3_column_text(stmt, 20), 49);
        strncpy(payroll->status, (const char *)sqlite3_column_text(stmt, 21), 19);
        strncpy(payroll->remarks, (const char *)sqlite3_column_text(stmt, 22), 199);

        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

/**
 * Update an existing payroll record
 * @param payroll - Payroll struct with updated values
 * @return 1 on success, -1 on failure
 */
int db_update_payroll(const Payroll *payroll) {
    if (db == NULL || payroll == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Database or payroll pointer is NULL");
        return -1;
    }

    const char *sql = "UPDATE payroll SET "
        "basic_salary = ?, house_rent = ?, medical = ?, conveyance = ?, "
        "dearness_allowance = ?, performance_bonus = ?, other_allowances = ?, "
        "total_allowances = ?, income_tax = ?, provident_fund = ?, "
        "health_insurance = ?, loan_deduction = ?, other_deductions = ?, "
        "total_deductions = ?, gross_salary = ?, net_salary = ?, "
        "payment_date = ?, payment_method = ?, status = ?, remarks = ? "
        "WHERE payroll_id = ?;";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        return -1;
    }

    // Bind all parameters
    sqlite3_bind_double(stmt, 1, payroll->basic_salary);
    sqlite3_bind_double(stmt, 2, payroll->house_rent);
    sqlite3_bind_double(stmt, 3, payroll->medical);
    sqlite3_bind_double(stmt, 4, payroll->conveyance);
    sqlite3_bind_double(stmt, 5, payroll->dearness_allowance);
    sqlite3_bind_double(stmt, 6, payroll->performance_bonus);
    sqlite3_bind_double(stmt, 7, payroll->other_allowances);
    sqlite3_bind_double(stmt, 8, payroll->total_allowances);
    sqlite3_bind_double(stmt, 9, payroll->income_tax);
    sqlite3_bind_double(stmt, 10, payroll->provident_fund);
    sqlite3_bind_double(stmt, 11, payroll->health_insurance);
    sqlite3_bind_double(stmt, 12, payroll->loan_deduction);
    sqlite3_bind_double(stmt, 13, payroll->other_deductions);
    sqlite3_bind_double(stmt, 14, payroll->total_deductions);
    sqlite3_bind_double(stmt, 15, payroll->gross_salary);
    sqlite3_bind_double(stmt, 16, payroll->net_salary);
    sqlite3_bind_text(stmt, 17, payroll->payment_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 18, payroll->payment_method, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 19, payroll->status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 20, payroll->remarks, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 21, payroll->payroll_id);

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to update payroll: %s", sqlite3_errmsg(db));
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    printf("[SUCCESS] Payroll updated: ID=%d\n", payroll->payroll_id);
    return 1;
}

/**
 * Delete a payroll record
 * @param payroll_id - ID of payroll to delete
 * @return 1 on success, -1 on failure
 */
int db_delete_payroll(int payroll_id) {
    if (db == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Database not initialized");
        return -1;
    }

    const char *sql = "DELETE FROM payroll WHERE payroll_id = ?;";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, payroll_id);
    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to delete payroll: %s", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    printf("[SUCCESS] Payroll deleted: ID=%d\n", payroll_id);
    return 1;
}

/**
 * Mark payroll as paid
 * @param payroll_id - ID of payroll
 * @param payment_date - Date of payment "DD-MM-YYYY"
 * @param payment_method - Method (Bank/Cash/Check)
 * @return 1 on success, -1 on failure
 */
int db_mark_payroll_paid(int payroll_id, const char *payment_date, const char *payment_method) {
    if (db == NULL || payment_date == NULL || payment_method == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Invalid parameters");
        return -1;
    }

    const char *sql = "UPDATE payroll SET status = 'Paid', payment_date = ?, payment_method = ? WHERE payroll_id = ?;";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, payment_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, payment_method, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, payroll_id);

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to mark paid: %s", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    printf("[SUCCESS] Payroll marked as paid: ID=%d, Date=%s\n", payroll_id, payment_date);
    return 1;
}

/**
 * Add salary slip record
 * @param slip - SalarySlip structure to insert
 * @return slip_id on success, -1 on failure
 */
int db_add_salary_slip(const SalarySlip *slip) {
    if (db == NULL || slip == NULL) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Database or slip pointer is NULL");
        return -1;
    }

    const char *sql = "INSERT INTO salary_slips ("
        "payroll_id, emp_id, emp_no, employee_name, designation, department, "
        "from_date, to_date, slip_date, basic_salary, house_rent, medical, "
        "conveyance, dearness_allowance, performance_bonus, other_allowances, "
        "total_allowances, income_tax, provident_fund, health_insurance, "
        "loan_deduction, other_deductions, total_deductions, gross_salary, "
        "net_salary, payment_status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to prepare SQL: %s", sqlite3_errmsg(db));
        return -1;
    }

    // Bind 26 parameters
    sqlite3_bind_int(stmt, 1, slip->payroll_id);
    sqlite3_bind_int(stmt, 2, slip->emp_id);
    sqlite3_bind_text(stmt, 3, slip->emp_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, slip->employee_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, slip->designation, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, slip->department, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, slip->from_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, slip->to_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, slip->slip_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 10, slip->basic_salary);
    sqlite3_bind_double(stmt, 11, slip->house_rent);
    sqlite3_bind_double(stmt, 12, slip->medical);
    sqlite3_bind_double(stmt, 13, slip->conveyance);
    sqlite3_bind_double(stmt, 14, slip->dearness_allowance);
    sqlite3_bind_double(stmt, 15, slip->performance_bonus);
    sqlite3_bind_double(stmt, 16, slip->other_allowances);
    sqlite3_bind_double(stmt, 17, slip->total_allowances);
    sqlite3_bind_double(stmt, 18, slip->income_tax);
    sqlite3_bind_double(stmt, 19, slip->provident_fund);
    sqlite3_bind_double(stmt, 20, slip->health_insurance);
    sqlite3_bind_double(stmt, 21, slip->loan_deduction);
    sqlite3_bind_double(stmt, 22, slip->other_deductions);
    sqlite3_bind_double(stmt, 23, slip->total_deductions);
    sqlite3_bind_double(stmt, 24, slip->gross_salary);
    sqlite3_bind_double(stmt, 25, slip->net_salary);
    sqlite3_bind_text(stmt, 26, slip->payment_status, -1, SQLITE_STATIC);

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        snprintf(payroll_error_msg, sizeof(payroll_error_msg),
                 "Failed to insert salary slip: %s", sqlite3_errmsg(db));
        fprintf(stderr, "[ERROR] %s\n", payroll_error_msg);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_int64 slip_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    printf("[SUCCESS] Salary slip added with ID: %lld\n", slip_id);
    return (int)slip_id;
}

/**
 * Get error message from last operation
 * @return Error message string
 */
const char* db_payroll_get_error() {
    return payroll_error_msg;
}
