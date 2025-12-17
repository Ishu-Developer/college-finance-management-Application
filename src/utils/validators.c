#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../../include/validators.h"
//STUDENT VALIDATORS

// ✅ ROLL NO: 2_08400__000__ (13 digits)
int validate_roll_no(const char *roll_no) {
    if (!roll_no) return 0;

    // Exactly 13 digits
    if (strlen(roll_no) != 13) return 0;

    // All characters must be digits
    for (int i = 0; i < 13; i++) {
        if (!isdigit((unsigned char)roll_no[i])) return 0;
    }
    return 1;
}


// Name: letters and spaces only, 3-100 characters
int validate_name(const char *name) {
    if (name == NULL || strlen(name) == 0) {
        return 0;  // Empty name
    }
    
    size_t len = strlen(name);
    if (len < 3 || len > 100) {
        return 0;  // Invalid length
    }
    
    // Check for letters and spaces only
    for (size_t i = 0; i < len; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ' && name[i] != '-') {
            return 0;  // Invalid character (allow hyphen for double names)
        }
    }
    return 1;
}

// ✅ YEAR: 1-4
int validate_year(const char *year) {
    if (!year || strlen(year) != 1) return 0;
    int y = atoi(year);
    return (y >= 1 && y <= 4);
}

// ✅ SEMESTER: 1-8
int validate_semester(const char *semester) {
    if (!semester || strlen(semester) != 1) return 0;
    int s = atoi(semester);
    return (s >= 1 && s <= 8);
}

// Gender: MALE, FEMALE, OTHER
int validate_gender(const char *gender) {
    if (gender == NULL || strlen(gender) == 0) {
        return 0;
    }
    
    if (strcasecmp(gender, "MALE") == 0 || 
        strcasecmp(gender, "FEMALE") == 0 || 
        strcasecmp(gender, "OTHER") == 0) {
        return 1;
    }
    return 0;
}

