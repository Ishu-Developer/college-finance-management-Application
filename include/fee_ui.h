#ifndef FEE_UI_H
#define FEE_UI_H

#include <gtk/gtk.h>
#include <sqlite3.h>
#include "database.h"

// Main UI Creation
void create_fee_ui(GtkWidget *container);

// Button Callbacks
void on_back_clicked(GtkButton *button, gpointer data);
void on_refresh_clicked(GtkButton *button, gpointer data);
void on_search_student_fee_clicked(GtkButton *button, gpointer data);
void on_save_fee_clicked(GtkButton *button, gpointer data);

#endif // FEE_UI_H