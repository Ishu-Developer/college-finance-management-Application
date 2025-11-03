/**
 * @file validators.c
 * @brief Input validation functions for all modules
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

/**
 * @brief Check if string is empty
 * @return 1 if empty, 0 if not empty
 */
int is_empty(const char *str) {
    if (str == NULL) return 1;
    return strlen(str) == 0;
}

/**
 * @brief Validate roll number (alphanumeric, 5-20 chars)
 * @return 1 if valid, 0 if invalid
 */
int validate_roll_no(const char *roll_no) {
    if (is_empty(roll_no)) return 0;
    if (strlen(roll_no) < 5 || strlen(roll_no) > 20) return 0;

    for (int i = 0; i < strlen(roll_no); i++) {
        if (!isalnum(roll_no[i])) return 0;
    }
    return 1;
}

/**
 * @brief Validate name (letters and spaces, 3-100 chars)
 * @return 1 if valid, 0 if invalid
 */
int validate_name(const char *name) {
    if (is_empty(name)) return 0;
    if (strlen(name) < 3 || strlen(name) > 100) return 0;

    for (int i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ') return 0;
    }
    return 1;
}

/**
 * @brief Validate mobile number (10 digits)
 * @return 1 if valid, 0 if invalid
 */
int validate_mobile(const char *mobile) {
    if (is_empty(mobile)) return 0;
    if (strlen(mobile) != 10) return 0;

    for (int i = 0; i < strlen(mobile); i++) {
        if (!isdigit(mobile[i])) return 0;
    }
    return 1;
}

/**
 * @brief Validate email address (basic check)
 * @return 1 if valid, 0 if invalid
 */
int validate_email(const char *email) {
    if (is_empty(email)) return 0;
    if (strlen(email) < 5 || strlen(email) > 100) return 0;

    const char *at = strchr(email, '@');
    if (at == NULL) return 0;

    const char *dot = strchr(at, '.');
    if (dot == NULL) return 0;

    if (at == email || dot == at + 1) return 0;
    return 1;
}

/**
 * @brief Validate amount (positive number)
 * @return 1 if valid, 0 if invalid
 */
int validate_amount(double amount) {
    return amount > 0;
}

/**
 * @brief Validate semester (1-8)
 * @return 1 if valid, 0 if invalid
 */
int validate_semester(int semester) {
    return semester >= 1 && semester <= 8;
}

/**
 * @brief Validate date format (YYYY-MM-DD)
 * @return 1 if valid, 0 if invalid
 */
int validate_date_format(const char *date) {
    if (is_empty(date)) return 0;
    if (strlen(date) != 10) return 0;

    // Format: YYYY-MM-DD
    if (date[4] != '-' || date[7] != '-') return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(date[i])) return 0;
    }

    int year = atoi(&date[0]);
    int month = atoi(&date[5]);
    int day = atoi(&date[8]);

    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;

    return 1;
}

/**
 * @brief Validate student information
 * @return 1 if all fields valid, 0 if any invalid
 */
int validate_student_input(const char *roll_no, const char *name,
                           const char *branch, const char *mobile) {
    if (!validate_roll_no(roll_no)) return 0;
    if (!validate_name(name)) return 0;
    if (is_empty(branch)) return 0;
    if (!validate_mobile(mobile)) return 0;
    return 1;
}

/**
 * @brief Validate fee information
 * @return 1 if valid, 0 if invalid
 */
int validate_fee_input(const char *fee_type, double amount,
                       const char *due_date) {
    if (is_empty(fee_type)) return 0;
    if (!validate_amount(amount)) return 0;
    if (!validate_date_format(due_date)) return 0;
    return 1;
}

/**
 * @brief Validate employee information
 * @return 1 if valid, 0 if invalid
 */
int validate_employee_input(const char *emp_no, const char *name,
                            const char *designation, double salary) {
    if (is_empty(emp_no)) return 0;
    if (!validate_name(name)) return 0;
    if (is_empty(designation)) return 0;
    if (!validate_amount(salary)) return 0;
    return 1;
}

/**
 * @brief Trim whitespace from string
 */
void trim_string(char *str) {
    if (str == NULL) return;

    int start = 0;
    while (isspace(str[start])) start++;

    int end = strlen(str) - 1;
    while (end >= start && isspace(str[end])) end--;

    if (start > 0) {
        memmove(str, str + start, end - start + 2);
    }
    str[end - start + 1] = '\0';
}

/**
 * @brief Convert string to uppercase
 */
void to_uppercase(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

/**
 * @brief Convert string to lowercase
 */
void to_lowercase(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}