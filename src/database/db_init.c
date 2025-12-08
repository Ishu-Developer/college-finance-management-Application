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


int db_create_tables() {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return 0;
    }

    // SQL statements array - each complete CREATE TABLE is a separate string
    const char *sql_statements[] = {
        // Students table
        "CREATE TABLE IF NOT EXISTS students ("
        "    student_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    roll_no INTEGER UNIQUE NOT NULL,"
        "    name TEXT NOT NULL,"
        "    gender TEXT NOT NULL,"
        "    father_name TEXT NOT NULL,"
        "    branch TEXT NOT NULL,"
        "    year INTEGER NOT NULL,"
        "    semester INTEGER NOT NULL,"
        "    category TEXT NOT NULL,"
        "    mobile INTEGER NOT NULL,"
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
        "emp_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "emp_no TEXT UNIQUE NOT NULL,"              // Employee Number (e.g., EMP001)
        "employee_name TEXT NOT NULL,"              // ✅ Employee Name
        "birth_date DATE,"                          // ✅ Birth Date (DOB)
        "department TEXT NOT NULL,"                 // ✅ Department (CSE, EE, ME, etc)
        "designation TEXT NOT NULL,"                // ✅ Designation (Professor, Asst Prof, etc)
        "reporting_person TEXT,"                    // ✅ Reporting Person (Manager/HOD)
        "email TEXT UNIQUE,"                        // ✅ Email Address
        "mobile_number TEXT,"                       // ✅ Mobile Number (10 digits)
        "base_salary REAL NOT NULL,"                // Base Salary
        "joining_date DATE,"                        // Joining Date
        "status TEXT DEFAULT 'Active',"             // Active/Inactive/On Leave
        "address TEXT,"                             // Address (Optional)
        "bank_account TEXT,"                        // Bank Account (Optional)
        "created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");",

        
        // Payroll table
        "CREATE TABLE IF NOT EXISTS payroll ("
        "payroll_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "emp_id INTEGER NOT NULL,"                  // Foreign Key to employees
        "month_year TEXT NOT NULL,"                 // Month-Year (e.g., "Dec-2025")
        "basic_salary REAL NOT NULL,"               // Base salary for that month

        // ALLOWANCES
        "house_rent REAL DEFAULT 0,"                // HRA
        "medical REAL DEFAULT 0,"                   // Medical Allowance
        "conveyance REAL DEFAULT 0,"                // Conveyance/Transport
        "dearness_allowance REAL DEFAULT 0,"        // DA
        "performance_bonus REAL DEFAULT 0,"         // Performance Bonus
        "other_allowances REAL DEFAULT 0,"          // Other Allowances
        "total_allowances REAL,"                    // Auto-calculated

        // DEDUCTIONS
        "income_tax REAL DEFAULT 0,"                // IT
        "provident_fund REAL DEFAULT 0,"            // PF
        "health_insurance REAL DEFAULT 0,"          // Insurance
        "loan_deduction REAL DEFAULT 0,"            // Loan EMI
        "other_deductions REAL DEFAULT 0,"          // Other Deductions
        "total_deductions REAL,"                    // Auto-calculated

        // FINAL CALCULATION
        "net_salary REAL,"                          // Auto-calculated: Basic + Allowances - Deductions
        "payment_date DATE,"                        // When payment was made
        "payment_method TEXT,"                      // Bank Transfer / Cheque / Check
        "status TEXT DEFAULT 'Pending',"            // Pending / Paid / Rejected
        "remarks TEXT,"                             // Additional remarks
        "created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"

        "FOREIGN KEY (emp_id) REFERENCES employees(emp_id),"
        "UNIQUE(emp_id, month_year),"               // Only one payroll per emp per month
        ");",
        
        // Salary Slip table
        "CREATE TABLE IF NOT EXISTS salary_slips ("
        "slip_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "payroll_id INTEGER NOT NULL,"              // FK to payroll
        "emp_id INTEGER NOT NULL,"                  // FK to employees
        "emp_no TEXT,"                              // Employee Number
        "employee_name TEXT,"                       // Employee Name
        "designation TEXT,"                         // Designation
        "department TEXT,"                          // Department
        
        // PERIOD
        "from_date DATE,"                           // Salary period from
        "to_date DATE,"                             // Salary period to
        "slip_date DATE,"                           // When slip was generated
        
        // SALARY COMPONENTS (Detailed breakdown)
        "basic_salary REAL,"
        "house_rent REAL,"
        "medical REAL,"
        "conveyance REAL,"
        "dearness_allowance REAL,"
        "performance_bonus REAL,"
        "other_allowances REAL,"
        "total_allowances REAL,"
        
        // DEDUCTIONS BREAKDOWN
        "income_tax REAL,"
        "provident_fund REAL,"
        "health_insurance REAL,"
        "loan_deduction REAL,"
        "other_deductions REAL,"
        "total_deductions REAL,"
        "gross_salary REAL,"                        // Basic + Allowances
        "net_salary REAL,"                          // Gross - Deductions
        "payment_status TEXT,"                      // Paid / Pending
        "FOREIGN KEY (payroll_id) REFERENCES payroll(payroll_id),"
        "FOREIGN KEY (emp_id) REFERENCES employees(emp_id),"
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
        
        NULL  // Terminator - marks end of array
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


void db_close() {
    if (db != NULL) {
        sqlite3_close(db);
        printf("[INFO] Database connection closed\n");
        db = NULL;
    }
}