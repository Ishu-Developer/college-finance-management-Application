#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <glib.h>                    // ✅ REQUIRED: For g_strlcpy()
#include "../../include/database.h"


extern sqlite3 *db;


// ============================================================================
// DATABASE FUNCTIONS FOR FEE MODULE
// ============================================================================


int db_get_all_fee_summary_rows(FeeTableRow **out_rows) {
    if (!db || !out_rows) return 0;

    const char *query = 
        "SELECT "
        "    s.student_id, "
        "    s.name, "
        "    s.roll_no, "
        "    s.branch, "
        "    s.year, "
        "    s.semester, "
        "    s.category, "
        "    s.mobile, "
        "    COALESCE(fs.institute_paid, 0) as institute_paid, "
        "    COALESCE(fs.hostel_paid, 0) as hostel_paid, "
        "    COALESCE(fs.mess_paid, 0) as mess_paid, "
        "    COALESCE(fs.other_paid, 0) as other_paid, "
        "    COALESCE(fs.total_paid, 0) as total_paid "
        "FROM Students s "
        "LEFT JOIN FeeSummary fs ON s.student_id = fs.student_id "
        "ORDER BY s.roll_no ASC";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare query: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // Count rows
    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        row_count++;
    }

    if (row_count == 0) {
        sqlite3_finalize(stmt);
        *out_rows = NULL;
        return 0;
    }

    sqlite3_reset(stmt);

    *out_rows = (FeeTableRow *)malloc(row_count * sizeof(FeeTableRow));
    if (*out_rows == NULL) {
        fprintf(stderr, "[ERROR] Memory allocation failed\n");
        sqlite3_finalize(stmt);
        return 0;
    }

    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && index < row_count) {
        FeeTableRow *row = &(*out_rows)[index];

        row->student_id = sqlite3_column_int(stmt, 0);
        
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        g_strlcpy(row->student_name, name ? name : "", sizeof(row->student_name));
        
        const char *roll_no = (const char *)sqlite3_column_text(stmt, 2);
        g_strlcpy(row->roll_no, roll_no ? roll_no : "", sizeof(row->roll_no));
        
        const char *branch = (const char *)sqlite3_column_text(stmt, 3);
        g_strlcpy(row->branch, branch ? branch : "", sizeof(row->branch));
        
        row->year = sqlite3_column_int(stmt, 4);
        row->semester = sqlite3_column_int(stmt, 5);
        
        const char *category = (const char *)sqlite3_column_text(stmt, 6);
        g_strlcpy(row->category, category ? category : "", sizeof(row->category));
        
        const char *mobile = (const char *)sqlite3_column_text(stmt, 7);
        g_strlcpy(row->mobile, mobile ? mobile : "", sizeof(row->mobile));
        
        row->institute_paid = sqlite3_column_double(stmt, 8);
        row->hostel_paid = sqlite3_column_double(stmt, 9);
        row->mess_paid = sqlite3_column_double(stmt, 10);
        row->other_paid = sqlite3_column_double(stmt, 11);
        row->total_paid = sqlite3_column_double(stmt, 12);

        strcpy(row->status, "Active");

        index++;
    }

    sqlite3_finalize(stmt);
    printf("[INFO] Retrieved %d fee summary rows\n", row_count);
    return row_count;
}


