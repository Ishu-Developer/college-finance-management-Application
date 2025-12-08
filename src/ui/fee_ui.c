#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "../../include/fee_ui.h"
#include "../../include/database.h"
#include "../../include/validators.h"

// Global variables
static GtkWidget *fee_table = NULL;
static GtkWidget *student_card_box = NULL;
static StudentIDCard current_student = {0};
static gboolean card_flipped = FALSE;
static GtkWidget *card_front = NULL;
static GtkWidget *card_back = NULL;

void refresh_fee_table() {
    printf("[INFO] Refreshing fee table\n");
    
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(fee_table)));
    if (store) {
        gtk_list_store_clear(store);
    }
    
    sqlite3_stmt *stmt = db_get_all_fees();
    if (stmt == NULL) {
        printf("[WARNING] No fees found in database\n");
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *roll_no = (const char *)sqlite3_column_text(stmt, 2);
        const char *name = (const char *)sqlite3_column_text(stmt, 3);
        const char *branch = (const char *)sqlite3_column_text(stmt, 4);
        int semester = sqlite3_column_int(stmt, 5);
        const char *fee_type = (const char *)sqlite3_column_text(stmt, 6);
        double amount = sqlite3_column_double(stmt, 7);
        double amount_paid = sqlite3_column_double(stmt, 8);
        double amount_due = sqlite3_column_double(stmt, 9);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 10);
        const char *status = (const char *)sqlite3_column_text(stmt, 11);
        
        char amount_str[20], paid_str[20], due_str[20];
        snprintf(amount_str, sizeof(amount_str), "₹ %.2f", amount);
        snprintf(paid_str, sizeof(paid_str), "₹ %.2f", amount_paid);
        snprintf(due_str, sizeof(due_str), "₹ %.2f", amount_due);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️",              // Edit
            1, "🗑️",              // Delete
            2, roll_no,           // Roll No
            3, name,              // Name
            4, branch,            // Branch
            5, semester,          // Semester
            6, fee_type,          // Fee Type
            7, amount_str,        // Amount
            8, paid_str,          // Paid
            9, due_str,           // Due
            10, due_date,         // Due Date
            11, status,           // Status
            -1);
        count++;
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] Loaded %d fees\n", count);
}

GtkWidget* create_card_front() {
    GtkWidget *card_frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(card_frame, 280, 380);
    
    GtkWidget *card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(card_box), 15);
    gtk_container_add(GTK_CONTAINER(card_frame), card_box);
    
    // Header: College Name
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header),
        "<span font='11' weight='bold' color='#1e3a8a'>L.D.A.H Rajkiya</span>\n"
        "<span font='10' weight='bold' color='#1e3a8a'>Engineering College</span>\n"
        "<span font='9' color='#1e3a8a'>Mainpuri</span>");
    gtk_label_set_justify(GTK_LABEL(header), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(card_box), header, FALSE, FALSE, 0);
    
    // Photo placeholder
    GtkWidget *photo_frame = gtk_frame_new(NULL);
    GtkWidget *photo = gtk_image_new_from_icon_name("gtk-dialog-question", GTK_ICON_SIZE_DIALOG);
    gtk_widget_set_size_request(photo, 120, 140);
    gtk_container_add(GTK_CONTAINER(photo_frame), photo);
    gtk_box_pack_start(GTK_BOX(card_box), photo_frame, FALSE, FALSE, 0);
    
    // Student details
    GtkWidget *details = gtk_label_new(NULL);
    char details_text[300];
    snprintf(details_text, sizeof(details_text),
        "<b>STUDENT DETAIL</b>\n"
        "<b>Name:</b> %s\n"
        "<b>Roll NO:</b> %d\n"
        "<b>COURSE:</b> B.Tech\n"
        "<b>Branch:</b> %s\n",
        current_student.name,
        current_student.roll_no,
        current_student.branch);
    gtk_label_set_markup(GTK_LABEL(details), details_text);
    gtk_label_set_justify(GTK_LABEL(details), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(card_box), details, FALSE, FALSE, 0);
    
    // Principal signature area
    GtkWidget *signature = gtk_label_new("Principal");
    gtk_label_set_justify(GTK_LABEL(signature), GTK_JUSTIFY_CENTER);
    gtk_box_pack_end(GTK_BOX(card_box), signature, FALSE, TRUE, 0);
    
    return card_frame;
}

