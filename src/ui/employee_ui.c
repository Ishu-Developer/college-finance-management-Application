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
 * @brief Refresh employee table from database
 */
void refresh_employee_list() {
    printf("[INFO] Refreshing employee table\n");
    
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(employee_table)));
    if (store) {
        gtk_list_store_clear(store);
    }
    
    sqlite3_stmt *stmt = db_get_all_employees();
    if (stmt == NULL) {
        printf("[WARNING] No employees found in database\n");
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int emp_id = sqlite3_column_int(stmt, 0);
        const char *emp_number = (const char *)sqlite3_column_text(stmt, 1);
        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        const char *designation = (const char *)sqlite3_column_text(stmt, 3);
        const char *department = (const char *)sqlite3_column_text(stmt, 4);
        const char *category = (const char *)sqlite3_column_text(stmt, 5);
        const char *email = (const char *)sqlite3_column_text(stmt, 6);
        const char *mobile = (const char *)sqlite3_column_text(stmt, 7);
        const char *doa = (const char *)sqlite3_column_text(stmt, 8);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️",              // Edit
            1, "🗑️",              // Delete
            2, emp_id,            // Employee ID (hidden)
            3, emp_number,        // Employee Number
            4, name,              // Name
            5, designation,       // Designation
            6, department,        // Department
            7, category,          // Category
            8, email,             // Email
            9, mobile,            // Mobile
            10, doa,              // DOA
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
 * @brief Handle Save Employee button
 */
void on_save_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
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
    
    // Call db_add_employee with CORRECT 10 parameters
    int result = db_add_employee(emp_no, name, designation, department, 
                                category, email, mobile, doa, salary, bank_account);
    
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
 * @brief Handle Search button click
 */
void on_search_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "🔍 Search Employee",
        NULL,
        GTK_DIALOG_MODAL,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Search", GTK_RESPONSE_OK,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 150);
    
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(content_area), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 15);
    
    GtkWidget *label = gtk_label_new("Search by Name, Employee Number, or Department:");
    gtk_box_pack_start(GTK_BOX(main_box), label, FALSE, FALSE, 0);
    
    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter search term");
    gtk_box_pack_start(GTK_BOX(main_box), search_entry, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *search_text = gtk_entry_get_text(GTK_ENTRY(search_entry));
        if (strlen(search_text) > 0) {
            printf("[INFO] Searching for: %s\n", search_text);
            
            GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(employee_table)));
            gtk_list_store_clear(store);
            
            sqlite3_stmt *stmt = db_search_employee(search_text);
            if (stmt == NULL) {
                printf("[WARNING] No employees found matching: %s\n", search_text);
                GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                    "No employees found for: %s", search_text);
                gtk_dialog_run(GTK_DIALOG(msg));
                gtk_widget_destroy(msg);
                refresh_employee_list();
                gtk_widget_destroy(dialog);
                return;
            }
            
            int count = 0;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int emp_id = sqlite3_column_int(stmt, 0);
                const char *emp_number = (const char *)sqlite3_column_text(stmt, 1);
                const char *name = (const char *)sqlite3_column_text(stmt, 2);
                const char *designation = (const char *)sqlite3_column_text(stmt, 3);
                const char *department = (const char *)sqlite3_column_text(stmt, 4);
                const char *category = (const char *)sqlite3_column_text(stmt, 5);
                const char *email = (const char *)sqlite3_column_text(stmt, 6);
                const char *mobile = (const char *)sqlite3_column_text(stmt, 7);
                const char *doa = (const char *)sqlite3_column_text(stmt, 8);
                
                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                    0, "✏️",
                    1, "🗑️",
                    2, emp_id,
                    3, emp_number,
                    4, name,
                    5, designation,
                    6, department,
                    7, category,
                    8, email,
                    9, mobile,
                    10, doa,
                    -1);
                count++;
            }
            
            sqlite3_finalize(stmt);
            printf("[INFO] Found %d employees matching: %s\n", count, search_text);
        }
    }
    
    gtk_widget_destroy(dialog);
}