// Branch: CSE, IT, ECE, CIVIL, ME, etc.
int validate_branch(const char *branch) {
    if (branch == NULL || strlen(branch) == 0) {
        return 0;
    }
    
    // Common engineering branches
    const char *valid_branches[] = {
        "CSE", "IT", "ECE", "CIVIL", "ME", 
        "EEE", "BIOTECH", "CHEMICAL", "MECHANICAL"
    };
    
    int num_branches = sizeof(valid_branches) / sizeof(valid_branches[0]);
    for (int i = 0; i < num_branches; i++) {
        if (strcmp(branch, valid_branches[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Category: General, OBC, SC, ST, EWS
int validate_category(const char *category) {
    if (category == NULL || strlen(category) == 0) {
        return 0;
    }
    
    const char *valid_categories[] = {
        "General", "OBC", "SC", "ST", "EWS"
    };
    
    int num_categories = sizeof(valid_categories) / sizeof(valid_categories[0]);
    for (int i = 0; i < num_categories; i++) {
        if (strcmp(category, valid_categories[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Mobile: exactly 10 digits
int validate_mobile(const char *mobile) {
    if (mobile == NULL || strlen(mobile) != 10) return 0;

    if (mobile[0] < '6' || mobile[0] > '9') return 0;

    for (int i = 0; i < 10; i++) {
        if (!isdigit((unsigned char)mobile[i])) return 0;
    }
    return 1;
}


// Email: standard format check (user@domain.com)
int validate_email(const char *email) {
    if (email == NULL || strlen(email) == 0) {
        return 0;
    }
    
    // Check for @ symbol
    const char *at = strchr(email, '@');
    if (at == NULL) {
        return 0;  // No @ symbol
    }
    
    // Check for . in domain
    const char *dot = strchr(at, '.');
    if (dot == NULL) {
        return 0;  // No dot in domain
    }
    
    // Email should have user@domain.extension format
    if (at == email || dot == at + 1 || *(dot + 1) == '\0') {
        return 0;  // Invalid format
    }
    
    return 1;
}

//EMPLOYEE VALIDATORS

// Employee number: E001-E999 format
int validate_emp_number(const char *emp_number) {
    if (emp_number == NULL || strlen(emp_number) != 4) {
        return 0;  // Must be exactly 4 chars
    }
    
    // Check format: E + 3 digits
    if (emp_number[0] != 'E') {
        return 0;
    }
    
    for (int i = 1; i < 4; i++) {
        if (!isdigit((unsigned char)emp_number[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Designation validation
int validate_designation(const char *designation) {
    if (designation == NULL || strlen(designation) == 0) {
        return 0;
    }
    
    const char *valid_designations[] = {
        "HOD", "Professor", "Assistant Professor", 
        "Lecturer", "Assistant Lecturer", "Guest Faculty",
         "Director"
    };
    
    int num_designations = sizeof(valid_designations) / sizeof(valid_designations[0]);
    for (int i = 0; i < num_designations; i++) {
        if (strcmp(designation, valid_designations[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Department validation
int validate_department(const char *department) {
    return validate_branch(department);  // Same as student branch
}

// Employee category: Permanent, Contract, Guest Faculty
int validate_emp_category(const char *emp_category) {
    if (emp_category == NULL || strlen(emp_category) == 0) {
        return 0;
    }
    
    const char *valid_categories[] = {
        "Permanent", "Contract", "Guest Faculty", "Temporary"
    };
    
    int num_categories = sizeof(valid_categories) / sizeof(valid_categories[0]);
    for (int i = 0; i < num_categories; i++) {
        if (strcmp(emp_category, valid_categories[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Date of Appointment: YYYY-MM-DD format
int validate_doa(const char *doa) {
    return validate_date(doa);  // Same as common date validation
}

// Salary: positive, minimum 15000
int validate_salary(double salary) {
    return (salary > 15000 && salary < 10000000) ? 1 : 0;  // Max 1 crore
}

// Bank account: 10-18 digits
int validate_bank_account(const char *bank_account) {
    if (bank_account == NULL || strlen(bank_account) == 0) {
        return 0;
    }
    
    size_t len = strlen(bank_account);
    if (len < 10 || len > 18) {
        return 0;  // Invalid length
    }
    
    // All characters must be digits
    for (size_t i = 0; i < len; i++) {
        if (!isdigit((unsigned char)bank_account[i])) {
            return 0;
        }
    }
    
    return 1;
}

//FEE VALIDATORS
// Fee type validation
int validate_fee_type(const char *fee_type) {
    if (fee_type == NULL || strlen(fee_type) == 0) {
        return 0;
    }
    
    const char *valid_fee_types[] = {
        "Tuition", "Hostel", "Transport", "Library",
        "Laboratory", "Examination", "Miscellaneous",
        "Development", "Sports", "Cultural"
    };
    
    int num_types = sizeof(valid_fee_types) / sizeof(valid_fee_types[0]);
    for (int i = 0; i < num_types; i++) {
        if (strcmp(fee_type, valid_fee_types[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Amount validation: positive, <= 500,000
int validate_amount(double amount) {
    return (amount > 0 && amount <= 500000) ? 1 : 0;
}

// Fee status: Pending, Paid, Partial, Overdue
int validate_fee_status(const char *fee_status) {
    if (fee_status == NULL || strlen(fee_status) == 0) {
        return 0;
    }
    
    const char *valid_statuses[] = {
        "Pending", "Paid", "Partial", "Overdue", "Cancelled"
    };
    
    int num_statuses = sizeof(valid_statuses) / sizeof(valid_statuses[0]);
    for (int i = 0; i < num_statuses; i++) {
        if (strcmp(fee_status, valid_statuses[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Due date: YYYY-MM-DD format
int validate_due_date(const char *due_date) {
    return validate_date(due_date);
}

// Payment method: Cash, Check, Bank Transfer, Online
int validate_payment_method(const char *payment_method) {
    if (payment_method == NULL || strlen(payment_method) == 0) {
        return 0;
    }
    
    const char *valid_methods[] = {
        "Cash", "Check", "Bank Transfer", "Online", "DD", "Credit Card"
    };
    
    int num_methods = sizeof(valid_methods) / sizeof(valid_methods[0]);
    for (int i = 0; i < num_methods; i++) {
        if (strcmp(payment_method, valid_methods[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

//PAYROLL VALIDATORS
// Month validation: 01-12
int validate_month(int month) {
    return (month >= 1 && month <= 12) ? 1 : 0;
}

// Year validation: 2020-2030
int validate_payroll_year(int year) {
    return (year >= 2020 && year <= 2030) ? 1 : 0;
}

// Allowance validation: positive, <= 500,000
int validate_allowance(double allowance) {
    return (allowance >= 0 && allowance <= 500000) ? 1 : 0;
}

// Deduction validation: positive, <= 500,000
int validate_deduction(double deduction) {
    return (deduction >= 0 && deduction <= 500000) ? 1 : 0;
}

// Payroll status: Pending, Generated, Approved, Rejected
int validate_payroll_status(const char *status) {
    if (status == NULL || strlen(status) == 0) {
        return 0;
    }
    
    const char *valid_statuses[] = {
        "Pending", "Generated", "Approved", "Rejected", "Paid", "Archived"
    };
    
    int num_statuses = sizeof(valid_statuses) / sizeof(valid_statuses[0]);
    for (int i = 0; i < num_statuses; i++) {
        if (strcmp(status, valid_statuses[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
//COMMON DATE VALIDATORS
int validate_date(const char *date) {
    if (date == NULL || strlen(date) != 10) {
        return 0;  // Must be exactly 10 chars
    }
    
    // Check format: YYYY-MM-DD
    if (date[4] != '-' || date[7] != '-') {
        return 0;  // Wrong separators
    }
    
    // Check all positions except separators are digits
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) {
            return 0;
        }
    }
    
    // Parse month and day for basic validation
    int month = atoi(&date[5]);
    int day = atoi(&date[8]);
    
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return 0;
    }
    
    return 1;
}

// Date range validation: start_date < end_date
int validate_date_range(const char *start_date, const char *end_date) {
    if (!validate_date(start_date) || !validate_date(end_date)) {
        return 0;
    }
    
    // Simple string comparison works for YYYY-MM-DD format
    if (strcmp(start_date, end_date) >= 0) {
        return 0;  // Start date should be before end date
    }
    
    return 1;
}
