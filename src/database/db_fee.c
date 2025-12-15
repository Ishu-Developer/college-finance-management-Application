#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include "../../include/database.h"

extern sqlite3 *db;
static char error_msg[256] = {0};

// ============================================================================
// CREATE FEE TABLE
// ============================================================================
int db_create_fee_table() {
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS fees ("
        "    fee_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    roll_no INTEGER NOT NULL UNIQUE,"
        "    institute_paid REAL DEFAULT 0,"
        "    institute_date TEXT,"
        "    institute_due REAL DEFAULT 0,"
        "    institute_mode TEXT,"
        "    hostel_paid REAL DEFAULT 0,"
        "    hostel_date TEXT,"
        "    hostel_due REAL DEFAULT 0,"
        "    hostel_mode TEXT,"
        "    mess_paid REAL DEFAULT 0,"
        "    mess_date TEXT,"
        "    mess_due REAL DEFAULT 0,"
        "    mess_mode TEXT,"
        "    other_paid REAL DEFAULT 0,"
        "    other_date TEXT,"
        "    other_due REAL DEFAULT 0,"
        "    other_mode TEXT,"
        "    total_paid REAL DEFAULT 0,"
        "    total_due REAL DEFAULT 0,"
        "    total_amount REAL DEFAULT 0,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    created_by INTEGER,"
        "    status INTEGER DEFAULT 0,"
        "    FOREIGN KEY(roll_no) REFERENCES students(roll_no) ON DELETE CASCADE"
        ");";
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        snprintf(error_msg, sizeof(error_msg), "SQL Error: %s", err_msg);
        sqlite3_free(err_msg);
        printf("[ERROR] Failed to create fee table: %s\n", error_msg);
        return 0;
    }
    
    printf("[INFO] Fee table created successfully\n");
    return 1;
}

// ============================================================================
// SAVE NEW FEE RECORD
// ============================================================================
int db_save_fee_record(FeeRecord *fee) {
    if (!fee) return 0;
    
    // Calculate totals
    fee->total_paid = fee->institute_paid + fee->hostel_paid + 
                      fee->mess_paid + fee->other_paid;
    fee->total_due = fee->institute_due + fee->hostel_due + 
                     fee->mess_due + fee->other_due;
    fee->total_amount = fee->total_paid + fee->total_due;
    
    // Get current timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(fee->created_at, sizeof(fee->created_at), 
             "%Y-%m-%d %H:%M:%S", tm_info);
    strcpy(fee->updated_at, fee->created_at);
    
    const char *sql = 
        "INSERT INTO fees ("
        "    roll_no, institute_paid, institute_date, institute_due, institute_mode,"
        "    hostel_paid, hostel_date, hostel_due, hostel_mode,"
        "    mess_paid, mess_date, mess_due, mess_mode,"
        "    other_paid, other_date, other_due, other_mode,"
        "    total_paid, total_due, total_amount,"
        "    created_at, updated_at, status"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        snprintf(error_msg, sizeof(error_msg), "Prepare Error: %s", 
                 sqlite3_errmsg(db));
        printf("[ERROR] %s\n", error_msg);
        return 0;
    }
    
    // Bind values
    sqlite3_bind_int(stmt, 1, fee->roll_no);
    sqlite3_bind_double(stmt, 2, fee->institute_paid);
    sqlite3_bind_text(stmt, 3, fee->institute_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, fee->institute_due);
    sqlite3_bind_text(stmt, 5, fee->institute_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 6, fee->hostel_paid);
    sqlite3_bind_text(stmt, 7, fee->hostel_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 8, fee->hostel_due);
    sqlite3_bind_text(stmt, 9, fee->hostel_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 10, fee->mess_paid);
    sqlite3_bind_text(stmt, 11, fee->mess_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 12, fee->mess_due);
    sqlite3_bind_text(stmt, 13, fee->mess_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 14, fee->other_paid);
    sqlite3_bind_text(stmt, 15, fee->other_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 16, fee->other_due);
    sqlite3_bind_text(stmt, 17, fee->other_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 18, fee->total_paid);
    sqlite3_bind_double(stmt, 19, fee->total_due);
    sqlite3_bind_double(stmt, 20, fee->total_amount);
    sqlite3_bind_text(stmt, 21, fee->created_at, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 22, fee->updated_at, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 23, fee->status);
    
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        snprintf(error_msg, sizeof(error_msg), "Execute Error: %s", 
                 sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        printf("[ERROR] Failed to save fee: %s\n", error_msg);
        return 0;
    }
    
    fee->fee_id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    printf("[SUCCESS] Fee record saved for roll_no: %d (ID: %d)\n", 
           fee->roll_no, fee->fee_id);
    return 1;
}

