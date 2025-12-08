#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "../../include/employee_ui.h"
#include "../../include/database.h"
#include "../../include/validators.h"

// Global variables
static GtkWidget *employee_table = NULL;
static GtkWidget *form_box = NULL;
static GtkWidget *emp_number_entry;
static GtkWidget *emp_name_entry;
static GtkWidget *emp_designation_combo;
static GtkWidget *emp_department_combo;
static GtkWidget *emp_category_combo;
static GtkWidget *emp_mobile_entry;
static GtkWidget *emp_email_entry;
static GtkWidget *emp_doa_entry;
static GtkWidget *emp_salary_entry;
static GtkWidget *emp_bank_account_entry;

/**
 * @brief Refresh employee table from database (FIXED for new API)
 */
void refresh_employee_list() {
    printf("[INFO] Refreshing employee table\n");
    
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(employee_table)));
    if (store) {
        gtk_list_store_clear(store);
    }
    
    sqlite3_stmt *stmt = NULL;
    int rc = db_get_all_employees(&stmt);  // ✅ FIXED: Pass pointer to stmt
    if (rc != 0 || stmt == NULL) {
        printf("[WARNING] No employees found or db_get_all_employees failed\n");
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int emp_id = sqlite3_column_int(stmt, 0);
        const char *emp_number = (const char *)sqlite3_column_text(stmt, 1);
        const char *emp_name = (const char *)sqlite3_column_text(stmt, 2);
        const char *birth_date = (const char *)sqlite3_column_text(stmt, 3);
        const char *department = (const char *)sqlite3_column_text(stmt, 4);
        const char *designation = (const char *)sqlite3_column_text(stmt, 5);
        const char *reporting_person = (const char *)sqlite3_column_text(stmt, 6);
        const char *email = (const char *)sqlite3_column_text(stmt, 7);
        const char *mobile = (const char *)sqlite3_column_text(stmt, 8);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️",              // Edit
            1, "🗑️",              // Delete
            2, emp_id,            // Employee ID (hidden)
            3, emp_number,        // Employee Number
            4, emp_name,          // Name (column 2 now)
            5, designation,       // Designation (column 5 now)
            6, department,        // Department (column 4 now)
            7, "N/A",             // Category (not in query, placeholder)
            8, email,             // Email
            9, mobile,            // Mobile
            10, birth_date,       // Birth Date (instead of DOA)
            -1);
        count++;
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] Loaded %d employees\n", count);
}

/**
 * @brief Handle Add Employee button click
 */
void on_add_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Add Employee button clicked\n");
    
    if (gtk_widget_get_visible(form_box)) {
        gtk_widget_hide(form_box);
    } else {
        gtk_widget_show_all(form_box);
        gtk_widget_grab_focus(emp_number_entry);
    }
}

/**
 * @brief Handle Save Employee button (FIXED for new Employee struct API)
 */
void on_save_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    // Get form values
    const char *emp_no = gtk_entry_get_text(GTK_ENTRY(emp_number_entry));
    const char *name = gtk_entry_get_text(GTK_ENTRY(emp_name_entry));
    const char *mobile = gtk_entry_get_text(GTK_ENTRY(emp_mobile_entry));
    const char *email = gtk_entry_get_text(GTK_ENTRY(emp_email_entry));
    const char *doa = gtk_entry_get_text(GTK_ENTRY(emp_doa_entry));
    const char *salary_str = gtk_entry_get_text(GTK_ENTRY(emp_salary_entry));
    const char *bank_account = gtk_entry_get_text(GTK_ENTRY(emp_bank_account_entry));
    
    // Convert salary string to double
    double salary = atof(salary_str);
    
    int designation_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(emp_designation_combo));
    const char *designation = (designation_idx == 0) ? "Faculty" :
                              (designation_idx == 1) ? "HOD" :
                              (designation_idx == 2) ? "Security" :
                              (designation_idx == 3) ? "Housekeeping" : "Other";
    
    int department_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(emp_department_combo));
    const char *department = (department_idx == 0) ? "CSE" :
                             (department_idx == 1) ? "EE" :
                             (department_idx == 2) ? "CIVIL" : "ME";
    
    int category_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(emp_category_combo));
    const char *category = (category_idx == 0) ? "Faculty" :
                           (category_idx == 1) ? "Staff" : "Support";
    
    printf("[INFO] Employee Validation - Name: %s, Emp#: %s, Dept: %s\n", name, emp_no, department);
    
    // Validation
    if (strlen(emp_no) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "❌ Employee Number cannot be empty!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    if (!validate_name(name)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "❌ Name contains invalid characters!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    if (!validate_mobile(mobile)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "❌ Mobile must be 10 digits!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    if (!validate_email(email)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "❌ Invalid email format!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    printf("[INFO] All validations passed\n");
    
    // ✅ FIXED: Create Employee struct and populate it
    Employee new_emp = {0};  // Zero-initialize
    strncpy(new_emp.emp_no, emp_no, sizeof(new_emp.emp_no) - 1);
    strncpy(new_emp.employee_name, name, sizeof(new_emp.employee_name) - 1);
    strncpy(new_emp.department, department, sizeof(new_emp.department) - 1);
    strncpy(new_emp.designation, designation, sizeof(new_emp.designation) - 1);
    strncpy(new_emp.email, email, sizeof(new_emp.email) - 1);
    strncpy(new_emp.mobile_number, mobile, sizeof(new_emp.mobile_number) - 1);
    strncpy(new_emp.joining_date, doa, sizeof(new_emp.joining_date) - 1);
    new_emp.base_salary = (float)salary;
    strncpy(new_emp.bank_account, bank_account, sizeof(new_emp.bank_account) - 1);
    strncpy(new_emp.status, "Active", sizeof(new_emp.status) - 1);
    
    // Call NEW API: db_add_employee(const Employee *)
    int result = db_add_employee(&new_emp);
    
    if (result > 0) {
        printf("[SUCCESS] Employee added with ID: %d\n", result);
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "✅ Employee Added Successfully!\nEmployee ID: %d\nName: %s\nDepartment: %s",
            result, name, department);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Clear form
        gtk_entry_set_text(GTK_ENTRY(emp_number_entry), "");
        gtk_entry_set_text(GTK_ENTRY(emp_name_entry), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(emp_designation_combo), 0);
        gtk_combo_box_set_active(GTK_COMBO_BOX(emp_department_combo), 0);
        gtk_combo_box_set_active(GTK_COMBO_BOX(emp_category_combo), 0);
        gtk_entry_set_text(GTK_ENTRY(emp_mobile_entry), "");
        gtk_entry_set_text(GTK_ENTRY(emp_email_entry), "");
        gtk_entry_set_text(GTK_ENTRY(emp_doa_entry), "");
        gtk_entry_set_text(GTK_ENTRY(emp_salary_entry), "");
        gtk_entry_set_text(GTK_ENTRY(emp_bank_account_entry), "");
        
        gtk_widget_hide(form_box);
        refresh_employee_list();
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "❌ Failed to add employee!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

/**
 * @brief Handle Refresh button click
 */
void on_refresh_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Refresh button clicked\n");
    refresh_employee_list();
}

/**
 * @brief Handle Search button click (SIMPLIFIED - uses refresh for now)
 */
void on_search_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Search clicked - showing all employees\n");
    refresh_employee_list();
}

