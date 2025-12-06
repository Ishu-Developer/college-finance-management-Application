#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/database.h"
#include "../include/student_ui.h"
#include "../include/fee_ui.h"
#include "../include/employee_ui.h"


// Global widgets
GtkWidget *main_window;
GtkWidget *content_notebook;
GtkWidget *sidebar_buttons[5];  

void on_sidebar_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;  // Mark unused parameter
    int page_num = GPOINTER_TO_INT(user_data);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(content_notebook), page_num);
    printf("[INFO] Switched to module page %d\n", page_num);
}


/**
 * @brief Callback when exit button is clicked
 * @param button The button widget
 * @param user_data User data (unused)
 */
void on_exit_clicked(GtkButton *button, gpointer user_data) {
    (void)button;        // Mark unused parameter
    (void)user_data;     // Mark unused parameter
    printf("[INFO] Exit button clicked - shutting down\n");
    gtk_main_quit();
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    printf("[INFO] Main window destroyed\n");
    gtk_main_quit();
}


void create_main_window() {
    // Create main window
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window),
        "College Finance Management System (CFMS)");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 700);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    
    printf("[INFO] Main window created\n");


    // Main container (vertical)
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), main_box);


    // =====================================================================
    // HEADER SECTION
    // =====================================================================
    
    // Title label
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label),
        "<span font='24' weight='bold'>College Finance Management System</span>");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(title_label, 10);
    gtk_widget_set_margin_bottom(title_label, 5);
    gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 0);


    // Subtitle label
    GtkWidget *subtitle_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(subtitle_label),
        "<span font='12' style='italic' color='#f9fd04ff'>"
        "L.D.A.H Rajkiya Engineering College Mainpuri</span>");
    gtk_widget_set_halign(subtitle_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(subtitle_label, 10);
    gtk_box_pack_start(GTK_BOX(main_box), subtitle_label, FALSE, FALSE, 0);

    printf("[INFO] Header created\n");


    // =====================================================================
    // MAIN CONTENT AREA (Sidebar + Notebook)
    // =====================================================================
    
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);


    // =====================================================================
    // SIDEBAR (Left)
    // =====================================================================
    
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(sidebar, 200, -1);
    gtk_widget_set_name(sidebar, "sidebar");
    
    // Add background color to sidebar
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "#sidebar { background-color: #070707ff; }", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(sidebar);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    gtk_box_pack_start(GTK_BOX(content_box), sidebar, FALSE, FALSE, 0);

    printf("[INFO] Sidebar created\n");


    // =====================================================================
    // SIDEBAR BUTTONS
    // =====================================================================
    
    // Sidebar buttons with emoji labels (FIXED: Removed Accounts Module)
    const char *button_labels[] = {
        "📊 Dashboard",
        "👨‍🎓 Student Mgmt",
        "👨‍💼 Employee Mgmt",
        "💰 Fee Management",
        "💵 Payroll"
    };

    int num_buttons = 5;  // FIXED: Changed to 5 (was 6, had Accounts)

    printf("[INFO] Creating sidebar buttons\n");
    for (int i = 0; i < num_buttons; i++) {
        sidebar_buttons[i] = gtk_button_new_with_label(button_labels[i]);
        gtk_widget_set_size_request(sidebar_buttons[i], -1, 60);
        gtk_button_set_relief(GTK_BUTTON(sidebar_buttons[i]), GTK_RELIEF_NONE);
        
        // Style the button
        GtkCssProvider *btn_css = gtk_css_provider_new();
        gtk_css_provider_load_from_data(btn_css,
            "button { color: white; font-weight: bold; }", -1, NULL);
        GtkStyleContext *btn_context = gtk_widget_get_style_context(sidebar_buttons[i]);
        gtk_style_context_add_provider(btn_context, GTK_STYLE_PROVIDER(btn_css),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        
        g_signal_connect(sidebar_buttons[i], "clicked",
            G_CALLBACK(on_sidebar_button_clicked), GINT_TO_POINTER(i));
        gtk_box_pack_start(GTK_BOX(sidebar), sidebar_buttons[i], FALSE, FALSE, 0);
    }

    printf("[INFO] %d sidebar buttons created\n", num_buttons);


    // Add exit button at bottom of sidebar
    GtkWidget *exit_button = gtk_button_new_with_label("❌ Exit");
    gtk_widget_set_size_request(exit_button, -1, 50);
    gtk_button_set_relief(GTK_BUTTON(exit_button), GTK_RELIEF_NONE);
    g_signal_connect(exit_button, "clicked", G_CALLBACK(on_exit_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(sidebar), exit_button, FALSE, FALSE, 0);


    // =====================================================================
    // CONTENT AREA (Notebook - Right)
    // =====================================================================

    content_notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(content_notebook), FALSE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(content_notebook), FALSE);
    gtk_box_pack_start(GTK_BOX(content_box), content_notebook, TRUE, TRUE, 0);

    printf("[INFO] Creating notebook pages\n");


    // =====================================================================
    // PAGE 0: Dashboard (Placeholder)
    // =====================================================================
    GtkWidget *dashboard = gtk_label_new("📊 Dashboard - Financial Overview (Phase 2)");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), dashboard, NULL);
    printf("[INFO] Dashboard page added (page 0)\n");


    // =====================================================================
    // PAGE 1: Student Management
    // =====================================================================
    GtkWidget *student_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    create_student_ui(student_container);
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), student_container, NULL);
    printf("[INFO] Student Management page added (page 1)\n");


    // =====================================================================
    // PAGE 2: Employee Management
    // =====================================================================
    GtkWidget *employee_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    create_employee_ui(employee_container);
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), employee_container, NULL);
    printf("[INFO] Employee Management page added (page 2)\n");


    // =====================================================================
    // PAGE 3: Fee Management
    // =====================================================================
    GtkWidget *fee_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    create_fee_ui(fee_container);
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), fee_container, NULL);
    printf("[INFO] Fee Management page added (page 3)\n");

    // NOTE: Accounts Module REMOVED as requested


    // =====================================================================
    // PAGE 4: Payroll (Placeholder for Phase 2)
    // =====================================================================
    GtkWidget *payroll_label = gtk_label_new("💵 Payroll Management (Phase 2)");
    gtk_notebook_append_page(GTK_NOTEBOOK(content_notebook), payroll_label, NULL);
    printf("[INFO] Payroll page added (page 4)\n");


    // =====================================================================
    // FOOTER SECTION
    // =====================================================================

    GtkWidget *footer = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(footer),
        "<span color='#e90505ff' size='bold'>"
        "<i>College Finance Management System v1.0 | Developed by Ishu </i>"
        "</span>");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(footer, 10);
    gtk_widget_set_margin_bottom(footer, 10);
    gtk_box_pack_start(GTK_BOX(main_box), footer, FALSE, FALSE, 0);

    printf("[INFO] Footer created\n");


    // =====================================================================
    // SIGNAL CONNECTIONS
    // =====================================================================

    // Connect window destroy signal
    g_signal_connect(main_window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    printf("[INFO] Main UI creation complete\n");
}


