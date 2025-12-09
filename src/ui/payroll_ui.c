#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../../include/payroll.h"

/* ============================================================================
 * STATIC VARIABLES (UI Components)
 * ============================================================================ */

// Main containers
static GtkWidget *payroll_main_box = NULL;
static GtkWidget *payroll_form_box = NULL;
// Employee Selection Section
static GtkWidget *employee_combo = NULL;
static GtkWidget *emp_name_label = NULL;
static GtkWidget *emp_dept_label = NULL;
static GtkWidget *emp_designation_label = NULL;
static GtkWidget *emp_salary_label = NULL;

// Month/Year Selection
static GtkWidget *month_combo = NULL;
static GtkWidget *year_spin = NULL;

// Allowances Input Fields
static GtkWidget *hra_entry = NULL;
static GtkWidget *medical_entry = NULL;
static GtkWidget *conveyance_entry = NULL;
static GtkWidget *da_entry = NULL;
static GtkWidget *bonus_entry = NULL;
static GtkWidget *other_allow_entry = NULL;

// Deductions Input Fields
static GtkWidget *it_entry = NULL;
static GtkWidget *pf_entry = NULL;
static GtkWidget *insurance_entry = NULL;
static GtkWidget *loan_entry = NULL;
static GtkWidget *other_deduct_entry = NULL;

// Calculation Display Labels
static GtkWidget *total_allow_label = NULL;
static GtkWidget *total_deduct_label = NULL;
static GtkWidget *gross_label = NULL;
static GtkWidget *net_label = NULL;

// Payroll Table
static GtkWidget *payroll_tree_view = NULL;
static GtkListStore *payroll_store = NULL;

// Current payroll being edited
static Payroll current_payroll = {0};
static int current_emp_id = -1;
static int current_payroll_id = -1;

/* ============================================================================
 * HELPER FUNCTIONS
 * ============================================================================ */

/**
 * Get float value from entry widget, return 0 if empty or invalid
 */
static float get_entry_float(GtkWidget *entry) {
    if (entry == NULL) return 0.0f;
    
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (text == NULL || strlen(text) == 0) return 0.0f;
    
    return (float)atof(text);
}

/**
 * Set entry widget with float value
 */
static void set_entry_float(GtkWidget *entry, float value) {
    if (entry == NULL) return;
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    gtk_entry_set_text(GTK_ENTRY(entry), buffer);
}

/**
 * Update calculation displays after any input change
 */
static void update_calculations() {
    if (payroll_form_box == NULL) return;

    // Read all values from UI
    current_payroll.house_rent = get_entry_float(hra_entry);
    current_payroll.medical = get_entry_float(medical_entry);
    current_payroll.conveyance = get_entry_float(conveyance_entry);
    current_payroll.dearness_allowance = get_entry_float(da_entry);
    current_payroll.performance_bonus = get_entry_float(bonus_entry);
    current_payroll.other_allowances = get_entry_float(other_allow_entry);

    current_payroll.income_tax = get_entry_float(it_entry);
    current_payroll.provident_fund = get_entry_float(pf_entry);
    current_payroll.health_insurance = get_entry_float(insurance_entry);
    current_payroll.loan_deduction = get_entry_float(loan_entry);
    current_payroll.other_deductions = get_entry_float(other_deduct_entry);

    // Calculate totals
    float total_allow = payroll_calculate_total_allowances(&current_payroll);
    float total_deduct = payroll_calculate_total_deductions(&current_payroll);
    float gross = payroll_calculate_gross(&current_payroll);
    float net = gross - total_deduct;

    // Update labels
    char buffer[64];

    snprintf(buffer, sizeof(buffer), "₹ %.2f", total_allow);
    gtk_label_set_text(GTK_LABEL(total_allow_label), buffer);

    snprintf(buffer, sizeof(buffer), "₹ %.2f", total_deduct);
    gtk_label_set_text(GTK_LABEL(total_deduct_label), buffer);

    snprintf(buffer, sizeof(buffer), "₹ %.2f", gross);
    gtk_label_set_text(GTK_LABEL(gross_label), buffer);

    snprintf(buffer, sizeof(buffer), "₹ %.2f", net);
    gtk_label_set_text(GTK_LABEL(net_label), buffer);

    printf("[DEBUG] Calculations updated: Allowances=%.2f, Deductions=%.2f, Gross=%.2f, Net=%.2f\n",
           total_allow, total_deduct, gross, net);
}

