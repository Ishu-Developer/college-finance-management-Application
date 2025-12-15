#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../include/fee_ui.h"
#include "../../include/database.h"
#include "../../include/validators.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
static GtkWidget *main_box = NULL;
static GtkWidget *student_card_box = NULL;
static gboolean search_box_visible = FALSE;
static GtkWidget *search_box = NULL;
static GtkWidget *search_entry = NULL;

static StudentIDCard current_student = {0};
static FeeRecord current_fee = {0};

// Fee form input widgets
static GtkWidget *fee_id_label = NULL;
static GtkWidget *inst_paid_entry = NULL;
static GtkWidget *inst_date_entry = NULL;
static GtkWidget *inst_due_entry = NULL;
static GtkWidget *inst_mode_combo = NULL;

static GtkWidget *hostel_paid_entry = NULL;
static GtkWidget *hostel_date_entry = NULL;
static GtkWidget *hostel_due_entry = NULL;
static GtkWidget *hostel_mode_combo = NULL;

static GtkWidget *mess_paid_entry = NULL;
static GtkWidget *mess_date_entry = NULL;
static GtkWidget *mess_due_entry = NULL;
static GtkWidget *mess_mode_combo = NULL;

static GtkWidget *other_paid_entry = NULL;
static GtkWidget *other_date_entry = NULL;
static GtkWidget *other_due_entry = NULL;
static GtkWidget *other_mode_combo = NULL;

static GtkWidget *total_label = NULL;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================
static void show_inline_search_box();
static void hide_inline_search_box();
static void on_search_go_clicked(GtkButton *button, gpointer data);
static void calculate_and_update_total();
static void load_fee_data();
static void clear_fee_form();
static void refresh_student_card_display();

// ============================================================================
// STUDENT CARD DISPLAY
// ============================================================================
static void refresh_student_card_display() {
    if (student_card_box == NULL) return;
    
    // Clear existing children
    GList *children = gtk_container_get_children(GTK_CONTAINER(student_card_box));
    for (GList *iter = children; iter; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    if (current_student.roll_no == 0) {
        GtkWidget *empty_label = gtk_label_new(
            "📌 No Student Selected\n\n"
            "Click Search to find\n"
            "a student by Roll No");
        gtk_widget_set_halign(empty_label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(empty_label, GTK_ALIGN_CENTER);
        gtk_label_set_justify(GTK_LABEL(empty_label), GTK_JUSTIFY_CENTER);
        gtk_container_add(GTK_CONTAINER(student_card_box), empty_label);
        gtk_widget_show_all(student_card_box);
        return;
    }
    
    // Create card content
    GtkWidget *card_content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(card_content), 15);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header),
        "<span font='11' weight='bold' color='#1976D2'>📇 STUDENT ID</span>");
    gtk_box_pack_start(GTK_BOX(card_content), header, FALSE, FALSE, 0);
    
    // Separator
    gtk_box_pack_start(GTK_BOX(card_content), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), 
                       FALSE, FALSE, 5);
    
    // Roll No
    GtkWidget *roll_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *roll_label = gtk_label_new("Roll No:");
    gtk_widget_set_size_request(roll_label, 80, -1);
    char roll_str[50];
    snprintf(roll_str, sizeof(roll_str), "<b>%d</b>", current_student.roll_no);
    GtkWidget *roll_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(roll_value), roll_str);
    gtk_box_pack_start(GTK_BOX(roll_box), roll_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(roll_box), roll_value, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card_content), roll_box, FALSE, FALSE, 0);
    
    // Name
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *name_label = gtk_label_new("Name:");
    gtk_widget_set_size_request(name_label, 80, -1);
    GtkWidget *name_value = gtk_label_new(current_student.name);
    gtk_label_set_line_wrap(GTK_LABEL(name_value), TRUE);
    gtk_box_pack_start(GTK_BOX(name_box), name_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(name_box), name_value, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card_content), name_box, FALSE, FALSE, 0);
    
    // Branch
    GtkWidget *branch_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *branch_label = gtk_label_new("Branch:");
    gtk_widget_set_size_request(branch_label, 80, -1);
    GtkWidget *branch_value = gtk_label_new(current_student.branch);
    gtk_box_pack_start(GTK_BOX(branch_box), branch_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(branch_box), branch_value, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card_content), branch_box, FALSE, FALSE, 0);
    
    // Mobile
    GtkWidget *mobile_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *mobile_label = gtk_label_new("Mobile:");
    gtk_widget_set_size_request(mobile_label, 80, -1);
    GtkWidget *mobile_value = gtk_label_new(current_student.mobile);
    gtk_box_pack_start(GTK_BOX(mobile_box), mobile_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mobile_box), mobile_value, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card_content), mobile_box, FALSE, FALSE, 0);
    
    // Email
    GtkWidget *email_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *email_label = gtk_label_new("Email:");
    gtk_widget_set_size_request(email_label, 80, -1);
    GtkWidget *email_value = gtk_label_new(current_student.email);
    gtk_label_set_line_wrap(GTK_LABEL(email_value), TRUE);
    gtk_box_pack_start(GTK_BOX(email_box), email_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(email_box), email_value, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card_content), email_box, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(student_card_box), card_content);
    gtk_widget_show_all(student_card_box);
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================
static void calculate_and_update_total() {
    double total_paid = 0, total_due = 0;
    
    total_paid += g_strtod(gtk_entry_get_text(GTK_ENTRY(inst_paid_entry)), NULL);
    total_due += g_strtod(gtk_entry_get_text(GTK_ENTRY(inst_due_entry)), NULL);
    
    total_paid += g_strtod(gtk_entry_get_text(GTK_ENTRY(hostel_paid_entry)), NULL);
    total_due += g_strtod(gtk_entry_get_text(GTK_ENTRY(hostel_due_entry)), NULL);
    
    total_paid += g_strtod(gtk_entry_get_text(GTK_ENTRY(mess_paid_entry)), NULL);
    total_due += g_strtod(gtk_entry_get_text(GTK_ENTRY(mess_due_entry)), NULL);
    
    total_paid += g_strtod(gtk_entry_get_text(GTK_ENTRY(other_paid_entry)), NULL);
    total_due += g_strtod(gtk_entry_get_text(GTK_ENTRY(other_due_entry)), NULL);
    
    double total = total_paid + total_due;
    
    char total_text[100];
    snprintf(total_text, sizeof(total_text), 
             "<span font='14' weight='bold' color='#1976D2'>₹ %.2f</span>", total);
    gtk_label_set_markup(GTK_LABEL(total_label), total_text);
}

