#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../../include/fee_ui.h"
#include "../../include/database.h"
#include "../../include/validators.h"

// THESE we actually need and use:
static GtkWidget *fee_table = NULL;           // ✅ Used in create_fee_ui()
static GtkWidget *student_card_box = NULL;    // ✅ Used in refresh_student_card()
static GtkWidget *fee_summary_box = NULL;     // ✅ Used in update_fee_summary()

static StudentIDCard current_student = {0};   // ✅ Used in search & display
static gboolean card_flipped = FALSE;         // ✅ Used in flip animation
static GtkWidget *card_front = NULL;          // ✅ Used in card display
static GtkWidget *card_back = NULL;           // ✅ Used in card display

static double total_amount = 0;               // ✅ Used in calculations
static double total_paid = 0;                 // ✅ Used in calculations
static double total_due = 0;                  // ✅ Used in calculations

// ============================================================================
// INNOVATIVE UI: Fee Summary with Progress Bars
// ============================================================================

void update_fee_summary() {
    printf("[INFO] Updating fee summary\n");
    
    // Clear previous summary
    GList *children = gtk_container_get_children(GTK_CONTAINER(fee_summary_box));
    for (GList *iter = children; iter; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    // Calculate percentages
    double paid_percent = (total_amount > 0) ? (total_paid / total_amount * 100) : 0;
    double due_percent = (total_amount > 0) ? (total_due / total_amount * 100) : 0;
    
    // Summary Title
    GtkWidget *summary_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(summary_title),
        "<span font='12' weight='bold'>📊 Fee Summary</span>");
    gtk_box_pack_start(GTK_BOX(fee_summary_box), summary_title, FALSE, FALSE, 5);
    
    // Total Amount Card
    GtkWidget *total_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *total_icon = gtk_label_new("💰");
    GtkWidget *total_label = gtk_label_new(NULL);
    char total_text[100];
    snprintf(total_text, sizeof(total_text), 
        "<b>Total Amount</b>\n<span color='#2196F3' font='14' weight='bold'>₹ %.2f</span>", 
        total_amount);
    gtk_label_set_markup(GTK_LABEL(total_label), total_text);
    gtk_label_set_line_wrap(GTK_LABEL(total_label), TRUE);
    gtk_box_pack_start(GTK_BOX(total_box), total_icon, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(total_box), total_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fee_summary_box), total_box, FALSE, FALSE, 5);
    
    // Paid Progress
    GtkWidget *paid_label = gtk_label_new(NULL);
    char paid_text[100];
    snprintf(paid_text, sizeof(paid_text), 
        "✅ Paid: ₹ %.2f (%.1f%%)", total_paid, paid_percent);
    gtk_label_set_markup(GTK_LABEL(paid_label), paid_text);
    gtk_widget_set_halign(paid_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(fee_summary_box), paid_label, FALSE, FALSE, 0);
    
    GtkWidget *paid_progress = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(paid_progress), paid_percent / 100.0);
    gtk_widget_set_size_request(paid_progress, -1, 15);
    gtk_box_pack_start(GTK_BOX(fee_summary_box), paid_progress, FALSE, FALSE, 0);
    
    // Due Progress
    GtkWidget *due_label = gtk_label_new(NULL);
    char due_text[100];
    snprintf(due_text, sizeof(due_text), 
        "⚠️ Due: ₹ %.2f (%.1f%%)", total_due, due_percent);
    gtk_label_set_markup(GTK_LABEL(due_label), due_text);
    gtk_widget_set_halign(due_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(fee_summary_box), due_label, FALSE, FALSE, 5);
    
    GtkWidget *due_progress = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(due_progress), due_percent / 100.0);
    gtk_widget_set_size_request(due_progress, -1, 15);
    gtk_box_pack_start(GTK_BOX(fee_summary_box), due_progress, FALSE, FALSE, 0);
    
    // Status indicator
    GtkWidget *status_label = gtk_label_new(NULL);
    if (total_due <= 0) {
        gtk_label_set_markup(GTK_LABEL(status_label),
            "<span color='#4CAF50' font='11' weight='bold'>✅ All Fees Paid</span>");
    } else if (paid_percent >= 50) {
        gtk_label_set_markup(GTK_LABEL(status_label),
            "<span color='#FF9800' font='11' weight='bold'>⚠️ Partial Payment</span>");
    } else {
        gtk_label_set_markup(GTK_LABEL(status_label),
            "<span color='#F44336' font='11' weight='bold'>❌ Payment Pending</span>");
    }
    gtk_widget_set_halign(status_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(fee_summary_box), status_label, FALSE, FALSE, 10);
    
    gtk_widget_show_all(fee_summary_box);
    printf("[INFO] Fee summary updated\n");
}

