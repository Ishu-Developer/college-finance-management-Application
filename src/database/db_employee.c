#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../../include/database.h"

// External database connection (from db_init.c)
extern sqlite3 *db;

// ============ EMPLOYEE MANAGEMENT FUNCTIONS ============

int db_add_employee(const Employee *emp) {
    if (!db || !emp) {
        fprintf(stderr, "[ERROR] Database or Employee struct is NULL\n");
        return -1;
    }

    const char *sql =
        "INSERT INTO employees ("
        "emp_no, emp_name, emp_dob, department, designation, category, "
        "reporting_person_name, reporting_person_id, email, mobile_number, "
        "address, base_salary, status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare insert: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, emp->emp_no);
    sqlite3_bind_text(stmt, 2, emp->emp_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, emp->emp_dob, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, emp->department, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, emp->designation, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, emp->category, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, emp->reporting_person_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, emp->reporting_person_id);
    sqlite3_bind_text(stmt, 9, emp->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, emp->mobile_number, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, emp->address, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 12, emp->base_salary);
    sqlite3_bind_text(stmt, 13, emp->status, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to insert employee: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    int emp_id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Employee added with ID: %d, Name: %s\n", emp_id, emp->emp_name);
    return emp_id;
}

int db_get_all_employees(sqlite3_stmt **out_stmt) {
    if (!db || !out_stmt) {
        fprintf(stderr, "[ERROR] Database or output stmt pointer is NULL\n");
        return -1;
    }

    const char *sql =
        "SELECT emp_id, emp_no, emp_name, emp_dob, department, designation, "
        "category, reporting_person_name, reporting_person_id, email, "
        "mobile_number, address, base_salary, status "
        "FROM employees ORDER BY emp_id DESC;";

    int rc = sqlite3_prepare_v2(db, sql, -1, out_stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare select: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

int db_get_employee_by_id(int emp_id, Employee *emp) {
    if (!db || !emp || emp_id <= 0) {
        fprintf(stderr, "[ERROR] Invalid database or employee ID\n");
        return -1;
    }

    const char *sql =
        "SELECT emp_id, emp_no, emp_name, emp_dob, department, designation, "
        "category, reporting_person_name, reporting_person_id, email, "
        "mobile_number, address, base_salary, status "
        "FROM employees WHERE emp_id = ?;";

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare select: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, emp_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        emp->emp_id = sqlite3_column_int(stmt, 0);
        emp->emp_no = sqlite3_column_int(stmt, 1);
        strncpy(emp->emp_name, (const char *)sqlite3_column_text(stmt, 2), sizeof(emp->emp_name) - 1);
        strncpy(emp->emp_dob, (const char *)sqlite3_column_text(stmt, 3), sizeof(emp->emp_dob) - 1);
        strncpy(emp->department, (const char *)sqlite3_column_text(stmt, 4), sizeof(emp->department) - 1);
        strncpy(emp->designation, (const char *)sqlite3_column_text(stmt, 5), sizeof(emp->designation) - 1);
        strncpy(emp->category, (const char *)sqlite3_column_text(stmt, 6), sizeof(emp->category) - 1);
        strncpy(emp->reporting_person_name, (const char *)sqlite3_column_text(stmt, 7), sizeof(emp->reporting_person_name) - 1);
        emp->reporting_person_id = sqlite3_column_int(stmt, 8);
        strncpy(emp->email, (const char *)sqlite3_column_text(stmt, 9), sizeof(emp->email) - 1);
        strncpy(emp->mobile_number, (const char *)sqlite3_column_text(stmt, 10), sizeof(emp->mobile_number) - 1);
        strncpy(emp->address, (const char *)sqlite3_column_text(stmt, 11), sizeof(emp->address) - 1);
        emp->base_salary = sqlite3_column_double(stmt, 12);
        strncpy(emp->status, (const char *)sqlite3_column_text(stmt, 13), sizeof(emp->status) - 1);

        sqlite3_finalize(stmt);
        return emp->emp_id;
    }

    sqlite3_finalize(stmt);
    return -1;
}

int db_update_employee(int emp_id, const Employee *emp) {
    if (!db || !emp || emp_id <= 0) {
        fprintf(stderr, "[ERROR] Invalid database or employee ID\n");
        return -1;
    }

    const char *sql =
        "UPDATE employees SET "
        "emp_no=?, emp_name=?, emp_dob=?, department=?, designation=?, category=?, "
        "reporting_person_name=?, reporting_person_id=?, email=?, mobile_number=?, "
        "address=?, base_salary=?, status=? "
        "WHERE emp_id=?;";

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare update: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, emp->emp_no);
    sqlite3_bind_text(stmt, 2, emp->emp_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, emp->emp_dob, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, emp->department, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, emp->designation, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, emp->category, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, emp->reporting_person_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, emp->reporting_person_id);
    sqlite3_bind_text(stmt, 9, emp->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, emp->mobile_number, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, emp->address, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 12, emp->base_salary);
    sqlite3_bind_text(stmt, 13, emp->status, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 14, emp_id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to update employee\n");
        return -1;
    }

    printf("[SUCCESS] Employee %d updated\n", emp_id);
    return emp_id;
}

int db_delete_employee(int emp_id) {
    if (!db || emp_id <= 0) {
        fprintf(stderr, "[ERROR] Invalid database or employee ID\n");
        return -1;
    }

    const char *sql = "DELETE FROM employees WHERE emp_id=?;";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare delete: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, emp_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to delete employee\n");
        return -1;
    }

    printf("[SUCCESS] Employee %d deleted\n", emp_id);
    return emp_id;
}

