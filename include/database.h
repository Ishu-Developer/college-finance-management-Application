#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * GLOBAL DATABASE CONNECTION
 * ============================================================================ */

extern sqlite3 *db;

/* ============================================================================
 * DATABASE INITIALIZATION & MANAGEMENT
 * ============================================================================ */

/**
 * @brief Initialize database connection
 * @return 1 on success, 0 on failure
 */
int db_init();

/**
 * @brief Create all required tables in database
 * @return 1 on success, 0 on failure
 */
int db_create_tables();

/**
 * @brief Close database connection and cleanup
 */
void db_close();

/**
 * @brief Get last error message from database
 * @return Error message string
 */
const char* db_get_error();

/* ============================================================================
 * STUDENT MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

typedef struct {
    int student_id;
    char roll_no[20];
    char name[100];
    char branch[50];
    int semester;
    char mobile[15];
    char email[100];
} Student;

/**
 * @brief Add new student to database
 * @param roll_no Student roll number (unique)
 * @param name Student name
 * @param branch Branch name (e.g., "CSE", "ECE")
 * @param semester Semester number (1-8)
 * @param mobile Mobile number
 * @param email Email address
 * @return student_id on success, -1 on failure
 */
int db_add_student(const char *roll_no, const char *name, const char *branch,
                   int semester, const char *mobile, const char *email);

/**
 * @brief Retrieve student by ID
 * @param student_id Student ID
 * @param student Pointer to Student struct to fill
 * @return 1 if found, 0 if not found
 */
int db_get_student(int student_id, Student *student);

/**
 * @brief Get all students as prepared statement
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_all_students();

/**
 * @brief Get students by branch
 * @param branch Branch name
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_students_by_branch(const char *branch);

/**
 * @brief Search student by roll number
 * @param roll_no Roll number to search
 * @param student Pointer to Student struct to fill
 * @return 1 if found, 0 if not found
 */
int db_search_student_by_rollno(const char *roll_no, Student *student);

/**
 * @brief Update student information
 * @param student_id Student ID
 * @param student Pointer to updated Student struct
 * @return 1 on success, 0 on failure
 */
int db_update_student(int student_id, const Student *student);

/**
 * @brief Delete student record
 * @param student_id Student ID to delete
 * @return 1 on success, 0 on failure
 */
int db_delete_student(int student_id);

/**
 * @brief Get total number of students
 * @return Number of students or -1 on error
 */
int db_get_student_count();

/* ============================================================================
 * FEE MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

typedef struct {
    int fee_id;
    int student_id;
    char fee_type[50];
    double amount;
    char due_date[20];
    char paid_date[20];
    char status[20];
    char payment_mode[30];
    char receipt_no[20];
} Fee;

/**
 * @brief Add fee record for student
 * @param student_id Student ID
 * @param fee_type Type of fee (e.g., "Tuition", "Hostel")
 * @param amount Fee amount
 * @param due_date Due date (YYYY-MM-DD format)
 * @return fee_id on success, -1 on failure
 */
int db_add_fee(int student_id, const char *fee_type, double amount,
               const char *due_date);

/**
 * @brief Get fee by ID
 * @param fee_id Fee ID
 * @param fee Pointer to Fee struct to fill
 * @return 1 if found, 0 if not found
 */
int db_get_fee(int fee_id, Fee *fee);

/**
 * @brief Get all fees for a student
 * @param student_id Student ID
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_student_fees(int student_id);

/**
 * @brief Get all unpaid fees
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_unpaid_fees();

/**
 * @brief Update fee status to paid
 * @param fee_id Fee ID
 * @param paid_date Payment date (YYYY-MM-DD)
 * @param payment_mode Mode of payment (e.g., "Cash", "Check")
 * @param receipt_no Receipt number
 * @return 1 on success, 0 on failure
 */
int db_update_fee_status(int fee_id, const char *paid_date,
                         const char *payment_mode, const char *receipt_no);

/**
 * @brief Get total fees due for a student
 * @param student_id Student ID
 * @return Total amount due or -1 on error
 */
double db_get_total_fees_due(int student_id);

/**
 * @brief Delete fee record
 * @param fee_id Fee ID to delete
 * @return 1 on success, 0 on failure
 */
int db_delete_fee(int fee_id);

/* ============================================================================
 * EMPLOYEE MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

typedef struct {
    int emp_id;
    char emp_no[20];
    char name[100];
    char designation[50];
    char department[50];
    char email[100];
    char mobile[15];
    double salary;
    char bank_account[30];
} Employee;

/**
 * @brief Add new employee
 * @param emp_no Employee number (unique)
 * @param name Employee name
 * @param designation Job designation
 * @param department Department
 * @param email Email address
 * @param mobile Mobile number
 * @param salary Monthly salary
 * @param bank_account Bank account number
 * @return emp_id on success, -1 on failure
 */
int db_add_employee(const char *emp_no, const char *name,
                    const char *designation, const char *department,
                    const char *email, const char *mobile, double salary,
                    const char *bank_account);

/**
 * @brief Get employee by ID
 * @param emp_id Employee ID
 * @param employee Pointer to Employee struct to fill
 * @return 1 if found, 0 if not found
 */
int db_get_employee(int emp_id, Employee *employee);

/**
 * @brief Get all employees
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_all_employees();

/**
 * @brief Update employee information
 * @param emp_id Employee ID
 * @param employee Pointer to updated Employee struct
 * @return 1 on success, 0 on failure
 */
