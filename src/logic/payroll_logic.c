#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../include/database.h" 

/* ============================================================================
 * CONSTANTS FOR TAX CALCULATION
 * ============================================================================ */

// Indian Income Tax Slabs 2024-25 (FY 2024-25)
#define TAX_SLAB_1_LIMIT    250000    // 0% up to 2.5 Lakhs
#define TAX_SLAB_2_LIMIT    500000    // 5% on 2.5L to 5L
#define TAX_SLAB_3_LIMIT    1000000   // 20% on 5L to 10L
#define TAX_SLAB_4_LIMIT    1500000   // 30% on 10L to 15L
// 30% above 15L

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ============================================================================ */

/**
 * Calculate total allowances from payroll structure
 * Sum: HRA + Medical + Conveyance + DA + Bonus + Other
 * @param payroll - Payroll struct
 * @return Total allowances amount
 */
float payroll_calculate_total_allowances(const Payroll *payroll) {
    if (payroll == NULL) {
        fprintf(stderr, "[ERROR] Payroll pointer is NULL\n");
        return 0.0f;
    }

    float total = 0.0f;
    
    total += payroll->house_rent;
    total += payroll->medical;
    total += payroll->conveyance;
    total += payroll->dearness_allowance;
    total += payroll->performance_bonus;
    total += payroll->other_allowances;

    printf("[DEBUG] Total Allowances: HRA=%.2f + Medical=%.2f + Conv=%.2f + DA=%.2f + Bonus=%.2f + Other=%.2f = %.2f\n",
           payroll->house_rent, payroll->medical, payroll->conveyance,
           payroll->dearness_allowance, payroll->performance_bonus,
           payroll->other_allowances, total);

    return total;
}

/**
 * Calculate total deductions from payroll structure
 * Sum: IT + PF + Insurance + Loan + Other
 * @param payroll - Payroll struct
 * @return Total deductions amount
 */
float payroll_calculate_total_deductions(const Payroll *payroll) {
    if (payroll == NULL) {
        fprintf(stderr, "[ERROR] Payroll pointer is NULL\n");
        return 0.0f;
    }

    float total = 0.0f;

    total += payroll->income_tax;
    total += payroll->provident_fund;
    total += payroll->health_insurance;
    total += payroll->loan_deduction;
    total += payroll->other_deductions;

    printf("[DEBUG] Total Deductions: IT=%.2f + PF=%.2f + Insure=%.2f + Loan=%.2f + Other=%.2f = %.2f\n",
           payroll->income_tax, payroll->provident_fund, payroll->health_insurance,
           payroll->loan_deduction, payroll->other_deductions, total);

    return total;
}

/**
 * Calculate gross salary
 * Gross = Basic + Total Allowances
 * @param payroll - Payroll struct
 * @return Gross salary amount
 */
float payroll_calculate_gross(const Payroll *payroll) {
    if (payroll == NULL) {
        fprintf(stderr, "[ERROR] Payroll pointer is NULL\n");
        return 0.0f;
    }

    float total_allowances = payroll_calculate_total_allowances(payroll);
    float gross = payroll->basic_salary + total_allowances;

    printf("[DEBUG] Gross Salary: Basic=%.2f + Allowances=%.2f = %.2f\n",
           payroll->basic_salary, total_allowances, gross);

    return gross;
}

/**
 * Calculate net salary
 * Net = Gross - Total Deductions
 * Updates payroll struct with calculated values
 * @param payroll - Payroll struct (will be modified with calculated values)
 * @return Net salary amount
 */
float payroll_calculate_net(Payroll *payroll) {
    if (payroll == NULL) {
        fprintf(stderr, "[ERROR] Payroll pointer is NULL\n");
        return 0.0f;
    }

    // Calculate components
    payroll->total_allowances = payroll_calculate_total_allowances(payroll);
    payroll->total_deductions = payroll_calculate_total_deductions(payroll);
    payroll->gross_salary = payroll->basic_salary + payroll->total_allowances;
    payroll->net_salary = payroll->gross_salary - payroll->total_deductions;

    printf("[INFO] ====== SALARY CALCULATION SUMMARY ======\n");
    printf("[INFO] Basic Salary:         ₹%.2f\n", payroll->basic_salary);
    printf("[INFO] Total Allowances:    +₹%.2f\n", payroll->total_allowances);
    printf("[INFO] Gross Salary:         ₹%.2f\n", payroll->gross_salary);
    printf("[INFO] Total Deductions:    -₹%.2f\n", payroll->total_deductions);
    printf("[INFO] NET SALARY:           ₹%.2f\n", payroll->net_salary);
    printf("[INFO] ========================================\n");

    return payroll->net_salary;
}

