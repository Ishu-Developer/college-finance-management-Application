#include <string.h>
#include <ctype.h>
#include "../../include/validators.h"

/**
 * @brief Validate roll number format
 */
int validate_roll_no(const char *roll_no) {
    if (roll_no == NULL || strlen(roll_no) == 0) {
        return 0;  // Empty roll number
    }
    
    // Roll number should be alphanumeric
    size_t len = strlen(roll_no);
    for (size_t i = 0; i < len; i++) {  // Fixed: size_t instead of int
        if (!isalnum((unsigned char)roll_no[i])) {
            return 0;  // Invalid character
        }
    }
    return 1;
}

/**
 * @brief Validate name format
 */
int validate_name(const char *name) {
    if (name == NULL || strlen(name) == 0) {
        return 0;  // Empty name
    }
    
    // Name should contain only letters and spaces
    size_t len = strlen(name);
    for (size_t i = 0; i < len; i++) {  // Fixed: size_t instead of int
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') {
            return 0;  // Invalid character
        }
    }
    return 1;
}

/**
 * @brief Validate email format
 */
int validate_email(const char *email) {
    if (email == NULL || strlen(email) == 0) {
        return 0;  // Empty email
    }
    
    if (strchr(email, '@') == NULL) {
        return 0;  // No @ symbol
    }
    
    if (strchr(email, '.') == NULL) {
        return 0;  // No dot in domain
    }
    
    return 1;
}

/**
 * @brief Validate mobile number
 */
int validate_mobile(const char *mobile) {
    if (mobile == NULL || strlen(mobile) == 0) {
        return 0;  // Empty mobile
    }
    
    // Mobile should contain only digits
    size_t len = strlen(mobile);
    if (len < 10 || len > 15) {
        return 0;  // Invalid length
    }
    
    for (size_t i = 0; i < len; i++) {  // Fixed: size_t instead of int
        if (!isdigit((unsigned char)mobile[i])) {
            return 0;  // Non-digit character
        }
    }
    
    return 1;
}

/**
 * @brief Validate amount
 */
int validate_amount(double amount) {
    return amount > 0;
}

/**
 * @brief Validate date format (YYYY-MM-DD)
 */
int validate_date(const char *date) {
    if (date == NULL || strlen(date) != 10) {
        return 0;  // Invalid format
    }
    
    // Check format: YYYY-MM-DD
    if (date[4] != '-' || date[7] != '-') {
        return 0;  // Wrong separator
    }
    
    // Check if all characters are digits except separators
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) {
            return 0;
        }
    }
    
    return 1;
}