int db_search_fee_summary_by_criteria(const char *search_text, FeeTableRow **out_rows) {
    if (!db || !out_rows || !search_text) return 0;

    const char *query = 
        "SELECT "
        "    s.student_id, "
        "    s.name, "
        "    s.roll_no, "
        "    s.branch, "
        "    s.year, "
        "    s.semester, "
        "    s.category, "
        "    s.mobile, "
        "    COALESCE(fs.institute_paid, 0), "
        "    COALESCE(fs.hostel_paid, 0), "
        "    COALESCE(fs.mess_paid, 0), "
        "    COALESCE(fs.other_paid, 0), "
        "    COALESCE(fs.total_paid, 0) "
        "FROM Students s "
        "LEFT JOIN FeeSummary fs ON s.student_id = fs.student_id "
        "WHERE s.name LIKE ? OR s.roll_no LIKE ? OR s.branch LIKE ? "
        "ORDER BY s.roll_no ASC";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare search: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    char pattern[512];
    snprintf(pattern, sizeof(pattern), "%%%s%%", search_text);

    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pattern, -1, SQLITE_TRANSIENT);

    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        row_count++;
    }

    if (row_count == 0) {
        sqlite3_finalize(stmt);
        *out_rows = NULL;
        return 0;
    }

    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pattern, -1, SQLITE_TRANSIENT);

    *out_rows = (FeeTableRow *)malloc(row_count * sizeof(FeeTableRow));
    if (*out_rows == NULL) {
        fprintf(stderr, "[ERROR] Memory allocation failed\n");
        sqlite3_finalize(stmt);
        return 0;
    }

    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && index < row_count) {
        FeeTableRow *row = &(*out_rows)[index];

        row->student_id = sqlite3_column_int(stmt, 0);
        
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        g_strlcpy(row->student_name, name ? name : "", sizeof(row->student_name));
        
        const char *roll_no = (const char *)sqlite3_column_text(stmt, 2);
        g_strlcpy(row->roll_no, roll_no ? roll_no : "", sizeof(row->roll_no));
        
        const char *branch = (const char *)sqlite3_column_text(stmt, 3);
        g_strlcpy(row->branch, branch ? branch : "", sizeof(row->branch));
        
        row->year = sqlite3_column_int(stmt, 4);
        row->semester = sqlite3_column_int(stmt, 5);
        
        const char *category = (const char *)sqlite3_column_text(stmt, 6);
        g_strlcpy(row->category, category ? category : "", sizeof(row->category));
        
        const char *mobile = (const char *)sqlite3_column_text(stmt, 7);
        g_strlcpy(row->mobile, mobile ? mobile : "", sizeof(row->mobile));
        
        row->institute_paid = sqlite3_column_double(stmt, 8);
        row->hostel_paid = sqlite3_column_double(stmt, 9);
        row->mess_paid = sqlite3_column_double(stmt, 10);
        row->other_paid = sqlite3_column_double(stmt, 11);
        row->total_paid = sqlite3_column_double(stmt, 12);

        strcpy(row->status, "Active");

        index++;
    }

    sqlite3_finalize(stmt);
    printf("[INFO] Found %d matching records\n", row_count);
    return row_count;
}


int db_get_fee_record(const char *roll_no, FeeRecord *out_fee) {
    if (!db || !roll_no || !out_fee) return 0;

    const char *query = 
        "SELECT "
        "    fee_id, student_id, roll_no, "
        "    COALESCE(SUM(CASE WHEN fee_type='Institute' THEN paid_amount ELSE 0 END), 0) as inst_paid, "
        "    COALESCE(SUM(CASE WHEN fee_type='Hostel' THEN paid_amount ELSE 0 END), 0) as host_paid, "
        "    COALESCE(SUM(CASE WHEN fee_type='Mess' THEN paid_amount ELSE 0 END), 0) as mess_paid, "
        "    COALESCE(SUM(CASE WHEN fee_type='Other' THEN paid_amount ELSE 0 END), 0) as other_paid "
        "FROM Fees WHERE roll_no = ?";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare fee query: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        out_fee->fee_id = sqlite3_column_int(stmt, 0);
        out_fee->student_id = sqlite3_column_int(stmt, 1);
        
        const char *rn = (const char *)sqlite3_column_text(stmt, 2);
        g_strlcpy(out_fee->roll_no, rn ? rn : "", sizeof(out_fee->roll_no));

        // Set paid amounts
        out_fee->institute_paid = sqlite3_column_double(stmt, 3);
        out_fee->hostel_paid = sqlite3_column_double(stmt, 4);
        out_fee->mess_paid = sqlite3_column_double(stmt, 5);
        out_fee->other_paid = sqlite3_column_double(stmt, 6);

        // Calculate total
        out_fee->total_paid = out_fee->institute_paid + out_fee->hostel_paid + 
                             out_fee->mess_paid + out_fee->other_paid;

        sqlite3_finalize(stmt);
        printf("[INFO] Fee record found for: %s\n", roll_no);
        return 1;
    }

    sqlite3_finalize(stmt);
    printf("[INFO] No fee record found for: %s\n", roll_no);
    return 0;
}


