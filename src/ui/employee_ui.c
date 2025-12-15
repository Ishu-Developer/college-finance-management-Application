#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../include/employee_ui.h"
#include "../../include/database.h"
#include "../../include/validators.h"

// Global Variables
static GtkWidget *employee_table = NULL;
static GtkWidget *form_box = NULL;
static GtkWidget *main_box = NULL;
static GtkListStore *employee_store = NULL;

// Employee Details
static GtkWidget *emp_no_entry;
static GtkWidget *emp_name_entry;
static GtkWidget *emp_dob_entry;
static GtkWidget *department_combo;
static GtkWidget *designation_combo;
static GtkWidget *category_combo;
static GtkWidget *reporting_person_entry;
static GtkWidget *reporting_person_id_entry;
static GtkWidget *email_entry;
static GtkWidget *mobile_entry;
static GtkWidget *address_entry;
static GtkWidget *salary_entry;

// Bank Details
static GtkWidget *account_holder_entry;
static GtkWidget *account_number_entry;
static GtkWidget *bank_name_entry;
static GtkWidget *ifsc_code_entry;
static GtkWidget *bank_address_entry;

// Edit mode
static int editing_emp_id = -1;
static Employee current_emp;
static BankDetails current_bank;

void clear_employee_form(void) {
    gtk_entry_set_text(GTK_ENTRY(emp_no_entry), "");
    gtk_entry_set_text(GTK_ENTRY(emp_name_entry), "");
    gtk_entry_set_text(GTK_ENTRY(emp_dob_entry), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(department_combo), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(designation_combo), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 0);
    gtk_entry_set_text(GTK_ENTRY(reporting_person_entry), "");
    gtk_entry_set_text(GTK_ENTRY(reporting_person_id_entry), "");
    gtk_entry_set_text(GTK_ENTRY(email_entry), "");
    gtk_entry_set_text(GTK_ENTRY(mobile_entry), "");
    gtk_entry_set_text(GTK_ENTRY(address_entry), "");
    gtk_entry_set_text(GTK_ENTRY(salary_entry), "");
    
    gtk_entry_set_text(GTK_ENTRY(account_holder_entry), "");
    gtk_entry_set_text(GTK_ENTRY(account_number_entry), "");
    gtk_entry_set_text(GTK_ENTRY(bank_name_entry), "");
    gtk_entry_set_text(GTK_ENTRY(ifsc_code_entry), "");
    gtk_entry_set_text(GTK_ENTRY(bank_address_entry), "");
    
    editing_emp_id = -1;
}

void refresh_employee_list(void) {
    printf("[INFO] Refreshing employee table\n");
    
    if (!employee_store) return;
    gtk_list_store_clear(employee_store);
    
    sqlite3_stmt *stmt = NULL;
    if (db_get_all_employees(&stmt) == 0 || stmt == NULL) {
        printf("[WARNING] No employees found\n");
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Read all 14 columns from SELECT
        int emp_id = sqlite3_column_int(stmt, 0);
        int emp_no = sqlite3_column_int(stmt, 1);
        const char *emp_name = (const char *)sqlite3_column_text(stmt, 2);
        const char *emp_dob = (const char *)sqlite3_column_text(stmt, 3);
        const char *dept = (const char *)sqlite3_column_text(stmt, 4);
        const char *desig = (const char *)sqlite3_column_text(stmt, 5);
        const char *cat = (const char *)sqlite3_column_text(stmt, 6);
        const char *rep_name = (const char *)sqlite3_column_text(stmt, 7);
        int rep_id = sqlite3_column_int(stmt, 8);
        const char *email = (const char *)sqlite3_column_text(stmt, 9);
        const char *mobile = (const char *)sqlite3_column_text(stmt, 10);
        const char *addr = (const char *)sqlite3_column_text(stmt, 11);
        double salary = sqlite3_column_double(stmt, 12);
        const char *status = (const char *)sqlite3_column_text(stmt, 13);
        
        char emp_no_str[20], rep_id_str[20], salary_str[20];  
        snprintf(emp_no_str, sizeof(emp_no_str), "%d", emp_no);
        snprintf(rep_id_str, sizeof(rep_id_str), "%d", rep_id);
        snprintf(salary_str, sizeof(salary_str), "%.2f", salary);
        
        GtkTreeIter iter;
        gtk_list_store_append(employee_store, &iter);
        gtk_list_store_set(employee_store, &iter,
            0, "✏️",                    // Edit
            1, "🗑️",                    // Delete
            2, emp_id,                  // ID (hidden)
            3, emp_no_str,              
            4, emp_name ? emp_name : "—",
            5, emp_dob ? emp_dob : "—",
            6, dept ? dept : "—",
            7, desig ? desig : "—",
            8, cat ? cat : "—",
            9, rep_name ? rep_name : "—",
            10, rep_id_str,
            11, email ? email : "—",
            12, mobile ? mobile : "—",
            13, addr ? addr : "—",
            14, salary_str,
            15, status ? status : "—",
            -1);
        count++;
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] Loaded %d employees\n", count);
}

