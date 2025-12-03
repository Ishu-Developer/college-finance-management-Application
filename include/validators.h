#ifndef VALIDATORS_H
#define VALIDATORS_H

int validate_roll_no(const char *roll_no);
int validate_name(const char *name);

int validate_email(const char *email);

int validate_mobile(const char *mobile);

int validate_amount(double amount);

int validate_date(const char *date);

#endif