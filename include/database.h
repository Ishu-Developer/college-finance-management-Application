#ifndef DATABASE_H
#define DATABASE_H

// Database related declarations

int db_init(void);                    // Returns int (0 for success, -1 for error)
int db_create_tables(void);           // Returns int
void db_close(void);                  // Returns void

void connect_database(void);
void disconnect_database(void);

#endif // DATABASE_H