void on_add_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    if (gtk_widget_get_visible(form_box)) {
        gtk_widget_hide(form_box);
    } else {
        clear_employee_form();
        gtk_widget_show_all(form_box);
        gtk_widget_grab_focus(emp_no_entry);
    }
}

void on_save_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    // Collect employee data
    Employee emp = {0};
    emp.emp_no = atoi(gtk_entry_get_text(GTK_ENTRY(emp_no_entry)));
    strncpy(emp.emp_name, gtk_entry_get_text(GTK_ENTRY(emp_name_entry)), sizeof(emp.emp_name) - 1);
    strncpy(emp.emp_dob, gtk_entry_get_text(GTK_ENTRY(emp_dob_entry)), sizeof(emp.emp_dob) - 1);
    
    int dept_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(department_combo));
    const char *depts[] = {"CSE", "IT", "EE", "CIVIL", "ME"};
    strncpy(emp.department, depts[dept_idx < 5 ? dept_idx : 0], sizeof(emp.department) - 1);
    
    int desig_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(designation_combo));
    const char *desigs[] = {"Director", "HOD", "Professor", "Assistant Professor", "Guest Faculty"};
    strncpy(emp.designation, desigs[desig_idx < 5 ? desig_idx : 0], sizeof(emp.designation) - 1);
    
    int cat_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(category_combo));
    const char *cats[] = {"Faculty", "Staff", "Support", "Contractual"};
    strncpy(emp.category, cats[cat_idx < 4 ? cat_idx : 0], sizeof(emp.category) - 1);
    
    strncpy(emp.reporting_person_name, gtk_entry_get_text(GTK_ENTRY(reporting_person_entry)), sizeof(emp.reporting_person_name) - 1);
    emp.reporting_person_id = atoi(gtk_entry_get_text(GTK_ENTRY(reporting_person_id_entry)));
    strncpy(emp.email, gtk_entry_get_text(GTK_ENTRY(email_entry)), sizeof(emp.email) - 1);
    strncpy(emp.mobile_number, gtk_entry_get_text(GTK_ENTRY(mobile_entry)), sizeof(emp.mobile_number) - 1);
    strncpy(emp.address, gtk_entry_get_text(GTK_ENTRY(address_entry)), sizeof(emp.address) - 1);
    emp.base_salary = atof(gtk_entry_get_text(GTK_ENTRY(salary_entry)));
    strncpy(emp.status, "Active", sizeof(emp.status) - 1);
    
    // Collect bank data
    BankDetails bank = {0};
    strncpy(bank.account_holder_name, gtk_entry_get_text(GTK_ENTRY(account_holder_entry)), sizeof(bank.account_holder_name) - 1);
    strncpy(bank.account_number, gtk_entry_get_text(GTK_ENTRY(account_number_entry)), sizeof(bank.account_number) - 1);
    strncpy(bank.bank_name, gtk_entry_get_text(GTK_ENTRY(bank_name_entry)), sizeof(bank.bank_name) - 1);
    strncpy(bank.ifsc_code, gtk_entry_get_text(GTK_ENTRY(ifsc_code_entry)), sizeof(bank.ifsc_code) - 1);
    strncpy(bank.bank_address, gtk_entry_get_text(GTK_ENTRY(bank_address_entry)), sizeof(bank.bank_address) - 1);
    
    // Validate
    if (emp.emp_no <= 0) {
        GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "❌ Employee Number required and must be > 0");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        return;
    }
    
    if (!validate_name(emp.emp_name)) {
        GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "❌ Invalid employee name");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        return;
    }
    
    if (!validate_mobile(emp.mobile_number)) {
        GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "❌ Mobile must be 10 digits");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        return;
    }
    
    if (strlen(emp.email) > 0 && !validate_email(emp.email)) {
        GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "❌ Invalid email");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        return;
    }
    
    // Save
    int emp_id;
    if (editing_emp_id > 0) {
        emp.emp_id = editing_emp_id;
        int result = db_update_employee(editing_emp_id, &emp);
        emp_id = result;
        if (result > 0 && strlen(bank.account_number) > 0) {
            bank.emp_id = editing_emp_id;
            db_update_bank_details(editing_emp_id, &bank);
        }
    } else {
        emp_id = db_add_employee(&emp);
        if (emp_id > 0 && strlen(bank.account_number) > 0) {
            bank.emp_id = emp_id;
            db_add_bank_details(&bank);
        }
    }
    
    if (emp_id > 0) {
        GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "✅ Saved!\nID: %d\nName: %s", emp_id, emp.emp_name);
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        
        clear_employee_form();
        gtk_widget_hide(form_box);
        refresh_employee_list();
    } else {
        GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "❌ Failed to save");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
    }
}

