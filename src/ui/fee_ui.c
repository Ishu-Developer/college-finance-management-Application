#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "../../include/fee_ui.h"
#include "../../include/database.h"

// Global variables
static GtkWidget *fee_table = NULL;
static GtkWidget *form_box = NULL;
static GtkWidget *error_label = NULL;

// Form widgets
static GtkWidget *form_roll_entry = NULL;
static GtkWidget *form_inst_paid_entry = NULL;
static GtkWidget *form_inst_date_entry = NULL;
static GtkWidget *form_inst_mode_entry = NULL;

static GtkWidget *form_hostel_paid_entry = NULL;
static GtkWidget *form_hostel_date_entry = NULL;
static GtkWidget *form_hostel_mode_entry = NULL;

static GtkWidget *form_mess_paid_entry = NULL;
static GtkWidget *form_mess_date_entry = NULL;
static GtkWidget *form_mess_mode_entry = NULL;

static GtkWidget *form_other_paid_entry = NULL;
static GtkWidget *form_other_date_entry = NULL;
static GtkWidget *form_other_mode_entry = NULL;

static FeeRecord current_fee_form;

// ============================================================================
// Helper Functions
// ============================================================================

static void clear_form(void) {
    memset(&current_fee_form, 0, sizeof(FeeRecord));
    
    gtk_entry_set_text(GTK_ENTRY(form_roll_entry), "");
    gtk_entry_set_text(GTK_ENTRY(form_inst_paid_entry), "0.00");
    gtk_entry_set_text(GTK_ENTRY(form_inst_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(form_inst_mode_entry), "");
    
    gtk_entry_set_text(GTK_ENTRY(form_hostel_paid_entry), "0.00");
    gtk_entry_set_text(GTK_ENTRY(form_hostel_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(form_hostel_mode_entry), "");
    
    gtk_entry_set_text(GTK_ENTRY(form_mess_paid_entry), "0.00");
    gtk_entry_set_text(GTK_ENTRY(form_mess_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(form_mess_mode_entry), "");
    
    gtk_entry_set_text(GTK_ENTRY(form_other_paid_entry), "0.00");
    gtk_entry_set_text(GTK_ENTRY(form_other_date_entry), "");
    gtk_entry_set_text(GTK_ENTRY(form_other_mode_entry), "");
    
    gtk_widget_hide(error_label);
}

// ✅ FIXED: Remove all *_due field references
static void load_add_fee_data(void) {
    char text[64];

    printf("[INFO] Loading fee data into form\n");

    // Institute paid ONLY
    snprintf(text, sizeof(text), "%.2f", current_fee_form.institute_paid);
    gtk_entry_set_text(GTK_ENTRY(form_inst_paid_entry), text);

    // Hostel paid ONLY
    snprintf(text, sizeof(text), "%.2f", current_fee_form.hostel_paid);
    gtk_entry_set_text(GTK_ENTRY(form_hostel_paid_entry), text);

    // Mess paid ONLY
    snprintf(text, sizeof(text), "%.2f", current_fee_form.mess_paid);
    gtk_entry_set_text(GTK_ENTRY(form_mess_paid_entry), text);

    // Other paid ONLY
    snprintf(text, sizeof(text), "%.2f", current_fee_form.other_paid);
    gtk_entry_set_text(GTK_ENTRY(form_other_paid_entry), text);

    // Set dates and modes if they exist
    gtk_entry_set_text(GTK_ENTRY(form_inst_date_entry), 
        current_fee_form.institute_date[0] != '\0' ? current_fee_form.institute_date : "");
    gtk_entry_set_text(GTK_ENTRY(form_inst_mode_entry),
        current_fee_form.institute_mode[0] != '\0' ? current_fee_form.institute_mode : "");

    gtk_entry_set_text(GTK_ENTRY(form_hostel_date_entry),
        current_fee_form.hostel_date[0] != '\0' ? current_fee_form.hostel_date : "");
    gtk_entry_set_text(GTK_ENTRY(form_hostel_mode_entry),
        current_fee_form.hostel_mode[0] != '\0' ? current_fee_form.hostel_mode : "");

    gtk_entry_set_text(GTK_ENTRY(form_mess_date_entry),
        current_fee_form.mess_date[0] != '\0' ? current_fee_form.mess_date : "");
    gtk_entry_set_text(GTK_ENTRY(form_mess_mode_entry),
        current_fee_form.mess_mode[0] != '\0' ? current_fee_form.mess_mode : "");

    gtk_entry_set_text(GTK_ENTRY(form_other_date_entry),
        current_fee_form.other_date[0] != '\0' ? current_fee_form.other_date : "");
    gtk_entry_set_text(GTK_ENTRY(form_other_mode_entry),
        current_fee_form.other_mode[0] != '\0' ? current_fee_form.other_mode : "");
}

static void refresh_fee_table(void) {
    printf("[INFO] Refreshing fee table\n");

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(fee_table)));
    if (!store) {
        printf("[ERROR] Fee table store is NULL\n");
        return;
    }

    gtk_list_store_clear(store);

    FeeTableRow *rows = NULL;
    int row_count = db_get_all_fee_summary_rows(&rows);

    if (row_count == 0) {
        printf("[INFO] No fee records found\n");
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "—", 1, "—", 2, "—", 3, "No records", 4, "—", 5, "—", 6, "—",
            -1);
        return;
    }

    for (int i = 0; i < row_count; i++) {
        FeeTableRow *row = &rows[i];
        
        char inst_str[20], hostel_str[20], mess_str[20], other_str[20], total_str[20];
        snprintf(inst_str, sizeof(inst_str), "%.2f", row->institute_paid);
        snprintf(hostel_str, sizeof(hostel_str), "%.2f", row->hostel_paid);
        snprintf(mess_str, sizeof(mess_str), "%.2f", row->mess_paid);
        snprintf(other_str, sizeof(other_str), "%.2f", row->other_paid);
        snprintf(total_str, sizeof(total_str), "%.2f", row->total_paid);

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, "✏️", 1, "🗑️", 2, row->roll_no, 3, row->student_name,
            4, inst_str, 5, hostel_str, 6, mess_str, 7, other_str,
            8, total_str,
            -1);
    }

    if (rows) {
        db_free_fee_table_rows(rows);
    }

    printf("[INFO] Loaded %d fee records\n", row_count);
}

