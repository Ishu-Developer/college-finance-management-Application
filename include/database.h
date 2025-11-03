#ifndef DATABASE_H
#define DATABASE_H

// Database related declarations

int db_init(void);
int db_create_tables(void);
void db_close(void);

void connect_database(void);
void disconnect_database(void);

#endif // DATABASE_H
