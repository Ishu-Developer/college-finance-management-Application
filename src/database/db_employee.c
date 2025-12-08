#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../../include/database.h"

// External database connection
extern sqlite3 *db;

/**
 * Add new employee using Employee struct (MATCHES HEADER)
 */
int db_add_employee(const Employee *emp) {
    if (!emp) {
        printf("[ERROR] NULL employee pointer\n");
        return -1;
    }
    
    const char *sql = 
        "INSERT INTO employees (emp_no, employee_name, birth_date, department, designation, "
        "reporting_person, email, mobile_number, base_salary, joining_date, status, address, bank_account) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare insert: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    // Bind Employee struct fields (MATCHES database.h Employee struct)
    sqlite3_bind_text(stmt, 1, emp->emp_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, emp->employee_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, emp->birth_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, emp->department, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, emp->designation, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, emp->reporting_person, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, emp->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, emp->mobile_number, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 9, emp->base_salary);
    sqlite3_bind_text(stmt, 10, emp->joining_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, emp->status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, emp->address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, emp->bank_account, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to insert employee: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    int emp_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    printf("[SUCCESS] Employee added: ID=%d, Name=%s\n", emp_id, emp->employee_name);
    return emp_id;
}

/**
 * Get all employees (MATCHES HEADER: sqlite3_stmt **stmt)
 */
int db_get_all_employees(sqlite3_stmt **stmt) {
    const char *sql = 
        "SELECT emp_id, emp_no, employee_name, birth_date, department, designation, "
        "reporting_person, email, mobile_number, base_salary, joining_date, status, address, bank_account "
        "FROM employees ORDER BY department, employee_name;";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, stmt, 0);
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare get_all_employees: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    return 0;  // Success - caller gets stmt via pointer
}

/**
 * Update employee (MATCHES HEADER: const Employee *)
 */
int db_update_employee(const Employee *emp) {
    if (!emp) {
        printf("[ERROR] NULL employee pointer\n");
        return -1;
    }
    
    const char *sql = 
        "UPDATE employees SET emp_no=?, employee_name=?, birth_date=?, department=?, "
        "designation=?, reporting_person=?, email=?, mobile_number=?, base_salary=?, "
        "joining_date=?, status=?, address=?, bank_account=? WHERE emp_id=?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare update: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    // Bind all fields including emp_id as WHERE condition
    sqlite3_bind_text(stmt, 1, emp->emp_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, emp->employee_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, emp->birth_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, emp->department, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, emp->designation, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, emp->reporting_person, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, emp->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, emp->mobile_number, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 9, emp->base_salary);
    sqlite3_bind_text(stmt, 10, emp->joining_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, emp->status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, emp->address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, emp->bank_account, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 14, emp->emp_id);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to update employee %d: %s\n", emp->emp_id, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Employee %d updated\n", emp->emp_id);
    return 0;
}

/**
 * Get employee by ID
 */
int db_get_employee(int emp_id, Employee *emp) {
    if (!emp) {
        printf("[ERROR] NULL employee pointer\n");
        return 0;
    }
    
    const char *sql = 
        "SELECT emp_id, emp_no, employee_name, birth_date, department, designation, "
        "reporting_person, email, mobile_number, base_salary, joining_date, status, address, bank_account "
        "FROM employees WHERE emp_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare get_employee: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, emp_id);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        emp->emp_id = sqlite3_column_int(stmt, 0);
        strncpy(emp->emp_no, (const char*)sqlite3_column_text(stmt, 1), sizeof(emp->emp_no)-1);
        strncpy(emp->employee_name, (const char*)sqlite3_column_text(stmt, 2), sizeof(emp->employee_name)-1);
        strncpy(emp->birth_date, (const char*)sqlite3_column_text(stmt, 3), sizeof(emp->birth_date)-1);
        strncpy(emp->department, (const char*)sqlite3_column_text(stmt, 4), sizeof(emp->department)-1);
        strncpy(emp->designation, (const char*)sqlite3_column_text(stmt, 5), sizeof(emp->designation)-1);
        strncpy(emp->reporting_person, (const char*)sqlite3_column_text(stmt, 6), sizeof(emp->reporting_person)-1);
        strncpy(emp->email, (const char*)sqlite3_column_text(stmt, 7), sizeof(emp->email)-1);
        strncpy(emp->mobile_number, (const char*)sqlite3_column_text(stmt, 8), sizeof(emp->mobile_number)-1);
        emp->base_salary = sqlite3_column_double(stmt, 9);
        strncpy(emp->joining_date, (const char*)sqlite3_column_text(stmt, 10), sizeof(emp->joining_date)-1);
        strncpy(emp->status, (const char*)sqlite3_column_text(stmt, 11), sizeof(emp->status)-1);
        strncpy(emp->address, (const char*)sqlite3_column_text(stmt, 12), sizeof(emp->address)-1);
        strncpy(emp->bank_account, (const char*)sqlite3_column_text(stmt, 13), sizeof(emp->bank_account)-1);
        
        sqlite3_finalize(stmt);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

/**
 * Delete employee by ID
 */
int db_delete_employee(int emp_id) {
    const char *sql = "DELETE FROM employees WHERE emp_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare delete: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, emp_id);
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to delete employee %d: %s\n", emp_id, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Employee %d deleted\n", emp_id);
    return 0;
}