// ============================================================================
// Button Callbacks
// ============================================================================

// ✅ FIXED: Remove all *_due field assignments and fix status assignment
static void on_form_save_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;

    printf("[INFO] Save button clicked\n");

    FeeRecord fee;
    memset(&fee, 0, sizeof(FeeRecord));

    // Get roll number
    const char *roll = gtk_entry_get_text(GTK_ENTRY(form_roll_entry));
    if (!roll || strlen(roll) == 0) {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Please enter roll number");
        gtk_widget_show(error_label);
        return;
    }

    g_strlcpy(fee.roll_no, roll, sizeof(fee.roll_no));

    // Institute fee (paid only)
    fee.institute_paid = g_strtod(
        gtk_entry_get_text(GTK_ENTRY(form_inst_paid_entry)), NULL);
    const char *inst_date = gtk_entry_get_text(GTK_ENTRY(form_inst_date_entry));
    const char *inst_mode = gtk_entry_get_text(GTK_ENTRY(form_inst_mode_entry));
    if (inst_date) g_strlcpy(fee.institute_date, inst_date, sizeof(fee.institute_date));
    if (inst_mode) g_strlcpy(fee.institute_mode, inst_mode, sizeof(fee.institute_mode));

    // Hostel fee (paid only)
    fee.hostel_paid = g_strtod(
        gtk_entry_get_text(GTK_ENTRY(form_hostel_paid_entry)), NULL);
    const char *host_date = gtk_entry_get_text(GTK_ENTRY(form_hostel_date_entry));
    const char *host_mode = gtk_entry_get_text(GTK_ENTRY(form_hostel_mode_entry));
    if (host_date) g_strlcpy(fee.hostel_date, host_date, sizeof(fee.hostel_date));
    if (host_mode) g_strlcpy(fee.hostel_mode, host_mode, sizeof(fee.hostel_mode));

    // Mess fee (paid only)
    fee.mess_paid = g_strtod(
        gtk_entry_get_text(GTK_ENTRY(form_mess_paid_entry)), NULL);
    const char *mess_date = gtk_entry_get_text(GTK_ENTRY(form_mess_date_entry));
    const char *mess_mode = gtk_entry_get_text(GTK_ENTRY(form_mess_mode_entry));
    if (mess_date) g_strlcpy(fee.mess_date, mess_date, sizeof(fee.mess_date));
    if (mess_mode) g_strlcpy(fee.mess_mode, mess_mode, sizeof(fee.mess_mode));

    // Other fee (paid only)
    fee.other_paid = g_strtod(
        gtk_entry_get_text(GTK_ENTRY(form_other_paid_entry)), NULL);
    const char *other_date = gtk_entry_get_text(GTK_ENTRY(form_other_date_entry));
    const char *other_mode = gtk_entry_get_text(GTK_ENTRY(form_other_mode_entry));
    if (other_date) g_strlcpy(fee.other_date, other_date, sizeof(fee.other_date));
    if (other_mode) g_strlcpy(fee.other_mode, other_mode, sizeof(fee.other_mode));

    // Calculate total paid
    fee.total_paid = fee.institute_paid + fee.hostel_paid + 
                     fee.mess_paid + fee.other_paid;

    // ✅ FIXED: status is char array, not int
    g_strlcpy(fee.status, "Submitted", sizeof(fee.status));

    printf("[INFO] Saving fee record for roll: %s (Total: %.2f)\n", 
           fee.roll_no, fee.total_paid);

    // Save to database
    if (db_save_fee_record(&fee)) {
        printf("[SUCCESS] Fee record saved successfully\n");
        gtk_label_set_text(GTK_LABEL(error_label), 
            "✅ Fee record saved successfully!");
        gtk_widget_show(error_label);
        
        clear_form();
        refresh_fee_table();
    } else {
        printf("[ERROR] Failed to save fee record\n");
        gtk_label_set_text(GTK_LABEL(error_label), 
            "❌ Database error: Failed to save fee record");
        gtk_widget_show(error_label);
    }
}

