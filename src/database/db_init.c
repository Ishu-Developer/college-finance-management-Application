/**
 * @file db_init.c
 * @brief Database initialization and table creation
 */

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/database.h"

// Global database connection
sqlite3 *db = NULL;
static char db_error_msg[512] = {0};

/**
 * @brief Initialize database connection
 */
int db_init() {
    int rc = sqlite3_open("college_finance.db", &db);
    
    if (rc != SQLITE_OK) {
        snprintf(db_error_msg, sizeof(db_error_msg), 
                "Cannot open database: %s", sqlite3_errmsg(db));
        fprintf(stderr, "%s\n", db_error_msg);
        return 0;
    }
    
    printf("[INFO] Database connection opened: college_finance.db\n");
    return 1;
}

/**
 * @brief Get last error message
 */
const char* db_get_error() {
    return db_error_msg;
}

/**
 * @brief Create all required tables
 */
int db_create_tables() {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return 0;
    }

    // SQL statements for creating all tables
    const char *sql_statements[] = {
        // Students table
        "CREATE TABLE IF NOT EXISTS students ("
        "    student_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    roll_no TEXT UNIQUE NOT NULL,"
        "    name TEXT NOT NULL,"
        "    branch TEXT NOT NULL,"
        "    semester INTEGER NOT NULL,"
        "    mobile TEXT NOT NULL,"
        "    email TEXT,"
        "    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");",

        // Fees table
        "CREATE TABLE IF NOT EXISTS fees ("
        "    fee_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    student_id INTEGER NOT NULL,"
        "    fee_type TEXT NOT NULL,"
        "    amount REAL NOT NULL,"
        "    due_date DATE,"
        "    paid_date DATE,"
        "    status TEXT DEFAULT 'Due',"
        "    payment_mode TEXT,"
        "    receipt_no TEXT UNIQUE,"
        "    FOREIGN KEY(student_id) REFERENCES students(student_id)"
        ");",

        // Employees table
        "CREATE TABLE IF NOT EXISTS employees ("
        "    emp_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    emp_no TEXT UNIQUE NOT NULL,"
        "    name TEXT NOT NULL,"
        "    designation TEXT NOT NULL,"
        "    department TEXT,"
        "    email TEXT,"
        "    mobile TEXT,"
        "    salary REAL NOT NULL,"
        "    bank_account TEXT,"
        "    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");",

        // Payroll table
        "CREATE TABLE IF NOT EXISTS payroll ("
        "    payroll_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    emp_id INTEGER NOT NULL,"
        "    month_year TEXT NOT NULL,"
        "    basic_salary REAL,"
        "    allowances REAL DEFAULT 0,"
        "    deductions REAL DEFAULT 0,"
        "    net_salary REAL,"
        "    payment_date DATE,"
        "    status TEXT DEFAULT 'Pending',"
        "    FOREIGN KEY(emp_id) REFERENCES employees(emp_id)"
        ");",

        // Accounts table
        "CREATE TABLE IF NOT EXISTS accounts ("
        "    account_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    account_type TEXT NOT NULL,"
        "    ledger_name TEXT NOT NULL,"
        "    opening_balance REAL,"
        "    current_balance REAL,"
        "    description TEXT,"
        "    tally_sync_status TEXT DEFAULT 'Pending'"
        ");",

        // Transactions table
        "CREATE TABLE IF NOT EXISTS transactions ("
        "    transaction_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    account_id INTEGER NOT NULL,"
        "    transaction_type TEXT NOT NULL,"
        "    amount REAL NOT NULL,"
        "    description TEXT,"
        "    transaction_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "    reference_no TEXT,"
        "    FOREIGN KEY(account_id) REFERENCES accounts(account_id)"
        ");",

        // Tally Sync Log table
        "CREATE TABLE IF NOT EXISTS tally_sync_log ("
        "    sync_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    sync_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "    record_type TEXT,"
        "    record_id INTEGER,"
        "    sync_status TEXT,"
        "    error_message TEXT"
        ");",

        NULL  // Terminator
    };

    // Execute all SQL statements
    for (int i = 0; sql_statements[i] != NULL; i++) {
        char *err = NULL;
        int rc = sqlite3_exec(db, sql_statements[i], NULL, NULL, &err);

        if (rc != SQLITE_OK) {
            snprintf(db_error_msg, sizeof(db_error_msg),
                    "SQL error: %s", err);
            fprintf(stderr, "[ERROR] %s\n", db_error_msg);
            sqlite3_free(err);
            return 0;
        }
    }

    printf("[INFO] All database tables created successfully\n");
    return 1;
}

/**
 * @brief Close database connection
 */
void db_close() {
    if (db != NULL) {
        sqlite3_close(db);
        printf("[INFO] Database connection closed\n");
        db = NULL;
    }
}