void on_search_employee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    // Inline search bar appears below buttons
    static GtkWidget *search_box = NULL;
    
    if (search_box && gtk_widget_get_visible(search_box)) {
        gtk_widget_hide(search_box);
        refresh_employee_list();  // Show all
        return;
    }
    
    if (!search_box) {
        search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(search_box), 10);
        
        GtkWidget *search_label = gtk_label_new("🔍 Search by Name/Emp#:");
        GtkWidget *search_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter name or emp number...");
        
        GtkWidget *search_do_btn = gtk_button_new_with_label("Find");
        GtkWidget *search_clear_btn = gtk_button_new_with_label("Clear");
        
        gtk_box_pack_start(GTK_BOX(search_box), search_label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(search_box), search_entry, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(search_box), search_do_btn, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(search_box), search_clear_btn, FALSE, FALSE, 0);
        
        gtk_box_pack_start(GTK_BOX(main_box), search_box, FALSE, FALSE, 0);
    }
    
    gtk_widget_show_all(search_box);
}

// Edit button callback
void on_edit_employee_clicked(GtkCellRenderer *renderer, gchar *path_str, gpointer user_data) {
    (void)renderer;
    (void)user_data;
    
    GtkTreeIter iter;
    if (!gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(employee_store), &iter, path_str)) {
        return;
    }
    
    int emp_id;
    gtk_tree_model_get(GTK_TREE_MODEL(employee_store), &iter, 2, &emp_id, -1);
    
    if (db_get_employee_by_id(emp_id, &current_emp) <= 0) {
        printf("[ERROR] Could not load employee %d\n", emp_id);
        return;
    }
    
    // Load bank details
    db_get_bank_details(emp_id, &current_bank);
    
    // Populate form
    char emp_no_str[20];
    snprintf(emp_no_str, sizeof(emp_no_str), "%d", current_emp.emp_no);
    gtk_entry_set_text(GTK_ENTRY(emp_no_entry), emp_no_str);
    gtk_entry_set_text(GTK_ENTRY(emp_name_entry), current_emp.emp_name);
    gtk_entry_set_text(GTK_ENTRY(emp_dob_entry), current_emp.emp_dob);
    gtk_entry_set_text(GTK_ENTRY(address_entry), current_emp.address);
    gtk_entry_set_text(GTK_ENTRY(email_entry), current_emp.email);
    gtk_entry_set_text(GTK_ENTRY(mobile_entry), current_emp.mobile_number);
    gtk_entry_set_text(GTK_ENTRY(reporting_person_entry), current_emp.reporting_person_name);
    
    char rep_id_str[20];
    snprintf(rep_id_str, sizeof(rep_id_str), "%d", current_emp.reporting_person_id);
    gtk_entry_set_text(GTK_ENTRY(reporting_person_id_entry), rep_id_str);
    
    char salary_str[20];
    snprintf(salary_str, sizeof(salary_str), "%.2f", current_emp.base_salary);
    gtk_entry_set_text(GTK_ENTRY(salary_entry), salary_str);
    
    gtk_entry_set_text(GTK_ENTRY(account_holder_entry), current_bank.account_holder_name);
    gtk_entry_set_text(GTK_ENTRY(account_number_entry), current_bank.account_number);
    gtk_entry_set_text(GTK_ENTRY(bank_name_entry), current_bank.bank_name);
    gtk_entry_set_text(GTK_ENTRY(ifsc_code_entry), current_bank.ifsc_code);
    gtk_entry_set_text(GTK_ENTRY(bank_address_entry), current_bank.bank_address);
    
    editing_emp_id = emp_id;
    gtk_widget_show_all(form_box);
    printf("[INFO] Editing employee %d\n", emp_id);
}

