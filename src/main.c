#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/database.h"
int db_create_tables();  
#include "../include/student_ui.h"
#include "../include/fee_ui.h"
#include "../include/employee_ui.h"
#include "../include/payroll.h"
#include "../include/payroll_ui.h"

GtkWidget *main_window;
GtkWidget *content_notebook;
GtkWidget *sidebar_buttons[6];

void on_sidebar_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;  // Mark unused parameter
    int page_num = GPOINTER_TO_INT(user_data);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), page_num);
    printf("[INFO] Switched to module page %d\n", page_num);
}

void on_exit_clicked(GtkButton *button, gpointer user_data) {
    (void)button;       
    (void)user_data;     
    printf("[INFO] Exit button clicked - shutting down\n");
    gtk_main_quit();
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    printf("[INFO] Main window destroyed\n");
    gtk_main_quit();
}

// ============================================================================
// NEW: Create Dashboard Page with Financial Overview
// ============================================================================
GtkWidget* create_dashboard_ui() {
    GtkWidget *dashboard_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(dashboard_box, 20);
    gtk_widget_set_margin_end(dashboard_box, 20);
    gtk_widget_set_margin_top(dashboard_box, 20);
    gtk_widget_set_margin_bottom(dashboard_box, 20);

    // Dashboard Title
    GtkWidget *dashboard_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(dashboard_title),
        "<span font='20' weight='bold'>📊 Financial Dashboard</span>");
    gtk_widget_set_halign(dashboard_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboard_box), dashboard_title, FALSE, FALSE, 0);

    // Separator
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(dashboard_box), separator, FALSE, FALSE, 5);

    // Quick Stats Container
    GtkWidget *stats_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(stats_grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(stats_grid), 15);
    gtk_widget_set_halign(stats_grid, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboard_box), stats_grid, FALSE, FALSE, 0);

    // Stat 1: Total Students
    GtkWidget *stat1_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat1_title),
        "<span font='14' weight='bold'>👨‍🎓 Total Students</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat1_title, 0, 0, 1, 1);

    GtkWidget *stat1_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat1_value),
        "<span font='18' weight='bold' color='#2196F3'>Loading...</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat1_value, 0, 1, 1, 1);

    // Stat 2: Total Employees
    GtkWidget *stat2_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat2_title),
        "<span font='14' weight='bold'>👨‍💼 Total Employees</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat2_title, 1, 0, 1, 1);

    GtkWidget *stat2_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat2_value),
        "<span font='18' weight='bold' color='#FF9800'>Loading...</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat2_value, 1, 1, 1, 1);

    // Stat 3: Pending Fees
    GtkWidget *stat3_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat3_title),
        "<span font='14' weight='bold'>💰 Pending Fees</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat3_title, 2, 0, 1, 1);

    GtkWidget *stat3_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat3_value),
        "<span font='18' weight='bold' color='#F44336'>Loading...</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat3_value, 2, 1, 1, 1);

    // Stat 4: Monthly Payroll
    GtkWidget *stat4_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat4_title),
        "<span font='14' weight='bold'>💵 Monthly Payroll</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat4_title, 3, 0, 1, 1);

    GtkWidget *stat4_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat4_value),
        "<span font='18' weight='bold' color='#4CAF50'>Loading...</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat4_value, 3, 1, 1, 1);

    // ========================================
    // Quick Actions Section
    // ========================================
    GtkWidget *actions_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(actions_title),
        "<span font='14' weight='bold'>⚡ Quick Actions</span>");
    gtk_widget_set_halign(actions_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboard_box), actions_title, FALSE, FALSE, 10);

    GtkWidget *actions_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(actions_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(actions_grid), 10);
    gtk_widget_set_halign(actions_grid, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboard_box), actions_grid, FALSE, FALSE, 0);

    // Quick action buttons
    const char *action_labels[] = {
        "➕ Add Student",
        "➕ Add Employee",
        "📋 Generate Fee Receipt",
        "💵 Generate Payroll"
    };

    for (int i = 0; i < 4; i++) {
        GtkWidget *action_btn = gtk_button_new_with_label(action_labels[i]);
        gtk_widget_set_size_request(action_btn, 150, 40);
        gtk_grid_attach(GTK_GRID(actions_grid), action_btn, i, 0, 1, 1);
    }

    // ========================================
    // Recent Activities Section
    // ========================================
    GtkWidget *recent_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(recent_title),
        "<span font='14' weight='bold'>📌 Recent Activities</span>");
    gtk_widget_set_halign(recent_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboard_box), recent_title, FALSE, FALSE, 10);

    GtkWidget *recent_text = gtk_label_new(
        "• System initialized\n"
        "• Database connected\n"
        "• All modules ready\n"
        "• Waiting for user input..."
    );
    gtk_widget_set_halign(recent_text, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboard_box), recent_text, FALSE, FALSE, 0);

    // Add scrollable area for future expansion
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), dashboard_box);

    printf("[INFO] Dashboard UI created successfully\n");
    return scrolled;
}

