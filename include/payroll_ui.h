#ifndef PAYROLL_UI_H
#define PAYROLL_UI_H

#include <gtk/gtk.h>

/* ============================================================================
 * PAYROLL UI FUNCTION PROTOTYPES
 * ============================================================================ */

/**
 * Create main payroll UI
 * Called when Payroll tab is selected
 * Sets up all GTK widgets for payroll management
 * @param container - GTK container to add payroll UI to
 */
void create_payroll_ui(GtkWidget *container);

/**
 * Refresh payroll table with current data from database
 * Called after any CRUD operation (Add, Update, Delete)
 * Clears existing table and reloads all payroll records
 */
void refresh_payroll_table();

/**
 * Show salary slip preview dialog
 * Displays formatted salary slip with box drawing characters
 * @param payroll_id - ID of payroll record to display
 */
void show_salary_slip_preview(int payroll_id);

/**
 * Generate and print salary slip
 * Opens print dialog for selected payroll record
 * @param payroll_id - ID of payroll record to print
 */
void print_salary_slip(int payroll_id);

/**
 * Populate employee combo box
 * Loads all active employees from database into dropdown
 * Called during UI initialization
 */
void populate_employee_combo();

/**
 * Clear/reset payroll form
 * Clears all input fields and resets calculations
 */
void clear_payroll_form();

/**
 * Load payroll record into form for editing
 * Fills all form fields with payroll data for selected record
 * @param payroll_id - ID of payroll to load
 */
void load_payroll_to_form(int payroll_id);

/**
 * Get current payroll data from form
 * Reads all input fields and returns payroll structure
 * @param payroll - Pointer to Payroll struct to fill
 * @return 1 on success, 0 if validation fails
 */
int get_payroll_from_form(Payroll *payroll);

/**
 * Update calculation displays
 * Recalculates and updates all salary calculation labels
 * Called whenever any input field changes
 */
void update_payroll_calculations();

#endif 