static void on_form_cancel_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;

    printf("[INFO] Cancel button clicked\n");
    clear_form();
    gtk_widget_hide(form_box);
}

// ✅ FIXED: Remove 'static' to match fee_ui.h declaration
void on_add_fee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;

    printf("[INFO] Add fee button clicked\n");

    if (gtk_widget_get_visible(form_box)) {
        gtk_widget_hide(form_box);
    } else {
        memset(&current_fee_form, 0, sizeof(FeeRecord));
        load_add_fee_data();
        gtk_widget_show_all(form_box);
        gtk_widget_hide(error_label);
        gtk_widget_grab_focus(form_roll_entry);
    }
}

// ✅ FIXED: Remove 'static' to match fee_ui.h declaration
void on_refresh_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;

    printf("[INFO] Refresh button clicked\n");
    refresh_fee_table();
}

static void on_delete_fee_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;

    printf("[INFO] Delete button clicked\n");

    GtkTreeView *tree = GTK_TREE_VIEW(fee_table);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree);
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *roll_no = NULL;
        gtk_tree_model_get(model, &iter, 2, &roll_no, -1);

        if (roll_no && strlen(roll_no) > 0) {
            if (db_delete_fee_record(roll_no)) {
                printf("[SUCCESS] Fee record deleted: %s\n", roll_no);
                gtk_label_set_text(GTK_LABEL(error_label), 
                    "✅ Fee record deleted successfully!");
                gtk_widget_show(error_label);
                refresh_fee_table();
            } else {
                printf("[ERROR] Failed to delete fee record\n");
                gtk_label_set_text(GTK_LABEL(error_label), 
                    "❌ Failed to delete fee record");
                gtk_widget_show(error_label);
            }
        }

        g_free(roll_no);
    } else {
        gtk_label_set_text(GTK_LABEL(error_label), "❌ Please select a record");
        gtk_widget_show(error_label);
    }
}

// ============================================================================
// UI Creation
// ============================================================================