// ============================================================================
// ENHANCED STUDENT CARD - Professional Design
// ============================================================================

GtkWidget* create_card_front() {
    GtkWidget *card_frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(card_frame, 320, 420);
    gtk_frame_set_shadow_type(GTK_FRAME(card_frame), GTK_SHADOW_ETCHED_IN);
    
    GtkWidget *card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(card_box), 0);
    gtk_container_add(GTK_CONTAINER(card_frame), card_box);
    
    // Header: College Name (Blue background)
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(header_box, -1, 60);
    GtkCssProvider *header_css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(header_css,
        "GtkBox { background-color: #1e3a8a; }", -1, NULL);
    GtkStyleContext *header_context = gtk_widget_get_style_context(header_box);
    gtk_style_context_add_provider(header_context, GTK_STYLE_PROVIDER(header_css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_container_set_border_width(GTK_CONTAINER(header_box), 10);
    
    GtkWidget *college_name = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(college_name),
        "<span font='11' weight='bold' color='white'>L.D.A.H RAJKIYA</span>\n"
        "<span font='10' weight='bold' color='white'>Engineering College</span>\n"
        "<span font='8' color='#E0E0E0'>Mainpuri, U.P.</span>");
    gtk_label_set_justify(GTK_LABEL(college_name), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(header_box), college_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card_box), header_box, FALSE, FALSE, 0);
    
    // Student Photo & Details
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(content_box), 15);
    gtk_box_pack_start(GTK_BOX(card_box), content_box, TRUE, TRUE, 0);
    
    // Photo placeholder with frame
    GtkWidget *photo_frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(photo_frame, 120, 140);
    gtk_frame_set_shadow_type(GTK_FRAME(photo_frame), GTK_SHADOW_IN);
    GtkWidget *photo = gtk_image_new_from_icon_name("gtk-dialog-question", GTK_ICON_SIZE_DIALOG);
    gtk_widget_set_size_request(photo, 120, 140);
    gtk_container_add(GTK_CONTAINER(photo_frame), photo);
    gtk_widget_set_halign(photo_frame, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(content_box), photo_frame, FALSE, FALSE, 0);
    
    // Student ID
    GtkWidget *id_label = gtk_label_new(NULL);
    char id_text[50];
    snprintf(id_text, sizeof(id_text), "Roll No: <b>%d</b>", current_student.roll_no);
    gtk_label_set_markup(GTK_LABEL(id_label), id_text);
    gtk_widget_set_halign(id_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(content_box), id_label, FALSE, FALSE, 0);
    
    // Student Name
    GtkWidget *name_label = gtk_label_new(NULL);
    char name_markup[150];
    snprintf(name_markup, sizeof(name_markup), "<b>%s</b>", current_student.name);
    gtk_label_set_markup(GTK_LABEL(name_label), name_markup);
    gtk_label_set_line_wrap(GTK_LABEL(name_label), TRUE);
    gtk_widget_set_halign(name_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(content_box), name_label, FALSE, FALSE, 0);
    
    // Course & Branch
    GtkWidget *course_label = gtk_label_new(NULL);
    char course_text[100];
    snprintf(course_text, sizeof(course_text), 
        "B.Tech - <b>%s</b>", current_student.branch);
    gtk_label_set_markup(GTK_LABEL(course_label), course_text);
    gtk_widget_set_halign(course_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(content_box), course_label, FALSE, FALSE, 0);
    
    // Separator
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(content_box), separator, FALSE, FALSE, 5);
    
    // Footer: Validity
    GtkWidget *footer_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(footer_label),
        "<span font='8' color='#666666'>Valid Till: 2025-12-31\n"
        "Issued: 2025-01-01</span>");
    gtk_label_set_justify(GTK_LABEL(footer_label), GTK_JUSTIFY_CENTER);
    gtk_box_pack_end(GTK_BOX(content_box), footer_label, FALSE, FALSE, 0);
    
    return card_frame;
}