/**
 * Callback for entry field changes - triggers recalculation
 */
static void on_entry_changed(GtkEditable *editable, gpointer user_data) {
    (void)editable;      // Explicitly ignore unused parameters
    (void)user_data;
    
    update_calculations(); // Rest of function
}

/* ============================================================================
 * EMPLOYEE SELECTION CALLBACKS
 * ============================================================================ */

/**
 * Callback when employee is selected from dropdown
 */
static void on_employee_combo_changed(GtkComboBox *combo, gpointer user_data) {
    (void)user_data;
    
    GtkTreeIter iter;
    // Rest of function    
    if (!gtk_combo_box_get_active_iter(combo, &iter)) {
        printf("[WARNING] No employee selected\n");
        return;
    }

    GtkTreeModel *model = gtk_combo_box_get_model(combo);
    int emp_id = -1;
    char emp_name[100] = "";
    char emp_dept[50] = "";
    char emp_designation[50] = "";
    float emp_salary = 0.0f;

    gtk_tree_model_get(model, &iter,
        0, &emp_id,
        1, &emp_name,
        2, &emp_dept,
        3, &emp_designation,
        4, &emp_salary,
        -1);

    printf("[INFO] Employee selected: ID=%d, Name=%s\n", emp_id, emp_name);

    // Update labels
    gtk_label_set_text(GTK_LABEL(emp_name_label), emp_name);
    gtk_label_set_text(GTK_LABEL(emp_dept_label), emp_dept);
    gtk_label_set_text(GTK_LABEL(emp_designation_label), emp_designation);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "₹ %.2f", emp_salary);
    gtk_label_set_text(GTK_LABEL(emp_salary_label), buffer);

    // Store basic salary
    current_payroll.emp_id = emp_id;
    current_payroll.basic_salary = emp_salary;

    // Clear form for new entry
    set_entry_float(hra_entry, 0.0f);
    set_entry_float(medical_entry, 0.0f);
    set_entry_float(conveyance_entry, 0.0f);
    set_entry_float(da_entry, 0.0f);
    set_entry_float(bonus_entry, 0.0f);
    set_entry_float(other_allow_entry, 0.0f);

    set_entry_float(it_entry, 0.0f);
    set_entry_float(pf_entry, 0.0f);
    set_entry_float(insurance_entry, 0.0f);
    set_entry_float(loan_entry, 0.0f);
    set_entry_float(other_deduct_entry, 0.0f);

    update_calculations();
}

/* ============================================================================
 * PAYROLL ACTION CALLBACKS
 * ============================================================================ */

/**
 * Calculate button clicked - trigger calculation
 */
static void on_calculate_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Calculate button clicked\n");
    // Rest of function
    char error_msg[500] = "";

    // Validate payroll data
    if (!payroll_validate(&current_payroll, error_msg, sizeof(error_msg))) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Validation Error: %s", error_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Calculate net salary
    payroll_calculate_net(&current_payroll);

    // Update display
    update_calculations();

    printf("[SUCCESS] Payroll calculated successfully\n");
}

/**
 * Save button clicked - save payroll to database
 */
