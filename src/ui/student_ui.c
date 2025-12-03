#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "../../include/student_ui.h"
#include "../../include/database.h"
#include "../../include/validators.h"

// Global variables
static GtkWidget *student_table = NULL;
static GtkWidget *form_box = NULL;
static GtkWidget *roll_no_entry;
static GtkWidget *name_entry;
static GtkWidget *father_name_entry;
static GtkWidget *branch_combo;
static GtkWidget *semester_combo;
static GtkWidget *gender_combo;
static GtkWidget *mobile_entry;
static GtkWidget *email_entry;
static GtkWidget *photo_label;
static char photo_path[500] = "";

void refresh_student_table() {
    printf("[INFO] Refreshing student table\n");
    
    // Clear existing rows
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(student_table)));
    if (store) {
        gtk_list_store_clear(store);
    }
    
    sqlite3_stmt *stmt = db_get_all_students();
    if (stmt == NULL) {
        printf("[WARNING] No students found in database\n");
        
        // Add demo data if no students
        printf("[INFO] Adding demo student data\n");
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️",           // Edit button
            1, "🗑️",           // Delete button
            2, "📷",           // Photo
            3, "Raj Kumar",    // Name
            4, "Sharma",       // Father Name
            5, "3562426782",   // Roll No
            6, "CSE",          // Branch
            7, 4,              // Semester (as number)
            8, "Male",         // Gender
            9, "raj@mail.com", // Email
            10, "9876543210",  // Mobile
            -1);
        printf("[INFO] Demo data loaded\n");
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // ✅ Read all columns from database
        int student_id = sqlite3_column_int(stmt, 0);
        const char *roll_no = (const char *)sqlite3_column_text(stmt, 1);
        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        const char *branch = (const char *)sqlite3_column_text(stmt, 3);
        int semester = sqlite3_column_int(stmt, 4);
        
        // Use student_id for logging (prevents unused variable warning)
        printf("[DEBUG] Loading student ID: %d\n", student_id);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️",           // Edit button
            1, "🗑️",           // Delete button
            2, "📷",           // Photo
            3, name,           // Name
            4, "—",            // Father Name (TODO: add to schema)
            5, roll_no,        // Roll No
            6, branch,         // Branch
            7, semester,       // Semester
            8, "—",            // Gender (TODO: add to schema)
            9, "—",            // Email (TODO: add to schema)
            10, "—",           // Mobile (TODO: add to schema)
            -1);
        count++;
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] Loaded %d students from database\n", count);
}

void on_select_photo_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Photo select button clicked\n");
    
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Student Photo",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Select", GTK_RESPONSE_ACCEPT,
        NULL);
    
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), ".");
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Image Files");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.bmp");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        if (filename) {
            strncpy(photo_path, filename, sizeof(photo_path) - 1);
            photo_path[sizeof(photo_path) - 1] = '\0';
            
            printf("[INFO] Photo selected: %s\n", photo_path);
            
            // ✅ FIXED: Increased buffer size and proper truncation
            char display_text[256];
            const char *filename_only = strrchr(photo_path, '/');
            if (!filename_only) filename_only = strrchr(photo_path, '\\');
            if (filename_only) filename_only++;
            else filename_only = photo_path;
            
            snprintf(display_text, sizeof(display_text), "📷 %s", filename_only);
            gtk_label_set_text(GTK_LABEL(photo_label), display_text);
            
            g_free(filename);
        }
    }
    
    gtk_widget_destroy(dialog);
}

