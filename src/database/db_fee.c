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
        "    roll_no TEXT NOT NULL UNIQUE,"
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
    
    if (strlen(fee->roll_no) == 0) {
        fprintf(stderr, "[ERROR] Roll number cannot be empty\n");
        return 0;
    }
    
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
    
    // ✅ CORRECTED: Bind roll_no as TEXT, not int!
    sqlite3_bind_text(stmt, 1, fee->roll_no, -1, SQLITE_STATIC);
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
    
    printf("[SUCCESS] Fee record saved for roll_no: %s (ID: %d)\n", 
           fee->roll_no, fee->fee_id);
    return 1;
}


// ============================================================================
// GET FEE RECORD BY ROLL NO
// ============================================================================
// ✅ CORRECTED: Change parameter from int to const char*
int db_get_fee_record(const char *roll_no, FeeRecord *fee) {
    if (!fee || !roll_no) return 0;
    
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
    
    // ✅ CORRECTED: Bind roll_no as TEXT, not int!
    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        fee->fee_id = sqlite3_column_int(stmt, 0);
        
        // ✅ CORRECTED: Get roll_no as TEXT
        const char *roll_no_text = (const char *)sqlite3_column_text(stmt, 1);
        if (roll_no_text) {
            strncpy(fee->roll_no, roll_no_text, sizeof(fee->roll_no) - 1);
            fee->roll_no[sizeof(fee->roll_no) - 1] = '\0';
        }
        
        fee->institute_paid = sqlite3_column_double(stmt, 2);
        strncpy(fee->institute_date, (const char*)sqlite3_column_text(stmt, 3) ?: "", sizeof(fee->institute_date) - 1);
        fee->institute_due = sqlite3_column_double(stmt, 4);
        strncpy(fee->institute_mode, (const char*)sqlite3_column_text(stmt, 5) ?: "", sizeof(fee->institute_mode) - 1);
        
        fee->hostel_paid = sqlite3_column_double(stmt, 6);
        strncpy(fee->hostel_date, (const char*)sqlite3_column_text(stmt, 7) ?: "", sizeof(fee->hostel_date) - 1);
        fee->hostel_due = sqlite3_column_double(stmt, 8);
        strncpy(fee->hostel_mode, (const char*)sqlite3_column_text(stmt, 9) ?: "", sizeof(fee->hostel_mode) - 1);
        
        fee->mess_paid = sqlite3_column_double(stmt, 10);
        strncpy(fee->mess_date, (const char*)sqlite3_column_text(stmt, 11) ?: "", sizeof(fee->mess_date) - 1);
        fee->mess_due = sqlite3_column_double(stmt, 12);
        strncpy(fee->mess_mode, (const char*)sqlite3_column_text(stmt, 13) ?: "", sizeof(fee->mess_mode) - 1);
        
        fee->other_paid = sqlite3_column_double(stmt, 14);
        strncpy(fee->other_date, (const char*)sqlite3_column_text(stmt, 15) ?: "", sizeof(fee->other_date) - 1);
        fee->other_due = sqlite3_column_double(stmt, 16);
        strncpy(fee->other_mode, (const char*)sqlite3_column_text(stmt, 17) ?: "", sizeof(fee->other_mode) - 1);
        
        fee->total_paid = sqlite3_column_double(stmt, 18);
        fee->total_due = sqlite3_column_double(stmt, 19);
        fee->total_amount = sqlite3_column_double(stmt, 20);
        strncpy(fee->created_at, (const char*)sqlite3_column_text(stmt, 21) ?: "", sizeof(fee->created_at) - 1);
        strncpy(fee->updated_at, (const char*)sqlite3_column_text(stmt, 22) ?: "", sizeof(fee->updated_at) - 1);
        fee->created_by = sqlite3_column_int(stmt, 23);
        fee->status = sqlite3_column_int(stmt, 24);
        
        sqlite3_finalize(stmt);
        printf("[INFO] Fee record retrieved for roll_no: %s\n", roll_no);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] No fee record found for roll_no: %s\n", roll_no);
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
// ✅ CORRECTED: Change parameter from int to const char*
int db_delete_fee_record(const char *roll_no) {
    if (!roll_no) return 0;
    
    const char *sql = "DELETE FROM fees WHERE roll_no = ?";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        snprintf(error_msg, sizeof(error_msg), "Prepare Error: %s", 
                 sqlite3_errmsg(db));
        return 0;
    }
    
    // ✅ CORRECTED: Bind roll_no as TEXT, not int!
    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        snprintf(error_msg, sizeof(error_msg), "Execute Error: %s", 
                 sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }
    
    sqlite3_finalize(stmt);
    printf("[SUCCESS] Fee record deleted for roll_no: %s\n", roll_no);
    return 1;
}

// ============================================================================
// GET STUDENT BY ROLL NUMBER (STRING SEARCH) - ADD THIS FUNCTION
// ============================================================================
int db_get_student_for_card_by_roll(const char *roll_no, StudentIDCard *card) {
    if (!roll_no || strlen(roll_no) == 0 || !card) {
        fprintf(stderr, "[ERROR] Invalid input\n");
        return 0;
    }
    
    const char *sql = "SELECT student_id, roll_no, name, branch, gender, mobile, email "
                      "FROM Students WHERE roll_no = ? LIMIT 1";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        card->student_id = sqlite3_column_int(stmt, 0);
        const char *col_roll = (const char *)sqlite3_column_text(stmt, 1);
        const char *col_name = (const char *)sqlite3_column_text(stmt, 2);
        const char *col_branch = (const char *)sqlite3_column_text(stmt, 3);
        const char *col_gender = (const char *)sqlite3_column_text(stmt, 4);
        const char *col_mobile = (const char *)sqlite3_column_text(stmt, 5);
        const char *col_email = (const char *)sqlite3_column_text(stmt, 6);
        
        strncpy(card->roll_no, col_roll ? col_roll : "", sizeof(card->roll_no) - 1);
        card->roll_no[sizeof(card->roll_no) - 1] = '\0';
        strncpy(card->name, col_name ? col_name : "", sizeof(card->name) - 1);
        card->name[sizeof(card->name) - 1] = '\0';
        strncpy(card->branch, col_branch ? col_branch : "", sizeof(card->branch) - 1);
        card->branch[sizeof(card->branch) - 1] = '\0';
        strncpy(card->gender, col_gender ? col_gender : "", sizeof(card->gender) - 1);
        card->gender[sizeof(card->gender) - 1] = '\0';
        strncpy(card->mobile, col_mobile ? col_mobile : "", sizeof(card->mobile) - 1);
        card->mobile[sizeof(card->mobile) - 1] = '\0';
        strncpy(card->email, col_email ? col_email : "", sizeof(card->email) - 1);
        card->email[sizeof(card->email) - 1] = '\0';
        
        sqlite3_finalize(stmt);
        printf("[SUCCESS] Student found by roll_no: %s\n", card->roll_no);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    printf("[WARNING] Student not found with roll_no: %s\n", roll_no);
    return 0;
}