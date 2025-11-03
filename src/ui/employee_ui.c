/**
 * @file src/ui/employee_ui.c
 * @brief Employee Management UI Module
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include "../../include/employee_ui.h"

/**
 * @brief Create and setup employee management UI
 */
void create_employee_ui(GtkWidget *container) {
    printf("[INFO] Creating Employee Management UI\n");

    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='16' weight='bold'>👨‍💼 Employee Management</span>");
    gtk_box_pack_start(GTK_BOX(container), title, FALSE, FALSE, 10);

    // Placeholder
    GtkWidget *label = gtk_label_new(
        "Employee Management Features:\n"
        "• Add/Edit/Delete Employees\n"
        "• Manage Employee Details\n"
        "• Track Employment History\n"
        "• Salary Information\n"
        "\n(Coming Soon - Phase 1 Days 2-5)");
    gtk_box_pack_start(GTK_BOX(container), label, TRUE, TRUE, 20);
}