void on_add_student_save_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    const char *roll_no = gtk_entry_get_text(GTK_ENTRY(roll_no_entry));
    const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
    const char *father_name = gtk_entry_get_text(GTK_ENTRY(father_name_entry));
    const char *mobile = gtk_entry_get_text(GTK_ENTRY(mobile_entry));
    const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
    
    int semester = gtk_combo_box_get_active(GTK_COMBO_BOX(semester_combo)) + 1;
    
    int gender_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(gender_combo));
    const char *gender = (gender_idx == 0) ? "Male" : (gender_idx == 1) ? "Female" : "Other";
    
    int branch_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(branch_combo));
    const char *branch = (branch_idx == 0) ? "CSE" : 
                        (branch_idx == 1) ? "EE" : 
                        (branch_idx == 2) ? "CIVIL" : 
                        (branch_idx == 3) ? "ME" : "IT";
    
    printf("[INFO] Add Student - Validating input\n");
    printf("[DEBUG] Name: %s, Roll: %s, Branch: %s, Gender: %s, Father: %s\n", 
           name, roll_no, branch, gender, father_name);
    
    // Validation
    if (strlen(roll_no) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "❌ Roll Number cannot be empty!");
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
    
    printf("[INFO] All validations passed - Adding student\n");
    
    int result = db_add_student(roll_no, name, branch, semester, mobile, email);
    
    if (result > 0) {
        printf("[SUCCESS] Student added with ID: %d\n", result);
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "✅ Student Added Successfully!\nStudent ID: %d\nName: %s\nGender: %s\nRoll: %s", 
            result, name, gender, roll_no);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Clear form fields
        gtk_entry_set_text(GTK_ENTRY(roll_no_entry), "");
        gtk_entry_set_text(GTK_ENTRY(name_entry), "");
        gtk_entry_set_text(GTK_ENTRY(father_name_entry), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(branch_combo), 0);
        gtk_entry_set_text(GTK_ENTRY(mobile_entry), "");
        gtk_entry_set_text(GTK_ENTRY(email_entry), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(semester_combo), 0);
        gtk_combo_box_set_active(GTK_COMBO_BOX(gender_combo), 0);
        gtk_label_set_text(GTK_LABEL(photo_label), "📷 No photo selected");
        strcpy(photo_path, "");
        
        refresh_student_table();
    } else {
        printf("[ERROR] Failed to add student\n");
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "❌ Failed to add student!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void on_add_student_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Add Student button clicked\n");
    
    if (gtk_widget_get_visible(form_box)) {
        printf("[INFO] Hiding form\n");
        gtk_widget_hide(form_box);
    } else {
        printf("[INFO] Showing form\n");
        gtk_widget_show_all(form_box);
        gtk_widget_grab_focus(roll_no_entry);
    }
}

void on_view_students_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Refreshing student list\n");
    refresh_student_table();
}

void on_search_student_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "🔍 Search Student",
        NULL,
        GTK_DIALOG_MODAL,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Search", GTK_RESPONSE_OK,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 120);
    
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(content_area), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 15);
    
    GtkWidget *label = gtk_label_new("Search by Roll Number:");
    gtk_box_pack_start(GTK_BOX(main_box), label, FALSE, FALSE, 0);
    
    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter roll number");
    gtk_box_pack_start(GTK_BOX(main_box), search_entry, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *search_text = gtk_entry_get_text(GTK_ENTRY(search_entry));
        if (strlen(search_text) > 0) {
            printf("[INFO] Searching for: %s\n", search_text);
            GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                "Search functionality coming soon...\nSearching for: %s", search_text);
            gtk_dialog_run(GTK_DIALOG(msg));
            gtk_widget_destroy(msg);
        }
    }
    
    gtk_widget_destroy(dialog);
}

