#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern sqlite3 *db;

int db_init(const char *db_path);
void db_close();
const char* db_get_error();
const char* db_payroll_get_error();

/* ============================================================================
 * STUDENT MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

typedef struct {
    int student_id;
    int roll_no;
    char name[100];
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
 * FEE STRUCTURES & DATA TYPES
 * ============================================================================ */

// Student ID Card Structure (for fee_ui display)
typedef struct {
    int student_id;
    int roll_no;
    char name[100];
    char father_name[100];
    char branch[50];
    char gender[20];
    char mobile[20];
    char email[100];
    char photo_path[500];
} StudentIDCard;

// Fee Record Structure (for fee form)
typedef struct {
    int fee_id;
    int roll_no;
    
    // Institute Fee
    double institute_paid;
    char institute_date[11];       // YYYY-MM-DD
    double institute_due;
    char institute_mode[20];       // DD / Cheque / Online
    
    // Hostel Fee (Optional)
    double hostel_paid;
    char hostel_date[11];
    double hostel_due;
    char hostel_mode[20];
    
    // Mess Fee (Optional)
    double mess_paid;
    char mess_date[11];
    double mess_due;
    char mess_mode[20];
    
    // Other Charges (Optional)
    double other_paid;
    char other_date[11];
    double other_due;
    char other_mode[20];
    
    // Calculated
    double total_paid;
    double total_due;
    double total_amount;
    
    // Metadata
    char created_at[19];           // YYYY-MM-DD HH:MM:SS
    char updated_at[19];
    int created_by;
    int status;                    // 0: Draft, 1: Submitted, 2: Verified
} FeeRecord;

/* ============================================================================
 * FEE MANAGEMENT - CRUD OPERATIONS
 * ============================================================================ */

int db_create_fee_table();
int db_save_fee_record(FeeRecord *fee);
int db_get_fee_record(int roll_no, FeeRecord *fee);
int db_update_fee_record(FeeRecord *fee);
int db_delete_fee_record(int roll_no);
int db_get_student_for_card(int roll_no, StudentIDCard *card);

/* ============================================================================
 * EMPLOYEE & PAYROLL STRUCTURES
 * ============================================================================ */
// ============ EMPLOYEE STRUCTURES ============

typedef struct {
    int emp_id;
    int emp_no;                    // Employee Number (integer)
    char emp_name[100];            // Employee Name
    char emp_dob[11];              // DOB (DD-MM-YYYY)
    char department[50];           // Department
    char designation[100];          // Designation
    char category[50];             // Category
    char reporting_person_name[100];  // Reporting Person Name
    int reporting_person_id;       // Reporting Person ID (optional)
    char email[100];               // Email (optional)
    char mobile_number[10];        // Mobile Number (10 digits)
    char address[200];             // Address
    float base_salary;             // Base Salary
    char status[20];               // Status
    char created_date;         // Created Date
} Employee;

typedef struct {
    int bank_id;
    int emp_id;                    // Foreign Key to Employee
    char account_holder_name[100]; // Account Holder Name
    char account_number[20];       // Account Number
    char bank_name[100];           // Bank Name
    char ifsc_code[12];            // IFSC Code
    char bank_address[200];        // Bank Address
} BankDetails;

// ============ DATABASE FUNCTIONS FOR EMPLOYEES ============

// Employee Management
int db_add_employee(const Employee *emp);
int db_get_all_employees(sqlite3_stmt **out_stmt);
int db_get_employee_by_id(int emp_id, Employee *emp);
int db_update_employee(int emp_id, const Employee *emp);
int db_delete_employee(int emp_id);
int db_get_employee_count(void);

// Bank Details Management
int db_add_bank_details(const BankDetails *bank);
int db_get_bank_details(int emp_id, BankDetails *bank);
int db_update_bank_details(int emp_id, const BankDetails *bank);
int db_delete_bank_details(int emp_id);


typedef struct {
    int payroll_id;
    int emp_id;
    char month_year[20];
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
    char payment_date[20];
    char payment_method[50];
    char status[20];
    char remarks[200];
} Payroll;

typedef struct {
    int slip_id;
    int payroll_id;
    int emp_id;
    char emp_no[20];
    char employee_name[100];
    char designation[50];
    char department[20];
    char from_date[20];
    char to_date[20];
    char slip_date[20];
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
    char payment_status[20];
} SalarySlip;

/* ============================================================================
 * DATABASE INITIALIZATION & TABLES
 * ============================================================================ */

int db_create_tables();
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

#endif  // DATABASE_H