// Delete button callback
void on_delete_employee_clicked(GtkCellRenderer *renderer, gchar *path_str, gpointer user_data) {
    (void)renderer;
    (void)user_data;
    
    GtkTreeIter iter;
    if (!gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(employee_store), &iter, path_str)) {
        return;
    }
    
    int emp_id;
    gtk_tree_model_get(GTK_TREE_MODEL(employee_store), &iter, 2, &emp_id, -1);
    
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
        "Delete employee %d?", emp_id);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
        if (db_delete_employee(emp_id) > 0) {
            GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                "✅ Deleted!");
            gtk_dialog_run(GTK_DIALOG(d));
            gtk_widget_destroy(d);
            refresh_employee_list();
        }
    }
    gtk_widget_destroy(dialog);
}

void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer user_data) {
    (void)user_data;
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    
    if (!gtk_tree_model_get_iter(model, &iter, path)) return;
    
    int col_num = gtk_tree_view_column_get_sort_column_id(col);
    int emp_id;
    gtk_tree_model_get(model, &iter, 2, &emp_id, -1);
    
    if (col_num == 0) {
        // Edit column clicked - load employee into form
        if (db_get_employee_by_id(emp_id, &current_emp) <= 0) {
            printf("[ERROR] Could not load employee %d\n", emp_id);
            return;
        }
        db_get_bank_details(emp_id, &current_bank);
        
        // Populate form...
        char emp_no_str[20];
        snprintf(emp_no_str, sizeof(emp_no_str), "%d", current_emp.emp_no);
        gtk_entry_set_text(GTK_ENTRY(emp_no_entry), emp_no_str);
        gtk_entry_set_text(GTK_ENTRY(emp_name_entry), current_emp.emp_name);
        gtk_entry_set_text(GTK_ENTRY(emp_dob_entry), current_emp.emp_dob);
        gtk_entry_set_text(GTK_ENTRY(address_entry), current_emp.address);
        gtk_entry_set_text(GTK_ENTRY(email_entry), current_emp.email);
        gtk_entry_set_text(GTK_ENTRY(mobile_entry), current_emp.mobile_number);
        gtk_entry_set_text(GTK_ENTRY(reporting_person_entry), current_emp.reporting_person_name);
        
        char rep_id_str[20];
        snprintf(rep_id_str, sizeof(rep_id_str), "%d", current_emp.reporting_person_id);
        gtk_entry_set_text(GTK_ENTRY(reporting_person_id_entry), rep_id_str);
        
        char salary_str[20];
        snprintf(salary_str, sizeof(salary_str), "%.2f", current_emp.base_salary);
        gtk_entry_set_text(GTK_ENTRY(salary_entry), salary_str);
        
        gtk_entry_set_text(GTK_ENTRY(account_holder_entry), current_bank.account_holder_name);
        gtk_entry_set_text(GTK_ENTRY(account_number_entry), current_bank.account_number);
        gtk_entry_set_text(GTK_ENTRY(bank_name_entry), current_bank.bank_name);
        gtk_entry_set_text(GTK_ENTRY(ifsc_code_entry), current_bank.ifsc_code);
        gtk_entry_set_text(GTK_ENTRY(bank_address_entry), current_bank.bank_address);
        
        editing_emp_id = emp_id;
        gtk_widget_show_all(form_box);
        printf("[INFO] Editing employee %d\n", emp_id);
        
    } else if (col_num == 1) {
        // Delete column clicked
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
            "Delete employee %d?", emp_id);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
            if (db_delete_employee(emp_id) > 0) {
                GtkWidget *d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                    "✅ Deleted!");
                gtk_dialog_run(GTK_DIALOG(d));
                gtk_widget_destroy(d);
                refresh_employee_list();
            }
        }
        gtk_widget_destroy(dialog);
    }
}

