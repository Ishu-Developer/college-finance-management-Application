#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int validate_roll_no(const char *roll_no);
int validate_name(const char *name);
int validate_year(const char *year);
int validate_semester(const char *semester);
int validate_gender(const char *gender);
int validate_branch(const char *branch);
int validate_category(const char *category);
int validate_mobile(const char *mobile);
int validate_email(const char *email);

int validate_emp_number(const char *emp_number);
int validate_designation(const char *designation);
int validate_department(const char *department);
int validate_emp_category(const char *emp_category);
int validate_doa(const char *doa);
int validate_salary(double salary);
int validate_bank_account(const char *bank_account);

int validate_fee_type(const char *fee_type);
int validate_amount(double amount);
int validate_fee_status(const char *fee_status);
int validate_due_date(const char *due_date);
int validate_payment_method(const char *payment_method);

int validate_month(int month);
int validate_payroll_year(int year);
int validate_allowance(double allowance);
int validate_deduction(double deduction);
int validate_payroll_status(const char *status);
int validate_date(const char *date);
int validate_date_range(const char *start_date, const char *end_date);

#endif 