GtkWidget* create_card_back() {
    GtkWidget *card_frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(card_frame, 320, 420);
    gtk_frame_set_shadow_type(GTK_FRAME(card_frame), GTK_SHADOW_ETCHED_IN);
    
    GtkWidget *card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(card_box), 15);
    gtk_container_add(GTK_CONTAINER(card_frame), card_box);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header),
        "<span font='11' weight='bold'>📱 CONTACT & BARCODE</span>");
    gtk_widget_set_halign(header, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(card_box), header, FALSE, FALSE, 0);
    
    // QR Code / Barcode placeholder
    GtkWidget *barcode_frame = gtk_frame_new(NULL);
    GtkWidget *barcode = gtk_image_new_from_icon_name("gtk-dialog-info", GTK_ICON_SIZE_DIALOG);
    gtk_widget_set_size_request(barcode, 150, 150);
    gtk_container_add(GTK_CONTAINER(barcode_frame), barcode);
    gtk_widget_set_halign(barcode_frame, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(card_box), barcode_frame, FALSE, FALSE, 5);
    
    // Contact Info
    GtkWidget *contact_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    GtkWidget *mobile_label = gtk_label_new(NULL);
    char mobile_text[100];
    snprintf(mobile_text, sizeof(mobile_text), 
        "<b>📞 Mobile:</b> %s", 
        current_student.mobile[0] ? current_student.mobile : "N/A");
    gtk_label_set_markup(GTK_LABEL(mobile_label), mobile_text);
    gtk_label_set_line_wrap(GTK_LABEL(mobile_label), TRUE);
    gtk_widget_set_halign(mobile_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(contact_box), mobile_label, FALSE, FALSE, 0);
    
    GtkWidget *email_label = gtk_label_new(NULL);
    char email_text[150];
    snprintf(email_text, sizeof(email_text), 
        "<b>📧 Email:</b> %s", 
        current_student.email[0] ? current_student.email : "N/A");
    gtk_label_set_markup(GTK_LABEL(email_label), email_text);
    gtk_label_set_line_wrap(GTK_LABEL(email_label), TRUE);
    gtk_widget_set_halign(email_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(contact_box), email_label, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(card_box), contact_box, FALSE, FALSE, 5);
    
    // Separator
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(card_box), separator, FALSE, FALSE, 5);
    
    // Terms
    GtkWidget *terms = gtk_label_new(
        "This ID Card is the property of the college.\n"
        "In case of loss, please notify the office immediately.");
    gtk_widget_set_size_request(terms, 290, -1);
    gtk_label_set_line_wrap(GTK_LABEL(terms), TRUE);
    gtk_label_set_justify(GTK_LABEL(terms), GTK_JUSTIFY_CENTER);
    gtk_widget_set_halign(terms, GTK_ALIGN_CENTER);
    gtk_box_pack_end(GTK_BOX(card_box), terms, FALSE, FALSE, 0);
    
    return card_frame;
}

