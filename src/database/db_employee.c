#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../../include/database.h"

// External database connection (from main database.c)
extern sqlite3 *db;

/**
 * @brief Add new employee to database
 * @param emp_no Employee number (unique)
 * @param name Employee name
 * @param designation Job designation
 * @param department Department (CSE, EE, CIVIL, ME)
 * @param category Category (Faculty, Staff, Support)
 * @param email Email address
 * @param mobile Mobile number
 * @param doa Date of appointment
 * @param salary Monthly salary
 * @param bank_account Bank account number
 * @return Employee ID on success, -1 on failure
 */
int db_add_employee(const char *emp_no, const char *name,
                    const char *designation, const char *department,
                    const char *category, const char *email, 
                    const char *mobile, const char *doa, double salary,
                    const char *bank_account) {
    
    if (!emp_no || !name || !designation || !department || !category || !email || !mobile || !doa || !bank_account) {
        printf("[ERROR] NULL parameters passed to db_add_employee\n");
        return -1;
    }
    
    const char *sql = 
        "INSERT INTO Employees (emp_no, name, designation, department, category, email, mobile, doa, salary, bank_account) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    // Bind parameters in correct order
    sqlite3_bind_text(stmt, 1, emp_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, designation, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, department, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, category, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, mobile, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, doa, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 9, salary);
    sqlite3_bind_text(stmt, 10, bank_account, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to insert employee: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    int emp_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    printf("[SUCCESS] Employee added with ID: %d (Emp#: %s, Name: %s, Dept: %s)\n", 
           emp_id, emp_no, name, department);
    
    return emp_id;
}

/**
 * @brief Get all employees from database
 * @return sqlite3_stmt with employee data, NULL on failure
 */
sqlite3_stmt *db_get_all_employees() {
    const char *sql = 
        "SELECT emp_id, emp_no, name, designation, department, category, email, mobile, doa "
        "FROM Employees "
        "ORDER BY department, designation DESC;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare get_all_employees statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    return stmt;
}

/**
 * @brief Search employees by name, number, or department
 * @param search_term Search keyword
 * @return sqlite3_stmt with matching employees, NULL on failure
 */
sqlite3_stmt *db_search_employee(const char *search_term) {
    if (!search_term) {
        printf("[ERROR] NULL search term\n");
        return NULL;
    }
    
    const char *sql = 
        "SELECT emp_id, emp_no, name, designation, department, category, email, mobile, doa "
        "FROM Employees "
        "WHERE emp_no LIKE ? OR name LIKE ? OR department LIKE ? "
        "ORDER BY department, name;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare search statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    // Create search pattern with wildcards
    char pattern[256];
    snprintf(pattern, sizeof(pattern), "%%%s%%", search_term);
    
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pattern, -1, SQLITE_TRANSIENT);
    
    return stmt;
}

/**
 * @brief Delete employee from database
 * @param emp_id Employee ID
 * @return 0 on success, -1 on failure
 */
int db_delete_employee(int emp_id) {
    const char *sql = "DELETE FROM Employees WHERE emp_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare delete statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, emp_id);
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to delete employee: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Employee ID %d deleted\n", emp_id);
    return 0;
}

/**
 * @brief Update employee information
 * @param emp_id Employee ID
 * @param employee Employee structure with updated data
 * @return 0 on success, -1 on failure
 */
int db_update_employee(int emp_id, const Employee *employee) {
    
    if (!employee) {
        printf("[ERROR] NULL employee pointer\n");
        return -1;
    }
    
    const char *sql = 
        "UPDATE Employees SET name=?, designation=?, department=?, category=?, "
        "email=?, mobile=?, doa=?, salary=?, bank_account=? WHERE emp_id=?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare update statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, employee->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, employee->designation, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, employee->department, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, employee->category, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, employee->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, employee->mobile, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, employee->doa, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 8, employee->salary);
    sqlite3_bind_text(stmt, 9, employee->bank_account, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 10, emp_id);
    
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to update employee: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Employee ID %d updated\n", emp_id);
    return 0;
}

/**
 * @brief Get employee by ID
 * @param emp_id Employee ID
 * @param employee Pointer to Employee struct to fill
 * @return 1 if found, 0 if not found
 */
int db_get_employee(int emp_id, Employee *employee) {
    
    if (!employee) {
        printf("[ERROR] NULL employee pointer\n");
        return 0;
    }
    
    const char *sql = 
        "SELECT emp_id, emp_no, name, designation, department, category, email, mobile, doa, salary, bank_account "
        "FROM Employees WHERE emp_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare get_employee statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, emp_id);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        employee->emp_id = sqlite3_column_int(stmt, 0);
        strncpy(employee->emp_no, (const char *)sqlite3_column_text(stmt, 1), sizeof(employee->emp_no) - 1);
        strncpy(employee->name, (const char *)sqlite3_column_text(stmt, 2), sizeof(employee->name) - 1);
        strncpy(employee->designation, (const char *)sqlite3_column_text(stmt, 3), sizeof(employee->designation) - 1);
        strncpy(employee->department, (const char *)sqlite3_column_text(stmt, 4), sizeof(employee->department) - 1);
        strncpy(employee->category, (const char *)sqlite3_column_text(stmt, 5), sizeof(employee->category) - 1);
        strncpy(employee->email, (const char *)sqlite3_column_text(stmt, 6), sizeof(employee->email) - 1);
        strncpy(employee->mobile, (const char *)sqlite3_column_text(stmt, 7), sizeof(employee->mobile) - 1);
        strncpy(employee->doa, (const char *)sqlite3_column_text(stmt, 8), sizeof(employee->doa) - 1);
        employee->salary = sqlite3_column_double(stmt, 9);
        strncpy(employee->bank_account, (const char *)sqlite3_column_text(stmt, 10), sizeof(employee->bank_account) - 1);
        
        sqlite3_finalize(stmt);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    printf("[WARNING] Employee ID %d not found\n", emp_id);
    return 0;
}

/**
 * @brief Get total number of employees
 * @return Number of employees or -1 on error
 */
int db_get_employee_count() {
    const char *sql = "SELECT COUNT(*) FROM Employees;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare count statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}