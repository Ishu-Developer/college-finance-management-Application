# college-finance-management-Application
A project based on Application Development using C language an Unified Software Excel + tally.
<br>
author - Ishu 
<br>
college-finance-management/
├── src/
│   ├── main.c
│   ├── database/
│   │   ├── db_init.c
│   │   ├── db_student.c
│   │   ├── db_employee.c
│   │   ├── db_fee.c
│   │   └── db_accounts.c
│   ├── ui/
│   │   ├── student_ui.c
│   │   ├── employee_ui.c
│   │   ├── fee_ui.c
│   │   ├── accounts_ui.c
│   │   ├── payroll_ui.c
│   │   └── dashboard_ui.c
│   ├── tally/
│   │   ├── tally_sync.c
│   │   └── tally_xml_handler.c
│   ├── reports/
│   │   ├── pdf_generator.c
│   │   └── receipt_generator.c
│   └── utils/
│       ├── logger.c
│       └── validators.c
├── include/
│   ├── database.h
│   ├── student_ui.h
│   ├── employee_ui.h
│   ├── fee_ui.h
│   ├── accounts_ui.h
│   ├── payroll_ui.h
│   ├── tally_sync.h
│   └── pdf_generator.h
├── resources/
│   ├── glade/
│   │   └── main_window.glade
│   └── icons/
├── Makefile
├── .gitignore
└── README.md


Main Window (850x600)
├── Top: Header/Title Bar
├── Left: Navigation Sidebar
│   ├── Student Management
│   ├── Employee Management
│   ├── Fee Management
│   ├── Accounts
│   ├── Payroll Management
│   ├── Dashboard/Reports
│   └── Exit
└── Right: Content Area (Notebook with tabs for each module)
    ├── Tab 0: Student Module
    ├── Tab 1: Employee Module
    ├── Tab 2: Fee Module
    ├── Tab 3: Accounts Module
    └── Tab 4: Payroll Module

┌─────────────────────────────────────────────────────────────┐
│ [SIDEBAR]              │   [NOTEBOOK SELECTED PAGE]         │
│ ───────────────────────┼────────────────────────────────────│
│ 📊 Dashboard           │ ┌───────────────────────────────┐  │
│ 👨‍🎓 Student Mgmt  <-- │ │ Student Management           │  │
│ 👨‍💼 Employee Mgmt      │ ├───────────────────────────────┤  │
│ 💰 Fee Management       │ │ [Add] [Edit] [Delete] [Refresh]│
│ 📈 Accounts             │ │                               │
│ 💵 Payroll              │ │ ┌────────────────────────────┐│
│                        │ │ │ Student List/Table/Grid    ││
│                        │ │ └────────────────────────────┘│
└────────────────────────┴─┴────────────────────────────────┘
