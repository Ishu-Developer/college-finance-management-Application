#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "../../include/student_ui.h"
#include "../../include/database.h"
#include "../../include/validators.h"

// Global variables
static GtkWidget *student_table = NULL;
static GtkWidget *form_box = NULL;
static GtkWidget *photo_label;
static char photo_path[500] = "";
static GtkWidget *name_entry;
static GtkWidget *gender_combo;
static GtkWidget *father_name_entry;
static GtkWidget *branch_combo;
static GtkWidget *year_combo;
static GtkWidget *semester_combo;
static GtkWidget *roll_no_entry;
static GtkWidget *category_combo;
static GtkWidget *mobile_entry;
static GtkWidget *email_entry;
static GtkWidget *error_label;  // ✅ NEW for validation messages
static GtkWidget *search_bar = NULL;
static GtkWidget *search_entry = NULL;

void refresh_student_table() {
    printf("[INFO] Refreshing student table\n");
    
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(student_table)));
    if (store) {
        gtk_list_store_clear(store);
    }
    
    sqlite3_stmt *stmt = db_get_all_students();
    if (stmt == NULL) {
        printf("[WARNING] No students found in database\n");
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "—", 1, "—", 2, "—", 3, 0, 4, "No student added",
            5, "—", 6, "—", 7, "—", 8, "—", 9, "—", 10, "—", 11, "—", 12, "—", 13, "—",
            -1);
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int student_id      = sqlite3_column_int(stmt, 0);
        const char *roll_no = (const char *)sqlite3_column_text(stmt, 1);  // ✅ FIX #1: Use _text!
        const char *name    = (const char *)sqlite3_column_text(stmt, 2);
        const char *gender  = (const char *)sqlite3_column_text(stmt, 3);
        const char *father  = (const char *)sqlite3_column_text(stmt, 4);
        const char *branch  = (const char *)sqlite3_column_text(stmt, 5);
        int year            = sqlite3_column_int(stmt, 6);
        int semester        = sqlite3_column_int(stmt, 7);
        const char *category = (const char *)sqlite3_column_text(stmt, 8);
        const char *mobile  = (const char *)sqlite3_column_text(stmt, 9);
        const char *email   = (const char *)sqlite3_column_text(stmt, 10);

        char year_str[20];
        snprintf(year_str, sizeof(year_str), "%d", year);

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️", 1, "🗑️", 2, "📷", 3, student_id, 4, name,
            5, gender ? gender : "—",
            6, father ? father : "—",
            7, branch ? branch : "—",
            8, year_str,
            9, semester,
            10, roll_no ? roll_no : "—",  
            11, category ? category : "—",
            12, mobile ? mobile : "—",
            13, email ? email : "—",
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
            
            char display_text[768];
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
    
    const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
    int gender_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(gender_combo));
    const char *gender = (gender_idx == 0) ? "Male" : (gender_idx == 1) ? "Female" : "Other";
    const char *father_name = gtk_entry_get_text(GTK_ENTRY(father_name_entry));
    int branch_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(branch_combo));
    const char *branch = (branch_idx == 0) ? "CSE" : 
                        (branch_idx == 1) ? "IT" : 
                        (branch_idx == 2) ? "CIVIL" : 
                        (branch_idx == 3) ? "ME" : "ECE";
    
    int year = gtk_combo_box_get_active(GTK_COMBO_BOX(year_combo)) + 1;
    int semester = gtk_combo_box_get_active(GTK_COMBO_BOX(semester_combo)) + 1;
    
    const char *roll_no_str = gtk_entry_get_text(GTK_ENTRY(roll_no_entry));  // ✅ Keep as string!
    
    int category_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(category_combo));
    const char *category = (category_idx == 0) ? "General" : 
                          (category_idx == 1) ? "OBC" : 
                          (category_idx == 2) ? "SC/ST" : "EWS";
    
    const char *mobile_str = gtk_entry_get_text(GTK_ENTRY(mobile_entry));
    const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
    
    printf("[INFO] Add Student - Validating input\n");
    printf("[DEBUG] Name: %s, Roll: %s, Year: %d, Mobile: %s\n", 
           name, roll_no_str, year, mobile_str);
    
    // ✅ VALIDATIONS using STRING-based validators
    if (!validate_roll_no(roll_no_str)) {
        gtk_label_set_text(GTK_LABEL(error_label), 
            "❌ Invalid Roll No (must be 14 digits)");
        gtk_widget_show(error_label);
        printf("[WARNING] Invalid roll number: %s\n", roll_no_str);
        return;
    }
    
    if (!validate_name(name)) {
        gtk_label_set_text(GTK_LABEL(error_label), 
            "❌ Name must be 3-100 letters (no numbers)");
        gtk_widget_show(error_label);
        return;
    }
    
    char year_str[20];
    snprintf(year_str, sizeof(year_str), "%d", year);
    if (!validate_year(year_str)) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Year must be 1-4");
        gtk_widget_show(error_label);
        return;
    }
    
    char semester_str[20];
    snprintf(semester_str, sizeof(semester_str), "%d", semester);
    if (!validate_semester(semester_str)) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Semester must be 1-8");
        gtk_widget_show(error_label);
        return;
    }
    
    if (!validate_mobile(mobile_str)) {
        gtk_label_set_text(GTK_LABEL(error_label), 
            "❌ Mobile must be exactly 10 digits (6-9 start)");
        gtk_widget_show(error_label);
        return;
    }
    
    if (!validate_email(email)) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Invalid email format");
        gtk_widget_show(error_label);
        return;
    }
    
    if (!validate_gender(gender)) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Invalid gender");
        gtk_widget_show(error_label);
        return;
    }
    
    if (!validate_branch(branch)) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Invalid branch");
        gtk_widget_show(error_label);
        return;
    }
    
    if (!validate_category(category)) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Invalid category");
        gtk_widget_show(error_label);
        return;
    }
    
    printf("[INFO] All validations passed - Adding student\n");
    
    // ✅ FIX #2: Pass roll_no_str directly (string), not converted to int!
    int result = db_add_student(name, gender, father_name, branch, year, semester, 
                               roll_no_str,  // STRING parameter!
                               category, mobile_str, email);
    
    if (result > 0) {
        printf("[SUCCESS] Student added with ID: %d\n", result);
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "✅ Student Added Successfully!\nStudent ID: %d\nName: %s\nRoll: %s",result, name, roll_no_str);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Clear form...
        gtk_entry_set_text(GTK_ENTRY(name_entry), "");
        gtk_entry_set_text(GTK_ENTRY(father_name_entry), "");
        gtk_entry_set_text(GTK_ENTRY(roll_no_entry), "");
        gtk_entry_set_text(GTK_ENTRY(mobile_entry), "");
        gtk_entry_set_text(GTK_ENTRY(email_entry), "");
        gtk_label_set_text(GTK_LABEL(photo_label), "📷 No photo selected");
        gtk_widget_hide(error_label);
        
        refresh_student_table();
    } else {
        printf("[ERROR] Failed to add student\n");
        gtk_label_set_text(GTK_LABEL(error_label), 
            "❌ Database error: Failed to add student");
        gtk_widget_show(error_label);
    }
}


