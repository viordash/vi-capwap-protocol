#pragma once

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

const char *log_time();

#define LOG_LEVEL_NONE 4
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_DEBUG 0

typedef enum {
    LOG_DEBUG = LOG_LEVEL_DEBUG,
    LOG_INFO = LOG_LEVEL_INFO,
    LOG_WARN = LOG_LEVEL_WARN,
    LOG_ERROR = LOG_LEVEL_ERROR,
    LOG_NONE = LOG_LEVEL_NONE,
} log_level_t;

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif // !LOG_LEVEL

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define log_e(format, ...) fprintf(stderr, "E (%s) " format "\n", log_time(), ##__VA_ARGS__)
#else
#define log_e(format, ...)                                                                         \
    do {                                                                                           \
        (void)(format);                                                                            \
        _Pragma("GCC diagnostic push")                                                             \
            _Pragma("GCC diagnostic ignored \"-Wunused-value\"")(void)(0, ##__VA_ARGS__);          \
        _Pragma("GCC diagnostic pop")                                                              \
    } while (0)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
#define log_w(format, ...) fprintf(stderr, "W (%s) " format "\n", log_time(), ##__VA_ARGS__)
#else
#define log_w(format, ...)                                                                         \
    do {                                                                                           \
        (void)(format);                                                                            \
        _Pragma("GCC diagnostic push")                                                             \
            _Pragma("GCC diagnostic ignored \"-Wunused-value\"")(void)(0, ##__VA_ARGS__);          \
        _Pragma("GCC diagnostic pop")                                                              \
    } while (0)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define log_i(format, ...) fprintf(stdout, "I (%s) " format "\n", log_time(), ##__VA_ARGS__)
#else
#define log_i(format, ...)                                                                         \
    do {                                                                                           \
        (void)(format);                                                                            \
        _Pragma("GCC diagnostic push")                                                             \
            _Pragma("GCC diagnostic ignored \"-Wunused-value\"")(void)(0, ##__VA_ARGS__);          \
        _Pragma("GCC diagnostic pop")                                                              \
    } while (0)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define log_d(format, ...) fprintf(stdout, "D (%s) " format "\n", log_time(), ##__VA_ARGS__)
#else
#define log_d(format, ...)                                                                         \
    do {                                                                                           \
        (void)(format);                                                                            \
        _Pragma("GCC diagnostic push")                                                             \
            _Pragma("GCC diagnostic ignored \"-Wunused-value\"")(void)(0, ##__VA_ARGS__);          \
        _Pragma("GCC diagnostic pop")                                                              \
    } while (0)
#endif

static inline void log_level_set(log_level_t level) {
    (void)level;
}