void setup_employee_table_styling(GtkTreeView *tree_view) {
    gtk_tree_view_set_grid_lines(tree_view, GTK_TREE_VIEW_GRID_LINES_BOTH);
}

void create_employee_ui(GtkWidget *container) {
    printf("[INFO] Creating Employee Management UI\n");
    
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(container), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    
    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span font='18' weight='bold'>👔 Employee Management</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);
    
    // Button box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    
    GtkWidget *add_btn = gtk_button_new_with_label("➕ Add");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_employee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), add_btn, FALSE, FALSE, 0);

    GtkWidget *refresh_btn = gtk_button_new_with_label("🔄 Refresh");
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(refresh_employee_list), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), refresh_btn, FALSE, FALSE, 0);

    GtkWidget *search_btn = gtk_button_new_with_label("🔍 Search");
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_employee_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(button_box), search_btn, FALSE, FALSE, 0);
    
    // ============================================
    // FORM SECTION (HIDDEN)
    // ============================================
    form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 10);
    gtk_box_pack_start(GTK_BOX(main_box), form_box, FALSE, FALSE, 0);
    gtk_widget_hide(form_box);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_box_pack_start(GTK_BOX(form_box), grid, FALSE, FALSE, 0);
    
    // Row 1: Emp#, Name, DOB
    GtkWidget *l;
    l = gtk_label_new("Emp#:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, 0, 1, 1);
    emp_no_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), emp_no_entry, 1, 0, 1, 1);
    
    l = gtk_label_new("Name:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 2, 0, 1, 1);
    emp_name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), emp_name_entry, 3, 0, 1, 1);
    
    l = gtk_label_new("DOB:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 4, 0, 1, 1);
    emp_dob_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emp_dob_entry), "DD-MM-YYYY");
    gtk_grid_attach(GTK_GRID(grid), emp_dob_entry, 5, 0, 1, 1);
    
    // Row 2: Department, Designation, Category
    l = gtk_label_new("Department:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, 1, 1, 1);
    department_combo = gtk_combo_box_text_new();
    const char *depts[] = {"CSE", "IT", "EE", "CIVIL", "ME", NULL};
    for (int i = 0; depts[i]; i++)
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(department_combo), depts[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(department_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), department_combo, 1, 1, 1, 1);
    
    l = gtk_label_new("Designation:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 2, 1, 1, 1);
    designation_combo = gtk_combo_box_text_new();
    const char *desigs[] = {"Director", "HOD", "Asst Prof", "Guest Faculty", NULL};
    for (int i = 0; desigs[i]; i++)
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(designation_combo), desigs[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(designation_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), designation_combo, 3, 1, 1, 1);
    
    l = gtk_label_new("Category:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 4, 1, 1, 1);
    category_combo = gtk_combo_box_text_new();
    const char *cats[] = {"Faculty", "Staff", "Support", "Contractual", NULL}; 
    for (int i = 0; cats[i]; i++)
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), cats[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), category_combo, 5, 1, 1, 1);
    
    // Row 3: Reporting Person, Reporting Person ID
    l = gtk_label_new("Reporting Person:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, 2, 1, 1);
    reporting_person_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), reporting_person_entry, 1, 2, 2, 1);
    
    l = gtk_label_new("Reporting ID:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 3, 2, 1, 1);
    reporting_person_id_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), reporting_person_id_entry, 4, 2, 2, 1);
    
    // Row 4: Email, Mobile, Address
    l = gtk_label_new("Email:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, 3, 1, 1);
    email_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), email_entry, 1, 3, 1, 1);
    
    l = gtk_label_new("Mobile:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 2, 3, 1, 1);
    mobile_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(mobile_entry), "10 digits");
    gtk_grid_attach(GTK_GRID(grid), mobile_entry, 3, 3, 1, 1);
    
    l = gtk_label_new("Address:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 4, 3, 1, 1);
    address_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), address_entry, 5, 3, 1, 1);
    
    // Row 5: Salary
    l = gtk_label_new("Base Salary (₹):"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, 4, 1, 1);
    salary_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), salary_entry, 1, 4, 1, 1);
    
    // Separator
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(grid), sep, 0, 5, 6, 1);
    
    // Bank Details Section
    GtkWidget *bank_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(bank_label), "<b>💳 Bank Details</b>");
    gtk_grid_attach(GTK_GRID(grid), bank_label, 0, 6, 2, 1);
    
    // Row 7: Account Holder, Account Number
    l = gtk_label_new("Account Holder:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, 7, 1, 1);
    account_holder_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), account_holder_entry, 1, 7, 2, 1);
    
    l = gtk_label_new("Account Number:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 3, 7, 1, 1);
    account_number_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), account_number_entry, 4, 7, 2, 1);
    
    // Row 8: Bank Name, IFSC, Bank Address
    l = gtk_label_new("Bank Name:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, 8, 1, 1);
    bank_name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), bank_name_entry, 1, 8, 1, 1);
    
    l = gtk_label_new("IFSC Code:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 2, 8, 1, 1);
    ifsc_code_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), ifsc_code_entry, 3, 8, 1, 1);
    
    l = gtk_label_new("Bank Address:"); gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 4, 8, 1, 1);
    bank_address_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), bank_address_entry, 5, 8, 1, 1);
    
    // Save button
    GtkWidget *save_btn = gtk_button_new_with_label("💾 Save");
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_employee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(form_box), save_btn, FALSE, FALSE, 0);
    
    // ============================================
    // TABLE SECTION
    // ============================================
    employee_store = gtk_list_store_new(16,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    
    employee_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(employee_store));
    g_object_unref(employee_store);

    setup_employee_table_styling(GTK_TREE_VIEW(employee_table));
    
    // Create renderers with padding
    GtkCellRenderer *text_renderer = gtk_cell_renderer_text_new();
    g_object_set(text_renderer, "xpad", 8, "ypad", 6, NULL);  // ✅ Cell padding
    
    // Column titles
    const char *titles[] = {"✏️", "🗑️", "Emp No.", "Name", "DOB", "Dept", "Desig", "Cat", "Rep Name", "Rep ID", "Email", "Mobile", "Address", "Salary", "Status"};
    
    // Add columns with proper sizing
    for (int i = 0; i < 15; i++) {
        GtkTreeViewColumn *col;
        
        if (i < 2) {
            // Action columns (Edit/Delete) - narrow
            col = gtk_tree_view_column_new_with_attributes(titles[i], text_renderer, "text", i, NULL);
            gtk_tree_view_column_set_fixed_width(col, 40);
            gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
            gtk_tree_view_column_set_alignment(col, 0.5);  // ✅ Center align icons
        } else {
            // Data columns
            col = gtk_tree_view_column_new_with_attributes(titles[i], text_renderer, "text", i + 1, NULL);
            gtk_tree_view_column_set_min_width(col, 80);
            gtk_tree_view_column_set_resizable(col, TRUE);  // ✅ Resizable
        }
        
        gtk_tree_view_append_column(GTK_TREE_VIEW(employee_table), col);
    }

    // Connect signal ONCE
    g_signal_connect(employee_table, "row-activated", G_CALLBACK(on_row_activated), NULL);
    
    // Scrolled window
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll, -1, 100);  // ✅ Min height 300px
    gtk_container_add(GTK_CONTAINER(scroll), employee_table);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);
    
    // Show UI
    gtk_widget_show_all(main_box);
    gtk_widget_hide(form_box);
    refresh_employee_list();
    printf("[SUCCESS] Employee UI created\n");
}