int db_save_fee_record(FeeRecord *fee) {
    if (!db || !fee) return 0;

    // Get student_id
    const char *get_student_query = "SELECT student_id FROM Students WHERE roll_no = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, get_student_query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare student query: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, fee->roll_no, -1, SQLITE_TRANSIENT);
    
    int student_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student_id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (student_id < 0) {
        fprintf(stderr, "[ERROR] Student not found for roll_no: %s\n", fee->roll_no);
        return 0;
    }

    // Insert fee records
    const char *insert_query = 
        "INSERT INTO Fees (student_id, roll_no, fee_type, paid_amount, paid_date, payment_mode, status, record_status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, insert_query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare insert: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int success = 1;

    // Insert Institute Fee
    if (fee->institute_paid > 0) {
        sqlite3_bind_int(stmt, 1, student_id);
        sqlite3_bind_text(stmt, 2, fee->roll_no, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, "Institute", -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, fee->institute_paid);
        sqlite3_bind_text(stmt, 5, fee->institute_date, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, fee->institute_mode, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, "Paid", -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 8, 1);  // record_status: 1 = Submitted

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "[ERROR] Failed to insert institute fee\n");
            success = 0;
        }
        sqlite3_reset(stmt);
    }

    // Insert Hostel Fee
    if (fee->hostel_paid > 0) {
        sqlite3_bind_int(stmt, 1, student_id);
        sqlite3_bind_text(stmt, 2, fee->roll_no, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, "Hostel", -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, fee->hostel_paid);
        sqlite3_bind_text(stmt, 5, fee->hostel_date, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, fee->hostel_mode, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, "Paid", -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 8, 1);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "[ERROR] Failed to insert hostel fee\n");
            success = 0;
        }
        sqlite3_reset(stmt);
    }

    // Insert Mess Fee
    if (fee->mess_paid > 0) {
        sqlite3_bind_int(stmt, 1, student_id);
        sqlite3_bind_text(stmt, 2, fee->roll_no, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, "Mess", -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, fee->mess_paid);
        sqlite3_bind_text(stmt, 5, fee->mess_date, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, fee->mess_mode, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, "Paid", -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 8, 1);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "[ERROR] Failed to insert mess fee\n");
            success = 0;
        }
        sqlite3_reset(stmt);
    }

    // Insert Other Fee
    if (fee->other_paid > 0) {
        sqlite3_bind_int(stmt, 1, student_id);
        sqlite3_bind_text(stmt, 2, fee->roll_no, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, "Other", -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, fee->other_paid);
        sqlite3_bind_text(stmt, 5, fee->other_date, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, fee->other_mode, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, "Paid", -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 8, 1);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "[ERROR] Failed to insert other fee\n");
            success = 0;
        }
    }

    sqlite3_finalize(stmt);

    if (success) {
        db_update_fee_summary(student_id, fee->roll_no, fee);
    }

    return success;
}


int db_update_fee_record(FeeRecord *fee) {
    if (!db || !fee) return 0;

    const char *get_student_query = "SELECT student_id FROM Students WHERE roll_no = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, get_student_query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare student query\n");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, fee->roll_no, -1, SQLITE_TRANSIENT);
    
    int student_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student_id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (student_id < 0) {
        fprintf(stderr, "[ERROR] Student not found\n");
        return 0;
    }

    // Delete old records
    const char *delete_query = "DELETE FROM Fees WHERE student_id = ? AND roll_no = ?";
    
    if (sqlite3_prepare_v2(db, delete_query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare delete\n");
        return 0;
    }

    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_text(stmt, 2, fee->roll_no, -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to delete old records\n");
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);

    return db_save_fee_record(fee);
}