static void load_fee_data() {
    if (current_student.roll_no == 0) {
        clear_fee_form();
        return;
    }
    
    if (db_get_fee_record(current_student.roll_no, &current_fee)) {
        char text[50];
        
        snprintf(text, sizeof(text), "%d", current_fee.fee_id);
        gtk_label_set_text(GTK_LABEL(fee_id_label), text);
        
        snprintf(text, sizeof(text), "%.2f", current_fee.institute_paid);
        gtk_entry_set_text(GTK_ENTRY(inst_paid_entry), text);
        gtk_entry_set_text(GTK_ENTRY(inst_date_entry), current_fee.institute_date);
        snprintf(text, sizeof(text), "%.2f", current_fee.institute_due);
        gtk_entry_set_text(GTK_ENTRY(inst_due_entry), text);
        gtk_combo_box_set_active_id(GTK_COMBO_BOX(inst_mode_combo), current_fee.institute_mode);
        
        snprintf(text, sizeof(text), "%.2f", current_fee.hostel_paid);
        gtk_entry_set_text(GTK_ENTRY(hostel_paid_entry), text);
        gtk_entry_set_text(GTK_ENTRY(hostel_date_entry), current_fee.hostel_date);
        snprintf(text, sizeof(text), "%.2f", current_fee.hostel_due);
        gtk_entry_set_text(GTK_ENTRY(hostel_due_entry), text);
        gtk_combo_box_set_active_id(GTK_COMBO_BOX(hostel_mode_combo), current_fee.hostel_mode);
        
        snprintf(text, sizeof(text), "%.2f", current_fee.mess_paid);
        gtk_entry_set_text(GTK_ENTRY(mess_paid_entry), text);
        gtk_entry_set_text(GTK_ENTRY(mess_date_entry), current_fee.mess_date);
        snprintf(text, sizeof(text), "%.2f", current_fee.mess_due);
        gtk_entry_set_text(GTK_ENTRY(mess_due_entry), text);
        gtk_combo_box_set_active_id(GTK_COMBO_BOX(mess_mode_combo), current_fee.mess_mode);
        
        snprintf(text, sizeof(text), "%.2f", current_fee.other_paid);
        gtk_entry_set_text(GTK_ENTRY(other_paid_entry), text);
        gtk_entry_set_text(GTK_ENTRY(other_date_entry), current_fee.other_date);
        snprintf(text, sizeof(text), "%.2f", current_fee.other_due);
        gtk_entry_set_text(GTK_ENTRY(other_due_entry), text);
        gtk_combo_box_set_active_id(GTK_COMBO_BOX(other_mode_combo), current_fee.other_mode);
        
        printf("[INFO] Fee data loaded for roll_no: %d\n", current_student.roll_no);
    } else {
        char text[50];
        snprintf(text, sizeof(text), "%d (New)", current_student.roll_no);
        gtk_label_set_text(GTK_LABEL(fee_id_label), text);
        memset(&current_fee, 0, sizeof(FeeRecord));
        current_fee.roll_no = current_student.roll_no;
    }
    
    calculate_and_update_total();
}

