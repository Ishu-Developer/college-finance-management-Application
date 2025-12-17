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
GtkWidget *sidebar_buttons[5];


void on_sidebar_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    int page_num = GPOINTER_TO_INT(user_data);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), page_num);
    printf("[INFO] Switched to module page %d\n", page_num);
}


void on_window_destroy(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    printf("[INFO] Main window destroyed\n");
    gtk_main_quit();
}

void on_add_student(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Add Student button clicked - navigating to Student Management\n");
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), 1);
}

void on_add_employee(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Add Employee button clicked - navigating to Employee Management\n");
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), 2);
}

void on_generate_fee_receipt_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Generate Fee Receipt button clicked - navigating to Fee Management\n");
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), 3);
}

void on_generate_payroll_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;
    printf("[INFO] Generate Payroll button clicked - navigating to Payroll Management\n");
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), 4);
}



GtkWidget* create_dashboard_ui() {
    GtkWidget *dashboard_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_start(dashboard_box, 30);
    gtk_widget_set_margin_end(dashboard_box, 30);
    gtk_widget_set_margin_top(dashboard_box, 25);
    gtk_widget_set_margin_bottom(dashboard_box, 25);


    // ========================================
    // Dashboard Title
    // ========================================
    GtkWidget *dashboard_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(dashboard_title),
        "<span font='24' weight='bold' foreground='#1a1a1a'>📊 Financial Dashboard</span>");
    gtk_widget_set_halign(dashboard_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboard_box), dashboard_title, FALSE, FALSE, 0);


    // Separator
    GtkWidget *separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(dashboard_box), separator1, FALSE, FALSE, 0);


    // ========================================
    // Quick Stats Container (Card-style)
    // ========================================
    GtkWidget *stats_frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(stats_frame), GTK_SHADOW_IN);
    
    GtkWidget *stats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(stats_box, 15);
    gtk_widget_set_margin_end(stats_box, 15);
    gtk_widget_set_margin_top(stats_box, 15);
    gtk_widget_set_margin_bottom(stats_box, 15);
    gtk_container_add(GTK_CONTAINER(stats_frame), stats_box);

    GtkWidget *stats_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stats_title),
        "<span font='14' weight='bold' foreground='#333333'>📈 System Overview</span>");
    gtk_widget_set_halign(stats_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(stats_box), stats_title, FALSE, FALSE, 0);

    GtkWidget *stats_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(stats_grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(stats_grid), 15);
    gtk_widget_set_halign(stats_grid, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(stats_box), stats_grid, FALSE, FALSE, 0);


    // Stat 1: Total Students
    GtkWidget *stat1_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat1_title),
        "<span font='13' weight='bold' foreground='#555555'>👨‍🎓 Total Students</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat1_title, 0, 0, 1, 1);

    GtkWidget *stat1_value = gtk_label_new(NULL);
    int student_count = db_get_student_count();
    char count_markup[100];
    snprintf(count_markup, sizeof(count_markup),
        "<span font='20' weight='bold' foreground='#2196F3'>%d</span>", student_count);
    gtk_label_set_markup(GTK_LABEL(stat1_value), count_markup);
    gtk_grid_attach(GTK_GRID(stats_grid), stat1_value, 0, 1, 1, 1);


    // Stat 2: Total Employees
    GtkWidget *stat2_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat2_title),
        "<span font='13' weight='bold' foreground='#555555'>👨‍💼 Total Employees</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat2_title, 1, 0, 1, 1);

    GtkWidget *stat2_value = gtk_label_new(NULL);
    int employee_count = db_get_employee_count();  
    char emp_markup[100];
    snprintf(emp_markup, sizeof(emp_markup),
        "<span font='20' weight='bold' foreground='#FF9800'>%d</span>", employee_count);
    gtk_label_set_markup(GTK_LABEL(stat2_value), emp_markup);
    gtk_grid_attach(GTK_GRID(stats_grid), stat2_value, 1, 1, 1, 1);

    // Stat 3: Pending Fees
    GtkWidget *stat3_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat3_title),
        "<span font='13' weight='bold' foreground='#555555'>💰 Pending Fees</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat3_title, 2, 0, 1, 1);

    GtkWidget *stat3_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat3_value),
        "<span font='20' weight='bold' foreground='#F44336'>Loading...</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat3_value, 2, 1, 1, 1);


    // Stat 4: Monthly Payroll
    GtkWidget *stat4_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat4_title),
        "<span font='13' weight='bold' foreground='#555555'>💵 Monthly Payroll</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat4_title, 3, 0, 1, 1);

    GtkWidget *stat4_value = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stat4_value),
        "<span font='20' weight='bold' foreground='#4CAF50'>Loading...</span>");
    gtk_grid_attach(GTK_GRID(stats_grid), stat4_value, 3, 1, 1, 1);

    gtk_box_pack_start(GTK_BOX(dashboard_box), stats_frame, FALSE, FALSE, 0);


    // ========================================
    // Quick Actions Section (Enhanced)
    // ========================================
    GtkWidget *actions_frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(actions_frame), GTK_SHADOW_IN);
    
    GtkWidget *actions_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(actions_box, 15);
    gtk_widget_set_margin_end(actions_box, 15);
    gtk_widget_set_margin_top(actions_box, 15);
    gtk_widget_set_margin_bottom(actions_box, 15);
    gtk_container_add(GTK_CONTAINER(actions_frame), actions_box);

    GtkWidget *actions_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(actions_title),
        "<span font='14' weight='bold' foreground='#333333'>⚡ Quick Actions</span>");
    gtk_widget_set_halign(actions_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(actions_box), actions_title, FALSE, FALSE, 0);

    GtkWidget *actions_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(actions_grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(actions_grid), 10);
    gtk_widget_set_halign(actions_grid, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(actions_box), actions_grid, FALSE, FALSE, 0);


    // Quick action buttons with callbacks
    struct {
        const char *label;
        GCallback callback;
    } action_buttons[] = {
        {"➕ Add Student", G_CALLBACK(on_add_student)},
        {"➕ Add Employee", G_CALLBACK(on_add_employee)},
        {"📋 Generate Fee Receipt", G_CALLBACK(on_generate_fee_receipt_clicked)},
        {"💵 Generate Payroll", G_CALLBACK(on_generate_payroll_clicked)}
    };

    for (int i = 0; i < 4; i++) {
        GtkWidget *action_btn = gtk_button_new_with_label(action_buttons[i].label);
        gtk_widget_set_size_request(action_btn, 180, 45);
        
        // Apply CSS styling to buttons
        GtkCssProvider *btn_css = gtk_css_provider_new();
        gtk_css_provider_load_from_data(btn_css,
            "button { padding: 8px 15px; font-weight: bold; }", -1, NULL);
        GtkStyleContext *btn_context = gtk_widget_get_style_context(action_btn);
        gtk_style_context_add_provider(btn_context, GTK_STYLE_PROVIDER(btn_css),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        
        g_signal_connect(action_btn, "clicked",
            action_buttons[i].callback, NULL);
        gtk_grid_attach(GTK_GRID(actions_grid), action_btn, i, 0, 1, 1);
    }

    gtk_box_pack_start(GTK_BOX(dashboard_box), actions_frame, FALSE, FALSE, 0);


    // ========================================
    // Recent Activities Section
    // ========================================
    GtkWidget *activities_frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(activities_frame), GTK_SHADOW_IN);
    
    GtkWidget *activities_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(activities_box, 15);
    gtk_widget_set_margin_end(activities_box, 15);
    gtk_widget_set_margin_top(activities_box, 15);
    gtk_widget_set_margin_bottom(activities_box, 15);
    gtk_container_add(GTK_CONTAINER(activities_frame), activities_box);

    GtkWidget *recent_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(recent_title),
        "<span font='14' weight='bold' foreground='#333333'>📌 Recent Activities</span>");
    gtk_widget_set_halign(recent_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(activities_box), recent_title, FALSE, FALSE, 0);

    GtkWidget *recent_text = gtk_label_new(
        "• System initialized successfully\n"
        "• Database connected and tables created\n"
        "• Ready for user operations\n"
        "• Use Quick Actions below for fast navigation"
    );
    gtk_widget_set_halign(recent_text, GTK_ALIGN_START);
    gtk_label_set_line_wrap(GTK_LABEL(recent_text), TRUE);
    gtk_box_pack_start(GTK_BOX(activities_box), recent_text, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(dashboard_box), activities_frame, FALSE, FALSE, 0);


    // Add scrollable area for future expansion
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), dashboard_box);

    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand_set(scrolled, TRUE);
    gtk_widget_set_hexpand_set(scrolled, TRUE);

    printf("[INFO] Redesigned Dashboard UI created successfully\n");
    return scrolled;
}


