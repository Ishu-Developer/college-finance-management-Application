#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern sqlite3 *db;

int db_init(const char *db_path);  // ✅ FIXED: Single declaration
void db_close();
const char* db_get_error();

/* ============================================================================
 * STUDENT MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

typedef struct {
    int student_id;
    int roll_no;
    char name[100];           // ✅ FIXED: Proper array size
    char gender[20];
    char father_name[100];
    char branch[50];
    int year;
    int semester;
    char category[20];
    long long mobile;
    char email[100];
} Student;

int db_add_student(const char *name, const char *gender, const char *father_name, 
                   const char *branch, int year, int semester, int roll_no, 
                   const char *category, long long mobile, const char *email);

int db_get_student(int student_id, Student *student);
sqlite3_stmt* db_get_all_students();
sqlite3_stmt* db_get_students_by_branch(const char *branch);
int db_search_student_by_rollno(int roll_no, Student *student);
int db_edit_student(int student_id, const Student *student);
int db_delete_student(int student_id);
int db_get_student_count();

/* ============================================================================
 * FEE MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

#ifndef FEE_STRUCTURES_DEFINED
#define FEE_STRUCTURES_DEFINED

typedef struct {
    int fee_id;
    int student_id;
    char roll_no[20];
    char name[100];
    char branch[20];
    int semester;
    char fee_type[50];
    double amount;
    double amount_paid;
    double amount_due;
    char due_date[10];
    char status[20];
    char created_at[30];
} Fee;

typedef struct {
    int student_id;
    int roll_no;
    char name[50];
    char father_name[50];
    char branch[20];
    char gender[15];
    char mobile[15];
    char email[100];
    char photo_path[500];
} StudentIDCard;
#endif

int db_fee_init();
int db_add_fee(const char *roll_no, const char *fee_type, double amount, const char *due_date);
sqlite3_stmt* db_get_all_fees();
sqlite3_stmt* db_get_fees_by_rollno(const char *roll_no);
int db_get_student_for_card(int roll_no, StudentIDCard *card);
int db_record_payment(int fee_id, double amount_paid, const char *payment_date);
int db_update_fee_status(int fee_id, const char *status);
int db_get_student_fee_summary(const char *roll_no, double *total_amount, 
                               double *total_paid, double *total_due);

/* ============================================================================
 * DATA STRUCTURES - EMPLOYEE & PAYROLL
 * ============================================================================ */

typedef struct {
    int emp_id;
    char emp_no[20];
    char employee_name[100];
    char birth_date[20];
    char department[50];
    char designation[50];
    char reporting_person[100];
    char email[100];
    char mobile_number[15];
    float base_salary;
    char joining_date[20];
    char status[20];
    char address[200];
    char bank_account[50];
} Employee;

/* ============================================================================
 * PAYROLL STRUCT - Matches payroll table exactly
 * ============================================================================ */
typedef struct {
    int payroll_id;
    int emp_id;
    char month_year[20];           // "Dec-2025"
    double basic_salary;
    double house_rent;
    double medical;
    double conveyance;
    double dearness_allowance;
    double performance_bonus;
    double other_allowances;
    double total_allowances;
    double income_tax;
    double provident_fund;
    double health_insurance;
    double loan_deduction;
    double other_deductions;
    double total_deductions;
    double gross_salary;
    double net_salary;
    char payment_date[20];         // "2025-12-01"
    char payment_method[50];       // "Bank Transfer"
    char status[20];               // "Pending", "Paid"
    char remarks[200];
} Payroll;

/* ============================================================================
 * SALARY SLIP STRUCT - Matches salary_slips table exactly
 * ============================================================================ */
typedef struct {
    int slip_id;
    int payroll_id;
    int emp_id;
    char emp_no[20];
    char employee_name[100];
    char designation[50];
    char department[20];
    char from_date[20];            // "2025-11-01"
    char to_date[20];              // "2025-11-30"
    char slip_date[20];            // "2025-12-01"
    double basic_salary;
    double house_rent;
    double medical;
    double conveyance;
    double dearness_allowance;
    double performance_bonus;
    double other_allowances;
    double total_allowances;
    double income_tax;
    double provident_fund;
    double health_insurance;
    double loan_deduction;
    double other_deductions;
    double total_deductions;
    double gross_salary;
    double net_salary;
    char payment_status[20];       // "Paid", "Pending"
} SalarySlip;

/* ============================================================================
 * DATABASE INITIALIZATION & TABLES
 * ============================================================================ */

int db_create_payroll_tables();

/* ============================================================================
 * PAYROLL FUNCTIONS
 * ============================================================================ */

int db_add_payroll(const Payroll *payroll);
int db_get_payroll(int payroll_id, Payroll *payroll);
int db_get_payroll_by_emp_month(int emp_id, const char *month_year, Payroll *payroll);
sqlite3_stmt* db_get_all_payroll();
sqlite3_stmt* db_get_payroll_by_employee(int emp_id);
sqlite3_stmt* db_get_payroll_by_month(const char *month_year);
int db_update_payroll(const Payroll *payroll);
int db_delete_payroll(int payroll_id);
int db_mark_payroll_paid(int payroll_id, const char *payment_date, const char *payment_method);

/* ============================================================================
 * EMPLOYEE FUNCTIONS
 * ============================================================================ */

int db_add_employee(const Employee *emp);
int db_get_employee(int emp_id, Employee *emp);
int db_get_all_employees(sqlite3_stmt **stmt);
int db_update_employee(const Employee *emp);
int db_delete_employee(int emp_id);

/* ============================================================================
 * ERROR HANDLING
 * ============================================================================ */

const char* db_payroll_get_error();

#endif  // DATABASE_H