GtkWidget* create_card_back() {
    GtkWidget *card_frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(card_frame, 280, 380);
    
    GtkWidget *card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(card_box), 15);
    gtk_container_add(GTK_CONTAINER(card_frame), card_box);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header),
        "<span font='11' weight='bold'>STUDENT INFORMATION</span>");
    gtk_box_pack_start(GTK_BOX(card_box), header, FALSE, FALSE, 0);
    
    // Barcode placeholder
    GtkWidget *barcode = gtk_image_new_from_icon_name("gtk-dialog-info", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_widget_set_size_request(barcode, 200, 80);
    gtk_box_pack_start(GTK_BOX(card_box), barcode, FALSE, FALSE, 5);
    
    // More details
    GtkWidget *more_details = gtk_label_new(NULL);
    char details_text[250];
    snprintf(details_text, sizeof(details_text),
        "<b>Mobile:</b> %s\n"
        "<b>Email:</b> %s\n"
        "\n"
        "Valid From: 2025-01-01\n"
        "Valid Till: 2025-12-31",
        current_student.mobile[0] ? current_student.mobile : "N/A",
        current_student.email[0] ? current_student.email : "N/A");
    gtk_label_set_markup(GTK_LABEL(more_details), details_text);
    gtk_label_set_justify(GTK_LABEL(more_details), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(card_box), more_details, FALSE, FALSE, 0);
    
    // Terms and conditions
    GtkWidget *terms = gtk_label_new(
        "This card is property of the college.\n"
        "Notify the office in case of loss.");
    gtk_widget_set_size_request(terms, 250, -1);
    gtk_label_set_line_wrap(GTK_LABEL(terms), TRUE);
    gtk_label_set_justify(GTK_LABEL(terms), GTK_JUSTIFY_CENTER);
    gtk_box_pack_end(GTK_BOX(card_box), terms, FALSE, FALSE, 0);
    
    return card_frame;
}

void on_card_clicked(GtkGestureMultiPress *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data) {
    (void)gesture;
    (void)n_press;
    (void)x;
    (void)y;
    (void)user_data;
    
    printf("[INFO] Card clicked - flipping\n");
    card_flipped = !card_flipped;
    
    if (card_flipped) {
        gtk_widget_hide(card_front);
        gtk_widget_show(card_back);
    } else {
        gtk_widget_hide(card_back);
        gtk_widget_show(card_front);
    }
}

void refresh_student_card() {
    printf("[INFO] Refreshing student card\n");
    
    if (current_student.student_id == 0) {
        // No student loaded - show empty state
        GtkWidget *empty_label = gtk_label_new("No Student Selected\n\nClick Search to select a student");
        gtk_widget_set_halign(empty_label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(empty_label, GTK_ALIGN_CENTER);
        gtk_container_add(GTK_CONTAINER(student_card_box), empty_label);
        gtk_widget_show(empty_label);
        return;
    }
    
    // Clear previous widgets
    GList *children = gtk_container_get_children(GTK_CONTAINER(student_card_box));
    for (GList *iter = children; iter; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    // Create flip container
    GtkWidget *flip_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(student_card_box), flip_container);
    
    // Create card front and back
    card_front = create_card_front();
    card_back = create_card_back();
    
    gtk_box_pack_start(GTK_BOX(flip_container), card_front, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(flip_container), card_back, TRUE, TRUE, 0);
    gtk_widget_hide(card_back);
    
    // Add click gesture for card flip
    GtkGesture *gesture = gtk_gesture_multi_press_new(card_front);
    g_signal_connect(gesture, "pressed", G_CALLBACK(on_card_clicked), NULL);
    
    gtk_widget_show_all(flip_container);
    
    printf("[INFO] Student card loaded for: %s\n", current_student.name);
}

void on_search_student_fee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    
    printf("[INFO] Search student button clicked\n");
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "🔍 Search Student",
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
    
    GtkWidget *label = gtk_label_new("Search by Roll Number:");
    gtk_box_pack_start(GTK_BOX(main_box), label, FALSE, FALSE, 0);
    
    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter roll number");
    gtk_box_pack_start(GTK_BOX(main_box), search_entry, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *roll_no = gtk_entry_get_text(GTK_ENTRY(search_entry));
        
        if (strlen(roll_no) > 0) {
            printf("[INFO] Searching for student: %s\n", roll_no);
            
            // Get student details
            if (db_get_student_for_card(atoi(roll_no), &current_student)) {
                printf("[SUCCESS] Student found: %s\n", current_student.name);
                card_flipped = FALSE;
                refresh_student_card();
                
                // Load fees for this student
                GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(fee_table)));
                gtk_list_store_clear(store);
                
                sqlite3_stmt *stmt = db_get_fees_by_rollno(roll_no);
                if (stmt != NULL) {
                    while (sqlite3_step(stmt) == SQLITE_ROW) {
                        const char *fee_type = (const char *)sqlite3_column_text(stmt, 6);
                        double amount = sqlite3_column_double(stmt, 7);
                        double amount_paid = sqlite3_column_double(stmt, 8);
                        double amount_due = sqlite3_column_double(stmt, 9);
                        const char *due_date = (const char *)sqlite3_column_text(stmt, 10);
                        const char *status = (const char *)sqlite3_column_text(stmt, 11);
                        
                        char amount_str[20], paid_str[20], due_str[20];
                        snprintf(amount_str, sizeof(amount_str), "₹ %.2f", amount);
                        snprintf(paid_str, sizeof(paid_str), "₹ %.2f", amount_paid);
                        snprintf(due_str, sizeof(due_str), "₹ %.2f", amount_due);
                        
                        GtkTreeIter iter;
                        gtk_list_store_append(store, &iter);
                        gtk_list_store_set(store, &iter,
                            0, "✏️",
                            1, "🗑️",
                            2, roll_no,
                            3, current_student.name,
                            4, current_student.branch,
                            5, fee_type,
                            6, amount_str,
                            7, paid_str,
                            8, due_str,
                            9, due_date,
                            10, status,
                            -1);
                    }
                    sqlite3_finalize(stmt);
                }
            } else {
                GtkWidget *error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                    "❌ Student not found!");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }
        }
    }
    
    gtk_widget_destroy(dialog);
}