void create_student_ui(GtkWidget *container) {
    printf("[INFO] Creating Student Management UI with full table layout\n");
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(container), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    
    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='16' weight='bold'>👨‍🎓 Student Management</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);
    
    // Button Box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 5);
    
    GtkWidget *add_btn = gtk_button_new_with_label("➕ Add Student");
    gtk_widget_set_size_request(add_btn, 150, 40);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_student_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), add_btn, FALSE, FALSE, 0);
    
    GtkWidget *refresh_btn = gtk_button_new_with_label("🔄 Refresh");
    gtk_widget_set_size_request(refresh_btn, 150, 40);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_view_students_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), refresh_btn, FALSE, FALSE, 0);
    
    GtkWidget *search_btn = gtk_button_new_with_label("🔍 Search");
    gtk_widget_set_size_request(search_btn, 150, 40);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_student_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), search_btn, FALSE, FALSE, 0);
    
    // ====================================================================
    // INLINE ADD STUDENT FORM (Hidden by default)
    // ====================================================================
    
    form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 10);
    gtk_widget_set_name(form_box, "form_box");
    gtk_box_pack_start(GTK_BOX(main_box), form_box, FALSE, FALSE, 0);
    gtk_widget_hide(form_box);
    
    GtkWidget *form_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(form_title),
        "<span font='12' weight='bold'>Add New Student</span>");
    gtk_box_pack_start(GTK_BOX(form_box), form_title, FALSE, FALSE, 0);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_box_pack_start(GTK_BOX(form_box), grid, FALSE, FALSE, 0);
    
    // Row 0: Photo Upload
    GtkWidget *photo_btn = gtk_button_new_with_label("📷 Select Photo");
    gtk_grid_attach(GTK_GRID(grid), photo_btn, 0, 0, 2, 1);
    g_signal_connect(photo_btn, "clicked", G_CALLBACK(on_select_photo_clicked), NULL);
    
    photo_label = gtk_label_new("📷 No photo selected");
    gtk_widget_set_halign(photo_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), photo_label, 2, 0, 2, 1);
    
    // Row 1: Roll No and Name
    GtkWidget *roll_label = gtk_label_new("Roll Number:");
    gtk_widget_set_halign(roll_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), roll_label, 0, 1, 1, 1);
    roll_no_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(roll_no_entry), "Roll Number");
    gtk_grid_attach(GTK_GRID(grid), roll_no_entry, 1, 1, 1, 1);
    
    GtkWidget *name_label = gtk_label_new("Name:");
    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), name_label, 2, 1, 1, 1);
    name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Student Name");
    gtk_grid_attach(GTK_GRID(grid), name_entry, 3, 1, 1, 1);
    
    // Row 2: Father Name and Gender
    GtkWidget *father_label = gtk_label_new("Father Name:");
    gtk_widget_set_halign(father_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), father_label, 0, 2, 1, 1);
    father_name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(father_name_entry), "Father's Name");
    gtk_grid_attach(GTK_GRID(grid), father_name_entry, 1, 2, 1, 1);
    
    GtkWidget *gender_label = gtk_label_new("Gender:");
    gtk_widget_set_halign(gender_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), gender_label, 2, 2, 1, 1);
    gender_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gender_combo), "Male");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gender_combo), "Female");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gender_combo), "Other");
    gtk_combo_box_set_active(GTK_COMBO_BOX(gender_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), gender_combo, 3, 2, 1, 1);
    
    // Row 3: Branch and Semester
    GtkWidget *branch_label = gtk_label_new("Branch:");
    gtk_widget_set_halign(branch_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), branch_label, 0, 3, 1, 1);
    branch_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "CSE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "EE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "CIVIL");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "ME");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "IT");
    gtk_combo_box_set_active(GTK_COMBO_BOX(branch_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), branch_combo, 1, 3, 1, 1);
    
    GtkWidget *semester_label = gtk_label_new("Semester:");
    gtk_widget_set_halign(semester_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), semester_label, 2, 3, 1, 1);
    semester_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "1st");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "2nd");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "3rd");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "4th");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "5th");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "6th");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "7th");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), "8th");
    gtk_combo_box_set_active(GTK_COMBO_BOX(semester_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), semester_combo, 3, 3, 1, 1);
    
    // Row 4: Mobile
    GtkWidget *mobile_label = gtk_label_new("Mobile:");
    gtk_widget_set_halign(mobile_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), mobile_label, 0, 4, 1, 1);
    mobile_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(mobile_entry), "10 digits");
    gtk_grid_attach(GTK_GRID(grid), mobile_entry, 1, 4, 3, 1);
    
    // Row 5: Email
    GtkWidget *email_label = gtk_label_new("Email:");
    gtk_widget_set_halign(email_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), email_label, 0, 5, 1, 1);
    email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "example@domain.com");
    gtk_grid_attach(GTK_GRID(grid), email_entry, 1, 5, 3, 1);
    
    // Form Buttons
    GtkWidget *form_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(form_box), form_button_box, FALSE, FALSE, 5);
    
    GtkWidget *save_btn = gtk_button_new_with_label("💾 Save Student");
    gtk_widget_set_size_request(save_btn, 150, 35);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_add_student_save_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(form_button_box), save_btn, FALSE, FALSE, 0);
    
    // ====================================================================
    // STUDENT TABLE WITH ALL COLUMNS & PROPER WIDTHS
    // ====================================================================
    
    GtkWidget *table_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(table_label), "<span weight='bold'>📋 Students List</span>");
    gtk_widget_set_halign(table_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(main_box), table_label, FALSE, FALSE, 5);
    
    // Scrolled window for table
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);
    
    // Create list store with 11 columns
    GtkListStore *store = gtk_list_store_new(11,
        G_TYPE_STRING,  // 0: Edit button
        G_TYPE_STRING,  // 1: Delete button
        G_TYPE_STRING,  // 2: Photo
        G_TYPE_STRING,  // 3: Name
        G_TYPE_STRING,  // 4: Father Name
        G_TYPE_STRING,  // 5: Roll No
        G_TYPE_STRING,  // 6: Branch
        G_TYPE_INT,     // 7: Semester
        G_TYPE_STRING,  // 8: Gender
        G_TYPE_STRING,  // 9: Email
        G_TYPE_STRING   // 10: Mobile
    );
    
    // Create tree view
    student_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(G_OBJECT(store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(student_table), GTK_TREE_VIEW_GRID_LINES_BOTH);
    gtk_container_add(GTK_CONTAINER(scroll), student_table);
    
    // Add columns with fixed widths
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col;
    
    // Column 0: Edit button
    col = gtk_tree_view_column_new_with_attributes("✏️ Edit", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_fixed_width(col, 50);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 1: Delete button
    col = gtk_tree_view_column_new_with_attributes("🗑️ Del", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_fixed_width(col, 50);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 2: Photo
    col = gtk_tree_view_column_new_with_attributes("📷", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_fixed_width(col, 40);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 3: Name
    col = gtk_tree_view_column_new_with_attributes("👤 Name", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_fixed_width(col, 120);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 4: Father Name
    col = gtk_tree_view_column_new_with_attributes("Father Name", renderer, "text", 4, NULL);
    gtk_tree_view_column_set_fixed_width(col, 120);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 5: Roll No
    col = gtk_tree_view_column_new_with_attributes("📝 Roll No", renderer, "text", 5, NULL);
    gtk_tree_view_column_set_fixed_width(col, 130);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 6: Branch
    col = gtk_tree_view_column_new_with_attributes("Branch", renderer, "text", 6, NULL);
    gtk_tree_view_column_set_fixed_width(col, 70);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 7: Semester
    col = gtk_tree_view_column_new_with_attributes("Sem", renderer, "text", 7, NULL);
    gtk_tree_view_column_set_fixed_width(col, 40);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 8: Gender
    col = gtk_tree_view_column_new_with_attributes("👨 Gender", renderer, "text", 8, NULL);
    gtk_tree_view_column_set_fixed_width(col, 75);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 9: Email
    col = gtk_tree_view_column_new_with_attributes("📧 Email", renderer, "text", 9, NULL);
    gtk_tree_view_column_set_fixed_width(col, 150);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    // Column 10: Mobile
    col = gtk_tree_view_column_new_with_attributes("📞 Mobile", renderer, "text", 10, NULL);
    gtk_tree_view_column_set_fixed_width(col, 110);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    
    gtk_widget_show_all(container);
    gtk_widget_hide(form_box);
    printf("[INFO] Student Management UI created successfully with 11 columns\n");
    
    // Load students on startup
    refresh_student_table();
}