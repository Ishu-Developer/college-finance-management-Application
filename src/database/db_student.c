#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <ctype.h> 
#include "../../include/database.h"


int db_add_student(const char *name, const char *gender, const char *father_name, 
                   const char *branch, int year, int semester, const char *roll_no, 
                   const char *category, const char *mobile, const char *email) {
    
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return -1;
    }
    if (!name || !gender || !father_name || !branch || !category || !email || !roll_no || !mobile) {
        fprintf(stderr, "[ERROR] NULL text parameter passed to db_add_student\n");
        return -1;
    }
    if (strlen(roll_no) != 13) {
        fprintf(stderr, "[ERROR] Roll number must be exactly 13 digits\n");
        return -1;
    }
    for (int i = 0; roll_no[i]; i++) {
        if (!isdigit(roll_no[i])) {
            fprintf(stderr, "[ERROR] Roll number must contain only digits\n");
            return -1;
        }
    }
    if (strlen(mobile) != 10) {
        fprintf(stderr, "[ERROR] Mobile must be exactly 10 digits\n");
        return -1;
    }
    
    for (int i = 0; mobile[i]; i++) {
        if (!isdigit(mobile[i])) {
            fprintf(stderr, "[ERROR] Mobile must contain only digits\n");
            return -1;
        }
    }
    
    if (year < 1 || year > 4) {
        fprintf(stderr, "[ERROR] Year must be 1-4\n");
        return -1;
    }

    if (semester < 1 || semester > 8) {
        fprintf(stderr, "[ERROR] Semester must be 1-8\n");
        return -1;
    }
    
    printf("[DEBUG] Adding student: %s (Roll: %s)\n", name, roll_no);
    printf("[DEBUG] Branch: %s, Year: %d, Sem: %d, Mobile: %s, Category: %s\n", 
           branch, year, semester, mobile, category);
    
    // Check for duplicate roll number using TEXT binding
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
    
    if (sqlite3_bind_text(check_stmt, 1, roll_no, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind roll_no in check query\n");
        sqlite3_finalize(check_stmt);
        return -1;
    }
    
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
        return -1;
    }
    
    // INSERT STATEMENT (all columns except photo)
    const char *sql =
        "INSERT INTO students (name, gender, father_name, branch, year, semester, "
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
    
    // Parameter binding (10 parameters, indices adjusted after photo removal)
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
    
    if (sqlite3_bind_text(stmt, 7, roll_no, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind roll_no\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 8, category, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to bind category\n");
        sqlite3_finalize(stmt);
        return -1;
    }
    
    if (sqlite3_bind_text(stmt, 9, mobile, -1, SQLITE_STATIC) != SQLITE_OK) {
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
    
    printf("[SUCCESS] Student added with ID: %d, Roll: %s, Name: %s, Mobile: %s\n", 
           student_id, roll_no, name, mobile);
    return student_id;
}


// Keep other functions unchanged...
int db_get_student(int student_id, Student *student) {
    (void)student_id;
    (void)student;
    return -1;
}


sqlite3_stmt* db_get_all_students() {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return NULL;
    }
    
    const char *sql =
        "SELECT student_id, roll_no, name, gender, father_name, branch, "
        "year, semester, category, mobile, email "
        "FROM students ORDER BY student_id DESC;";
    sqlite3_stmt *stmt = NULL;
    
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    return stmt;
}


int db_edit_student(int student_id, const Student *student) {
    if (!db || !student) {
        fprintf(stderr, "[ERROR] Invalid db or student pointer\n");
        return -1;
    }

    const char *sql =
        "UPDATE students SET "
        "name = ?, gender = ?, father_name = ?, branch = ?, "
        "year = ?, semester = ?, roll_no = ?, category = ?, mobile = ?, email = ? "
        "WHERE student_id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Prepare failed: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Parameter binding (10 parameters + 1 for WHERE clause = 11 total)
    sqlite3_bind_text(stmt, 1, student->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, student->gender, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, student->father_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, student->branch, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, student->year);
    sqlite3_bind_int(stmt, 6, student->semester);
    sqlite3_bind_text(stmt, 7, student->roll_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, student->category, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, student->mobile, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, student->email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 11, student_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Update failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}


int db_delete_student(int student_id) {
    if (!db) return -1;

    const char *sql = "DELETE FROM students WHERE student_id = ?;";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_int(stmt, 1, student_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Delete failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}


int db_search_student_by_rollno(const char *roll_no, Student *student) {
    if (!db || !roll_no || !student)
        return -1;

    const char *sql =
        "SELECT student_id, roll_no, name, gender, father_name, branch, "
        "year, semester, category, mobile, email "
        "FROM students WHERE roll_no = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return -1;  // Not found
    }

    // Column indices adjusted after photo removal
    student->student_id = sqlite3_column_int(stmt, 0);
    strcpy(student->roll_no, (const char *)sqlite3_column_text(stmt, 1));
    strcpy(student->name, (const char *)sqlite3_column_text(stmt, 2));
    strcpy(student->gender, (const char *)sqlite3_column_text(stmt, 3));
    strcpy(student->father_name, (const char *)sqlite3_column_text(stmt, 4));
    strcpy(student->branch, (const char *)sqlite3_column_text(stmt, 5));
    student->year = sqlite3_column_int(stmt, 6);
    student->semester = sqlite3_column_int(stmt, 7);
    strcpy(student->category, (const char *)sqlite3_column_text(stmt, 8));
    strcpy(student->mobile, (const char *)sqlite3_column_text(stmt, 9));
    strcpy(student->email, (const char *)sqlite3_column_text(stmt, 10));

    sqlite3_finalize(stmt);
    return 0;
}


sqlite3_stmt* db_get_students_by_branch(const char *branch) {
    if (!db || !branch) return NULL;

    const char *sql =
        "SELECT student_id, roll_no, name, gender, branch, year, semester "
        "FROM students WHERE branch = ? ORDER BY roll_no;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;

    sqlite3_bind_text(stmt, 1, branch, -1, SQLITE_STATIC);

    return stmt;  // Caller must sqlite3_step() + sqlite3_finalize()
}


int db_get_student_count() {
    if (db == NULL) {
        printf("[ERROR] Database not connected\n");
        return 0;
    }
    
    const char *sql = "SELECT COUNT(*) FROM students";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare count statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    printf("[INFO] Total students in database: %d\n", count);
    return count;
}