void on_card_clicked(GtkGestureMultiPress *gesture, gint n_press, 
                     gdouble x, gdouble y, gpointer user_data) {
    (void)gesture; (void)n_press; (void)x; (void)y; (void)user_data;
    
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
    printf("[INFO] Refreshing student card for: %s\n", current_student.name);
    
    // Clear previous
    GList *children = gtk_container_get_children(GTK_CONTAINER(student_card_box));
    for (GList *iter = children; iter; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    if (current_student.student_id == 0) {
        GtkWidget *empty = gtk_label_new("No Student Selected\n\nClick Search ➡️");
        gtk_widget_set_halign(empty, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(empty, GTK_ALIGN_CENTER);
        gtk_container_add(GTK_CONTAINER(student_card_box), empty);
        gtk_widget_show_all(student_card_box);
        return;
    }
    
    // Create cards
    card_front = create_card_front();
    card_back = create_card_back();
    
    gtk_container_add(GTK_CONTAINER(student_card_box), card_front);
    gtk_container_add(GTK_CONTAINER(student_card_box), card_back);
    gtk_widget_hide(card_back);
    
    // Add click gesture
    GtkGesture *gesture = gtk_gesture_multi_press_new(card_front);
    g_signal_connect(gesture, "pressed", G_CALLBACK(on_card_clicked), NULL);
    
    gtk_widget_show_all(student_card_box);
    card_flipped = FALSE;
    
    printf("[INFO] Student card refreshed\n");
}

// ============================================================================
// FEE TABLE MANAGEMENT
// ============================================================================

void refresh_fee_table_for_student(int roll_no) {
    printf("[INFO] Refreshing fee table for roll: %d\n", roll_no);
    
    // Reset summary
    total_amount = 0;
    total_paid = 0;
    total_due = 0;
    
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(fee_table)));
    gtk_list_store_clear(store);
    
    sqlite3_stmt *stmt = db_get_fees_by_rollno((const char *)&roll_no);
    if (stmt == NULL) {
        printf("[WARNING] No fees found for roll: %d\n", roll_no);
        update_fee_summary();
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *fee_type = (const char *)sqlite3_column_text(stmt, 6);
        double amount = sqlite3_column_double(stmt, 7);
        double amount_paid = sqlite3_column_double(stmt, 8);
        double amount_due = sqlite3_column_double(stmt, 9);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 10);
        const char *status = (const char *)sqlite3_column_text(stmt, 11);
        
        // Update summary
        total_amount += amount;
        total_paid += amount_paid;
        total_due += amount_due;
        
        // Format for display
        char amount_str[30], paid_str[30], due_str[30];
        snprintf(amount_str, sizeof(amount_str), "₹ %.2f", amount);
        snprintf(paid_str, sizeof(paid_str), "₹ %.2f", amount_paid);
        snprintf(due_str, sizeof(due_str), "₹ %.2f", amount_due);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️",
            1, "🗑️",
            2, fee_type,
            3, amount_str,
            4, paid_str,
            5, due_str,
            6, due_date,
            7, status,
            -1);
        count++;
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] Loaded %d fee records\n", count);
    update_fee_summary();
}

// CORRECT ✅ (unique name for fee module)
void on_search_student_fee_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    
    printf("[INFO] Search student button clicked\n");
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "🔍 Search Student by Roll Number",
        NULL,
        GTK_DIALOG_MODAL,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Search", GTK_RESPONSE_OK,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 150);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(content), box);
    gtk_container_set_border_width(GTK_CONTAINER(box), 15);
    
    GtkWidget *label = gtk_label_new("Enter Student Roll Number:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "1001");
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *roll_str = gtk_entry_get_text(GTK_ENTRY(entry));
        
        // Validate input
        if (!validate_roll_no(roll_str)) {
            GtkWidget *error = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                "❌ Invalid roll number!\nMust be between 1001-999999");
            gtk_dialog_run(GTK_DIALOG(error));
            gtk_widget_destroy(error);
        } else {
            int roll_no = atoi(roll_str);
            
            // Try to fetch student
            if (db_get_student_for_card(roll_no, &current_student)) {
                printf("[SUCCESS] Student found: %s\n", current_student.name);
                card_flipped = FALSE;
                refresh_student_card();
                refresh_fee_table_for_student(roll_no);
                
                GtkWidget *success = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                    "✅ Student found: %s", current_student.name);
                gtk_dialog_run(GTK_DIALOG(success));
                gtk_widget_destroy(success);
            } else {
                GtkWidget *notfound = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                    "❌ Student not found!\nRoll No: %d", roll_no);
                gtk_dialog_run(GTK_DIALOG(notfound));
                gtk_widget_destroy(notfound);
            }
        }
    }
    
    gtk_widget_destroy(dialog);
}

void on_refresh_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    
    if (current_student.student_id > 0) {
        refresh_fee_table_for_student(current_student.roll_no);
        printf("[INFO] Fee table refreshed\n");
    }
}

// ============================================================================
// MAIN UI CREATION - Tabbed Professional Interface
// ============================================================================

