#ifndef FEE_H
#define FEE_H

typedef struct {
    int student_id;
    float amount;
    char due_date[20];
} Fee;

void add_fee(Fee f);
void remove_fee(int student_id);
void display_fees(void);

#endif // FEE_H
