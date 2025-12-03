#include <stdio.h>
#include <time.h>
#include "../../include/logger.h"

void log_info(const char *message) {
    printf("[INFO] %s\n", message);     
}

void log_error(const char *message) {
    fprintf(stderr, "[ERROR] %s\n", message);
}

void log_debug(const char *message) {
    #ifdef DEBUG
    printf("[DEBUG] %s\n", message);
    #else
    (void)message;  // Mark as intentionally unused when DEBUG not defined
    #endif
}