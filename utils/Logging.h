#pragma once

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
#endif

static inline void log_level_set(log_level_t level) {
    (void)level;
}

#ifndef log_i

#if __has_include(<spdlog/spdlog.h>)

#if (LOG_LEVEL == LOG_LEVEL_NONE)
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif
#elif (LOG_LEVEL == LOG_LEVEL_ERROR)
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
#endif
#elif (LOG_LEVEL == LOG_LEVEL_WARN)
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#endif
#elif (LOG_LEVEL == LOG_LEVEL_INFO)
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#else
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif
#endif

#include <spdlog/spdlog.h>

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define log_e(fmt_, ...) SPDLOG_ERROR(fmt_, ##__VA_ARGS__)
#else
#define log_e(fmt_, ...) (void)0
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
#define log_w(fmt_, ...) SPDLOG_WARN(fmt_, ##__VA_ARGS__)
#else
#define log_w(fmt_, ...) (void)0
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define log_i(fmt_, ...) SPDLOG_INFO(fmt_, ##__VA_ARGS__)
#else
#define log_i(fmt_, ...) (void)0
#endif

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define log_d(fmt_, ...) SPDLOG_DEBUG(fmt_, ##__VA_ARGS__)
#else
#define log_d(fmt_, ...) (void)0
#endif

#elif __has_include(<fmt/format.h>)

#include <fmt/format.h>

const char *log_time();

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define log_e(fmt_, ...) fmt::print(stderr, "E ({}) " fmt_ "\n", log_time(), ##__VA_ARGS__)
#else
#define log_e(fmt_, ...) (void)0
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
#define log_w(fmt_, ...) fmt::print(stderr, "W ({}) " fmt_ "\n", log_time(), ##__VA_ARGS__)
#else
#define log_w(fmt_, ...) (void)0
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define log_i(fmt_, ...) fmt::print(stdout, "I ({}) " fmt_ "\n", log_time(), ##__VA_ARGS__)
#else
#define log_i(fmt_, ...) (void)0
#endif

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define log_d(fmt_, ...) fmt::print(stdout, "D ({}) " fmt_ "\n", log_time(), ##__VA_ARGS__)
#else
#define log_d(fmt_, ...) (void)0
#endif

#else
#error "capwap-protocol logging requires either spdlog or libfmt (apt install libfmt-dev)"
#endif

#endif // !log_i