int db_update_fee_summary(int student_id, const char *roll_no, const FeeRecord *fee) {
    if (!db || !roll_no) return 0;

    const char *upsert_query = 
        "INSERT INTO FeeSummary (student_id, roll_no, institute_paid, hostel_paid, mess_paid, other_paid, total_paid, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP) "
        "ON CONFLICT(student_id) DO UPDATE SET "
        "  institute_paid = excluded.institute_paid, "
        "  hostel_paid = excluded.hostel_paid, "
        "  mess_paid = excluded.mess_paid, "
        "  other_paid = excluded.other_paid, "
        "  total_paid = excluded.total_paid, "
        "  updated_at = CURRENT_TIMESTAMP";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, upsert_query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare fee summary query\n");
        return 0;
    }

    double total = fee->institute_paid + fee->hostel_paid + fee->mess_paid + fee->other_paid;

    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_text(stmt, 2, roll_no, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, fee->institute_paid);
    sqlite3_bind_double(stmt, 4, fee->hostel_paid);
    sqlite3_bind_double(stmt, 5, fee->mess_paid);
    sqlite3_bind_double(stmt, 6, fee->other_paid);
    sqlite3_bind_double(stmt, 7, total);

    int result = sqlite3_step(stmt) == SQLITE_DONE ? 1 : 0;

    if (!result) {
        fprintf(stderr, "[ERROR] Failed to update fee summary\n");
    }

    sqlite3_finalize(stmt);
    return result;
}


int db_delete_fee_record(const char *roll_no) {
    if (!db || !roll_no) return 0;

    const char *delete_query = "DELETE FROM Fees WHERE roll_no = ?";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, delete_query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare delete: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt) == SQLITE_DONE ? 1 : 0;
    sqlite3_finalize(stmt);

    if (result) {
        printf("[INFO] Fee record deleted for: %s\n", roll_no);
        
        // Also delete from fee summary
        const char *summary_delete = "DELETE FROM FeeSummary WHERE roll_no = ?";
        if (sqlite3_prepare_v2(db, summary_delete, -1, &stmt, NULL) != SQLITE_OK) {
            fprintf(stderr, "[ERROR] Failed to delete fee summary\n");
            return result;
        }
        sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "[ERROR] Failed to delete fee record for: %s\n", roll_no);
    }

    return result;
}


int db_get_student_for_card_by_roll(const char *roll_no, StudentIDCard *out_student) {
    if (!db || !roll_no || !out_student) return 0;

    const char *query = 
        "SELECT student_id, name, roll_no, branch, mobile, email, year, semester, category, father_name, gender "
        "FROM Students WHERE roll_no = ? LIMIT 1";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare student query\n");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        out_student->student_id = sqlite3_column_int(stmt, 0);
        
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        g_strlcpy(out_student->name, name ? name : "", sizeof(out_student->name));
        
        const char *rn = (const char *)sqlite3_column_text(stmt, 2);
        g_strlcpy(out_student->roll_no, rn ? rn : "", sizeof(out_student->roll_no));
        
        const char *branch = (const char *)sqlite3_column_text(stmt, 3);
        g_strlcpy(out_student->branch, branch ? branch : "", sizeof(out_student->branch));
        
        const char *mobile = (const char *)sqlite3_column_text(stmt, 4);
        g_strlcpy(out_student->mobile, mobile ? mobile : "", sizeof(out_student->mobile));
        
        const char *email = (const char *)sqlite3_column_text(stmt, 5);
        g_strlcpy(out_student->email, email ? email : "", sizeof(out_student->email));

        out_student->year = sqlite3_column_int(stmt, 6);
        out_student->semester = sqlite3_column_int(stmt, 7);
        
        const char *category = (const char *)sqlite3_column_text(stmt, 8);
        g_strlcpy(out_student->category, category ? category : "", sizeof(out_student->category));
        
        const char *father_name = (const char *)sqlite3_column_text(stmt, 9);
        g_strlcpy(out_student->father_name, father_name ? father_name : "", sizeof(out_student->father_name));
        
        const char *gender = (const char *)sqlite3_column_text(stmt, 10);
        g_strlcpy(out_student->gender, gender ? gender : "", sizeof(out_student->gender));

        sqlite3_finalize(stmt);
        printf("[INFO] Student found: %s\n", out_student->name);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}


