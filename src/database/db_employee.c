/**
 * @file db_employee.c - Stub
 */
#include "../include/database.h"

int db_add_employee(const char *emp_no, const char *name, const char *designation,
                    const char *department, const char *email, const char *mobile,
                    double salary, const char *bank_account) {
    return -1;
}

int db_get_employee(int emp_id, Employee *employee) {
    return 0;
}

sqlite3_stmt* db_get_all_employees() {
    return NULL;
}

int db_update_employee(int emp_id, const Employee *employee) {
    return 0;
}

int db_delete_employee(int emp_id) {
    return 0;
}

int db_get_employee_count() {
    return 0;
}