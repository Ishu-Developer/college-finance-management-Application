#ifndef LOGGER_H
#define LOGGER_H

/**
 * @brief Log informational message
 * @param message Message to log
 */
void log_info(const char *message);

/**
 * @brief Log error message
 * @param message Error message to log
 */
void log_error(const char *message);

/**
 * @brief Log debug message (only in DEBUG mode)
 * @param message Debug message to log
 */
void log_debug(const char *message);

#endif