static void on_save_payroll_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Save Payroll button clicked\n");
    // Rest of function
    // Get month/year from combo/spinner
    gint active_month = gtk_combo_box_get_active(GTK_COMBO_BOX(month_combo));
    gint year = (gint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(year_spin));

    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    if (active_month < 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Please select a month");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    snprintf(current_payroll.month_year, sizeof(current_payroll.month_year),
             "%s-%d", months[active_month], year);

    strcpy(current_payroll.status, "Pending");
    strcpy(current_payroll.payment_date, "");
    strcpy(current_payroll.payment_method, "");

    // Save to database
    int payroll_id = db_add_payroll(&current_payroll);

    if (payroll_id > 0) {
        current_payroll_id = payroll_id;

        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Payroll saved successfully!\nPayroll ID: %d", payroll_id);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // Refresh table
        refresh_payroll_table();
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Failed to save payroll: %s", db_payroll_get_error());
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

/**
 * Reset button clicked - clear form
 */
static void on_reset_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Reset button clicked\n");
    // Rest of function
    gtk_combo_box_set_active(GTK_COMBO_BOX(employee_combo), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(month_combo), -1);

    set_entry_float(hra_entry, 0.0f);
    set_entry_float(medical_entry, 0.0f);
    set_entry_float(conveyance_entry, 0.0f);
    set_entry_float(da_entry, 0.0f);
    set_entry_float(bonus_entry, 0.0f);
    set_entry_float(other_allow_entry, 0.0f);

    set_entry_float(it_entry, 0.0f);
    set_entry_float(pf_entry, 0.0f);
    set_entry_float(insurance_entry, 0.0f);
    set_entry_float(loan_entry, 0.0f);
    set_entry_float(other_deduct_entry, 0.0f);

    gtk_label_set_text(GTK_LABEL(emp_name_label), "---");
    gtk_label_set_text(GTK_LABEL(emp_dept_label), "---");
    gtk_label_set_text(GTK_LABEL(emp_designation_label), "---");
    gtk_label_set_text(GTK_LABEL(emp_salary_label), "---");

    memset(&current_payroll, 0, sizeof(Payroll));
    current_emp_id = -1;
    current_payroll_id = -1;

    update_calculations();
}

/**
 * Delete payroll record
 */
static void on_delete_payroll_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Delete Payroll button clicked\n");
    // Rest of function
    if (current_payroll_id <= 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "No payroll selected");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Confirmation dialog
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Delete payroll ID %d?", current_payroll_id);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (response == GTK_RESPONSE_YES) {
        if (db_delete_payroll(current_payroll_id) == 1) {
            GtkWidget *info = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Payroll deleted successfully");
            gtk_dialog_run(GTK_DIALOG(info));
            gtk_widget_destroy(info);

            refresh_payroll_table();
            on_reset_clicked(NULL, NULL);
        } else {
            GtkWidget *error = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Failed to delete payroll");
            gtk_dialog_run(GTK_DIALOG(error));
            gtk_widget_destroy(error);
        }
    }
}

/**
 * Mark payroll as paid
 */
static void on_mark_paid_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Mark Paid button clicked\n");
    // Rest of function
    if (current_payroll_id <= 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "No payroll selected");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // TODO: Show dialog for payment date and method
    // For now, use current date
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char payment_date[20];
    strftime(payment_date, sizeof(payment_date), "%d-%m-%Y", tm_info);

    if (db_mark_payroll_paid(current_payroll_id, payment_date, "Bank Transfer") == 1) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Payroll marked as paid\nDate: %s", payment_date);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        refresh_payroll_table();
    }
}

/**
 * Print/View salary slip
 */
static void on_print_slip_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Print Slip button clicked\n");
    // Rest of function 
    if (current_payroll_id <= 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "No payroll selected");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Format salary slip
    SalarySlip slip;
    payroll_build_salary_slip(&current_payroll, &slip);

    // Format as text
    char slip_text[4096];
    payroll_format_slip_text(&slip, slip_text, sizeof(slip_text));

    // Show in dialog
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Salary Slip",
        NULL,
        GTK_DIALOG_MODAL,
        "Print", GTK_RESPONSE_OK,
        "Close", GTK_RESPONSE_CANCEL,
        NULL);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 700, 600);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, slip_text, -1);

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), text_view);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), scrolled);

    gtk_widget_show_all(dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK) {
        printf("[INFO] Print functionality would be triggered here\n");
        // TODO: Implement actual printing
    }

    gtk_widget_destroy(dialog);
}

/* ============================================================================
 * PAYROLL TABLE FUNCTIONS
 * ============================================================================ */

/**
 * Refresh payroll table with current data from database
 */
