#include "n32l4/inc/hal_flash.h"
#include "n32l40x.h"
#include <string.h>
#include <stdio.h>

#define ERASE_DATA (~0)
#define TURN_DATA(data) (~(data))
/**
 * @brief 初始化flash
 *
 */
void hal_flash_init(void)
{
}

/**
 * @brief 向flash写入数据
 *
 * @param addr 地址
 * @param buf 数据缓存
 * @param len 数据长度
 */
void hal_flash_write(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    /* Check addr */
    if (addr < FLASH_BASE || addr > ((DBG->ID & 0x000F0000) >> 1) + FLASH_BASE || len == 0)
        return;

    /* Unlocks the FLASH Program Erase Controller */
    FLASH_Unlock();

    /* Check */
    for (uint16_t i = 0; i < len; i++)
    {
        if ((uint8_t)ERASE_DATA != (*(__IO uint8_t *)(addr + i)))
        {
            FLASH_EraseOnePage(addr + i);
        }
    }

    /* Program */
    {
        union
        {
            uint32_t word;
            uint8_t byte[sizeof(uint32_t)];
        } data;
        uint16_t pos = 0;

        // /* 未对齐4字节 */
        // if ((pos = (addr & (sizeof(uint32_t) - 1))) != 0)
        // {
        //     FLASH_STS d;

        //     addr -= pos;
        //     data.word = (*(__IO uint32_t *)addr);
        //     // data.word = 0;

        //     // memcpy(data.byte + pos, buf, sizeof(uint32_t) - pos);
        //     memset(data.byte + pos, 0x01, sizeof(uint32_t) - pos);

        //     FLASH_SetSMPSELStatus(FLASH_SMPSEL_SMP2);
        //     d = FLASH_ProgramWord(addr, TURN_DATA(data.word));
        //     if (FLASH_COMPL != d)
        //     {
        //         printf("f:%d %d\n", d, FLASH_GetSMPSELStatus());
        //     }
        //     buf += sizeof(uint32_t) - pos;
        //     addr += sizeof(uint32_t);
        //     len -= sizeof(uint32_t) - pos;
        //     pos = 0;
        // }

        for (; len >= 4; len -= 4, pos += 4)
        {
            memcpy(data.byte, buf + pos, sizeof(uint32_t));
            if (FLASH_COMPL != FLASH_ProgramWord(addr + pos, TURN_DATA(data.word)) /* Program data */
                || (data.word != TURN_DATA((*(__IO uint32_t *)(addr + pos)))))     /* check data */
            {
                len = 0;
                break;
            }
        }

        if (len != 0)
        {
            data.word = TURN_DATA(ERASE_DATA);
            memcpy(data.byte, (buf + pos), len);
            FLASH_ProgramWord(addr + pos, TURN_DATA(data.word));
        }
    }

    /* Locks the FLASH Program Erase Controller */
    FLASH_Lock();
}

/**
 * @brief 向flash读出数据
 *
 * @param addr 地址
 * @param buf 数据缓存
 * @param len 数据长度
 */
void hal_flash_read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    if (addr < FLASH_BASE || addr > ((DBG->ID & 0x000F0000) >> 1) + FLASH_BASE)
        return;

    for (uint16_t i = 0; i < len; i++)
        buf[i] = TURN_DATA((*(__IO uint8_t *)(addr + i)));
}

/**
 * @brief 擦除flash
 *
 * @param addr flash地址
 */
void hal_flash_erase(uint32_t addr)
{
    if (addr < FLASH_BASE || addr > ((DBG->ID & 0x000F0000) >> 1) + FLASH_BASE)
        return;

    /* Unlocks the FLASH Program Erase Controller */
    FLASH_Unlock();

    FLASH_EraseOnePage(addr);

    /* Locks the FLASH Program Erase Controller */
    FLASH_Lock();
}