void create_main_window() {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window),
        "CFMS - College Finance Management System");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1000, 650);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
   
    printf("[INFO] Main window created\n");

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), main_box);

    // Title 
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label),
        "<span font='24' weight='bold'>College Financial Management System</span>");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(title_label, 10);
    gtk_widget_set_margin_bottom(title_label, 5);
    gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 0);

    // Subtitle 
    GtkWidget *subtitle_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(subtitle_label),
        "<span font='12' style='italic' color='#fa8703ff'>"
        "L.D.A.H Rajkiya Engineering College Mainpuri</span>");
    gtk_widget_set_halign(subtitle_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(subtitle_label, 10);
    gtk_box_pack_start(GTK_BOX(main_box), subtitle_label, FALSE, FALSE, 0);

    // Content area (sidebar + notebook)
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);

    // Sidebar
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(sidebar, 200, -1);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "GtkBox#sidebar { background-color: #757272ff; }", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(sidebar);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
   
    gtk_box_pack_start(GTK_BOX(content_box), sidebar, FALSE, FALSE, 0);

    // Sidebar buttons with emoji labels
    const char *button_labels[] = {
        "📊 Dashboard",
        "👨‍🎓 Student Mgmt",
        "👨‍💼 Employee Mgmt",
        "💰 Fee Management",
        "💵 Payroll"
    };

    printf("[INFO] Creating sidebar buttons\n");
    for (int i = 0; i < 5; i++) {
        sidebar_buttons[i] = gtk_button_new_with_label(button_labels[i]);
        gtk_widget_set_size_request(sidebar_buttons[i], -1, 60);
        gtk_button_set_relief(GTK_BUTTON(sidebar_buttons[i]), GTK_RELIEF_NONE);
        g_signal_connect(sidebar_buttons[i], "clicked",
            G_CALLBACK(on_sidebar_button_clicked), GINT_TO_POINTER(i));
        gtk_box_pack_start(GTK_BOX(sidebar), sidebar_buttons[i], FALSE, FALSE, 0);
    }

    content_notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(content_notebook), FALSE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(content_notebook), FALSE);
    gtk_box_pack_start(GTK_BOX(content_box), content_notebook, TRUE, TRUE, 0);

    printf("[INFO] Creating notebook pages\n");

    // Page 0: Dashboard with financial overview
    GtkWidget *dashboard_page = create_dashboard_ui();
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), dashboard_page, NULL);

    // Page 1: Student Management
    GtkWidget *student_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    create_student_ui(student_container);
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), student_container, NULL);

    // Page 2: Employee Management
    GtkWidget *employee_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    create_employee_ui(employee_container);
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), employee_container, NULL);

    // Page 3: Fee Management
    GtkWidget *fee_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    create_fee_ui(fee_container);
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), fee_container, NULL);

    // Page 4: Payroll Management
    GtkWidget *payroll_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    create_payroll_ui(payroll_tab);
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), payroll_tab, NULL);

    // Footer
    GtkWidget *footer = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(footer),
        "<span color='#eeaf02ff' size='large'>"
        "<i>CFMS v1.0 | Developed by Ishu &amp; Mohsin | L.D.A.H Engineering College Mainpuri</i>"
        "</span>");
    gtk_widget_set_size_request(footer, -1, 70);
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(footer, 10);
    gtk_widget_set_margin_bottom(footer, 50);
    gtk_box_pack_start(GTK_BOX(main_box), footer, FALSE, TRUE, 0);

    g_signal_connect(main_window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    printf("[INFO] Main UI creation complete\n");
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                   College Finance Management System            ║\n");
    printf("║               L.D.A.H Rajkiya Engineering College Mainpuri     ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("[INFO] Initializing database...\n");
    if (!db_init("data/college_finance.db")) {
        printf("[ERROR] Database init failed\n");
        return 1;
    }
    if (!db_create_tables()) {
        printf("[ERROR] Failed to create tables: %s\n", db_get_error());
        db_close();
        return 1;
    }
    printf("[INFO] Database tables initialized\n");
    printf("[INFO] Initializing GTK...\n");
    gtk_init(&argc, &argv);
    create_main_window();
    printf("[INFO] Showing main window\n");
    gtk_widget_show_all(main_window);
    
    // Set Dashboard (page 0) as the initial visible page
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), 0);
    printf("[INFO] Dashboard loaded on startup\n");
    
    printf("[INFO] Application started - waiting for user interaction\n\n");
    gtk_main();
    printf("\n[INFO] Cleaning up...\n");
    db_close();
    printf("[INFO] Application closed successfully\n");
    printf("\n");
    return 0;
}