// ============================================================================
// GET FEE RECORD BY ROLL NO
// ============================================================================
int db_get_fee_record(int roll_no, FeeRecord *fee) {
    if (!fee) return 0;
    
    const char *sql = 
        "SELECT fee_id, roll_no, institute_paid, institute_date, institute_due, institute_mode,"
        "       hostel_paid, hostel_date, hostel_due, hostel_mode,"
        "       mess_paid, mess_date, mess_due, mess_mode,"
        "       other_paid, other_date, other_due, other_mode,"
        "       total_paid, total_due, total_amount,"
        "       created_at, updated_at, created_by, status "
        "FROM fees WHERE roll_no = ?";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        snprintf(error_msg, sizeof(error_msg), "Prepare Error: %s", 
                 sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, roll_no);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        fee->fee_id = sqlite3_column_int(stmt, 0);
        fee->roll_no = sqlite3_column_int(stmt, 1);
        fee->institute_paid = sqlite3_column_double(stmt, 2);
        strcpy(fee->institute_date, (const char*)sqlite3_column_text(stmt, 3) ?: "");
        fee->institute_due = sqlite3_column_double(stmt, 4);
        strcpy(fee->institute_mode, (const char*)sqlite3_column_text(stmt, 5) ?: "");
        
        fee->hostel_paid = sqlite3_column_double(stmt, 6);
        strcpy(fee->hostel_date, (const char*)sqlite3_column_text(stmt, 7) ?: "");
        fee->hostel_due = sqlite3_column_double(stmt, 8);
        strcpy(fee->hostel_mode, (const char*)sqlite3_column_text(stmt, 9) ?: "");
        
        fee->mess_paid = sqlite3_column_double(stmt, 10);
        strcpy(fee->mess_date, (const char*)sqlite3_column_text(stmt, 11) ?: "");
        fee->mess_due = sqlite3_column_double(stmt, 12);
        strcpy(fee->mess_mode, (const char*)sqlite3_column_text(stmt, 13) ?: "");
        
        fee->other_paid = sqlite3_column_double(stmt, 14);
        strcpy(fee->other_date, (const char*)sqlite3_column_text(stmt, 15) ?: "");
        fee->other_due = sqlite3_column_double(stmt, 16);
        strcpy(fee->other_mode, (const char*)sqlite3_column_text(stmt, 17) ?: "");
        
        fee->total_paid = sqlite3_column_double(stmt, 18);
        fee->total_due = sqlite3_column_double(stmt, 19);
        fee->total_amount = sqlite3_column_double(stmt, 20);
        strcpy(fee->created_at, (const char*)sqlite3_column_text(stmt, 21) ?: "");
        strcpy(fee->updated_at, (const char*)sqlite3_column_text(stmt, 22) ?: "");
        fee->created_by = sqlite3_column_int(stmt, 23);
        fee->status = sqlite3_column_int(stmt, 24);
        
        sqlite3_finalize(stmt);
        printf("[INFO] Fee record retrieved for roll_no: %d\n", roll_no);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] No fee record found for roll_no: %d\n", roll_no);
    return 0;
}

