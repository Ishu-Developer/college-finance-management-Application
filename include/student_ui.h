#include <gtk/gtk.h>
#ifndef STUDENT_UI_H
#define STUDENT_UI_H

void create_student_ui(GtkWidget *container);

void on_add_student_clicked(GtkButton *button, gpointer user_data);

void on_view_students_clicked(GtkButton *button, gpointer user_data);

void on_search_student_clicked(GtkButton *button, gpointer user_data);

void on_edit_student_clicked(GtkButton *button, gpointer user_data);

void on_delete_student_clicked(GtkButton *button, gpointer user_data);


#endif