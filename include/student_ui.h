#ifndef STUDENT_UI_H
#define STUDENT_UI_H

#include <gtk/gtk.h>

/**
 * @brief Create student management UI
 * @param container Parent container widget
 */
void create_student_ui(GtkWidget *container);

/**
 * @brief Refresh student list from database
 */
void refresh_student_list();

/**
 * @brief Callback when add student button clicked
 */
void on_add_student_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback when edit student button clicked
 */
void on_edit_student_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback when delete student button clicked
 */
void on_delete_student_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback for search functionality
 */
void on_search_student_clicked(GtkButton *button, gpointer user_data);

#endif