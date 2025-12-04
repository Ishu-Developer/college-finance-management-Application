#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../../include/database.h"
#include "../../include/fee_ui.h"

// External database connection
extern sqlite3 *db;

/**
 * @brief Initialize fee table in database
 */
int db_fee_init() {
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS Fees ("
        "  fee_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  student_id INTEGER NOT NULL,"
        "  roll_no TEXT NOT NULL,"
        "  fee_type TEXT NOT NULL,"
        "  amount REAL NOT NULL,"
        "  amount_paid REAL DEFAULT 0.0,"
        "  amount_due REAL NOT NULL,"
        "  due_date TEXT NOT NULL,"
        "  status TEXT DEFAULT 'Pending',"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (student_id) REFERENCES Students(student_id)"
        ");";
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to create Fees table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    printf("[INFO] Fees table initialized successfully\n");
    
    // Create index on roll_no for faster searches
    const char *index_sql = "CREATE INDEX IF NOT EXISTS idx_fee_rollno ON Fees(roll_no);";
    rc = sqlite3_exec(db, index_sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("[WARNING] Failed to create index: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    return 0;
}

/**
 * @brief Add new fee record
 */
int db_add_fee(const char *roll_no, const char *fee_type, double amount, const char *due_date) {
    
    if (!roll_no || !fee_type || !due_date || amount <= 0) {
        printf("[ERROR] Invalid parameters in db_add_fee\n");
        return -1;
    }
    
    // Get student_id from roll_no
    const char *get_id_sql = "SELECT student_id FROM Students WHERE roll_no = ?;";
    sqlite3_stmt *id_stmt;
    int rc = sqlite3_prepare_v2(db, get_id_sql, -1, &id_stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare get_id statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(id_stmt, 1, roll_no, -1, SQLITE_STATIC);
    
    int student_id = -1;
    if (sqlite3_step(id_stmt) == SQLITE_ROW) {
        student_id = sqlite3_column_int(id_stmt, 0);
    }
    sqlite3_finalize(id_stmt);
    
    if (student_id == -1) {
        printf("[ERROR] Student with roll_no %s not found\n", roll_no);
        return -1;
    }
    
    const char *sql = 
        "INSERT INTO Fees (student_id, roll_no, fee_type, amount, amount_due, due_date, status) "
        "VALUES (?, ?, ?, ?, ?, ?, 'Pending');";
    
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_text(stmt, 2, roll_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, fee_type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, amount);
    sqlite3_bind_double(stmt, 5, amount);  // Initially amount_due = amount
    sqlite3_bind_text(stmt, 6, due_date, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to insert fee: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    int fee_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    printf("[SUCCESS] Fee added with ID: %d (Roll: %s, Type: %s, Amount: %.2f)\n", 
           fee_id, roll_no, fee_type, amount);
    
    return fee_id;
}

/**
 * @brief Get all fees with student details
 */
sqlite3_stmt* db_get_all_fees() {
    const char *sql = 
        "SELECT f.fee_id, f.student_id, f.roll_no, s.name, s.branch, s.semester, "
        "f.fee_type, f.amount, f.amount_paid, f.amount_due, f.due_date, f.status "
        "FROM Fees f "
        "LEFT JOIN Students s ON f.student_id = s.student_id "
        "ORDER BY f.due_date;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare get_all_fees: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    return stmt;
}

/**
 * @brief Get fees by roll number
 */
sqlite3_stmt* db_get_fees_by_rollno(const char *roll_no) {
    
    if (!roll_no) {
        printf("[ERROR] NULL roll_no in db_get_fees_by_rollno\n");
        return NULL;
    }
    
    const char *sql = 
        "SELECT f.fee_id, f.student_id, f.roll_no, s.name, s.branch, s.semester, "
        "f.fee_type, f.amount, f.amount_paid, f.amount_due, f.due_date, f.status "
        "FROM Fees f "
        "LEFT JOIN Students s ON f.student_id = s.student_id "
        "WHERE f.roll_no = ? "
        "ORDER BY f.fee_type;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare get_fees_by_rollno: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_TRANSIENT);
    
    return stmt;
}

/**
 * @brief Get student details for ID card display
 */
int db_get_student_for_card(const char *roll_no, StudentIDCard *card) {
    
    if (!roll_no || !card) {
        printf("[ERROR] NULL parameters in db_get_student_for_card\n");
        return 0;
    }
    
    const char *sql = 
        "SELECT student_id, roll_no, name, branch, semester FROM Students "
        "WHERE roll_no = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        card->student_id = sqlite3_column_int(stmt, 0);
        strncpy(card->roll_no, (const char *)sqlite3_column_text(stmt, 1), sizeof(card->roll_no) - 1);
        strncpy(card->name, (const char *)sqlite3_column_text(stmt, 2), sizeof(card->name) - 1);
        strncpy(card->branch, (const char *)sqlite3_column_text(stmt, 3), sizeof(card->branch) - 1);
        card->semester = sqlite3_column_int(stmt, 4);
        
        sqlite3_finalize(stmt);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    printf("[WARNING] Student with roll_no %s not found\n", roll_no);
    return 0;
}

/**
 * @brief Record payment for a fee
 */
int db_record_payment(int fee_id, double amount_paid, const char *payment_date) {
    
    if (fee_id <= 0 || amount_paid <= 0 || !payment_date) {
        printf("[ERROR] Invalid parameters in db_record_payment\n");
        return -1;
    }
    
    // Get current fee details
    const char *get_fee_sql = "SELECT amount_paid, amount_due FROM Fees WHERE fee_id = ?;";
    sqlite3_stmt *get_stmt;
    int rc = sqlite3_prepare_v2(db, get_fee_sql, -1, &get_stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to get fee details: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(get_stmt, 1, fee_id);
    
    double current_paid = 0, current_due = 0;
    if (sqlite3_step(get_stmt) == SQLITE_ROW) {
        current_paid = sqlite3_column_double(get_stmt, 0);
        current_due = sqlite3_column_double(get_stmt, 1);
    }
    sqlite3_finalize(get_stmt);
    
    double new_paid = current_paid + amount_paid;
    double new_due = current_due - amount_paid;
    
    if (new_due < 0) {
        printf("[ERROR] Payment exceeds due amount\n");
        return -1;
    }
    
    // Update fee
    const char *update_sql = 
        "UPDATE Fees SET amount_paid = ?, amount_due = ?, "
        "status = CASE WHEN ? <= 0 THEN 'Paid' WHEN ? > 0 AND ? < ? THEN 'Partial' ELSE 'Pending' END "
        "WHERE fee_id = ?;";
    
    sqlite3_stmt *update_stmt;
    rc = sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare update: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    double original_amount = current_paid + current_due;
    sqlite3_bind_double(update_stmt, 1, new_paid);
    sqlite3_bind_double(update_stmt, 2, new_due);
    sqlite3_bind_double(update_stmt, 3, new_due);
    sqlite3_bind_double(update_stmt, 4, new_paid);
    sqlite3_bind_double(update_stmt, 5, original_amount);
    sqlite3_bind_double(update_stmt, 6, original_amount);
    sqlite3_bind_int(update_stmt, 7, fee_id);
    
    rc = sqlite3_step(update_stmt);
    
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to record payment: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(update_stmt);
        return -1;
    }
    
    sqlite3_finalize(update_stmt);
    printf("[SUCCESS] Payment recorded: Fee ID %d, Amount: %.2f\n", fee_id, amount_paid);
    
    return 1;
}

/**
 * @brief Update fee status
 */
int db_update_fee_status(int fee_id, const char *status) {
    
    if (fee_id <= 0 || !status) {
        printf("[ERROR] Invalid parameters in db_update_fee_status\n");
        return -1;
    }
    
    const char *sql = "UPDATE Fees SET status = ? WHERE fee_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare update: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, status, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, fee_id);
    
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to update status: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

/**
 * @brief Get total fees and dues for a student
 */
int db_get_student_fee_summary(const char *roll_no, double *total_amount, 
                               double *total_paid, double *total_due) {
    
    if (!roll_no || !total_amount || !total_paid || !total_due) {
        printf("[ERROR] NULL parameters in db_get_student_fee_summary\n");
        return 0;
    }
    
    const char *sql = 
        "SELECT SUM(amount), SUM(amount_paid), SUM(amount_due) FROM Fees "
        "WHERE roll_no = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare summary query: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        *total_amount = sqlite3_column_double(stmt, 0);
        *total_paid = sqlite3_column_double(stmt, 1);
        *total_due = sqlite3_column_double(stmt, 2);
        
        sqlite3_finalize(stmt);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}