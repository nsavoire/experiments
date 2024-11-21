#pragma once

#include <cstdarg>

enum LOG_LVL : int8_t {
  LL_FORCE_ALERT = -1,
  LL_FORCE_CRITICAL = -2,
  LL_FORCE_ERROR = -3,
  LL_FORCE_WARNING = -4,
  LL_FORCE_NOTICE = -5,
  LL_FORCE_INFORMATIONAL = -6,
  LL_FORCE_DEBUG = -7,
  LL_EMERGENCY = 0, // No force override because always printed
  LL_ALERT = 1,
  LL_CRITICAL = 2,
  LL_ERROR = 3,
  LL_WARNING = 4,
  LL_NOTICE = 5,
  LL_INFORMATIONAL = 6,
  LL_DEBUG = 7,
  LL_LENGTH,
};

inline bool LOG_is_logging_enabled_for_level(LOG_LVL level) {
    return true;
}

inline void olprintfln(int lvl, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

#define LG_IF_LVL_OK(level, ...)                                               \
  do {                                                                         \
    if (LOG_is_logging_enabled_for_level(level)) {                             \
      olprintfln(level, __VA_ARGS__);                                          \
    }                                                                          \
  } while (false)

#define LG_ERR(...) LG_IF_LVL_OK(LL_ERROR, __VA_ARGS__)
#define LG_WRN(...) LG_IF_LVL_OK(LL_WARNING, __VA_ARGS__)
#define LG_NTC(...) LG_IF_LVL_OK(LL_NOTICE, __VA_ARGS__)
#define LG_NFO(...) LG_IF_LVL_OK(LL_INFORMATIONAL, __VA_ARGS__)
#define LG_DBG(...) LG_IF_LVL_OK(LL_DEBUG, __VA_ARGS__)
#define PRINT_NFO(...) LG_IF_LVL_OK(-1 * LL_INFORMATIONAL, __VA_ARGS__)