/**
 * @brief Main entry point of the application
 * 
 * Initializes database and GTK, creates main window, and runs main loop
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, 1 on failure
 */
int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                   College Finance Management System            ║\n");
    printf("║               L.D.A.H Rajkiya Engineering College Mainpuri     ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");


    // =====================================================================
    // DATABASE INITIALIZATION
    // =====================================================================

    printf("[INFO] Initializing database...\n");
    if (!db_init()) {
        fprintf(stderr, "[ERROR] Failed to initialize database\n");
        fprintf(stderr, "[ERROR] %s\n", db_get_error());
        return 1;
    }

    printf("[INFO] Database initialized successfully\n");


    // =====================================================================
    // FEE MODULE INITIALIZATION
    // =====================================================================

    printf("[INFO] Initializing fee module...\n");
    if (db_fee_init() < 0) {
        fprintf(stderr, "[ERROR] Failed to initialize fee module\n");
        fprintf(stderr, "[ERROR] %s\n", db_get_error());
        db_close();
        return 1;
    }

    printf("[INFO] Fee module initialized successfully\n\n");


    // =====================================================================
    // GTK INITIALIZATION
    // =====================================================================

    printf("[INFO] Initializing GTK...\n");
    gtk_init(&argc, &argv);

    printf("[INFO] GTK initialized\n\n");


    // =====================================================================
    // CREATE MAIN WINDOW
    // =====================================================================

    printf("[INFO] Creating main window...\n");
    create_main_window();


    // =====================================================================
    // SHOW WINDOW AND START MAIN LOOP
    // =====================================================================

    printf("[INFO] Showing main window\n");
    gtk_widget_show_all(main_window);

    printf("[INFO] Application started - waiting for user interaction\n\n");

    // Run GTK main loop
    gtk_main();


    // =====================================================================
    // CLEANUP
    // =====================================================================

    printf("\n[INFO] Cleaning up...\n");
    db_close();

    printf("[INFO] Application closed successfully\n");
    printf("\n");

    return 0;
}

//cd "/d/project/project using c/college-finance-management-Application"
