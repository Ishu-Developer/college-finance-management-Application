#ifndef FEE_UI_H
#define FEE_UI_H

#include <gtk/gtk.h>
#include <sqlite3.h>
#include "database.h"  // Include to get Fee and StudentIDCard structures

void create_fee_ui(GtkWidget *container);


void refresh_fee_table();

void refresh_student_card();


void on_search_student_fee_clicked(GtkButton *button, gpointer user_data);


void on_refresh_fee_clicked(GtkButton *button, gpointer user_data);


#endif // FEE_UI_H