/**
 * @brief Create Employee Management UI (UNCHANGED)
 */
void create_employee_ui(GtkWidget *container) {
    printf("[INFO] Creating Employee Management UI\n");
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(container), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    
    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='16' weight='bold'>👔 Employee Management</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);
    
    // Button Box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 5);
    
    GtkWidget *add_btn = gtk_button_new_with_label("➕ Add Employee");
    gtk_widget_set_size_request(add_btn, 150, 40);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_employee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), add_btn, FALSE, FALSE, 0);
    
    GtkWidget *refresh_btn = gtk_button_new_with_label("🔄 Refresh");
    gtk_widget_set_size_request(refresh_btn, 150, 40);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_employee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), refresh_btn, FALSE, FALSE, 0);
    
    GtkWidget *search_btn = gtk_button_new_with_label("🔍 Search");
    gtk_widget_set_size_request(search_btn, 150, 40);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_employee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), search_btn, FALSE, FALSE, 0);
    
    // ADD EMPLOYEE FORM (unchanged - kept for compatibility)
    form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 10);
    gtk_widget_set_name(form_box, "form_box");
    gtk_box_pack_start(GTK_BOX(main_box), form_box, FALSE, FALSE, 0);
    gtk_widget_hide(form_box);
    
    // Form grid (unchanged)
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    
    // Row 1: Employee Number and Name
    GtkWidget *emp_number_label = gtk_label_new("Employee Number:");
    gtk_widget_set_halign(emp_number_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), emp_number_label, 0, 1, 1, 1);
    emp_number_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_number_entry), "E001");
    gtk_grid_attach(GTK_GRID(grid), emp_number_entry, 1, 1, 1, 1);
    
    GtkWidget *name_label = gtk_label_new("Full Name:");
    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), name_label, 2, 1, 1, 1);
    emp_name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_name_entry), "John Doe");
    gtk_grid_attach(GTK_GRID(grid), emp_name_entry, 3, 1, 1, 1);
    
    // Row 2: Designation and Department
    GtkWidget *designation_label = gtk_label_new("Designation:");
    gtk_widget_set_halign(designation_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), designation_label, 0, 2, 1, 1);
    emp_designation_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_designation_combo), "Faculty");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_designation_combo), "HOD");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_designation_combo), "Security");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_designation_combo), "Housekeeping");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_designation_combo), "Other");
    gtk_combo_box_set_active(GTK_COMBO_BOX(emp_designation_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), emp_designation_combo, 1, 2, 1, 1);
    
    GtkWidget *department_label = gtk_label_new("Department:");
    gtk_widget_set_halign(department_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), department_label, 2, 2, 1, 1);
    emp_department_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_department_combo), "CSE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_department_combo), "EE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_department_combo), "CIVIL");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_department_combo), "ME");
    gtk_combo_box_set_active(GTK_COMBO_BOX(emp_department_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), emp_department_combo, 3, 2, 1, 1);
    
    // Row 3: Category and Mobile
    GtkWidget *category_label = gtk_label_new("Category:");
    gtk_widget_set_halign(category_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), category_label, 0, 3, 1,
