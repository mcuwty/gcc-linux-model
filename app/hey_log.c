
#include <string.h>
#include <sys/time.h>

#include "hey_log.h"

#define MAX_TAG_LEN  16
#define MAX_TAGS     32

int log_current_level = LOG_DEF_LEVEL;

struct log_level_s {
    char tag[MAX_TAG_LEN + 1];
    int level;
};

struct log_level_s g_module_log_levels[MAX_TAGS];

int sdk_set_log_level(int level)
{
    if((level >= LOG_LEVEL_EMERGENCY) && (level <= LOG_LEVEL_DEBUG)) {
        log_current_level = level;
        return 0;
    } else {
        return -1;
    }
}

int get_current_log_level(char* tag)
{
    int i;
    for(i = 0; i < MAX_TAGS; i++) {
        if(!strcmp(tag, g_module_log_levels[i].tag)) {
            return g_module_log_levels[i].level;
        }
    }

    //if not set level by tag, return default level.
    return log_current_level;
}

int log_printable(char* tag, int level)
{
    int sys_level;

    sys_level = get_current_log_level(tag);

    if(level <= sys_level) {
        return 1;
    } else {
        return 0;
    }
}