void refresh_payroll_table() {
    if (payroll_store == NULL) {
        fprintf(stderr, "[ERROR] Payroll store is NULL\n");
        return;
    }

    printf("[INFO] Refreshing payroll table...\n");

    // Clear existing data
    gtk_list_store_clear(payroll_store);

    // Get all payroll records from database
    sqlite3_stmt *stmt = db_get_all_payroll();

    if (stmt == NULL) {
        fprintf(stderr, "[ERROR] Failed to get payroll records: %s\n",
                db_payroll_get_error());
        return;
    }

    int count = 0;

    // Add rows to store
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        GtkTreeIter iter;
        gtk_list_store_append(payroll_store, &iter);

        int payroll_id = sqlite3_column_int(stmt, 0);
        int emp_id = sqlite3_column_int(stmt, 1);
        const char *month_year = (const char *)sqlite3_column_text(stmt, 2);
        float basic = sqlite3_column_double(stmt, 3);
        float net = sqlite3_column_double(stmt, 18);
        const char *status = (const char *)sqlite3_column_text(stmt, 21);

        gtk_list_store_set(payroll_store, &iter,
            0, payroll_id,
            1, emp_id,
            2, month_year,
            3, basic,
            4, net,
            5, status,
            -1);

        count++;
    }

    sqlite3_finalize(stmt);

    printf("[SUCCESS] Payroll table refreshed: %d records\n", count);
}

/**
 * Callback for table row selection
 */
static void on_payroll_row_selected(GtkTreeView *tree_view, GtkTreePath *path,
                                    GtkTreeViewColumn *col, gpointer user_data) {
    (void)col;
    (void)user_data;
    
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    // Rest of function
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        int payroll_id = 0;
        gtk_tree_model_get(model, &iter, 0, &payroll_id, -1);

        printf("[INFO] Payroll row selected: ID=%d\n", payroll_id);

        // Load payroll data
        if (db_get_payroll(payroll_id, &current_payroll) == 1) {
            current_payroll_id = payroll_id;

            // Update form with payroll data
            set_entry_float(hra_entry, current_payroll.house_rent);
            set_entry_float(medical_entry, current_payroll.medical);
            set_entry_float(conveyance_entry, current_payroll.conveyance);
            set_entry_float(da_entry, current_payroll.dearness_allowance);
            set_entry_float(bonus_entry, current_payroll.performance_bonus);
            set_entry_float(other_allow_entry, current_payroll.other_allowances);

            set_entry_float(it_entry, current_payroll.income_tax);
            set_entry_float(pf_entry, current_payroll.provident_fund);
            set_entry_float(insurance_entry, current_payroll.health_insurance);
            set_entry_float(loan_entry, current_payroll.loan_deduction);
            set_entry_float(other_deduct_entry, current_payroll.other_deductions);

            update_calculations();
        }
    }
}

/* ============================================================================
 * MAIN UI CREATION FUNCTION
 * ============================================================================ */

/**
 * Create main payroll UI
 */
