#ifndef PAYROLL_H
#define PAYROLL_H

#include "database.h"  // Get Employee and Payroll from database.h

/* ============================================================================
 * BUSINESS LOGIC FUNCTION PROTOTYPES (payroll_logic.c)
 * ============================================================================ */

float payroll_calculate_total_allowances(const Payroll *payroll);
float payroll_calculate_total_deductions(const Payroll *payroll);
float payroll_calculate_gross(const Payroll *payroll);
float payroll_calculate_net(Payroll *payroll);
int payroll_validate(const Payroll *payroll, char *error_msg, size_t error_len);
int payroll_build_salary_slip(const Payroll *payroll, void *slip);
int payroll_format_slip_text(const void *slip, char *buffer, size_t buffer_size);
float payroll_get_monthly_summary(const char *month_year, const char *department);

/* ============================================================================
 * UI FUNCTION PROTOTYPES (payroll_ui.c)
 * ============================================================================ */

void create_payroll_ui(GtkWidget *container);
void refresh_payroll_table();
void show_salary_slip_preview(int payroll_id);
void print_salary_slip(int payroll_id);

#endif  // PAYROLL_H
