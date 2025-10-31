#ifndef STUDENT_H
#define STUDENT_H

typedef struct {
    int id;
    char name[50];
    int age;
    char course[50];
} Student;

void add_student(Student s);
void remove_student(int id);
void display_students(void);

#endif // STUDENT_H