void create_fee_ui(GtkWidget *container) {
    printf("[INFO] Creating Fee Management UI\n");

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(container), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);

    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='20' weight='bold'>💰 Fee Management</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);

    // Button Box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 5);

    GtkWidget *add_btn = gtk_button_new_with_label("➕ Add Fee");
    gtk_widget_set_size_request(add_btn, 120, 40);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_fee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), add_btn, FALSE, FALSE, 0);

    GtkWidget *refresh_btn = gtk_button_new_with_label("🔄 Refresh");
    gtk_widget_set_size_request(refresh_btn, 120, 40);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), refresh_btn, FALSE, FALSE, 0);

    GtkWidget *delete_btn = gtk_button_new_with_label("🗑️ Delete");
    gtk_widget_set_size_request(delete_btn, 120, 40);
    g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_fee_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), delete_btn, FALSE, FALSE, 0);

    // ====================================================================
    // FORM BOX (Hidden by default)
    // ====================================================================

    form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 10);
    gtk_widget_set_name(form_box, "form_box");
    gtk_box_pack_start(GTK_BOX(main_box), form_box, FALSE, FALSE, 0);
    gtk_widget_hide(form_box);

    GtkWidget *form_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(form_title),
        "<span font='12' weight='bold'>➕ Add Fee Record</span>");
    gtk_box_pack_start(GTK_BOX(form_box), form_title, FALSE, FALSE, 0);

    // Error label
    error_label = gtk_label_new("");
    gtk_label_set_use_markup(GTK_LABEL(error_label), TRUE);
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(form_box), error_label, FALSE, FALSE, 0);
    gtk_widget_hide(error_label);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_box_pack_start(GTK_BOX(form_box), grid, FALSE, FALSE, 0);

    // Row 0: Roll Number
    GtkWidget *roll_label = gtk_label_new("Roll Number:");
    gtk_widget_set_halign(roll_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), roll_label, 0, 0, 1, 1);
    form_roll_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_roll_entry), "2408400100031");
    gtk_grid_attach(GTK_GRID(grid), form_roll_entry, 1, 0, 3, 1);

    // Row 1: Institute Fee - PAID ONLY
    GtkWidget *inst_paid_label = gtk_label_new("Institute Paid:");
    gtk_widget_set_halign(inst_paid_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), inst_paid_label, 0, 1, 1, 1);
    form_inst_paid_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_inst_paid_entry), "0.00");
    gtk_grid_attach(GTK_GRID(grid), form_inst_paid_entry, 1, 1, 1, 1);

    GtkWidget *inst_date_label = gtk_label_new("Date:");
    gtk_widget_set_halign(inst_date_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), inst_date_label, 2, 1, 1, 1);
    form_inst_date_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_inst_date_entry), "DD-MM-YYYY");
    gtk_grid_attach(GTK_GRID(grid), form_inst_date_entry, 3, 1, 1, 1);

    // Row 2: Institute Mode
    GtkWidget *inst_mode_label = gtk_label_new("Mode:");
    gtk_widget_set_halign(inst_mode_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), inst_mode_label, 0, 2, 1, 1);
    form_inst_mode_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_inst_mode_entry), "DD/Cheque/Online");
    gtk_grid_attach(GTK_GRID(grid), form_inst_mode_entry, 1, 2, 3, 1);

    // Row 3: Hostel Fee - PAID ONLY
    GtkWidget *hostel_paid_label = gtk_label_new("Hostel Paid:");
    gtk_widget_set_halign(hostel_paid_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), hostel_paid_label, 0, 3, 1, 1);
    form_hostel_paid_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_hostel_paid_entry), "0.00");
    gtk_grid_attach(GTK_GRID(grid), form_hostel_paid_entry, 1, 3, 1, 1);

    GtkWidget *hostel_date_label = gtk_label_new("Date:");
    gtk_widget_set_halign(hostel_date_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), hostel_date_label, 2, 3, 1, 1);
    form_hostel_date_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_hostel_date_entry), "DD-MM-YYYY");
    gtk_grid_attach(GTK_GRID(grid), form_hostel_date_entry, 3, 3, 1, 1);

    // Row 4: Hostel Mode
    GtkWidget *hostel_mode_label = gtk_label_new("Mode:");
    gtk_widget_set_halign(hostel_mode_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), hostel_mode_label, 0, 4, 1, 1);
    form_hostel_mode_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_hostel_mode_entry), "DD/Cheque/Online");
    gtk_grid_attach(GTK_GRID(grid), form_hostel_mode_entry, 1, 4, 3, 1);

    // Row 5: Mess Fee - PAID ONLY
    GtkWidget *mess_paid_label = gtk_label_new("Mess Paid:");
    gtk_widget_set_halign(mess_paid_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), mess_paid_label, 0, 5, 1, 1);
    form_mess_paid_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_mess_paid_entry), "0.00");
    gtk_grid_attach(GTK_GRID(grid), form_mess_paid_entry, 1, 5, 1, 1);

    GtkWidget *mess_date_label = gtk_label_new("Date:");
    gtk_widget_set_halign(mess_date_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), mess_date_label, 2, 5, 1, 1);
    form_mess_date_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_mess_date_entry), "DD-MM-YYYY");
    gtk_grid_attach(GTK_GRID(grid), form_mess_date_entry, 3, 5, 1, 1);

    // Row 6: Mess Mode
    GtkWidget *mess_mode_label = gtk_label_new("Mode:");
    gtk_widget_set_halign(mess_mode_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), mess_mode_label, 0, 6, 1, 1);
    form_mess_mode_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_mess_mode_entry), "DD/Cheque/Online");
    gtk_grid_attach(GTK_GRID(grid), form_mess_mode_entry, 1, 6, 3, 1);

    // Row 7: Other Fee - PAID ONLY
    GtkWidget *other_paid_label = gtk_label_new("Other Paid:");
    gtk_widget_set_halign(other_paid_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), other_paid_label, 0, 7, 1, 1);
    form_other_paid_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_other_paid_entry), "0.00");
    gtk_grid_attach(GTK_GRID(grid), form_other_paid_entry, 1, 7, 1, 1);

    GtkWidget *other_date_label = gtk_label_new("Date:");
    gtk_widget_set_halign(other_date_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), other_date_label, 2, 7, 1, 1);
    form_other_date_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_other_date_entry), "DD-MM-YYYY");
    gtk_grid_attach(GTK_GRID(grid), form_other_date_entry, 3, 7, 1, 1);

    // Row 8: Other Mode
    GtkWidget *other_mode_label = gtk_label_new("Mode:");
    gtk_widget_set_halign(other_mode_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), other_mode_label, 0, 8, 1, 1);
    form_other_mode_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(form_other_mode_entry), "DD/Cheque/Online");
    gtk_grid_attach(GTK_GRID(grid), form_other_mode_entry, 1, 8, 3, 1);

    // Form buttons
    GtkWidget *form_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(form_box), form_button_box, FALSE, FALSE, 5);

    GtkWidget *save_btn = gtk_button_new_with_label("💾 Save");
    gtk_widget_set_size_request(save_btn, 100, 35);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_form_save_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(form_button_box), save_btn, FALSE, FALSE, 0);

    GtkWidget *cancel_btn = gtk_button_new_with_label("❌ Cancel");
    gtk_widget_set_size_request(cancel_btn, 100, 35);
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_form_cancel_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(form_button_box), cancel_btn, FALSE, FALSE, 0);

    // ====================================================================
    // FEE TABLE
    // ====================================================================

    GtkWidget *table_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(table_label), "<span weight='bold'>📋 Fee Records</span>");
    gtk_widget_set_halign(table_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(main_box), table_label, FALSE, FALSE, 5);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);

    GtkListStore *store = gtk_list_store_new(9,
        G_TYPE_STRING,  // 0: Edit
        G_TYPE_STRING,  // 1: Delete
        G_TYPE_STRING,  // 2: Roll No
        G_TYPE_STRING,  // 3: Name
        G_TYPE_STRING,  // 4: Institute
        G_TYPE_STRING,  // 5: Hostel
        G_TYPE_STRING,  // 6: Mess
        G_TYPE_STRING,  // 7: Other
        G_TYPE_STRING   // 8: Total
    );

    fee_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(G_OBJECT(store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(fee_table), GTK_TREE_VIEW_GRID_LINES_BOTH);
    gtk_container_add(GTK_CONTAINER(scroll), fee_table);

    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col;

    const char *col_titles[] = {
        "✏️", "🗑️", "Roll No", "Name", "Institute", "Hostel", "Mess", "Other", "Total"
    };
    int col_widths[] = {40, 40, 110, 120, 90, 90, 90, 90, 100};

    for (int i = 0; i < 9; i++) {
        col = gtk_tree_view_column_new_with_attributes(col_titles[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_fixed_width(col, col_widths[i]);
        gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
        gtk_tree_view_append_column(GTK_TREE_VIEW(fee_table), col);
    }

    gtk_widget_show_all(container);
    gtk_widget_hide(form_box);
    gtk_widget_hide(error_label);

    printf("[INFO] Fee Management UI created successfully\n");

    refresh_fee_table();
}