/**
 * Calculate income tax based on Indian tax slabs (2024-25)
 * Progressive tax calculation
 * @param gross_salary - Annual gross salary
 * @return Income tax amount
 */
float payroll_calculate_income_tax(float annual_salary) {
    if (annual_salary <= 0) {
        return 0.0f;
    }

    float tax = 0.0f;

    // Tax Slab Calculation (Indian Income Tax)
    if (annual_salary <= TAX_SLAB_1_LIMIT) {
        // 0% - No tax
        tax = 0.0f;
    }
    else if (annual_salary <= TAX_SLAB_2_LIMIT) {
        // 5% on amount above 2.5L
        tax = (annual_salary - TAX_SLAB_1_LIMIT) * 0.05f;
    }
    else if (annual_salary <= TAX_SLAB_3_LIMIT) {
        // 5% on 2.5L-5L + 20% on 5L-10L
        tax = (TAX_SLAB_2_LIMIT - TAX_SLAB_1_LIMIT) * 0.05f;
        tax += (annual_salary - TAX_SLAB_2_LIMIT) * 0.20f;
    }
    else if (annual_salary <= TAX_SLAB_4_LIMIT) {
        // 5% on 2.5L-5L + 20% on 5L-10L + 30% on 10L-15L
        tax = (TAX_SLAB_2_LIMIT - TAX_SLAB_1_LIMIT) * 0.05f;
        tax += (TAX_SLAB_3_LIMIT - TAX_SLAB_2_LIMIT) * 0.20f;
        tax += (annual_salary - TAX_SLAB_3_LIMIT) * 0.30f;
    }
    else {
        // 30% above 15L
        tax = (TAX_SLAB_2_LIMIT - TAX_SLAB_1_LIMIT) * 0.05f;
        tax += (TAX_SLAB_3_LIMIT - TAX_SLAB_2_LIMIT) * 0.20f;
        tax += (TAX_SLAB_4_LIMIT - TAX_SLAB_3_LIMIT) * 0.30f;
        tax += (annual_salary - TAX_SLAB_4_LIMIT) * 0.30f;
    }

    // Monthly tax (divide by 12)
    float monthly_tax = tax / 12.0f;

    printf("[DEBUG] Tax Calculation: Annual Salary=%.2f, Annual Tax=%.2f, Monthly Tax=%.2f\n",
           annual_salary, tax, monthly_tax);

    return monthly_tax;
}

/**
 * Validate payroll data for consistency and correctness
 * Checks: emp_id, month_year, basic_salary, allowances, deductions
 * @param payroll - Payroll struct to validate
 * @param error_msg - Buffer to store error message
 * @param error_len - Size of error buffer
 * @return 1 if valid, 0 if invalid (error_msg filled)
 */
int payroll_validate(const Payroll *payroll, char *error_msg, size_t error_len) {
    if (payroll == NULL) {
        snprintf(error_msg, error_len, "Payroll structure is NULL");
        fprintf(stderr, "[ERROR] %s\n", error_msg);
        return 0;
    }

    if (error_msg == NULL || error_len == 0) {
        fprintf(stderr, "[ERROR] Error message buffer is invalid\n");
        return 0;
    }

    // Check emp_id
    if (payroll->emp_id <= 0) {
        snprintf(error_msg, error_len, "Invalid Employee ID: %d (must be > 0)", payroll->emp_id);
        fprintf(stderr, "[ERROR] %s\n", error_msg);
        return 0;
    }

    // Check month_year
    if (strlen(payroll->month_year) == 0) {
        snprintf(error_msg, error_len, "Month/Year cannot be empty");
        fprintf(stderr, "[ERROR] %s\n", error_msg);
        return 0;
    }

    // Check basic_salary
    if (payroll->basic_salary <= 0) {
        snprintf(error_msg, error_len, "Basic salary must be greater than 0 (%.2f)", payroll->basic_salary);
        fprintf(stderr, "[ERROR] %s\n", error_msg);
        return 0;
    }

    // Check allowances are non-negative
    if (payroll->house_rent < 0 || payroll->medical < 0 || 
        payroll->conveyance < 0 || payroll->dearness_allowance < 0 ||
        payroll->performance_bonus < 0 || payroll->other_allowances < 0) {
        snprintf(error_msg, error_len, "Allowances cannot be negative");
        fprintf(stderr, "[ERROR] %s\n", error_msg);
        return 0;
    }

    // Check deductions are non-negative
    if (payroll->income_tax < 0 || payroll->provident_fund < 0 ||
        payroll->health_insurance < 0 || payroll->loan_deduction < 0 ||
        payroll->other_deductions < 0) {
        snprintf(error_msg, error_len, "Deductions cannot be negative");
        fprintf(stderr, "[ERROR] %s\n", error_msg);
        return 0;
    }

    // Check total deductions don't exceed gross salary
    float total_allow = payroll_calculate_total_allowances(payroll);
    float total_deduct = payroll_calculate_total_deductions(payroll);
    float gross = payroll->basic_salary + total_allow;

    if (total_deduct > gross) {
        snprintf(error_msg, error_len, 
                 "Total deductions (₹%.2f) cannot exceed gross salary (₹%.2f)",
                 total_deduct, gross);
        fprintf(stderr, "[ERROR] %s\n", error_msg);
        return 0;
    }

    printf("[SUCCESS] Payroll validation passed for emp_id=%d, month=%s\n",
           payroll->emp_id, payroll->month_year);
    return 1;
}

