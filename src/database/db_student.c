#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../../include/database.h"

int db_add_student(const char *roll_no, const char *name, const char *branch,
                   int semester, const char *mobile, const char *email) {
    
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return -1;
    }
    
    if (!roll_no || !name || !branch || !mobile || !email) {
        fprintf(stderr, "[ERROR] NULL parameter passed to db_add_student\n");
        return -1;
    }
    
    // ✅ FIXED: %d → %s for roll_no (it's a string, not int)
    printf("[DEBUG] Adding student: %s (%s)\n", name, roll_no);
    
    // ✅ FIXED: %d → %s for mobile (it's a string, not int)
    printf("[DEBUG] Branch: %s, Semester: %d, Mobile: %s, Email: %s\n", 
           branch, semester, mobile, email);
    
    // Check if roll number already exists
    const char *check_sql = "SELECT COUNT(*) FROM Students WHERE roll_no = ?;";
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
    
    sqlite3_bind_text(check_stmt, 1, roll_no, -1, SQLITE_STATIC);
    
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(check_stmt, 0);
        sqlite3_finalize(check_stmt);
        check_stmt = NULL;
        
        if (count > 0) {
            fprintf(stderr, "[ERROR] Roll number %s already exists\n", roll_no);
            return -1;
        }
    } else {
        fprintf(stderr, "[ERROR] Failed to check existing roll number\n");
        sqlite3_finalize(check_stmt);
        check_stmt = NULL;
        return -1;
    }
    
    if (check_stmt != NULL) {
        sqlite3_finalize(check_stmt);
        check_stmt = NULL;
    }
    
    // Insert new student
    const char *sql = "INSERT INTO Students (roll_no, name, branch, semester, mobile, email, status) "
                      "VALUES (?, ?, ?, ?, ?, ?, 'Active');";
    
    sqlite3_stmt *stmt = NULL;
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (result != SQLITE_OK) {
        fprintf(stderr, "[ERROR] SQL prepare error: %s\n", sqlite3_errmsg(db));
        if (stmt != NULL) {
            sqlite3_finalize(stmt);
        }
        return -1;
    }
    
    if (stmt == NULL) {
        fprintf(stderr, "[ERROR] Statement is NULL after prepare\n");
        return -1;
    }
    
    // Bind parameters with proper error checking
    if (sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind roll_no: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 3, branch, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind branch: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_int(stmt, 4, semester) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind semester: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 5, mobile, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind mobile: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 6, email, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind email: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    printf("[DEBUG] Executing INSERT statement\n");
    
    // Execute
    result = sqlite3_step(stmt);
    
    if (result != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to insert student: %s (code: %d)\n", 
                sqlite3_errmsg(db), result);
        sqlite3_finalize(stmt);
        return -1;
    }
    
    // Get the student ID
    sqlite3_int64 last_id = sqlite3_last_insert_rowid(db);
    int student_id = (int)last_id;
    
    printf("[DEBUG] Last insert ID: %d\n", student_id);
    
    sqlite3_finalize(stmt);
    stmt = NULL;
    
    printf("[SUCCESS] Student added with ID: %d, Roll: %s, Name: %s\n", 
           student_id, roll_no, name);
    return student_id;
}

int db_get_student(int student_id, Student *student) {
    (void)student_id;
    (void)student;
    return 0;  // TODO: Implement for Day 3
}

sqlite3_stmt* db_get_all_students() {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized in db_get_all_students\n");
        return NULL;
    }
    
    const char *sql = "SELECT student_id, roll_no, name, branch, semester FROM Students ORDER BY student_id DESC;";
    sqlite3_stmt *stmt = NULL;
    
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    if (stmt == NULL) {
        fprintf(stderr, "[ERROR] Statement is NULL in db_get_all_students\n");
        return NULL;
    }
    
    return stmt;
}

int db_update_student(int student_id, const Student *student) {
    (void)student_id;
    (void)student;
    return 0;  // TODO: Implement for Day 4
}

int db_delete_student(int student_id) {
    (void)student_id;
    return 0;  // TODO: Implement for Day 4
}

int db_search_student_by_rollno(const char *roll_no, Student *student) {
    (void)roll_no;
    (void)student;
    return 0;  // TODO: Implement for Day 3
}

sqlite3_stmt* db_get_students_by_branch(const char *branch) {
    (void)branch;
    return NULL;  // TODO: Implement for Day 3
}

/**
 * @brief Get total number of students
 */
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
