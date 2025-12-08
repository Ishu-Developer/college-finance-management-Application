#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../../include/database.h"

// ✅ CORRECTED FUNCTION SIGNATURE
int db_add_student(const char *name, const char *gender, const char *father_name, 
                   const char *branch, int year, int semester, int roll_no, 
                   const char *category, long long mobile, const char *email) {
    
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return -1;
    }
    
    // ✅ Validation for all parameters
    if (!name || !gender || !father_name || !branch || !category || !email) {
        fprintf(stderr, "[ERROR] NULL text parameter passed to db_add_student\n");
        return -1;
    }
    
    if (roll_no <= 0) {
        fprintf(stderr, "[ERROR] Roll number must be positive\n");
        return -1;
    }
    
    if (mobile <= 0 || mobile > 9999999999LL) {  // 10-digit max: 9,999,999,999
        fprintf(stderr, "[ERROR] Mobile must be 10-digit positive integer\n");
        return -1;
    }
    
    if (year < 1 || year > 4) {
        fprintf(stderr, "[ERROR] Year must be 1-4\n");
        return -1;
    }
    
    printf("[DEBUG] Adding student: %s (Roll: %d)\n", name, roll_no);
    printf("[DEBUG] Branch: %s, Year: %d, Sem: %d, Mobile: %lld, Category: %s\n", 
           branch, year, semester, mobile, category);
    
    // Check for duplicate roll number
    const char *check_sql = "SELECT COUNT(*) FROM students WHERE roll_no = ?;";
    sqlite3_stmt *check_stmt = NULL;
    
    int result = sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL);
    if (result != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare check statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    if (check_stmt == NULL) {
        fprintf(stderr, "[ERROR] Check statement is NULL\n");
        return -1;
    }
    
    sqlite3_bind_int(check_stmt, 1, roll_no);
    
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(check_stmt, 0);
        sqlite3_finalize(check_stmt);
        check_stmt = NULL;
        
        if (count > 0) {
            fprintf(stderr, "[ERROR] Roll number %d already exists\n", roll_no);
            return -1;
        }
    } else {
        fprintf(stderr, "[ERROR] Failed to check existing roll number\n");
        sqlite3_finalize(check_stmt);
        return -1;
    }
    
    // ✅ CORRECTED INSERT STATEMENT (all columns with correct types)
    const char *sql = "INSERT INTO students (name, gender, father_name, branch, year, semester, "
                      "roll_no, category, mobile, email) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt *stmt = NULL;
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (result != SQLITE_OK) {
        fprintf(stderr, "[ERROR] SQL prepare error: %s\n", sqlite3_errmsg(db));
        if (stmt != NULL) sqlite3_finalize(stmt);
        return -1;
    }
    
    if (stmt == NULL) {
        fprintf(stderr, "[ERROR] Statement is NULL after prepare\n");
        return -1;
    }
    
    // ✅ CORRECTED PARAMETER BINDING (text and integers as needed)
    if (sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind name\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 2, gender, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind gender\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 3, father_name, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind father_name\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 4, branch, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind branch\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_int(stmt, 5, year) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind year\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_int(stmt, 6, semester) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind semester\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_int(stmt, 7, roll_no) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind roll_no\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 8, category, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind category\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_int64(stmt, 9, mobile) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind mobile\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 10, email, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind email\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    printf("[DEBUG] Executing INSERT statement\n");
    
    result = sqlite3_step(stmt);
    
    if (result != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to insert student: %s (code: %d)\n", 
                sqlite3_errmsg(db), result);
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_int64 last_id = sqlite3_last_insert_rowid(db);
    int student_id = (int)last_id;
    
    printf("[DEBUG] Last insert ID: %d\n", student_id);
    
    sqlite3_finalize(stmt);
    
    printf("[SUCCESS] Student added with ID: %d, Roll: %d, Name: %s, Mobile: %lld\n", 
           student_id, roll_no, name, mobile);
    return student_id;
}

// Keep other functions unchanged...
int db_get_student(int student_id, Student *student) {
    (void)student_id;
    (void)student;
    return 0;
}

sqlite3_stmt* db_get_all_students() {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return NULL;
    }
    
    const char *sql = "SELECT student_id, roll_no, name, branch, year, semester, mobile FROM students ORDER BY student_id DESC;";
    sqlite3_stmt *stmt = NULL;
    
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    return stmt;
}

// ... other functions remain the same

int db_edit_student(int student_id, const Student *student) {
    (void)student_id;
    (void)student;
    return 0;  // TODO: Implement for Day 4
}

int db_delete_student(int student_id) {
    (void)student_id;
    return 0;  // TODO: Implement for Day 4
}

int db_search_student_by_rollno(int roll_no, Student *student) {
    (void)roll_no;
    (void)student;
    return 0;  // TODO: Implement for Day 3
}

sqlite3_stmt* db_get_students_by_branch(const char *branch) {
    (void)branch;
    return NULL;  // TODO: Implement for Day 3
}

int db_get_student_count() {
    if (db == NULL) {
        return 0;
    }
    
    const char *sql = "SELECT COUNT(*) FROM Students;";
    sqlite3_stmt *stmt = NULL;
    
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK || stmt == NULL) {
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}