/**
 * @brief Create Employee Management UI
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
    
    // ====================================================================
    // ADD EMPLOYEE FORM
    // ====================================================================
    
    form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 10);
    gtk_widget_set_name(form_box, "form_box");
    gtk_box_pack_start(GTK_BOX(main_box), form_box, FALSE, FALSE, 0);
    gtk_widget_hide(form_box);
    
    GtkWidget *form_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(form_title),
        "<span font='12' weight='bold'>Add New Employee</span>");
    gtk_box_pack_start(GTK_BOX(form_box), form_title, FALSE, FALSE, 0);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_box_pack_start(GTK_BOX(form_box), grid, FALSE, FALSE, 0);
    
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
    gtk_grid_attach(GTK_GRID(grid), category_label, 0, 3, 1, 1);
    emp_category_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_category_combo), "Faculty");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_category_combo), "Staff");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(emp_category_combo), "Support");
    gtk_combo_box_set_active(GTK_COMBO_BOX(emp_category_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), emp_category_combo, 1, 3, 1, 1);
    
    GtkWidget *mobile_label = gtk_label_new("Mobile:");
    gtk_widget_set_halign(mobile_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), mobile_label, 2, 3, 1, 1);
    emp_mobile_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_mobile_entry), "10 digits");
    gtk_grid_attach(GTK_GRID(grid), emp_mobile_entry, 3, 3, 1, 1);
    
    // Row 4: Email and DOA
    GtkWidget *email_label = gtk_label_new("Email:");
    gtk_widget_set_halign(email_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), email_label, 0, 4, 1, 1);
    emp_email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_email_entry), "name@ldah.edu.in");
    gtk_grid_attach(GTK_GRID(grid), emp_email_entry, 1, 4, 1, 1);
    
    GtkWidget *doa_label = gtk_label_new("Date of Appointment:");
    gtk_widget_set_halign(doa_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), doa_label, 2, 4, 1, 1);
    emp_doa_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_doa_entry), "YYYY-MM-DD");
    gtk_grid_attach(GTK_GRID(grid), emp_doa_entry, 3, 4, 1, 1);
    
    // Row 5: Salary and Bank Account
    GtkWidget *salary_label = gtk_label_new("Salary:");
    gtk_widget_set_halign(salary_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), salary_label, 0, 5, 1, 1);
    emp_salary_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_salary_entry), "50000.00");
    gtk_grid_attach(GTK_GRID(grid), emp_salary_entry, 1, 5, 1, 1);
    
    GtkWidget *bank_account_label = gtk_label_new("Bank Account:");
    gtk_widget_set_halign(bank_account_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), bank_account_label, 2, 5, 1, 1);
    emp_bank_account_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_bank_account_entry), "1234567890");
    gtk_grid_attach(GTK_GRID(grid), emp_bank_account_entry, 3, 5, 1, 1);
    
    // Form Buttons
    GtkWidget *form_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(form_box), form_button_box, FALSE, FALSE, 5);
    
    GtkWidget *save_btn = gtk_button_new_with_label("💾 Save Employee");
    gtk_widget_set_size_request(save_btn, 150, 35);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_employee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(form_button_box), save_btn, FALSE, FALSE, 0);
    
    // ====================================================================
    // EMPLOYEE TABLE
    // ====================================================================
    
    GtkWidget *table_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(table_label), "<span weight='bold'>📋 Employees List</span>");
    gtk_widget_set_halign(table_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(main_box), table_label, FALSE, FALSE, 5);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);
    
    // Create list store (11 columns)
    GtkListStore *store = gtk_list_store_new(11,
        G_TYPE_STRING,  // 0: Edit
        G_TYPE_STRING,  // 1: Delete
        G_TYPE_INT,     // 2: Employee ID
        G_TYPE_STRING,  // 3: Emp Number
        G_TYPE_STRING,  // 4: Name
        G_TYPE_STRING,  // 5: Designation
        G_TYPE_STRING,  // 6: Department
        G_TYPE_STRING,  // 7: Category
        G_TYPE_STRING,  // 8: Email
        G_TYPE_STRING,  // 9: Mobile
        G_TYPE_STRING   // 10: DOA
    );
    
    employee_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(G_OBJECT(store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(employee_table), GTK_TREE_VIEW_GRID_LINES_BOTH);
    gtk_container_add(GTK_CONTAINER(scroll), employee_table);
    
    // Add columns with fixed widths
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col;
    
    col = gtk_tree_view_column_new_with_attributes("✏️", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_fixed_width(col, 40);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("🗑️", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_fixed_width(col, 40);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Emp#", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_fixed_width(col, 70);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 4, NULL);
    gtk_tree_view_column_set_fixed_width(col, 120);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Designation", renderer, "text", 5, NULL);
    gtk_tree_view_column_set_fixed_width(col, 100);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Department", renderer, "text", 6, NULL);
    gtk_tree_view_column_set_fixed_width(col, 90);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Category", renderer, "text", 7, NULL);
    gtk_tree_view_column_set_fixed_width(col, 80);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Email", renderer, "text", 8, NULL);
    gtk_tree_view_column_set_fixed_width(col, 130);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Mobile", renderer, "text", 9, NULL);
    gtk_tree_view_column_set_fixed_width(col, 110);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("DOA", renderer, "text", 10, NULL);
    gtk_tree_view_column_set_fixed_width(col, 100);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    
    gtk_widget_show_all(container);
    gtk_widget_hide(form_box);
    printf("[INFO] Employee Management UI created successfully\n");
    
    // Load employees
    refresh_employee_list();
}