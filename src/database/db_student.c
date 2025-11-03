/**
 * @file db_student.c
 * @brief Student CRUD operations implementation
 */

 
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/database.h"

/**
 * @brief Add new student to database
 */
int db_add_student(const char *roll_no, const char *name, const char *branch,
                   int semester, const char *mobile, const char *email) {
    if (db == NULL) {
        fprintf(stderr, "[ERROR] Database not initialized\n");
        return -1;
    }

    const char *sql = "INSERT INTO students (roll_no, name, branch, semester, mobile, email) "
                      "VALUES (?, ?, ?, ?, ?, ?)";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, branch, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, semester);
    sqlite3_bind_text(stmt, 5, mobile, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, email, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] Failed to insert student: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    printf("[INFO] Student added: %s (%s)\n", name, roll_no);
    return (int)sqlite3_last_insert_rowid(db);
}

/**
 * @brief Retrieve student by ID
 */
int db_get_student(int student_id, Student *student) {
    if (db == NULL || student == NULL) return 0;

    const char *sql = "SELECT * FROM students WHERE student_id = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_int(stmt, 1, student_id);

    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student->student_id = sqlite3_column_int(stmt, 0);
        strncpy(student->roll_no, (const char*)sqlite3_column_text(stmt, 1), sizeof(student->roll_no)-1);
        strncpy(student->name, (const char*)sqlite3_column_text(stmt, 2), sizeof(student->name)-1);
        strncpy(student->branch, (const char*)sqlite3_column_text(stmt, 3), sizeof(student->branch)-1);
        student->semester = sqlite3_column_int(stmt, 4);
        strncpy(student->mobile, (const char*)sqlite3_column_text(stmt, 5), sizeof(student->mobile)-1);
        strncpy(student->email, (const char*)sqlite3_column_text(stmt, 6), sizeof(student->email)-1);
        found = 1;
    }

    sqlite3_finalize(stmt);
    return found;
}

/**
 * @brief Get all students
 */
sqlite3_stmt* db_get_all_students() {
    if (db == NULL) return NULL;

    const char *sql = "SELECT * FROM students ORDER BY roll_no";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR] Query failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    return stmt;
}

/**
 * @brief Get students by branch
 */
sqlite3_stmt* db_get_students_by_branch(const char *branch) {
    if (db == NULL) return NULL;

    const char *sql = "SELECT * FROM students WHERE branch = ? ORDER BY roll_no";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return NULL;
    }

    sqlite3_bind_text(stmt, 1, branch, -1, SQLITE_STATIC);
    return stmt;
}

/**
 * @brief Search student by roll number
 */
int db_search_student_by_rollno(const char *roll_no, Student *student) {
    if (db == NULL || student == NULL) return 0;

    const char *sql = "SELECT * FROM students WHERE roll_no = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_text(stmt, 1, roll_no, -1, SQLITE_STATIC);

    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student->student_id = sqlite3_column_int(stmt, 0);
        strncpy(student->roll_no, (const char*)sqlite3_column_text(stmt, 1), sizeof(student->roll_no)-1);
        strncpy(student->name, (const char*)sqlite3_column_text(stmt, 2), sizeof(student->name)-1);
        strncpy(student->branch, (const char*)sqlite3_column_text(stmt, 3), sizeof(student->branch)-1);
        student->semester = sqlite3_column_int(stmt, 4);
        strncpy(student->mobile, (const char*)sqlite3_column_text(stmt, 5), sizeof(student->mobile)-1);
        strncpy(student->email, (const char*)sqlite3_column_text(stmt, 6), sizeof(student->email)-1);
        found = 1;
    }

    sqlite3_finalize(stmt);
    return found;
}

/**
 * @brief Update student information
 */
int db_update_student(int student_id, const Student *student) {
    if (db == NULL || student == NULL) return 0;

    const char *sql = "UPDATE students SET roll_no=?, name=?, branch=?, semester=?, mobile=?, email=? "
                      "WHERE student_id=?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_text(stmt, 1, student->roll_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, student->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, student->branch, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, student->semester);
    sqlite3_bind_text(stmt, 5, student->mobile, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, student->email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, student_id);

    int success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (success) {
        printf("[INFO] Student updated: ID %d\n", student_id);
    }

    return success;
}

/**
 * @brief Delete student record
 */
int db_delete_student(int student_id) {
    if (db == NULL) return 0;

    const char *sql = "DELETE FROM students WHERE student_id = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_int(stmt, 1, student_id);

    int success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (success) {
        printf("[INFO] Student deleted: ID %d\n", student_id);
    }

    return success;
}

/**
 * @brief Get total number of students
 */
int db_get_student_count() {
    if (db == NULL) return -1;

    const char *sql = "SELECT COUNT(*) FROM students";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}