/**
 * @file db_accounts.c - Stub
 */
#include "../include/database.h"

int db_add_account(const char *account_type, const char *ledger_name,
                   double opening_balance, const char *description) {
    return -1;
}

int db_get_account(int account_id, Account *account) {
    return 0;
}

sqlite3_stmt* db_get_all_accounts() {
    return NULL;
}

int db_update_account_balance(int account_id, double new_balance) {
    return 0;
}

int db_update_account_sync_status(int account_id, const char *status) {
    return 0;
}

int db_delete_account(int account_id) {
    return 0;
}