int db_get_student_card_by_id(int student_id, StudentIDCard *out_student) {
    if (!db || !out_student) return 0;

    const char *query = 
        "SELECT student_id, name, roll_no, branch, mobile, email, year, semester, category, father_name, gender "
        "FROM Students WHERE student_id = ? LIMIT 1";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare student query\n");
        return 0;
    }

    sqlite3_bind_int(stmt, 1, student_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        out_student->student_id = sqlite3_column_int(stmt, 0);
        
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        g_strlcpy(out_student->name, name ? name : "", sizeof(out_student->name));
        
        const char *rn = (const char *)sqlite3_column_text(stmt, 2);
        g_strlcpy(out_student->roll_no, rn ? rn : "", sizeof(out_student->roll_no));
        
        const char *branch = (const char *)sqlite3_column_text(stmt, 3);
        g_strlcpy(out_student->branch, branch ? branch : "", sizeof(out_student->branch));
        
        const char *mobile = (const char *)sqlite3_column_text(stmt, 4);
        g_strlcpy(out_student->mobile, mobile ? mobile : "", sizeof(out_student->mobile));
        
        const char *email = (const char *)sqlite3_column_text(stmt, 5);
        g_strlcpy(out_student->email, email ? email : "", sizeof(out_student->email));

        out_student->year = sqlite3_column_int(stmt, 6);
        out_student->semester = sqlite3_column_int(stmt, 7);
        
        const char *category = (const char *)sqlite3_column_text(stmt, 8);
        g_strlcpy(out_student->category, category ? category : "", sizeof(out_student->category));
        
        const char *father_name = (const char *)sqlite3_column_text(stmt, 9);
        g_strlcpy(out_student->father_name, father_name ? father_name : "", sizeof(out_student->father_name));
        
        const char *gender = (const char *)sqlite3_column_text(stmt, 10);
        g_strlcpy(out_student->gender, gender ? gender : "", sizeof(out_student->gender));

        sqlite3_finalize(stmt);
        printf("[INFO] Student card loaded: %s\n", out_student->name);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}


// ============================================================================
// MEMORY MANAGEMENT
// ============================================================================

void db_free_fee_table_rows(FeeTableRow *rows) {
    if (rows) {
        free(rows);
        printf("[INFO] Fee table rows freed\n");
    }
}


int db_create_fee_table(void) {
    if (!db) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return 0;
    }

    const char *fee_tables[] = {
        "CREATE TABLE IF NOT EXISTS FeeSummary ("
        "    summary_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    student_id INTEGER NOT NULL,"
        "    roll_no TEXT NOT NULL,"
        "    institute_paid REAL DEFAULT 0.0,"
        "    hostel_paid REAL DEFAULT 0.0,"
        "    mess_paid REAL DEFAULT 0.0,"
        "    other_paid REAL DEFAULT 0.0,"
        "    total_paid REAL DEFAULT 0.0,"
        "    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY(student_id) REFERENCES Students(student_id)"
        ")",
        NULL
    };

    for (int i = 0; fee_tables[i] != NULL; i++) {
        char *err = NULL;
        int rc = sqlite3_exec(db, fee_tables[i], NULL, NULL, &err);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "[ERROR] Failed to create fee table: %s\n", err);
            sqlite3_free(err);
            return 0;
        }
    }

    printf("[INFO] Fee tables created successfully\n");
    return 1;
}