/**
 * @file src/ui/fee_ui.c
 * @brief Fee Management UI Module
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include "../../include/fee_ui.h"

/**
 * @brief Create and setup fee management UI
 */
void create_fee_ui(GtkWidget *container) {
    printf("[INFO] Creating Fee Management UI\n");

    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='16' weight='bold'>💰 Fee Management</span>");
    gtk_box_pack_start(GTK_BOX(container), title, FALSE, FALSE, 10);

    // Placeholder
    GtkWidget *label = gtk_label_new(
        "Fee Management Features:\n"
        "• Record Student Fees\n"
        "• Track Payment Status\n"
        "• Generate Fee Receipts\n"
        "• Fee Collection Reports\n"
        "• Payment Mode Tracking\n"
        "\n(Coming Soon - Phase 1 Days 2-5)");
    gtk_box_pack_start(GTK_BOX(container), label, TRUE, TRUE, 20);
}