void create_fee_ui(GtkWidget *container) {
    printf("[INFO] Creating Fee Management UI with professional design\n");
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(container), main_box);
    
    // ====================================================================
    // HEADER
    // ====================================================================
    
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(header_box), 15);
    gtk_box_pack_start(GTK_BOX(main_box), header_box, FALSE, FALSE, 0);
    
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='18' weight='bold'>💰 Fee Management System</span>");
    gtk_box_pack_start(GTK_BOX(header_box), title, FALSE, FALSE, 0);
    
    GtkWidget *subtitle = gtk_label_new("Manage student fees and track payments");
    gtk_widget_set_halign(subtitle, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(header_box), subtitle, FALSE, FALSE, 0);
    
    // ====================================================================
    // BUTTON BAR
    // ====================================================================
    
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(button_box), 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    
    GtkWidget *search_btn = gtk_button_new_with_label("🔍 Search Student");
    gtk_widget_set_size_request(search_btn, 150, 40);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_student_fee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), search_btn, FALSE, FALSE, 0);
    
    GtkWidget *refresh_btn = gtk_button_new_with_label("🔄 Refresh");
    gtk_widget_set_size_request(refresh_btn, 120, 40);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), refresh_btn, FALSE, FALSE, 0);
    
    // ====================================================================
    // MAIN CONTENT: Two Column Layout
    // ====================================================================
    
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(content_box), 10);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);
    
    // -------- LEFT SIDE: Student Card + Summary --------
    
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(left_box, 360, -1);
    gtk_box_pack_start(GTK_BOX(content_box), left_box, FALSE, TRUE, 0);
    
    // Student Card
    GtkWidget *card_frame = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(card_frame), "📇 Student ID Card (Click to flip)");
    student_card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(card_frame), student_card_box);
    GtkWidget *empty = gtk_label_new("No Student Selected\n\n👇 Click Search");
    gtk_widget_set_halign(empty, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(empty, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(student_card_box), empty);
    gtk_widget_set_size_request(card_frame, -1, 450);
    gtk_box_pack_start(GTK_BOX(left_box), card_frame, TRUE, TRUE, 0);
    
    // Fee Summary
    GtkWidget *summary_frame = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(summary_frame), "📊 Payment Summary");
    fee_summary_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(fee_summary_box), 10);
    gtk_container_add(GTK_CONTAINER(summary_frame), fee_summary_box);
    gtk_widget_set_size_request(summary_frame, -1, 200);
    gtk_box_pack_start(GTK_BOX(left_box), summary_frame, FALSE, TRUE, 0);
    
    // -------- RIGHT SIDE: Fee Table --------
    
    GtkWidget *table_frame = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(table_frame), "📋 Fee Details");
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(table_frame), scroll);
    gtk_box_pack_start(GTK_BOX(content_box), table_frame, TRUE, TRUE, 0);
    
    // Fee Table Model (8 columns)
    GtkListStore *store = gtk_list_store_new(8,
        G_TYPE_STRING,  // 0: Edit
        G_TYPE_STRING,  // 1: Delete
        G_TYPE_STRING,  // 2: Fee Type
        G_TYPE_STRING,  // 3: Amount
        G_TYPE_STRING,  // 4: Paid
        G_TYPE_STRING,  // 5: Due
        G_TYPE_STRING,  // 6: Due Date
        G_TYPE_STRING   // 7: Status
    );
    
    fee_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(G_OBJECT(store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(fee_table), GTK_TREE_VIEW_GRID_LINES_BOTH);
    gtk_container_add(GTK_CONTAINER(scroll), fee_table);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col;
    
    const char *col_titles[] = {"✏️", "🗑️", "Fee Type", "Amount", "Paid", "Due", "Due Date", "Status"};
    int col_widths[] = {35, 35, 90, 80, 80, 80, 90, 80};
    
    for (int i = 0; i < 8; i++) {
        col = gtk_tree_view_column_new_with_attributes(col_titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_fixed_width(col, col_widths[i]);
        gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
        gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    }
    
    gtk_widget_show_all(container);
    update_fee_summary();
    printf("[INFO] Fee Management UI created successfully\n");
}
