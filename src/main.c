#include <gtk/gtk.h>
#include "../include/database.h"
#include "../include/student.h"
#include "../include/employee.h"
#include "../include/fee.h"

// Main widgets
GtkWidget *main_window;
GtkWidget *content_notebook;
GtkWidget *sidebar_buttons[6];

// Callback function for sidebar buttons
void on_sidebar_button_clicked(GtkButton *button, gpointer user_data) {
    int page_num = GPOINTER_TO_INT(user_data);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), page_num);
}

void on_exit_clicked(GtkButton *button, gpointer user_data) {
    gtk_main_quit();
}

void create_main_window() {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), 
        "Financial Management Solution for College - Accountant");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 700);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);

    // Main container
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), main_box);

    // Title label
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), 
        "<span font='24' weight='bold'>Financial Management Solution</span>");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 5);

    // Subtitle label
    GtkWidget *subtitle_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(subtitle_label), 
        "<span font='12' style='italic'>L.D.A.H Rajkiya Engineering College Mainpuri</span>");
    gtk_widget_set_halign(subtitle_label, GTK_ALIGN_CENTER);    
    gtk_box_pack_start(GTK_BOX(main_box), subtitle_label, FALSE, FALSE, 2);


    // ===== MAIN CONTENT BOX (Sidebar + Content Area) =====
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);

    // ===== LEFT SIDEBAR =====
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(sidebar, 200, -1);
    gtk_style_context_add_class(gtk_widget_get_style_context(sidebar), "sidebar");
    gtk_box_pack_start(GTK_BOX(content_box), sidebar, FALSE, FALSE, 0);

    // Sidebar buttons
    const char *button_labels[] = {
        "📊 Dashboard",
        "👨‍🎓 Student Mgmt",
        "👨‍💼 Employee Mgmt",
        "💰 Fee Management",
        "📈 Accounts",
        "💵 Payroll"
    };

    for (int i = 0; i < 6; i++) {
        sidebar_buttons[i] = gtk_button_new_with_label(button_labels[i]);
        gtk_widget_set_size_request(sidebar_buttons[i], -1, 50);
        gtk_button_set_relief(GTK_BUTTON(sidebar_buttons[i]), GTK_RELIEF_NONE);
        g_signal_connect(sidebar_buttons[i], "clicked", 
            G_CALLBACK(on_sidebar_button_clicked), GINT_TO_POINTER(i));
        gtk_box_pack_start(GTK_BOX(sidebar), sidebar_buttons[i], FALSE, FALSE, 0);
    }

    // Add exit button at bottom of sidebar
    GtkWidget *exit_button = gtk_button_new_with_label("❌ Exit");
    gtk_widget_set_size_request(exit_button, -1, 50);
    gtk_button_set_relief(GTK_BUTTON(exit_button), GTK_RELIEF_NONE);
    g_signal_connect(exit_button, "clicked", G_CALLBACK(on_exit_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(sidebar), exit_button, FALSE, FALSE, 0);

    // ===== RIGHT CONTENT AREA (Notebook) =====
    content_notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(content_notebook), FALSE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(content_notebook), FALSE);
    gtk_box_pack_start(GTK_BOX(content_box), content_notebook, TRUE, TRUE, 0);

    // PAGE 0: Dashboard (placeholder)
    GtkWidget *dashboard = gtk_label_new("Dashboard - Financial Overview");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), dashboard, NULL);

    // PAGE 1: Student Management
    GtkWidget *student_page = gtk_label_new("Student Management");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), student_page, NULL);


    // PAGE 2: Employee Management (placeholder for now)
    GtkWidget *employee_label = gtk_label_new("Employee Management");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), employee_label, NULL);

    // PAGE 3: Fee Management (placeholder for now)
    GtkWidget *fee_label = gtk_label_new("Fee Management");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), fee_label, NULL);

    // PAGE 4: Accounts (placeholder for now)
    GtkWidget *accounts_label = gtk_label_new("Accounts Payable & Receivable");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), accounts_label, NULL);

    // PAGE 5: Payroll Management (placeholder for now)
    GtkWidget *payroll_label = gtk_label_new("Payroll Management");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), payroll_label, NULL);

    // ===== FOOTER =====
    GtkWidget *footer = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(footer), 
        "<span color='red' weight='bold'><small><i>Developed by Ishu | Version 1.0</i></small></span>");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(main_box), footer, FALSE, FALSE, 10);

    // Connect destroy signal
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

int main(int argc, char *argv[]) {
    // Initialize database
    if (!db_init()) {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }
    
    if (!db_create_tables()) {
        fprintf(stderr, "Failed to create tables\n");
        db_close();
        return 1;
    }

    gtk_init(&argc, &argv);
    create_main_window();
    gtk_widget_show_all(main_window);
    gtk_main();
    
    db_close();
    return 0;
}


//cd "/d/project/project using c/college-finance-management-Application"