void on_add_student_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Add Student button clicked\n");
    
    if (gtk_widget_get_visible(form_box)) {
        printf("[INFO] Hiding form\n");
        gtk_widget_hide(form_box);
        gtk_widget_hide(error_label);
    } else {
        printf("[INFO] Showing form\n");
        gtk_widget_show_all(form_box);
        gtk_widget_hide(error_label);  // Hide error on new form
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
    
    printf("[INFO] Search button clicked\n");
    
    // If search bar exists and is visible, hide it
    if (search_bar && gtk_widget_get_visible(search_bar)) {
        printf("[INFO] Hiding search bar\n");
        gtk_widget_hide(search_bar);
        refresh_student_table();
        return;
    }
    
    // Create search bar if it doesn't exist
    if (search_bar == NULL) {
        printf("[INFO] Creating search bar\n");
        
        search_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(search_bar), 10);
        gtk_widget_set_name(search_bar, "search_bar");
        
        // Label
        GtkWidget *label = gtk_label_new("🔍 Search by Roll No:");
        gtk_widget_set_size_request(label, 150, -1);
        gtk_box_pack_start(GTK_BOX(search_bar), label, FALSE, FALSE, 0);
        
        // Entry
        search_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter Roll No (e.g., 1001)");
        gtk_widget_set_size_request(search_entry, 200, -1);
        gtk_box_pack_start(GTK_BOX(search_bar), search_entry, FALSE, FALSE, 0);
        
        // Search button
        GtkWidget *search_btn = gtk_button_new_with_label("🔎 Find");
        gtk_widget_set_size_request(search_btn, 100, -1);
        gtk_box_pack_start(GTK_BOX(search_bar), search_btn, FALSE, FALSE, 0);
        
        // Clear button
        GtkWidget *clear_btn = gtk_button_new_with_label("✖️ Clear");
        gtk_widget_set_size_request(clear_btn, 100, -1);
        gtk_box_pack_start(GTK_BOX(search_bar), clear_btn, FALSE, FALSE, 0);
        
        // Connect signals
        g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_perform_inline), NULL);
        g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_search_clear), NULL);
    }
    
    // Show search bar
    printf("[INFO] Showing search bar\n");
    gtk_widget_show_all(search_bar);
    gtk_widget_grab_focus(search_entry);
}


