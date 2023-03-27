#include "debug.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include "bsp_config.h"
#include "common.h"

#if !(defined DISABLE_LOG)
#define LOG_BUF_SIZE 128
#define debug_output(log, size) while(!hal_uart_write(0, (uint8_t *)log_buf, log_len))
#define debug_save(log, size) log_flash(log, size)

static Shell shell;
static uint32_t index_address;

static signed short shell_write(char *data, unsigned short len)
{
    while(!hal_uart_write(UART_DEBUG, (uint8_t *)data, len));
    return len;
}

static signed short shell_read(char *data, unsigned short len)
{
    return hal_uart_receive(UART_DEBUG, (uint8_t *)data, len);
}

static void log_flash(void *buf, uint16_t len)
{
    len = (index_address + len) < (LOG_FLASH_LENGHT + LOG_FLASH_START_ADDRESS)
              ? len
              : LOG_FLASH_LENGHT + LOG_FLASH_START_ADDRESS - index_address;

    hal_flash_write(index_address, (uint8_t *)buf, len);

    index_address += len;
    if ((index_address & (sizeof(uint32_t) - 1)))
        index_address += sizeof(uint32_t) - (index_address & (sizeof(uint32_t) - 1));
}

/**
 * @brief 日志打印
 *
 * @param level 日志等级
 * @param file 文件路径
 * @param line 行号
 * @param storage 1：储藏 0：不储藏
 */
void debug_printf(char level, char *file, size_t line, unsigned char storage, const char *format, ...)
{
    static char log_buf[LOG_BUF_SIZE];
    va_list args;
    size_t log_len = 0;
    char level_string[] = {'E', 'W', 'I', 'D'};

    if (level <= DEBUG_LOG_LEVEL)
        log_len = sprintf(log_buf, "[%c]%s(%d):", level_string[level], file, line);

    va_start(args, format);
    log_len += vsnprintf(log_buf + log_len, LOG_BUF_SIZE - log_len, format, args);
    va_end(args);

    log_buf[log_len++] = '\n';

    debug_output(log_buf, log_len);
    if (storage == 1)
        debug_save(log_buf, log_len);
}

void log_init(void)
{
    uint8_t byte;

    index_address = LOG_FLASH_START_ADDRESS;

    for (uint32_t i = 0; i < LOG_FLASH_LENGHT; i++)
    {
        hal_flash_read(index_address, &byte, 1);
        if (byte == 0 && !(index_address & (sizeof(uint32_t) - 1)))
            break;
        index_address++;
    }
}

/**
 * @brief 初始化shell
 *
 */
void shell_init(void)
{
    static char buff[256];

    shell.write = shell_write;
    shell.read = shell_read;
    shellInit(&shell, buff, sizeof(buff));

    log_init();
}
REG_APP_INIT(shell_init);

/**
 * @brief 处理shell数据
 *
 */
void shell_process(void)
{
    shellTask(&shell);
}
REG_APP_TASK(shell_process);
REG_APP_TASK(shell_init);
/**
 * @brief flash命令解析
 */
static int shell_flash(int argc, char *argv[])
{
    if (argc >= 3)
    {
        uint8_t byte;
        uint8_t hex = 0;
        uint32_t addr = shellExtParsePara(&shell, argv[1]);
        uint32_t lenght = shellExtParsePara(&shell, argv[2]);

        if (addr < 0x08000000 || addr > 0x08000000 + 128 * 1024)
        {
            printf("out of 'addr'");
            return -1;
        }

        if (argc >= 4 && !strcmp("-x", argv[3]))
            hex = 1;

        for (uint32_t i = 0; i < lenght; i++)
        {
            hal_flash_read(addr + i, &byte, 1);

            if (hex == 1)
            {
                if (!(i & 15))
                    printf("\n");
                printf("%02x ", byte);
            }
            else
            {
                if (byte != 0)
                {
                    printf("%c", byte);
                }
                else if (!((addr + i) & (sizeof(uint32_t) - 1)))
                {
                    printf("flash ok:%d\n", i);
                    return 0;
                }
            }
        }

        return 0;
    }
    else
    {
        printf("cmd fail");

        return -1;
    }
}

static int shell_board_debug(int argc, char *argv[])
{
    if (argc >= 2)
    {
        uint8_t state = shellExtParsePara(&shell, argv[1]);

        if (state == 0)
        {
            board_debug_deinit();
            printf("debug off\n");
        }
        else
        {
            board_debug_init();
            printf("debug on\n");
        }
        return 0;
    }
    else
    {
        printf("cmd fail");

        return -1;
    }
}

/* shell cmd */
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), flash, shell_flash, "usage: flash <addrs> <lenght> [-x]");
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), debug, shell_board_debug, "usage: debug 0/debug 1");
#endif
