#ifndef FEE_UI_H
#define FEE_UI_H

#include <gtk/gtk.h>
#include <sqlite3.h>
#include "database.h"  // Include to get Fee and StudentIDCard structures


/**
 * @brief Create fee management UI
 * @param container Parent GTK container
 */
void create_fee_ui(GtkWidget *container);


/**
 * @brief Refresh fee table display
 */
void refresh_fee_table();


/**
 * @brief Refresh student ID card display
 */
void refresh_student_card();


/**
 * @brief Search student by roll number
 * @param button GtkButton widget
 * @param user_data User data (unused)
 */
void on_search_student_fee_clicked(GtkButton *button, gpointer user_data);


/**
 * @brief Handle refresh button click
 * @param button GtkButton widget
 * @param user_data User data (unused)
 */
void on_refresh_fee_clicked(GtkButton *button, gpointer user_data);


#endif // FEE_UI_H