static void clear_fee_form() {
    gtk_label_set_text(GTK_LABEL(fee_id_label), "- (No Student)");
    gtk_entry_set_text(GTK_ENTRY(inst_paid_entry), "0");
    gtk_entry_set_text(GTK_ENTRY(inst_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(inst_due_entry), "0");
    gtk_combo_box_set_active(GTK_COMBO_BOX(inst_mode_combo), 0);
    
    gtk_entry_set_text(GTK_ENTRY(hostel_paid_entry), "0");
    gtk_entry_set_text(GTK_ENTRY(hostel_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(hostel_due_entry), "0");
    gtk_combo_box_set_active(GTK_COMBO_BOX(hostel_mode_combo), 0);
    
    gtk_entry_set_text(GTK_ENTRY(mess_paid_entry), "0");
    gtk_entry_set_text(GTK_ENTRY(mess_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(mess_due_entry), "0");
    gtk_combo_box_set_active(GTK_COMBO_BOX(mess_mode_combo), 0);
    
    gtk_entry_set_text(GTK_ENTRY(other_paid_entry), "0");
    gtk_entry_set_text(GTK_ENTRY(other_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(other_due_entry), "0");
    gtk_combo_box_set_active(GTK_COMBO_BOX(other_mode_combo), 0);
    
    calculate_and_update_total();
}

// ============================================================================
// SAVE FEE DATA
// ============================================================================
void on_save_fee_clicked(GtkButton *button, gpointer data) {
    (void)button; (void)data;
    
    if (current_student.roll_no == 0) {
        g_print("[ERROR] No student selected\n");
        return;
    }
    
    FeeRecord fee = {0};
    fee.roll_no = current_student.roll_no;
    
    fee.institute_paid = g_strtod(gtk_entry_get_text(GTK_ENTRY(inst_paid_entry)), NULL);
    strcpy(fee.institute_date, gtk_entry_get_text(GTK_ENTRY(inst_date_entry)));
    fee.institute_due = g_strtod(gtk_entry_get_text(GTK_ENTRY(inst_due_entry)), NULL);
    strcpy(fee.institute_mode, gtk_combo_box_get_active_id(GTK_COMBO_BOX(inst_mode_combo)) ?: "");
    
    fee.hostel_paid = g_strtod(gtk_entry_get_text(GTK_ENTRY(hostel_paid_entry)), NULL);
    strcpy(fee.hostel_date, gtk_entry_get_text(GTK_ENTRY(hostel_date_entry)));
    fee.hostel_due = g_strtod(gtk_entry_get_text(GTK_ENTRY(hostel_due_entry)), NULL);
    strcpy(fee.hostel_mode, gtk_combo_box_get_active_id(GTK_COMBO_BOX(hostel_mode_combo)) ?: "");
    
    fee.mess_paid = g_strtod(gtk_entry_get_text(GTK_ENTRY(mess_paid_entry)), NULL);
    strcpy(fee.mess_date, gtk_entry_get_text(GTK_ENTRY(mess_date_entry)));
    fee.mess_due = g_strtod(gtk_entry_get_text(GTK_ENTRY(mess_due_entry)), NULL);
    strcpy(fee.mess_mode, gtk_combo_box_get_active_id(GTK_COMBO_BOX(mess_mode_combo)) ?: "");
    
    fee.other_paid = g_strtod(gtk_entry_get_text(GTK_ENTRY(other_paid_entry)), NULL);
    strcpy(fee.other_date, gtk_entry_get_text(GTK_ENTRY(other_date_entry)));
    fee.other_due = g_strtod(gtk_entry_get_text(GTK_ENTRY(other_due_entry)), NULL);
    strcpy(fee.other_mode, gtk_combo_box_get_active_id(GTK_COMBO_BOX(other_mode_combo)) ?: "");
    
    fee.status = 1;
    
    int result;
    if (current_fee.fee_id > 0) {
        fee.fee_id = current_fee.fee_id;
        result = db_update_fee_record(&fee);
    } else {
        result = db_save_fee_record(&fee);
    }
    
    if (result) {
        g_print("[SUCCESS] Fee saved for roll_no: %d\n", current_student.roll_no);
        memcpy(&current_fee, &fee, sizeof(FeeRecord));
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "✅ Fee Details Saved Successfully!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        g_print("[ERROR] Failed to save fee\n");
    }
}

// ============================================================================
// BUTTON CALLBACKS
// ============================================================================
void on_back_clicked(GtkButton *button, gpointer data) {
    (void)button; (void)data;
    if (search_box_visible) hide_inline_search_box();
}

void on_refresh_clicked(GtkButton *button, gpointer data) {
    (void)button; (void)data;
    load_fee_data();
}

void on_search_student_fee_clicked(GtkButton *button, gpointer data) {
    (void)button; (void)data;
    if (!search_box_visible) show_inline_search_box();
    else hide_inline_search_box();
}

static void show_inline_search_box() {
    if (search_entry == NULL) {
        search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_halign(search_box, GTK_ALIGN_CENTER);
        
        GtkWidget *label = gtk_label_new("🔍 Roll No: ");
        gtk_box_pack_start(GTK_BOX(search_box), label, FALSE, FALSE, 5);
        
        search_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "1001");
        gtk_widget_set_size_request(search_entry, 150, 35);
        gtk_entry_set_max_length(GTK_ENTRY(search_entry), 6);
        gtk_box_pack_start(GTK_BOX(search_box), search_entry, FALSE, FALSE, 5);
        
        GtkWidget *go_btn = gtk_button_new_with_label("Search");
        g_signal_connect(go_btn, "clicked", G_CALLBACK(on_search_go_clicked), NULL);
        gtk_widget_set_size_request(go_btn, 100, 35);
        gtk_box_pack_start(GTK_BOX(search_box), go_btn, FALSE, FALSE, 5);
        
        if (main_box) {
            gtk_box_pack_start(GTK_BOX(main_box), search_box, FALSE, FALSE, 10);
        }
    }
    gtk_widget_show_all(search_box);
    search_box_visible = TRUE;
    gtk_widget_grab_focus(search_entry);
}

static void hide_inline_search_box() {
    if (search_box) {
        gtk_widget_hide(search_box);
        search_box_visible = FALSE;
    }
}

static void on_search_go_clicked(GtkButton *button, gpointer data) {
    (void)button; (void)data;
    
    const char *roll_str = gtk_entry_get_text(GTK_ENTRY(search_entry));
    if (strlen(roll_str) == 0) return;
    
    int roll_no = atoi(roll_str);
    if (db_get_student_for_card(roll_no, &current_student)) {
        refresh_student_card_display();
        load_fee_data();
        printf("[SUCCESS] Student found: %s\n", current_student.name);
    } else {
        printf("[ERROR] Student not found\n");
    }
    hide_inline_search_box();
}

// ============================================================================
// CREATE FEE UI WITH TWO-COLUMN LAYOUT
// ============================================================================
void create_fee_ui(GtkWidget *container) {
    printf("[INFO] Creating Fee Form UI\n");
    
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(main_box, 20);
    gtk_widget_set_margin_end(main_box, 20);
    gtk_widget_set_margin_top(main_box, 20);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), main_box);
    gtk_container_add(GTK_CONTAINER(container), scrolled);
    
    // HEADER
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header),
        "<span font='18' weight='bold'>💰 Fee Management</span>");
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);
    
    // BUTTON BAR
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_pack_start(GTK_BOX(main_box), btn_box, FALSE, FALSE, 10);
    
    GtkWidget *left_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(btn_box), left_btns, FALSE, FALSE, 0);
    
    GtkWidget *back_btn = gtk_button_new_with_label("⬅️ Back");
    gtk_widget_set_size_request(back_btn, 100, 40);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_back_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(left_btns), back_btn, FALSE, FALSE, 0);
    
    GtkWidget *refresh_btn = gtk_button_new_with_label("🔄 Refresh");
    gtk_widget_set_size_request(refresh_btn, 100, 40);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(left_btns), refresh_btn, FALSE, FALSE, 0);
    
    GtkWidget *right_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(right_btns, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(btn_box), right_btns, TRUE, TRUE, 0);
    
    GtkWidget *search_btn = gtk_button_new_with_label("🔍 Search");
    gtk_widget_set_size_request(search_btn, 120, 40);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_student_fee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(right_btns), search_btn, FALSE, FALSE, 0);
    
    // TWO-COLUMN LAYOUT
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);
    
    // LEFT SIDE: STUDENT CARD
    GtkWidget *card_frame = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(card_frame), "📇 Student Information");
    student_card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(card_frame), student_card_box);
    gtk_widget_set_size_request(card_frame, 280, -1);
    gtk_box_pack_start(GTK_BOX(content_box), card_frame, FALSE, TRUE, 0);
    
    // Show empty message initially
    GtkWidget *empty = gtk_label_new("📌 No Student Selected\n\nClick Search →");
    gtk_widget_set_halign(empty, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(empty, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(student_card_box), empty);
    
    // RIGHT SIDE: FEE FORM
    GtkWidget *form_frame = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(form_frame), "📋 Fee Details Form");
    gtk_box_pack_start(GTK_BOX(content_box), form_frame, TRUE, TRUE, 0);
    
    GtkWidget *form_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(form_scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(form_frame), form_scroll);
    
    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 15);
    gtk_container_add(GTK_CONTAINER(form_scroll), form_box);
    
    // Row 1: Fee ID
    GtkWidget *row1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    GtkWidget *label1 = gtk_label_new("Fee ID");
    gtk_widget_set_size_request(label1, 120, -1);
    fee_id_label = gtk_label_new("- (No Student)");
    gtk_box_pack_start(GTK_BOX(row1), label1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row1), fee_id_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), row1, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
    
    // Row 2: Headers
    GtkWidget *header_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    GtkWidget *h1 = gtk_label_new("<b>Fee Type</b>");
    gtk_label_set_use_markup(GTK_LABEL(h1), TRUE);
    gtk_widget_set_size_request(h1, 140, -1);
    GtkWidget *h2 = gtk_label_new("<b>Paid (₹)</b>");
    gtk_label_set_use_markup(GTK_LABEL(h2), TRUE);
    gtk_widget_set_size_request(h2, 80, -1);
    GtkWidget *h3 = gtk_label_new("<b>Date</b>");
    gtk_label_set_use_markup(GTK_LABEL(h3), TRUE);
    gtk_widget_set_size_request(h3, 100, -1);
    GtkWidget *h4 = gtk_label_new("<b>Due (₹)</b>");
    gtk_label_set_use_markup(GTK_LABEL(h4), TRUE);
    gtk_widget_set_size_request(h4, 80, -1);
    GtkWidget *h5 = gtk_label_new("<b>Mode</b>");
    gtk_label_set_use_markup(GTK_LABEL(h5), TRUE);
    
    gtk_box_pack_start(GTK_BOX(header_row), h1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h4, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h5, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), header_row, FALSE, FALSE, 5);
    
    // Create combo box model
    GtkListStore *mode_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;
    gtk_list_store_append(mode_store, &iter);
    gtk_list_store_set(mode_store, &iter, 0, "DD", 1, "DD", -1);
    gtk_list_store_append(mode_store, &iter);
    gtk_list_store_set(mode_store, &iter, 0, "Cheque", 1, "Cheque", -1);
    gtk_list_store_append(mode_store, &iter);
    gtk_list_store_set(mode_store, &iter, 0, "Online", 1, "Online", -1);
    
    // Macro for creating fee rows
    #define CREATE_FEE_ROW(fee_name, paid_ptr, date_ptr, due_ptr, mode_ptr) \
    do { \
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20); \
        GtkWidget *label = gtk_label_new(fee_name); \
        gtk_widget_set_size_request(label, 140, -1); \
        paid_ptr = gtk_entry_new(); \
        gtk_entry_set_text(GTK_ENTRY(paid_ptr), "0"); \
        gtk_widget_set_size_request(paid_ptr, 80, -1); \
        g_signal_connect(paid_ptr, "changed", G_CALLBACK(calculate_and_update_total), NULL); \
        date_ptr = gtk_entry_new(); \
        gtk_entry_set_placeholder_text(GTK_ENTRY(date_ptr), "YYYY-MM-DD"); \
        gtk_widget_set_size_request(date_ptr, 100, -1); \
        due_ptr = gtk_entry_new(); \
        gtk_entry_set_text(GTK_ENTRY(due_ptr), "0"); \
        gtk_widget_set_size_request(due_ptr, 80, -1); \
        g_signal_connect(due_ptr, "changed", G_CALLBACK(calculate_and_update_total), NULL); \
        mode_ptr = gtk_combo_box_new_with_model(GTK_TREE_MODEL(mode_store)); \
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new(); \
        gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(mode_ptr), renderer, TRUE); \
        gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(mode_ptr), renderer, "text", 0, NULL); \
        gtk_box_pack_start(GTK_BOX(row), label, FALSE, FALSE, 0); \
        gtk_box_pack_start(GTK_BOX(row), paid_ptr, FALSE, FALSE, 0); \
        gtk_box_pack_start(GTK_BOX(row), date_ptr, FALSE, FALSE, 0); \
        gtk_box_pack_start(GTK_BOX(row), due_ptr, FALSE, FALSE, 0); \
        gtk_box_pack_start(GTK_BOX(row), mode_ptr, FALSE, FALSE, 0); \
        gtk_box_pack_start(GTK_BOX(form_box), row, FALSE, FALSE, 0); \
    } while(0)
    
    CREATE_FEE_ROW("💳 Institute Fee", inst_paid_entry, inst_date_entry, inst_due_entry, inst_mode_combo);
    CREATE_FEE_ROW("🏠 Hostel Fee", hostel_paid_entry, hostel_date_entry, hostel_due_entry, hostel_mode_combo);
    CREATE_FEE_ROW("🍽️ Mess Fee", mess_paid_entry, mess_date_entry, mess_due_entry, mess_mode_combo);
    CREATE_FEE_ROW("📌 Other Charges", other_paid_entry, other_date_entry, other_due_entry, other_mode_combo);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
    
    // Total Row
    GtkWidget *total_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    GtkWidget *total_label_text = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(total_label_text), "<b>💰 TOTAL FEE</b>");
    gtk_widget_set_size_request(total_label_text, 140, -1);
    total_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(total_label), "<span font='12' weight='bold' color='#1976D2'>₹ 0.00</span>");
    gtk_box_pack_start(GTK_BOX(total_row), total_label_text, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(total_row), gtk_label_new(""), FALSE, FALSE, 80);
    gtk_box_pack_start(GTK_BOX(total_row), gtk_label_new(""), FALSE, FALSE, 100);
    gtk_box_pack_start(GTK_BOX(total_row), gtk_label_new(""), FALSE, FALSE, 80);
    gtk_box_pack_start(GTK_BOX(total_row), total_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), total_row, FALSE, FALSE, 10);
    
    // SAVE BUTTON
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(form_box), button_box, FALSE, FALSE, 20);
    
    GtkWidget *save_btn = gtk_button_new_with_label("💾 Save Fee Details");
    gtk_widget_set_size_request(save_btn, 200, 45);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_fee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), save_btn, FALSE, FALSE, 0);
    
    gtk_widget_show_all(container);
    printf("[INFO] Fee Form UI created successfully\n");
}
