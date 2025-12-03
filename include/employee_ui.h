#ifndef EMPLOYEE_UI_H
#define EMPLOYEE_UI_H

#include <gtk/gtk.h>

// UI Functions
void create_employee_ui(GtkWidget *container);
void refresh_employee_list();

// Button callbacks
void on_add_employee_clicked(GtkButton *button, gpointer user_data);
void on_save_employee_clicked(GtkButton *button, gpointer user_data);
void on_refresh_employee_clicked(GtkButton *button, gpointer user_data);
void on_search_employee_clicked(GtkButton *button, gpointer user_data);
void on_edit_employee_clicked(GtkButton *button, gpointer user_data);
void on_delete_employee_clicked(GtkButton *button, gpointer user_data);

#endif 