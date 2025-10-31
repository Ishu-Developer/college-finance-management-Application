#ifndef EMPLOYEE_H
#define EMPLOYEE_H

typedef struct {
    int id;
    char name[50];
    char position[50];
    float salary;
} Employee;

void add_employee(Employee e);
void remove_employee(int id);
void display_employees(void);

#endif // EMPLOYEE_H