/**
 * Build salary slip from payroll record
 * Copies all relevant data from payroll to salary slip
 * Note: Employee name, designation, department should be loaded separately
 * @param payroll - Source payroll record
 * @param slip - Destination salary slip
 * @return 1 on success, 0 on failure
 */
int payroll_build_salary_slip(const Payroll *payroll, SalarySlip *slip) {
    if (payroll == NULL || slip == NULL) {
        fprintf(stderr, "[ERROR] Payroll or SalarySlip pointer is NULL\n");
        return 0;
    }

    // Clear salary slip
    memset(slip, 0, sizeof(SalarySlip));

    // Basic Info
    slip->payroll_id = payroll->payroll_id;
    slip->emp_id = payroll->emp_id;

    // Salary Components
    slip->basic_salary = payroll->basic_salary;
    slip->house_rent = payroll->house_rent;
    slip->medical = payroll->medical;
    slip->conveyance = payroll->conveyance;
    slip->dearness_allowance = payroll->dearness_allowance;
    slip->performance_bonus = payroll->performance_bonus;
    slip->other_allowances = payroll->other_allowances;
    slip->total_allowances = payroll->total_allowances;

    // Deductions
    slip->income_tax = payroll->income_tax;
    slip->provident_fund = payroll->provident_fund;
    slip->health_insurance = payroll->health_insurance;
    slip->loan_deduction = payroll->loan_deduction;
    slip->other_deductions = payroll->other_deductions;
    slip->total_deductions = payroll->total_deductions;

    // Final amounts
    slip->gross_salary = payroll->gross_salary;
    slip->net_salary = payroll->net_salary;

    strcpy(slip->payment_status, payroll->status);

    printf("[SUCCESS] Salary slip built for payroll_id=%d, emp_id=%d\n",
           payroll->payroll_id, payroll->emp_id);
    return 1;
}

/**
 * Format salary slip as text for display/printing
 * Creates a formatted string representation of salary slip
 * @param slip - Salary slip to format
 * @param buffer - Output buffer for text
 * @param buffer_size - Size of output buffer
 * @return Number of characters written to buffer
 */
