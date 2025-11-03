/**
 * @file src/ui/student_ui.c
 * @brief Student Management UI Module
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include "../../include/student_ui.h"

/**
 * @brief Create and setup student management UI
 */
void create_student_ui(GtkWidget *container) {
    printf("[INFO] Creating Student Management UI\n");

    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span font='16' weight='bold'>👨‍🎓 Student Management</span>");
    gtk_box_pack_start(GTK_BOX(container), title, FALSE, FALSE, 10);

    // Placeholder
    GtkWidget *label = gtk_label_new(
        "Student Management Features:\n"
        "• Add/Edit/Delete Students\n"
        "• Search by Roll No or Name\n"
        "• View Student Details\n"
        "• Track Student History\n"
        "\n(Coming Soon - Phase 1 Days 2-5)");
    gtk_box_pack_start(GTK_BOX(container), label, TRUE, TRUE, 20);
}
