#ifndef FEE_UI_H
#define FEE_UI_H

#include <gtk/gtk.h>

void create_fee_ui(GtkWidget *container);
void refresh_fee_list();
void on_add_fee_clicked(GtkButton *button, gpointer user_data);
void on_mark_paid_clicked(GtkButton *button, gpointer user_data);
void on_generate_receipt_clicked(GtkButton *button, gpointer user_data);

#endif