// Search callback - CORRECTED
void on_search_perform_inline(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    const char *search_roll_no = gtk_entry_get_text(GTK_ENTRY(search_entry));
    
    if (!search_roll_no || strlen(search_roll_no) == 0) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Please enter a roll number");
        gtk_widget_show(error_label);
        return;
    }
    
    printf("[INFO] Searching for roll: %s\n", search_roll_no);
    
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(student_table)));
    gtk_list_store_clear(store);
    
    sqlite3_stmt *stmt = db_get_all_students();
    int found = 0;
    
    if (stmt != NULL) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int student_id      = sqlite3_column_int(stmt, 0);
            const char *roll_no = (const char *)sqlite3_column_text(stmt, 1);  // ✅ Use _text!
            const char *name    = (const char *)sqlite3_column_text(stmt, 2);
            const char *gender  = (const char *)sqlite3_column_text(stmt, 3);
            const char *father  = (const char *)sqlite3_column_text(stmt, 4);
            const char *branch  = (const char *)sqlite3_column_text(stmt, 5);
            int year            = sqlite3_column_int(stmt, 6);
            int semester        = sqlite3_column_int(stmt, 7);
            const char *category = (const char *)sqlite3_column_text(stmt, 8);
            const char *mobile  = (const char *)sqlite3_column_text(stmt, 9);
            const char *email   = (const char *)sqlite3_column_text(stmt, 10);

            // ✅ FIX #4: Only add row if roll number MATCHES search
            if (roll_no && strcmp(roll_no, search_roll_no) == 0) {
                char year_str[20];
                snprintf(year_str, sizeof(year_str), "%d", year);

                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                    0, "✏️", 1, "🗑️", 2, "📷", 3, student_id, 4, name,
                    5, gender ? gender : "—",
                    6, father ? father : "—",
                    7, branch ? branch : "—",
                    8, year_str,
                    9, semester,
                    10, roll_no,  // ✅ Use directly
                    11, category ? category : "—",
                    12, mobile ? mobile : "—",
                    13, email ? email : "—",
                    -1);
                found++;
            }
        }
        sqlite3_finalize(stmt);
    }
    
    if (found > 0) {
        gtk_label_set_text(GTK_LABEL(error_label), "✅ Student found!");
        gtk_widget_show(error_label);
        printf("[SUCCESS] Found %d student(s)\n", found);
    } else {
        gtk_label_set_text(GTK_LABEL(error_label), 
            "❌ No student found with this roll number");
        gtk_widget_show(error_label);
        printf("[WARNING] Student not found\n");
    }
}
// Clear search
void on_search_clear(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Clear search\n");
    
    gtk_entry_set_text(GTK_ENTRY(search_entry), "");
    gtk_widget_hide(search_bar);
    gtk_widget_hide(error_label);
    
    refresh_student_table();
}
void create_student_ui(GtkWidget *container) {
    printf("[INFO] Creating Student Management UI\n");
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(container), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    
    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='20' weight='bold'>👨‍🎓 Student Management</span>");
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
        "<span font='12' weight='bold'>➕ Add New Student</span>");
    gtk_box_pack_start(GTK_BOX(form_box), form_title, FALSE, FALSE, 0);
    
    // ✅ NEW: Error label for validation messages
    error_label = gtk_label_new("");
    gtk_label_set_use_markup(GTK_LABEL(error_label), TRUE);
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(form_box), error_label, FALSE, FALSE, 0);
    gtk_widget_hide(error_label);
    
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
    gtk_entry_set_placeholder_text(GTK_ENTRY(roll_no_entry), "2408400100031");
    gtk_grid_attach(GTK_GRID(grid), roll_no_entry, 1, 1, 1, 1);
    
    GtkWidget *name_label = gtk_label_new("Name:");
    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), name_label, 2, 1, 1, 1);
    name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Full Name");
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
    
    // Row 3: Branch, Year, and Semester
    GtkWidget *branch_label = gtk_label_new("Branch:");
    gtk_widget_set_halign(branch_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), branch_label, 0, 3, 1, 1);
    branch_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "CSE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "IT");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "ECE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "CIVIL");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(branch_combo), "ME");
    gtk_combo_box_set_active(GTK_COMBO_BOX(branch_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), branch_combo, 1, 3, 1, 1);
    
    GtkWidget *year_label = gtk_label_new("Year:");
    gtk_widget_set_halign(year_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), year_label, 2, 3, 1, 1);
    year_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(year_combo), "1st Year");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(year_combo), "2nd Year");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(year_combo), "3rd Year");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(year_combo), "4th Year");
    gtk_combo_box_set_active(GTK_COMBO_BOX(year_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), year_combo, 3, 3, 1, 1);
    
    // Row 4: Semester and Category
    GtkWidget *semester_label = gtk_label_new("Semester:");
    gtk_widget_set_halign(semester_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), semester_label, 0, 4, 1, 1);
    semester_combo = gtk_combo_box_text_new();
    for (int i = 1; i <= 8; i++) {
        char sem_text[10];
        snprintf(sem_text, sizeof(sem_text), "Sem %d", i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(semester_combo), sem_text);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(semester_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), semester_combo, 1, 4, 1, 1);
    
    GtkWidget *category_label = gtk_label_new("Category:");
    gtk_widget_set_halign(category_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), category_label, 2, 4, 1, 1);
    category_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), "General");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), "OBC");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), "SC/ST");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(category_combo), "EWS");
    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), category_combo, 3, 4, 1, 1);
    
    // Row 5: Mobile
    GtkWidget *mobile_label = gtk_label_new("Mobile (10 digits):");
    gtk_widget_set_halign(mobile_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), mobile_label, 0, 5, 1, 1);
    mobile_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(mobile_entry), "9876543210");
    gtk_grid_attach(GTK_GRID(grid), mobile_entry, 1, 5, 3, 1);
    
    // Row 6: Email
    GtkWidget *email_label = gtk_label_new("Email:");
    gtk_widget_set_halign(email_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), email_label, 0, 6, 1, 1);
    email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "student@example.com");
    gtk_grid_attach(GTK_GRID(grid), email_entry, 1, 6, 3, 1);
    
    // Form Buttons
    GtkWidget *form_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(form_box), form_button_box, FALSE, FALSE, 5);
    
    GtkWidget *save_btn = gtk_button_new_with_label("💾 Save Student");
    gtk_widget_set_size_request(save_btn, 150, 35);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_add_student_save_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(form_button_box), save_btn, FALSE, FALSE, 0);
    
    // ====================================================================
    // STUDENT TABLE WITH ALL COLUMNS
    // ====================================================================
    
    GtkWidget *table_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(table_label), "<span weight='bold'>📋 Students List</span>");
    gtk_widget_set_halign(table_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(main_box), table_label, FALSE, FALSE, 5);
        // Search bar (hidden initially)
    search_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(search_bar), 10);
    gtk_widget_set_name(search_bar, "search_bar");
    gtk_box_pack_start(GTK_BOX(main_box), search_bar, FALSE, FALSE, 0);
    gtk_widget_hide(search_bar);

    GtkWidget *search_label = gtk_label_new("🔍 Search by Roll No:");
    gtk_widget_set_size_request(search_label, 150, -1);
    gtk_box_pack_start(GTK_BOX(search_bar), search_label, FALSE, FALSE, 0);

    search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter Roll No");
    gtk_widget_set_size_request(search_entry, 200, -1);
    gtk_box_pack_start(GTK_BOX(search_bar), search_entry, FALSE, FALSE, 0);

    GtkWidget *search_find_btn = gtk_button_new_with_label("🔎 Find");
    gtk_widget_set_size_request(search_find_btn, 100, -1);
    gtk_box_pack_start(GTK_BOX(search_bar), search_find_btn, FALSE, FALSE, 0);
    g_signal_connect(search_find_btn, "clicked", G_CALLBACK(on_search_perform_inline), NULL);

    GtkWidget *search_clear_btn = gtk_button_new_with_label("✖️ Clear");
    gtk_widget_set_size_request(search_clear_btn, 100, -1);
    gtk_box_pack_start(GTK_BOX(search_bar), search_clear_btn, FALSE, FALSE, 0);
    g_signal_connect(search_clear_btn, "clicked", G_CALLBACK(on_search_clear), NULL);

    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);
    
    GtkListStore *store = gtk_list_store_new(14,
        G_TYPE_STRING,  // 0: Edit
        G_TYPE_STRING,  // 1: Delete
        G_TYPE_STRING,  // 2: Photo
        G_TYPE_INT,     // 3: Student ID
        G_TYPE_STRING,  // 4: Name
        G_TYPE_STRING,  // 5: Gender
        G_TYPE_STRING,  // 6: Father Name
        G_TYPE_STRING,  // 7: Branch
        G_TYPE_STRING,  // 8: Year
        G_TYPE_INT,     // 9: Semester
        G_TYPE_STRING,  // 10: Roll No
        G_TYPE_STRING,  // 11: Category
        G_TYPE_STRING,  // 12: Mobile
        G_TYPE_STRING   // 13: Email
    );
    
    student_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(G_OBJECT(store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(student_table), GTK_TREE_VIEW_GRID_LINES_BOTH);
    gtk_container_add(GTK_CONTAINER(scroll), student_table);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col;
    
    const char *col_titles[] = {
        "✏️", "🗑️", "📷", "ID", "👤 Name", "Gender", "Father Name", 
        "Branch", "Year", "Sem", "📝 Roll No", "Category", "📞 Mobile", "📧 Email"
    };
    int col_widths[] = {40, 40, 40, 40, 120, 70, 120, 70, 50, 45, 110, 90, 110, 150};
    
    for (int i = 0; i < 14; i++) {
        col = gtk_tree_view_column_new_with_attributes(col_titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_fixed_width(col, col_widths[i]);
        gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
        gtk_tree_view_append_column(GTK_TREE_VIEW(student_table), col);
    }
    
    gtk_widget_show_all(container);
    gtk_widget_hide(form_box);
    gtk_widget_hide(error_label);
    printf("[INFO] Student Management UI created successfully\n");
    
    refresh_student_table();
}
