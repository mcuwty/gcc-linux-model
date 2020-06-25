#ifndef HEY_LOG_H
#define HEY_LOG_H

#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PRINT(fmt,args...) printf(fmt,##args)

enum LOG_LEVEL_ENUM {
    LOG_LEVEL_EMERGENCY = 0,
    LOG_LEVEL_ALERT     = 1,
    LOG_LEVEL_CRITICAL  = 2,
    LOG_LEVEL_ERR       = 3,
    LOG_LEVEL_WARN      = 4,
    LOG_LEVEL_NOTICE    = 5,
    LOG_LEVEL_INFO      = 6,
    LOG_LEVEL_DEBUG     = 7
};

//default log level can be changed in Makefile
#ifndef LOG_DEF_LEVEL
#define LOG_DEF_LEVEL LOG_LEVEL_INFO
#endif

#define HEY_CONFIG_LOG_COLORS

#ifdef HEY_CONFIG_LOG_COLORS
#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V
#else //HEY_CONFIG_LOG_COLORS
#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_RESET_COLOR
#endif //HEY_CONFIG_LOG_COLORS

#ifndef TAG
#define TAG "hey"
#endif

int log_printable(char* tag, int level);

#define LEVEL_STR(level)  (level==LOG_LEVEL_DEBUG) ? "D" : (  \
                        (level == LOG_LEVEL_INFO) ? "I" :     \
                        ((level == LOG_LEVEL_WARN) ? "W" : "E") )

#define HEY_LOG(TAG, level, color, fmt, args...)              \
    do {                                                      \
      struct timespec _time_spec_;                            \
      clock_gettime(CLOCK_REALTIME, &_time_spec_);            \
      unsigned int _log_ts_ = _time_spec_.tv_sec;             \
      if( log_printable(TAG, level) ) {                       \
          PRINT("%s",color);                                  \
          PRINT("%s ", LEVEL_STR(level));                     \
          PRINT("(%u)", _log_ts_);                            \
          PRINT(" %s: ", TAG);                                \
          PRINT(fmt, ##args);                                 \
          PRINT("%s",LOG_RESET_COLOR);                        \
      }                                                       \
    } while (0)

#define HEY_LOG_BYTES(bytes, length)                          \
  do {                                                        \
        PRINT("DEBUG: <%s:%d>: \n", __func__, __LINE__);      \
        if( (char *)bytes == (char *)NULL ) { break; }        \
        uint16_t i;                                           \
        for (i = 0; i < length; i++)                          \
        {                                                     \
          PRINT(" %02X", (unsigned char)bytes[i]);            \
       if(0 == (i+1)%32) PRINT("\n");                         \
        }                                                     \
      PRINT("\n");                                            \
  } while (0)

#define HEY_DBG(fmt, arg...)   HEY_LOG(TAG, LOG_LEVEL_DEBUG, LOG_RESET_COLOR, fmt,##arg)
#define HEY_INF(fmt, arg...)   HEY_LOG(TAG, LOG_LEVEL_INFO,  LOG_COLOR_I,  fmt,##arg)
#define HEY_WRN(fmt, arg...)   HEY_LOG(TAG, LOG_LEVEL_WARN,  LOG_COLOR_W,  fmt,##arg)
#define HEY_ERR(fmt, arg...)   HEY_LOG(TAG, LOG_LEVEL_ERR,   LOG_COLOR_E,  fmt,##arg)

#ifdef __cplusplus
}
#endif

#endif /* HEY_LOG_H */