void create_payroll_ui(GtkWidget *container) {
    printf("[INFO] Creating Payroll UI...\n");

    if (container == NULL) {
        fprintf(stderr, "[ERROR] Container is NULL\n");
        return;
    }

    // Main box (vertical)
    payroll_main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(container), payroll_main_box);
    gtk_container_set_border_width(GTK_CONTAINER(payroll_main_box), 10);

    // ===== EMPLOYEE SELECTION SECTION =====
    GtkWidget *emp_frame = gtk_frame_new("Employee Selection");
    gtk_box_pack_start(GTK_BOX(payroll_main_box), emp_frame, FALSE, FALSE, 0);

    GtkWidget *emp_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(emp_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(emp_grid), 5);
    gtk_container_add(GTK_CONTAINER(emp_frame), emp_grid);
    gtk_container_set_border_width(GTK_CONTAINER(emp_grid), 10);

    // Employee dropdown
    GtkWidget *emp_label = gtk_label_new("Select Employee:");
    gtk_grid_attach(GTK_GRID(emp_grid), emp_label, 0, 0, 1, 1);

    employee_combo = gtk_combo_box_new();
    gtk_grid_attach(GTK_GRID(emp_grid), employee_combo, 1, 0, 1, 1);

    // TODO: Populate employee combo from database
    g_signal_connect(employee_combo, "changed",
                    G_CALLBACK(on_employee_combo_changed), NULL);

    // Employee info labels
    emp_name_label = gtk_label_new("Name: ---");
    gtk_grid_attach(GTK_GRID(emp_grid), emp_name_label, 0, 1, 2, 1);

    emp_dept_label = gtk_label_new("Department: ---");
    gtk_grid_attach(GTK_GRID(emp_grid), emp_dept_label, 0, 2, 2, 1);

    emp_designation_label = gtk_label_new("Designation: ---");
    gtk_grid_attach(GTK_GRID(emp_grid), emp_designation_label, 0, 3, 2, 1);

    emp_salary_label = gtk_label_new("Base Salary: ---");
    gtk_grid_attach(GTK_GRID(emp_grid), emp_salary_label, 0, 4, 2, 1);

    // ===== PERIOD SELECTION =====
    GtkWidget *period_frame = gtk_frame_new("Payroll Period");
    gtk_box_pack_start(GTK_BOX(payroll_main_box), period_frame, FALSE, FALSE, 0);

    GtkWidget *period_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(period_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(period_grid), 5);
    gtk_container_add(GTK_CONTAINER(period_frame), period_grid);
    gtk_container_set_border_width(GTK_CONTAINER(period_grid), 10);

    GtkWidget *month_label = gtk_label_new("Month:");
    gtk_grid_attach(GTK_GRID(period_grid), month_label, 0, 0, 1, 1);

    month_combo = gtk_combo_box_text_new();
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), months[i]);
    }
    gtk_grid_attach(GTK_GRID(period_grid), month_combo, 1, 0, 1, 1);

    GtkWidget *year_label = gtk_label_new("Year:");
    gtk_grid_attach(GTK_GRID(period_grid), year_label, 2, 0, 1, 1);

    year_spin = gtk_spin_button_new_with_range(2020, 2050, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(year_spin), 2025);
    gtk_grid_attach(GTK_GRID(period_grid), year_spin, 3, 0, 1, 1);

    // ===== ALLOWANCES SECTION =====
    GtkWidget *allow_frame = gtk_frame_new("Allowances (₹)");
    gtk_box_pack_start(GTK_BOX(payroll_main_box), allow_frame, FALSE, FALSE, 0);

    GtkWidget *allow_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(allow_grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(allow_grid), 5);
    gtk_container_add(GTK_CONTAINER(allow_frame), allow_grid);
    gtk_container_set_border_width(GTK_CONTAINER(allow_grid), 10);

    // HRA
    GtkWidget *hra_label = gtk_label_new("HRA:");
    gtk_grid_attach(GTK_GRID(allow_grid), hra_label, 0, 0, 1, 1);
    hra_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(hra_entry), "0");
    gtk_grid_attach(GTK_GRID(allow_grid), hra_entry, 1, 0, 1, 1);
    g_signal_connect(hra_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Medical
    GtkWidget *med_label = gtk_label_new("Medical:");
    gtk_grid_attach(GTK_GRID(allow_grid), med_label, 2, 0, 1, 1);
    medical_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(medical_entry), "0");
    gtk_grid_attach(GTK_GRID(allow_grid), medical_entry, 3, 0, 1, 1);
    g_signal_connect(medical_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Conveyance
    GtkWidget *conv_label = gtk_label_new("Conveyance:");
    gtk_grid_attach(GTK_GRID(allow_grid), conv_label, 4, 0, 1, 1);
    conveyance_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(conveyance_entry), "0");
    gtk_grid_attach(GTK_GRID(allow_grid), conveyance_entry, 5, 0, 1, 1);
    g_signal_connect(conveyance_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // DA
    GtkWidget *da_label = gtk_label_new("DA:");
    gtk_grid_attach(GTK_GRID(allow_grid), da_label, 0, 1, 1, 1);
    da_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(da_entry), "0");
    gtk_grid_attach(GTK_GRID(allow_grid), da_entry, 1, 1, 1, 1);
    g_signal_connect(da_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Bonus
    GtkWidget *bonus_label = gtk_label_new("Bonus:");
    gtk_grid_attach(GTK_GRID(allow_grid), bonus_label, 2, 1, 1, 1);
    bonus_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(bonus_entry), "0");
    gtk_grid_attach(GTK_GRID(allow_grid), bonus_entry, 3, 1, 1, 1);
    g_signal_connect(bonus_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Other Allowances
    GtkWidget *other_allow_label = gtk_label_new("Other:");
    gtk_grid_attach(GTK_GRID(allow_grid), other_allow_label, 4, 1, 1, 1);
    other_allow_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(other_allow_entry), "0");
    gtk_grid_attach(GTK_GRID(allow_grid), other_allow_entry, 5, 1, 1, 1);
    g_signal_connect(other_allow_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // ===== DEDUCTIONS SECTION =====
    GtkWidget *deduct_frame = gtk_frame_new("Deductions (₹)");
    gtk_box_pack_start(GTK_BOX(payroll_main_box), deduct_frame, FALSE, FALSE, 0);

    GtkWidget *deduct_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(deduct_grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(deduct_grid), 5);
    gtk_container_add(GTK_CONTAINER(deduct_frame), deduct_grid);
    gtk_container_set_border_width(GTK_CONTAINER(deduct_grid), 10);

    // Income Tax
    GtkWidget *it_label = gtk_label_new("Income Tax:");
    gtk_grid_attach(GTK_GRID(deduct_grid), it_label, 0, 0, 1, 1);
    it_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(it_entry), "0");
    gtk_grid_attach(GTK_GRID(deduct_grid), it_entry, 1, 0, 1, 1);
    g_signal_connect(it_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // PF
    GtkWidget *pf_label = gtk_label_new("Provident Fund:");
    gtk_grid_attach(GTK_GRID(deduct_grid), pf_label, 2, 0, 1, 1);
    pf_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(pf_entry), "0");
    gtk_grid_attach(GTK_GRID(deduct_grid), pf_entry, 3, 0, 1, 1);
    g_signal_connect(pf_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Insurance
    GtkWidget *insure_label = gtk_label_new("Insurance:");
    gtk_grid_attach(GTK_GRID(deduct_grid), insure_label, 4, 0, 1, 1);
    insurance_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(insurance_entry), "0");
    gtk_grid_attach(GTK_GRID(deduct_grid), insurance_entry, 5, 0, 1, 1);
    g_signal_connect(insurance_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Loan Deduction
    GtkWidget *loan_label = gtk_label_new("Loan:");
    gtk_grid_attach(GTK_GRID(deduct_grid), loan_label, 0, 1, 1, 1);
    loan_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(loan_entry), "0");
    gtk_grid_attach(GTK_GRID(deduct_grid), loan_entry, 1, 1, 1, 1);
    g_signal_connect(loan_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // Other Deductions
    GtkWidget *other_deduct_label = gtk_label_new("Other:");
    gtk_grid_attach(GTK_GRID(deduct_grid), other_deduct_label, 2, 1, 1, 1);
    other_deduct_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(other_deduct_entry), "0");
    gtk_grid_attach(GTK_GRID(deduct_grid), other_deduct_entry, 3, 1, 1, 1);
    g_signal_connect(other_deduct_entry, "changed", G_CALLBACK(on_entry_changed), NULL);

    // ===== CALCULATION SUMMARY =====
    GtkWidget *summary_frame = gtk_frame_new("Calculation Summary");
    gtk_box_pack_start(GTK_BOX(payroll_main_box), summary_frame, FALSE, FALSE, 0);

    GtkWidget *summary_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(summary_grid), 30);
    gtk_grid_set_row_spacing(GTK_GRID(summary_grid), 5);
    gtk_container_add(GTK_CONTAINER(summary_frame), summary_grid);
    gtk_container_set_border_width(GTK_CONTAINER(summary_grid), 10);

    // Total Allowances
    GtkWidget *total_allow_title = gtk_label_new("Total Allowances:");
    gtk_grid_attach(GTK_GRID(summary_grid), total_allow_title, 0, 0, 1, 1);
    total_allow_label = gtk_label_new("₹ 0.00");
    gtk_grid_attach(GTK_GRID(summary_grid), total_allow_label, 1, 0, 1, 1);

    // Total Deductions
    GtkWidget *total_deduct_title = gtk_label_new("Total Deductions:");
    gtk_grid_attach(GTK_GRID(summary_grid), total_deduct_title, 2, 0, 1, 1);
    total_deduct_label = gtk_label_new("₹ 0.00");
    gtk_grid_attach(GTK_GRID(summary_grid), total_deduct_label, 3, 0, 1, 1);

    // Gross Salary
    GtkWidget *gross_title = gtk_label_new("Gross Salary:");
    gtk_grid_attach(GTK_GRID(summary_grid), gross_title, 0, 1, 1, 1);
    gross_label = gtk_label_new("₹ 0.00");
    gtk_grid_attach(GTK_GRID(summary_grid), gross_label, 1, 1, 1, 1);

    // Net Salary (Bold/Prominent)
    GtkWidget *net_title = gtk_label_new("NET SALARY:");
    gtk_widget_set_name(net_title, "payroll-net-title");
    gtk_grid_attach(GTK_GRID(summary_grid), net_title, 2, 1, 1, 1);
    net_label = gtk_label_new("₹ 0.00");
    gtk_widget_set_name(net_label, "payroll-net-value");
    gtk_grid_attach(GTK_GRID(summary_grid), net_label, 3, 1, 1, 1);

    // ===== ACTION BUTTONS =====
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(payroll_main_box), button_box, FALSE, FALSE, 0);

    GtkWidget *calc_btn = gtk_button_new_with_label("💰 Calculate");
    g_signal_connect(calc_btn, "clicked", G_CALLBACK(on_calculate_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), calc_btn, FALSE, FALSE, 0);

    GtkWidget *save_btn = gtk_button_new_with_label("💾 Save");
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_payroll_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), save_btn, FALSE, FALSE, 0);

    GtkWidget *reset_btn = gtk_button_new_with_label("🔄 Reset");
    g_signal_connect(reset_btn, "clicked", G_CALLBACK(on_reset_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), reset_btn, FALSE, FALSE, 0);

    GtkWidget *print_btn = gtk_button_new_with_label("🖨️ Print Slip");
    g_signal_connect(print_btn, "clicked", G_CALLBACK(on_print_slip_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), print_btn, FALSE, FALSE, 0);

    GtkWidget *paid_btn = gtk_button_new_with_label("✓ Mark Paid");
    g_signal_connect(paid_btn, "clicked", G_CALLBACK(on_mark_paid_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), paid_btn, FALSE, FALSE, 0);

    GtkWidget *delete_btn = gtk_button_new_with_label("🗑️ Delete");
    g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_payroll_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), delete_btn, FALSE, FALSE, 0);

    // ===== PAYROLL TABLE =====
    GtkWidget *table_frame = gtk_frame_new("Payroll Records");
    gtk_box_pack_start(GTK_BOX(payroll_main_box), table_frame, TRUE, TRUE, 0);

    // Create list store: payroll_id, emp_id, month_year, basic, net, status
    payroll_store = gtk_list_store_new(6,
        G_TYPE_INT,     // payroll_id
        G_TYPE_INT,     // emp_id
        G_TYPE_STRING,  // month_year
        G_TYPE_DOUBLE,  // basic
        G_TYPE_DOUBLE,  // net
        G_TYPE_STRING   // status
    );

    payroll_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(payroll_store));
    g_signal_connect(payroll_tree_view, "row-activated",
                    G_CALLBACK(on_payroll_row_selected), NULL);

    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes("ID",
        renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(payroll_tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("Emp ID",
        renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(payroll_tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("Month/Year",
        renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(payroll_tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("Basic",
        renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(payroll_tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("Net Salary",
        renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(payroll_tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("Status",
        renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(payroll_tree_view), col);

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), payroll_tree_view);
    gtk_container_add(GTK_CONTAINER(table_frame), scrolled);

    gtk_widget_show_all(payroll_main_box);

    printf("[SUCCESS] Payroll UI created successfully\n");
}