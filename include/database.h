#ifndef DATABASE_H
#define DATABASE_H


#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern sqlite3 *db;


int db_init();
void db_close();
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


int db_add_student(const char *roll_no, const char *name, const char *branch,
                   int semester, const char *mobile, const char *email);


int db_get_student(int student_id, Student *student);


sqlite3_stmt* db_get_all_students();


sqlite3_stmt* db_get_students_by_branch(const char *branch);


int db_search_student_by_rollno(const char *roll_no, Student *student);


int db_update_student(int student_id, const Student *student);


int db_delete_student(int student_id);


int db_get_student_count();


/* ============================================================================
 * FEE MANAGEMENT - CRUD OPERATIONS (NEW MODULE)
 * ============================================================================ */

// Guard to prevent duplicate structure definitions
#ifndef FEE_STRUCTURES_DEFINED
#define FEE_STRUCTURES_DEFINED

typedef struct {
    int fee_id;
    int student_id;
    char roll_no[20];
    char name[100];
    char branch[20];
    int semester;
    char fee_type[50];              // Institute, Hostel, Mess, Library, Sports, Medical
    double amount;
    double amount_paid;
    double amount_due;
    char due_date[15];              // YYYY-MM-DD
    char status[20];                // Pending, Partial, Paid
    char created_at[30];
} Fee;

typedef struct {
    int student_id;
    char roll_no[20];
    char name[100];
    char father_name[100];
    char branch[20];
    int semester;
    char gender[15];
    char mobile[10];
    char email[100];
    char photo_path[500];
} StudentIDCard;

#endif  // FEE_STRUCTURES_DEFINED


/**
 * @brief Initialize fee table in database
 */
int db_fee_init();


/**
 * @brief Add new fee record
 * @param roll_no Student roll number
 * @param fee_type Type of fee
 * @param amount Fee amount
 * @param due_date Due date (YYYY-MM-DD)
 * @return Fee ID on success, -1 on failure
 */
int db_add_fee(const char *roll_no, const char *fee_type, double amount, const char *due_date);


/**
 * @brief Get all fees with student details
 * @return SQLite statement with fee records
 */
sqlite3_stmt* db_get_all_fees();


sqlite3_stmt* db_get_fees_by_rollno(const char *roll_no);


int db_get_student_for_card(const char *roll_no, StudentIDCard *card);


int db_record_payment(int fee_id, double amount_paid, const char *payment_date);

int db_update_fee_status(int fee_id, const char *status);


int db_get_student_fee_summary(const char *roll_no, double *total_amount, 
                               double *total_paid, double *total_due);


/* ============================================================================
 * EMPLOYEE MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */


typedef struct {
    int emp_id;
    char emp_no[20];
    char name[100];
    char designation[50];
    char department[50];
    char category[30];              // Faculty, Staff, Support
    char email[100];
    char mobile[15];
    char doa[15];                   // Date of Appointment (YYYY-MM-DD)
    double salary;
    char bank_account[30];
} Employee;


int db_add_employee(const char *emp_no, const char *name,
                    const char *designation, const char *department,
                    const char *category, const char *email, 
                    const char *mobile, const char *doa, double salary,
                    const char *bank_account);


int db_get_employee(int emp_id, Employee *employee);


sqlite3_stmt* db_get_all_employees();


sqlite3_stmt* db_search_employee(const char *search_term);


int db_update_employee(int emp_id, const Employee *employee);


int db_delete_employee(int emp_id);


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


int db_add_account(const char *account_type, const char *ledger_name,
                   double opening_balance, const char *description);


int db_get_account(int account_id, Account *account);


sqlite3_stmt* db_get_all_accounts();


int db_update_account_balance(int account_id, double new_balance);


int db_update_account_sync_status(int account_id, const char *status);


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


int db_log_transaction(int account_id, const char *transaction_type,
                       double amount, const char *description,
                       const char *reference_no);

sqlite3_stmt* db_get_account_transactions(int account_id);


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


int db_add_payroll(int emp_id, const char *month_year, double basic_salary,
                   double allowances, double deductions);

int db_get_payroll(int payroll_id, Payroll *payroll);


sqlite3_stmt* db_get_all_payroll();


sqlite3_stmt* db_get_employee_payroll(int emp_id, const char *month_year);


int db_update_payroll_status(int payroll_id, const char *status,
                             const char *payment_date);


int db_delete_payroll(int payroll_id);


/* ============================================================================
 * TALLY SYNCHRONIZATION LOGGING
 * ============================================================================ */


int db_log_tally_sync(const char *record_type, int record_id,
                      const char *sync_status, const char *error_message);


sqlite3_stmt* db_get_failed_syncs();


sqlite3_stmt* db_get_sync_log_by_date(const char *start_date,
                                       const char *end_date);


#endif