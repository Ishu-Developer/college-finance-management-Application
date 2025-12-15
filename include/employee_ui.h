#ifndef EMPLOYEE_UI_H
#define EMPLOYEE_UI_H

#include <gtk/gtk.h>
#include "database.h"

// Main UI creation function
void create_employee_ui(GtkWidget *container);

// Callback declarations - MATCH your .c implementation
void on_edit_employee_clicked(GtkCellRenderer *renderer, gchar *path_str, gpointer user_data);
void on_delete_employee_clicked(GtkCellRenderer *renderer, gchar *path_str, gpointer user_data);
void on_add_employee_clicked(GtkButton *button, gpointer user_data);
void on_save_employee_clicked(GtkButton *button, gpointer user_data);
void on_search_employee_clicked(GtkButton *button, gpointer user_data);
void refresh_employee_list(void);
void clear_employee_form(void);

#endif // EMPLOYEE_UI_H