int db_get_employee_count(void) {
    if (!db) {
        printf("[ERROR] Database not connected\n");
        return 0;
    }

    const char *sql = "SELECT COUNT(*) FROM employees;";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare count statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    printf("[INFO] Total employees in database: %d\n", count);
    return count;
}

// ============ BANK DETAILS FUNCTIONS ============

int db_add_bank_details(const BankDetails *bank) {
    if (!db || !bank) {
        fprintf(stderr, "[ERROR] Database or BankDetails struct is NULL\n");
        return -1;
    }

    const char *sql =
        "INSERT INTO bank_details ("
        "emp_id, account_holder_name, account_number, bank_name, ifsc_code, bank_address) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare bank insert: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, bank->emp_id);
    sqlite3_bind_text(stmt, 2, bank->account_holder_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, bank->account_number, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, bank->bank_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, bank->ifsc_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, bank->bank_address, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to insert bank details: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    int bank_id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Bank details added for employee %d\n", bank->emp_id);
    return bank_id;
}

int db_get_bank_details(int emp_id, BankDetails *bank) {
    if (!db || !bank || emp_id <= 0) {
        fprintf(stderr, "[ERROR] Invalid database or employee ID\n");
        return -1;
    }

    const char *sql =
        "SELECT bank_id, emp_id, account_holder_name, account_number, "
        "bank_name, ifsc_code, bank_address "
        "FROM bank_details WHERE emp_id = ?;";

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare bank select: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, emp_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        bank->bank_id = sqlite3_column_int(stmt, 0);
        bank->emp_id = sqlite3_column_int(stmt, 1);
        strncpy(bank->account_holder_name, (const char *)sqlite3_column_text(stmt, 2), sizeof(bank->account_holder_name) - 1);
        strncpy(bank->account_number, (const char *)sqlite3_column_text(stmt, 3), sizeof(bank->account_number) - 1);
        strncpy(bank->bank_name, (const char *)sqlite3_column_text(stmt, 4), sizeof(bank->bank_name) - 1);
        strncpy(bank->ifsc_code, (const char *)sqlite3_column_text(stmt, 5), sizeof(bank->ifsc_code) - 1);
        strncpy(bank->bank_address, (const char *)sqlite3_column_text(stmt, 6), sizeof(bank->bank_address) - 1);

        sqlite3_finalize(stmt);
        return bank->bank_id;
    }

    sqlite3_finalize(stmt);
    return -1;
}

int db_update_bank_details(int emp_id, const BankDetails *bank) {
    if (!db || !bank || emp_id <= 0) {
        fprintf(stderr, "[ERROR] Invalid database or employee ID\n");
        return -1;
    }

    const char *sql =
        "UPDATE bank_details SET "
        "account_holder_name=?, account_number=?, bank_name=?, "
        "ifsc_code=?, bank_address=? "
        "WHERE emp_id=?;";

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare bank update: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, bank->account_holder_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, bank->account_number, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, bank->bank_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, bank->ifsc_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, bank->bank_address, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, emp_id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to update bank details\n");
        return -1;
    }

    printf("[SUCCESS] Bank details updated for employee %d\n", emp_id);
    return emp_id;
}

int db_delete_bank_details(int emp_id) {
    if (!db || emp_id <= 0) {
        fprintf(stderr, "[ERROR] Invalid database or employee ID\n");
        return -1;
    }

    const char *sql = "DELETE FROM bank_details WHERE emp_id=?;";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare bank delete: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, emp_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to delete bank details\n");
        return -1;
    }

    printf("[SUCCESS] Bank details deleted for employee %d\n", emp_id);
    return emp_id;
}