int payroll_format_slip_text(const SalarySlip *slip, char *buffer, size_t buffer_size) {
    if (slip == NULL || buffer == NULL || buffer_size == 0) {
        fprintf(stderr, "[ERROR] Invalid parameters for formatting salary slip\n");
        return 0;
    }

    int written = 0;

    // Header
    written += snprintf(buffer + written, buffer_size - written,
        "╔════════════════════════════════════════════════════════════╗\n");
    written += snprintf(buffer + written, buffer_size - written,
        "║              SALARY SLIP / PAYROLL STATEMENT              ║\n");
    written += snprintf(buffer + written, buffer_size - written,
        "╠════════════════════════════════════════════════════════════╣\n");

    // Employee Info
    written += snprintf(buffer + written, buffer_size - written,
        "║ Employee:     %-45s ║\n", slip->employee_name);
    written += snprintf(buffer + written, buffer_size - written,
        "║ Emp No:       %-45s ║\n", slip->emp_no);
    written += snprintf(buffer + written, buffer_size - written,
        "║ Designation:  %-45s ║\n", slip->designation);
    written += snprintf(buffer + written, buffer_size - written,
        "║ Department:   %-45s ║\n", slip->department);

    // Period
    written += snprintf(buffer + written, buffer_size - written,
        "║ Period:       %s to %s                          ║\n",
        slip->from_date, slip->to_date);
    written += snprintf(buffer + written, buffer_size - written,
        "╠════════════════════════════════════════════════════════════╣\n");

    // Earnings
    written += snprintf(buffer + written, buffer_size - written,
        "║ EARNINGS:                                                  ║\n");
    written += snprintf(buffer + written, buffer_size - written,
        "║   Basic Salary:              ₹ %12.2f           ║\n", slip->basic_salary);
    written += snprintf(buffer + written, buffer_size - written,
        "║   House Rent Allowance:      ₹ %12.2f           ║\n", slip->house_rent);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Medical Allowance:         ₹ %12.2f           ║\n", slip->medical);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Conveyance Allowance:      ₹ %12.2f           ║\n", slip->conveyance);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Dearness Allowance:        ₹ %12.2f           ║\n", slip->dearness_allowance);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Performance Bonus:         ₹ %12.2f           ║\n", slip->performance_bonus);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Other Allowances:          ₹ %12.2f           ║\n", slip->other_allowances);
    written += snprintf(buffer + written, buffer_size - written,
        "├────────────────────────────────────────────────────────────┤\n");
    written += snprintf(buffer + written, buffer_size - written,
        "║ Total Earnings:              ₹ %12.2f           ║\n", slip->total_allowances + slip->basic_salary);

    // Deductions
    written += snprintf(buffer + written, buffer_size - written,
        "╠════════════════════════════════════════════════════════════╣\n");
    written += snprintf(buffer + written, buffer_size - written,
        "║ DEDUCTIONS:                                                ║\n");
    written += snprintf(buffer + written, buffer_size - written,
        "║   Income Tax:                ₹ %12.2f           ║\n", slip->income_tax);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Provident Fund:            ₹ %12.2f           ║\n", slip->provident_fund);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Health Insurance:          ₹ %12.2f           ║\n", slip->health_insurance);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Loan Deduction:            ₹ %12.2f           ║\n", slip->loan_deduction);
    written += snprintf(buffer + written, buffer_size - written,
        "║   Other Deductions:          ₹ %12.2f           ║\n", slip->other_deductions);
    written += snprintf(buffer + written, buffer_size - written,
        "├────────────────────────────────────────────────────────────┤\n");
    written += snprintf(buffer + written, buffer_size - written,
        "║ Total Deductions:            ₹ %12.2f           ║\n", slip->total_deductions);

    // Net Salary
    written += snprintf(buffer + written, buffer_size - written,
        "╠════════════════════════════════════════════════════════════╣\n");
    written += snprintf(buffer + written, buffer_size - written,
        "║ NET SALARY (In Hand):        ₹ %12.2f           ║\n", slip->net_salary);
    written += snprintf(buffer + written, buffer_size - written,
        "║ Payment Status:              %-45s ║\n", slip->payment_status);
    written += snprintf(buffer + written, buffer_size - written,
        "╚════════════════════════════════════════════════════════════╝\n");

    return written;
}

/**
 * Get monthly payroll summary for a department
 * @param month_year - Month and year (e.g., "Dec-2025")
 * @param department - Department name (NULL for all departments)
 * @return Total payroll amount, -1 on error
 */
float payroll_get_monthly_summary(const char *month_year, const char *department) {
    if (month_year == NULL) {
        fprintf(stderr, "[ERROR] Month/Year cannot be NULL\n");
        return -1.0f;
    }

    printf("[INFO] Calculating payroll summary for %s, Department: %s\n",
           month_year, department ? department : "All");

    // This will be implemented to query database
    // For now, return placeholder
    printf("[WARNING] payroll_get_monthly_summary: Database query not yet implemented\n");

    return 0.0f;
}

/**
 * Calculate PF (Provident Fund) contribution
 * Standard: 12% of basic salary
 * @param basic_salary - Basic salary amount
 * @return PF contribution amount
 */
float payroll_calculate_pf(float basic_salary) {
    if (basic_salary <= 0) {
        return 0.0f;
    }

    // Standard PF: 12% of basic salary
    float pf = basic_salary * 0.12f;

    printf("[DEBUG] PF Calculation: Basic=%.2f * 12%% = %.2f\n", basic_salary, pf);

    return pf;
}

/**
 * Calculate HRA (House Rent Allowance)
 * Standard: 40% of basic salary (can vary by location)
 * @param basic_salary - Basic salary amount
 * @return HRA amount
 */
float payroll_calculate_hra(float basic_salary) {
    if (basic_salary <= 0) {
        return 0.0f;
    }

    // Standard HRA: 40% of basic salary
    float hra = basic_salary * 0.40f;

    printf("[DEBUG] HRA Calculation: Basic=%.2f * 40%% = %.2f\n", basic_salary, hra);

    return hra;
}

/**
 * Calculate DA (Dearness Allowance)
 * Standard: 50% of basic salary
 * @param basic_salary - Basic salary amount
 * @return DA amount
 */
float payroll_calculate_da(float basic_salary) {
    if (basic_salary <= 0) {
        return 0.0f;
    }

    // Standard DA: 50% of basic salary
    float da = basic_salary * 0.50f;

    printf("[DEBUG] DA Calculation: Basic=%.2f * 50%% = %.2f\n", basic_salary, da);

    return da;
}