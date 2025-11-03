#ifndef EMPLOYEE_UI_H
#define EMPLOYEE_UI_H

#include <gtk/gtk.h>

void create_employee_ui(GtkWidget *container);
void refresh_employee_list();
void on_add_employee_clicked(GtkButton *button, gpointer user_data);
void on_edit_employee_clicked(GtkButton *button, gpointer user_data);
void on_delete_employee_clicked(GtkButton *button, gpointer user_data);

#endif