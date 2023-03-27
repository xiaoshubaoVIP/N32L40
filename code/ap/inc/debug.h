#ifndef __DEBUG_H__
#define __DEBUG_H__

/* 日志打印开关 */
//#define DISABLE_LOG 

/* 
    日志等级
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#if !(defined LOG_LEVEL)
    #define LOG_LEVEL INFO_LOG_LEVEL
#endif

#define ERR_LOG_LEVEL 0
#define WARN_LOG_LEVEL 1
#define INFO_LOG_LEVEL 2
#define DEBUG_LOG_LEVEL 3

#if defined (DISABLE_LOG)
    #define debug(fmt, ...)

    #define debug_printbuff(buff, len, fmt, ...)

    #define debug_info(fmt, ...)

    #define debug_warning(fmt, ...)

    #define debug_error(fmt, ...)

    #define printbuff(buff, len, fmt, arg...)

    #define debug_noinfo(fmt, arg...)

    #define log_noinfo(fmt, arg...)

    #include <stdio.h>
#else
    #include <stdio.h>

    #define LOG_FLASH_START_ADDRESS 0x08000000 + 1024 * 70
    #define LOG_FLASH_LENGHT 1024 * 30


    void debug_printf(char level, char *file, size_t line, unsigned char storage, const char *format, ...);

    #ifndef LOG_LEVEL
        #define LOG_LEVEL INFO_LOG_LEVEL
    #endif

    #if LOG_LEVEL < INFO_LOG_LEVEL
        #define debug_info(fmt, arg...)
        #define log_info(fmt, arg...)
    #else
        #define debug_info(fmt, arg...) debug_printf(INFO_LOG_LEVEL, FILE_NAME, __LINE__, 0, fmt, ##arg)
        #define log_info(fmt, arg...) debug_printf(INFO_LOG_LEVEL, FILE_NAME, __LINE__, 1, fmt, ##arg)
    #endif

    #if LOG_LEVEL < WARN_LOG_LEVEL
        #define debug_warning(fmt, arg...)
        #define log_warning(fmt, arg...)
    #else
        #define debug_warning(fmt, arg...) debug_printf(WARN_LOG_LEVEL, FILE_NAME, __LINE__, 0, fmt, ##arg)
        #define log_warning(fmt, arg...) debug_printf(WARN_LOG_LEVEL, FILE_NAME, __LINE__, 1, fmt, ##arg)
    #endif

    #define debug_error(fmt, arg...) debug_printf(ERR_LOG_LEVEL, FILE_NAME, __LINE__, 0, fmt, ##arg)
    #define log_error(fmt, arg...) debug_printf(ERR_LOG_LEVEL, FILE_NAME, __LINE__, 1, fmt, ##arg)

    #define debug_noinfo(fmt, arg...) debug_printf(DEBUG_LOG_LEVEL + 1, NULL, 0, 0, fmt, ##arg)
    #define log_noinfo(fmt, arg...) debug_printf(DEBUG_LOG_LEVEL + 1, NULL, 0, 1, fmt, ##arg)

    #if LOG_LEVEL < DEBUG_LOG_LEVEL
        #define debug(fmt, arg...)
        #define debug_printbuff(buff, len, fmt, arg...)
    #else
        #define debug(fmt, arg...) debug_printf(DEBUG_LOG_LEVEL + 1, NULL, 0, 0, fmt, ##arg)
        #define debug_printbuff(buff, len, fmt, arg...) \
        do{ \
            unsigned short i; \
            debug(fmt, ##arg); \
            for (i = 0; i < len; i++)\
                printf("%02x ", buff[i]); \
            printf("\n\r"); \
        }while(0)
    #endif

    #define printbuff(buff, len, fmt, arg...) \
        do{ \
            unsigned short i; \
            printf(fmt, ##arg); \
            for (i = 0; i < len; i++)\
                printf("%02x ", buff[i]); \
            printf("\n\r"); \
        }while(0)

    void shell_process(void);
    void shell_init(void);

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
#define FILE_NAME __MODULE__
#elif defined(__ICCARM__) || defined(__ICCRX__)
#define FILE_NAME __FILE__
#elif defined(__GNUC__)
#define FILE_NAME __FILE__
#else
#define FILE_NAME 
#endif
#endif

#undef LOG_LEVEL

#endif