void create_main_window() {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window),
        "CFMS - College Finance Management System");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1000, 700);
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
        "<span font='12' style='italic' foreground='#fa8703'>"
        "L.D.A.H Rajkiya Engineering College Mainpuri</span>");
    gtk_widget_set_halign(subtitle_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(subtitle_label, 10);
    gtk_box_pack_start(GTK_BOX(main_box), subtitle_label, FALSE, FALSE, 0);


    // Content area (sidebar + notebook)
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_vexpand(content_box, TRUE);
    gtk_widget_set_hexpand(content_box, TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);


    // Sidebar
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(sidebar, 220, -1);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "GtkBox#sidebar { background-color: #2c3e50; }", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(sidebar);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
   
    gtk_box_pack_start(GTK_BOX(content_box), sidebar, FALSE, FALSE, 0);


    // Sidebar buttons with emoji labels
    const char *button_labels[] = {
        "📊 Dashboard",
        "👨‍🎓 Student Management",
        "👨‍💼 Employee Management",
        "💰 Fee Management",
        "💵 Payroll"
    };


    printf("[INFO] Creating sidebar buttons\n");
        printf("[INFO] Creating sidebar buttons\n");
    for (int i = 0; i < 5; i++) {
        sidebar_buttons[i] = gtk_button_new_with_label(button_labels[i]);
        gtk_widget_set_size_request(sidebar_buttons[i], -1, 60);
        gtk_button_set_relief(GTK_BUTTON(sidebar_buttons[i]), GTK_RELIEF_NONE);
        
        // Style sidebar buttons
        GtkCssProvider *btn_css = gtk_css_provider_new();
        gtk_css_provider_load_from_data(btn_css,
            "button { color: white; font-weight: bold; background-color: #34495e; padding: 10px; }\n"
            "button:hover { background-color: #3d5a73; }\n"
            "button:active { background-color: #2c3e50; }", -1, NULL);
        GtkStyleContext *btn_context = gtk_widget_get_style_context(sidebar_buttons[i]);
        gtk_style_context_add_provider(btn_context, GTK_STYLE_PROVIDER(btn_css),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        
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
        "<span foreground='#eeaf02' size='11'>"  
        "<i>CFMS v1.0 | Developed by Ishu &amp; Mohsin | L.D.A.H Engineering College Mainpuri</i>"
        "</span>");
    gtk_widget_set_size_request(footer, -1, 40);
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(footer, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(footer, 5);
    gtk_widget_set_margin_bottom(footer, 5);
    gtk_box_pack_end(GTK_BOX(main_box), footer, FALSE, FALSE, 0);
    gtk_widget_show(footer);


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