// ============================================================================
// UPDATE FEE RECORD
// ============================================================================
int db_update_fee_record(FeeRecord *fee) {
    if (!fee || fee->fee_id == 0) return 0;
    
    // Recalculate totals
    fee->total_paid = fee->institute_paid + fee->hostel_paid + 
                      fee->mess_paid + fee->other_paid;
    fee->total_due = fee->institute_due + fee->hostel_due + 
                     fee->mess_due + fee->other_due;
    fee->total_amount = fee->total_paid + fee->total_due;
    
    // Update timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(fee->updated_at, sizeof(fee->updated_at), 
             "%Y-%m-%d %H:%M:%S", tm_info);
    
    const char *sql = 
        "UPDATE fees SET "
        "    institute_paid=?, institute_date=?, institute_due=?, institute_mode=?,"
        "    hostel_paid=?, hostel_date=?, hostel_due=?, hostel_mode=?,"
        "    mess_paid=?, mess_date=?, mess_due=?, mess_mode=?,"
        "    other_paid=?, other_date=?, other_due=?, other_mode=?,"
        "    total_paid=?, total_due=?, total_amount=?,"
        "    updated_at=?, status=? "
        "WHERE fee_id=?";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        snprintf(error_msg, sizeof(error_msg), "Prepare Error: %s", 
                 sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_double(stmt, 1, fee->institute_paid);
    sqlite3_bind_text(stmt, 2, fee->institute_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, fee->institute_due);
    sqlite3_bind_text(stmt, 4, fee->institute_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 5, fee->hostel_paid);
    sqlite3_bind_text(stmt, 6, fee->hostel_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 7, fee->hostel_due);
    sqlite3_bind_text(stmt, 8, fee->hostel_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 9, fee->mess_paid);
    sqlite3_bind_text(stmt, 10, fee->mess_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 11, fee->mess_due);
    sqlite3_bind_text(stmt, 12, fee->mess_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 13, fee->other_paid);
    sqlite3_bind_text(stmt, 14, fee->other_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 15, fee->other_due);
    sqlite3_bind_text(stmt, 16, fee->other_mode, -1, SQLITE_STATIC);
    
    sqlite3_bind_double(stmt, 17, fee->total_paid);
    sqlite3_bind_double(stmt, 18, fee->total_due);
    sqlite3_bind_double(stmt, 19, fee->total_amount);
    sqlite3_bind_text(stmt, 20, fee->updated_at, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 21, fee->status);
    sqlite3_bind_int(stmt, 22, fee->fee_id);
    
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        snprintf(error_msg, sizeof(error_msg), "Execute Error: %s", 
                 sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }
    
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Fee record updated (ID: %d)\n", fee->fee_id);
    return 1;
}

// ============================================================================
// DELETE FEE RECORD
// ============================================================================
int db_delete_fee_record(int roll_no) {
    const char *sql = "DELETE FROM fees WHERE roll_no = ?";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        snprintf(error_msg, sizeof(error_msg), "Prepare Error: %s", 
                 sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, roll_no);
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        snprintf(error_msg, sizeof(error_msg), "Execute Error: %s", 
                 sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }
    
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Fee record deleted for roll_no: %d\n", roll_no);
    return 1;
}

// ============================================================================
// GET STUDENT FOR ID CARD
// ============================================================================
int db_get_student_for_card(int roll_no, StudentIDCard *card) {
    if (roll_no <= 0 || !card) {
        printf("[ERROR] Invalid parameters in db_get_student_for_card\n");
        return 0;
    }

    const char *sql =
        "SELECT student_id, roll_no, name, branch, mobile, email FROM students "
        "WHERE roll_no = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, roll_no);
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        card->student_id = sqlite3_column_int(stmt, 0);
        card->roll_no = sqlite3_column_int(stmt, 1);

        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        if (name) strncpy(card->name, name, sizeof(card->name) - 1);

        const char *branch = (const char *)sqlite3_column_text(stmt, 3);
        if (branch) strncpy(card->branch, branch, sizeof(card->branch) - 1);

        const char *mobile = (const char *)sqlite3_column_text(stmt, 4);
        if (mobile) strncpy(card->mobile, mobile, sizeof(card->mobile) - 1);

        const char *email = (const char *)sqlite3_column_text(stmt, 5);
        if (email) strncpy(card->email, email, sizeof(card->email) - 1);

        sqlite3_finalize(stmt);
        printf("[INFO] Student card data retrieved for roll_no: %d\n", roll_no);
        return 1;
    }

    sqlite3_finalize(stmt);
    printf("[WARNING] Student with roll_no %d not found\n", roll_no);
    return 0;
}