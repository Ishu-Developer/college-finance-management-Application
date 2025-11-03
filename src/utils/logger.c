/**
 * @file src/utils/logger.c
 * @brief Logging utility
 */

#include <stdio.h>
#include <time.h>

void log_info(const char *message) {
    printf("[INFO] %s\n", message);
}

void log_error(const char *message) {
    fprintf(stderr, "[ERROR] %s\n", message);
}

void log_debug(const char *message) {
    #ifdef DEBUG
    printf("[DEBUG] %s\n", message);
    #endif
}