void on_refresh_fee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Refresh button clicked\n");
    refresh_fee_table();
}

void create_fee_ui(GtkWidget *container) {
    printf("[INFO] Creating Fee Management UI\n");
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(container), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    
    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='16' weight='bold'>💰 Fee Management</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);
    
    // Button Box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 5);
    
    GtkWidget *search_btn = gtk_button_new_with_label("🔍 Search");
    gtk_widget_set_size_request(search_btn, 150, 40);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_student_fee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), search_btn, FALSE, FALSE, 0);
    
    GtkWidget *refresh_btn = gtk_button_new_with_label("🔄 Refresh");
    gtk_widget_set_size_request(refresh_btn, 150, 40);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_fee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), refresh_btn, FALSE, FALSE, 0);
    
    // Main content horizontal split
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);
    
    // ====================================================================
    // LEFT SIDE: Student ID Card
    // ====================================================================
    
    GtkWidget *card_frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(card_frame, 320, -1);
    gtk_frame_set_label(GTK_FRAME(card_frame), "📇 Student ID Card (Click to flip)");
    
    student_card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(card_frame), student_card_box);
    gtk_box_pack_start(GTK_BOX(content_box), card_frame, FALSE, TRUE, 0);
    
    // Initial empty state
    GtkWidget *empty = gtk_label_new("No Student Selected\n\nClick Search to select");
    gtk_widget_set_halign(empty, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(empty, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(student_card_box), empty);
    
    // ====================================================================
    // RIGHT SIDE: Fee Table
    // ====================================================================
    
    GtkWidget *table_frame = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(table_frame), "📋 Fee Details");
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(table_frame), scroll);
    gtk_box_pack_start(GTK_BOX(content_box), table_frame, TRUE, TRUE, 0);
    
    // Create list store (12 columns)
    GtkListStore *store = gtk_list_store_new(12,
        G_TYPE_STRING,  // 0: Edit
        G_TYPE_STRING,  // 1: Delete
        G_TYPE_STRING,  // 2: Roll No
        G_TYPE_STRING,  // 3: Name
        G_TYPE_STRING,  // 4: Branch
        G_TYPE_INT,     // 5: Semester
        G_TYPE_STRING,  // 6: Fee Type
        G_TYPE_STRING,  // 7: Amount
        G_TYPE_STRING,  // 8: Paid
        G_TYPE_STRING,  // 9: Due
        G_TYPE_STRING,  // 10: Due Date
        G_TYPE_STRING   // 11: Status
    );
    
    fee_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(G_OBJECT(store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(fee_table), GTK_TREE_VIEW_GRID_LINES_BOTH);
    gtk_container_add(GTK_CONTAINER(scroll), fee_table);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col;
    
    col = gtk_tree_view_column_new_with_attributes("✏️", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_fixed_width(col, 40);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("🗑️", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_fixed_width(col, 40);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Roll No", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_fixed_width(col, 80);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_fixed_width(col, 100);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Branch", renderer, "text", 4, NULL);
    gtk_tree_view_column_set_fixed_width(col, 60);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Sem", renderer, "text", 5, NULL);
    gtk_tree_view_column_set_fixed_width(col, 40);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Fee Type", renderer, "text", 6, NULL);
    gtk_tree_view_column_set_fixed_width(col, 80);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Amount", renderer, "text", 7, NULL);
    gtk_tree_view_column_set_fixed_width(col, 70);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Paid", renderer, "text", 8, NULL);
    gtk_tree_view_column_set_fixed_width(col, 70);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Due", renderer, "text", 9, NULL);
    gtk_tree_view_column_set_fixed_width(col, 70);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Due Date", renderer, "text", 10, NULL);
    gtk_tree_view_column_set_fixed_width(col, 90);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    col = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", 11, NULL);
    gtk_tree_view_column_set_fixed_width(col, 80);
    gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    
    gtk_widget_show_all(container);
    printf("[INFO] Fee Management UI created successfully\n");
    
    // Initialize database
    db_fee_init();
}