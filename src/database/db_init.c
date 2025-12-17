#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/database.h"

// Global database connection
sqlite3 *db = NULL;
static char db_error_msg[512] = {0};

int db_init(const char *db_path) {
    int rc = sqlite3_open(db_path ? db_path : "college_finance.db", &db);
    
    if (rc != SQLITE_OK) {
        snprintf(db_error_msg, sizeof(db_error_msg), 
                "Cannot open database: %s", sqlite3_errmsg(db));
        fprintf(stderr, "%s\n", db_error_msg);
        return 0;
    }
    
    printf("[INFO] Database connection opened: college_finance.db\n");
    return 1;
}

const char* db_get_error() {
    return db_error_msg;
}

const char* db_payroll_get_error() {
    return db_error_msg;
}

int db_create_tables() {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return 0;
    }

    const char *sql_statements[] = {
        // Students table
        "CREATE TABLE IF NOT EXISTS Students ("
        "  student_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  roll_no TEXT UNIQUE NOT NULL,"
        "  name TEXT NOT NULL,"
        "  gender TEXT,"
        "  father_name TEXT,"
        "  branch TEXT,"
        "  year INTEGER,"
        "  semester INTEGER,"
        "  category TEXT,"
        "  mobile TEXT,"
        "  email TEXT,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
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
        "    FOREIGN KEY(student_id) REFERENCES Students(student_id)"
        ");",
        
        // UPDATED: Employees table with all 12 fields
        "CREATE TABLE IF NOT EXISTS employees ("
        "    emp_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    emp_no INTEGER UNIQUE NOT NULL,"
        "    emp_name TEXT NOT NULL,"
        "    emp_dob TEXT,"
        "    department TEXT NOT NULL,"
        "    designation TEXT NOT NULL,"
        "    category TEXT,"
        "    reporting_person_name TEXT,"
        "    reporting_person_id INTEGER,"
        "    email TEXT UNIQUE,"
        "    mobile_number TEXT NOT NULL,"
        "    address TEXT,"
        "    base_salary REAL NOT NULL,"
        "    status TEXT DEFAULT 'Active',"
        "    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");",
        
        // NEW: Bank Details table
        "CREATE TABLE IF NOT EXISTS bank_details ("
        "    bank_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    emp_id INTEGER UNIQUE NOT NULL,"
        "    account_holder_name TEXT NOT NULL,"
        "    account_number TEXT NOT NULL,"
        "    bank_name TEXT NOT NULL,"
        "    ifsc_code TEXT NOT NULL,"
        "    bank_address TEXT,"
        "    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY(emp_id) REFERENCES employees(emp_id) ON DELETE CASCADE"
        ");",
        
        // Payroll table
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
        "    total_allowances REAL,"
        "    income_tax REAL DEFAULT 0,"
        "    provident_fund REAL DEFAULT 0,"
        "    health_insurance REAL DEFAULT 0,"
        "    loan_deduction REAL DEFAULT 0,"
        "    other_deductions REAL DEFAULT 0,"
        "    total_deductions REAL,"
        "    net_salary REAL,"
        "    payment_date DATE,"
        "    payment_method TEXT,"
        "    status TEXT DEFAULT 'Pending',"
        "    remarks TEXT,"
        "    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY(emp_id) REFERENCES employees(emp_id),"
        "    UNIQUE(emp_id, month_year)"
        ");",
        
        // Salary Slip table
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
        "    FOREIGN KEY(payroll_id) REFERENCES payroll(payroll_id),"
        "    FOREIGN KEY(emp_id) REFERENCES employees(emp_id)"
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
        
        NULL
    };

    // Execute all SQL statements
    for (int i = 0; sql_statements[i] != NULL; i++) {
        char *err = NULL;
        int rc = sqlite3_exec(db, sql_statements[i], NULL, NULL, &err);

        if (rc != SQLITE_OK) {
            snprintf(db_error_msg, sizeof(db_error_msg),
                    "SQL error on statement %d: %s", i, err);
            fprintf(stderr, "[ERROR] %s\n", db_error_msg);
            sqlite3_free(err);
            return 0;
        }
    }

    printf("[INFO] All database tables created successfully\n");
    return 1;
}

void db_close() {
    if (db != NULL) {
        sqlite3_close(db);
        printf("[INFO] Database connection closed\n");
        db = NULL;
    }
}