int db_update_employee(int emp_id, const Employee *employee);

/**
 * @brief Delete employee record
 * @param emp_id Employee ID to delete
 * @return 1 on success, 0 on failure
 */
int db_delete_employee(int emp_id);

/**
 * @brief Get total number of employees
 * @return Number of employees or -1 on error
 */
int db_get_employee_count();

/* ============================================================================
 * ACCOUNTS MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

typedef struct {
    int account_id;
    char account_type[50];
    char ledger_name[100];
    double opening_balance;
    double current_balance;
    char description[256];
    char tally_sync_status[20];
} Account;

/**
 * @brief Add new account/ledger
 * @param account_type Type (e.g., "Asset", "Liability", "Expense")
 * @param ledger_name Ledger name
 * @param opening_balance Opening balance
 * @param description Description
 * @return account_id on success, -1 on failure
 */
int db_add_account(const char *account_type, const char *ledger_name,
                   double opening_balance, const char *description);

/**
 * @brief Get account by ID
 * @param account_id Account ID
 * @param account Pointer to Account struct to fill
 * @return 1 if found, 0 if not found
 */
int db_get_account(int account_id, Account *account);

/**
 * @brief Get all accounts
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_all_accounts();

/**
 * @brief Update account balance
 * @param account_id Account ID
 * @param new_balance New balance
 * @return 1 on success, 0 on failure
 */
int db_update_account_balance(int account_id, double new_balance);

/**
 * @brief Update Tally sync status
 * @param account_id Account ID
 * @param status Sync status
 * @return 1 on success, 0 on failure
 */
int db_update_account_sync_status(int account_id, const char *status);

/**
 * @brief Delete account record
 * @param account_id Account ID to delete
 * @return 1 on success, 0 on failure
 */
int db_delete_account(int account_id);

/* ============================================================================
 * TRANSACTION LOGGING
 * ============================================================================ */

typedef struct {
    int transaction_id;
    int account_id;
    char transaction_type[20];
    double amount;
    char description[256];
    char transaction_date[20];
    char reference_no[50];
} Transaction;

/**
 * @brief Log a transaction
 * @param account_id Account ID
 * @param transaction_type "Debit" or "Credit"
 * @param amount Transaction amount
 * @param description Transaction description
 * @param reference_no Reference number (optional)
 * @return transaction_id on success, -1 on failure
 */
int db_log_transaction(int account_id, const char *transaction_type,
                       double amount, const char *description,
                       const char *reference_no);

/**
 * @brief Get all transactions for an account
 * @param account_id Account ID
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_account_transactions(int account_id);

/**
 * @brief Get transactions in date range
 * @param start_date Start date (YYYY-MM-DD)
 * @param end_date End date (YYYY-MM-DD)
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_transactions_by_date(const char *start_date,
                                          const char *end_date);

/* ============================================================================
 * PAYROLL MANAGEMENT
 * ============================================================================ */

typedef struct {
    int payroll_id;
    int emp_id;
    char month_year[20];
    double basic_salary;
    double allowances;
    double deductions;
    double net_salary;
    char payment_date[20];
    char status[20];
} Payroll;

/**
 * @brief Add payroll record
 * @param emp_id Employee ID
 * @param month_year Month (YYYY-MM format)
 * @param basic_salary Basic salary
 * @param allowances Total allowances
 * @param deductions Total deductions
 * @return payroll_id on success, -1 on failure
 */
int db_add_payroll(int emp_id, const char *month_year, double basic_salary,
                   double allowances, double deductions);

/**
 * @brief Get payroll record
 * @param payroll_id Payroll ID
 * @param payroll Pointer to Payroll struct to fill
 * @return 1 if found, 0 if not found
 */
int db_get_payroll(int payroll_id, Payroll *payroll);

/**
 * @brief Get all payroll records
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_all_payroll();

/**
 * @brief Get payroll for employee in month
 * @param emp_id Employee ID
 * @param month_year Month (YYYY-MM format)
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_employee_payroll(int emp_id, const char *month_year);

/**
 * @brief Update payroll status
 * @param payroll_id Payroll ID
 * @param status Status (e.g., "Pending", "Paid", "Processed")
 * @param payment_date Payment date (YYYY-MM-DD)
 * @return 1 on success, 0 on failure
 */
int db_update_payroll_status(int payroll_id, const char *status,
                             const char *payment_date);

/**
 * @brief Delete payroll record
 * @param payroll_id Payroll ID
 * @return 1 on success, 0 on failure
 */
int db_delete_payroll(int payroll_id);

/* ============================================================================
 * TALLY SYNCHRONIZATION LOGGING
 * ============================================================================ */

/**
 * @brief Log Tally sync event
 * @param record_type Type of record ("Student", "Fee", etc.)
 * @param record_id Record ID
 * @param sync_status Sync status ("Success", "Failed", "Pending")
 * @param error_message Error message (NULL if successful)
 * @return 1 on success, 0 on failure
 */
int db_log_tally_sync(const char *record_type, int record_id,
                      const char *sync_status, const char *error_message);

/**
 * @brief Get all failed syncs for retry
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_failed_syncs();

/**
 * @brief Get sync log by date range
 * @param start_date Start date (YYYY-MM-DD)
 * @param end_date End date (YYYY-MM-DD)
 * @return Prepared statement or NULL on error
 */
sqlite3_stmt* db_get_sync_log_by_date(const char *start_date,
                                       const char *end_date);

#endif /* DATABASE_H */