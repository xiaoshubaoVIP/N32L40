#include "n32l4/inc/hal_uart.h"
#include "n32l40x.h"
#include <string.h>
#include <stdio.h>
#include "system.h"

#pragma diag_suppress 1296

#define min(x, y) ((x) < (y) ? (x) : (y))
// #define queue_len
#define ENABLE_UART
#define CTRL1_UEN_SET ((uint16_t)0x2000)   /*!< USART Enable Mask */
#define CTRL1_UEN_RESET ((uint16_t)0xDFFF) /*!< USART Disable Mask */

/* 接收数据buff */
static uint8_t rx_buff[UART_RX_BUF_LEN];
static uint16_t rx_in;
static uint16_t rx_out;

/* 发送数据buff */
static uint8_t tx_buff[UART_TX_BUF_LEN];
static uint16_t tx_in;
static uint16_t tx_out;
static uint16_t tx_len;



static uint8_t is_dma_init;

static void dma_init(void)
{
    DMA_InitType DMA_InitStructure;

    if (is_dma_init == true)
        return;

    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA, ENABLE);

    /* DMA RX */
    rx_out = rx_in = 0;
    DMA_DeInit(DMA_CH5);
    DMA_InitStructure.PeriphAddr = (uint32_t)(&USART2->DAT);
    DMA_InitStructure.MemAddr = (uint32_t)rx_buff;
    DMA_InitStructure.Direction = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize = UART_RX_BUF_LEN;
    DMA_InitStructure.PeriphInc = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
    DMA_InitStructure.MemDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.CircularMode = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority = DMA_PRIORITY_MEDIUM;
    DMA_InitStructure.Mem2Mem = DMA_M2M_DISABLE;
    DMA_CH5->CHSEL = DMA_REMAP_USART2_RX; // 映射到DMA5
    DMA_Init(DMA_CH5, &DMA_InitStructure);
    DMA_EnableChannel(DMA_CH5, ENABLE);

    /* DMA TX */
    tx_in = tx_out = 0;
    DMA_DeInit(DMA_CH6);
    DMA_InitStructure.MemAddr = (uint32_t)tx_buff;
    DMA_InitStructure.Direction = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.CircularMode = DMA_MODE_NORMAL;
    DMA_CH6->CHSEL = DMA_REMAP_USART2_TX;
    DMA_Init(DMA_CH6, &DMA_InitStructure);
    DMA_ConfigInt(DMA_CH6, DMA_INT_TXC, ENABLE);
    /* 关闭DMA通道 */
    DMA_CH6->CHCFG &= (uint16_t)(~DMA_CHCFG1_CHEN);

    is_dma_init = true;
}

void hal_uart_init(uart_id_t dev, uint32_t bound)
{
#if defined ENABLE_UART
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /*Init GPIO init struct*/
    GPIO_InitStruct(&GPIO_InitStructure);

    // Uart deinit
    RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_USART2, ENABLE);
    RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_USART2, DISABLE);

    // Turn on the clock of usart peripheral
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, ENABLE);

    GPIO_InitStructure.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART2;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);

    /* Configure USART as interrupt source */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    /*Set the priority*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /*Set the sub priority */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /*Enable interrupt */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* Initialize configuration NVIC */
    NVIC_Init(&NVIC_InitStructure);

    /* Configure DMA as interrupt source */
    NVIC_InitStructure.NVIC_IRQChannel = DMA_Channel6_IRQn;
    /*Set the priority*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /*Set the sub priority */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /*Enable interrupt */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* Initialize configuration NVIC */
    NVIC_Init(&NVIC_InitStructure);

    // Configure parameters of usart port
    // Configure baud rate
    USART_InitStructure.BaudRate = bound;
    // Configure the length of frame data
    USART_InitStructure.WordLength = USART_WL_8B;
    // Configure stop bits
    USART_InitStructure.StopBits = USART_STPB_1;
    // Configure check bit
    USART_InitStructure.Parity = USART_PE_NO;
    // Configure hardware flow control
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    // Configure working mode, send and receive together
    USART_InitStructure.Mode = USART_MODE_RX | USART_MODE_TX;
    // Complete initialization configuration of usart port
    USART_Init(USART2, &USART_InitStructure);

    // Enable usart
    USART_EnableDMA(USART2, USART_DMAREQ_RX | USART_DMAREQ_TX, ENABLE);

    // Enable usart port receive interrupt
    USART_ConfigInt(USART2, USART_INT_IDLEF, ENABLE);

    USART_Enable(USART2, ENABLE);
    is_dma_init = false;
#endif
}

/**
 * @brief 串口发送数据
 *
 * @param dev 目标串口
 * @param buff 要发送的数据
 * @param len 要发送的长度
 * @return int 失败返回0，成功返回1
 */
uint8_t hal_uart_write(uart_id_t dev, uint8_t buff[], uint16_t len)
{
    if (((sizeof(tx_buff) - ((tx_in - tx_out) & (sizeof(tx_buff) - 1))) <= len))
        return 0;
    
    if (len == 0)
        return 1;

    dma_init();

    if (len > 1)
    {
        uint16_t l;

        l = sizeof(tx_buff) - ((tx_in - tx_out) & (sizeof(tx_buff) - 1));
        len = min(len, l);
        /* first put the data starting from fifo->in to buffer end */
        l = min(len, sizeof(tx_buff) - tx_in);
        memcpy(tx_buff + tx_in, buff, l);

        /* then put the rest (if any) at the beginning of the buffer */
        memcpy(tx_buff, buff + l, len - l);

        tx_in = (tx_in + len) & (sizeof(tx_buff) - 1);
    }
    else
    {
        tx_buff[tx_in] = *buff;
        tx_in = (tx_in + 1) & (sizeof(tx_buff) - 1);
    }

    if (!SystemGetActiveFlag(eSystemEventUartTx) && !(DMA_CH6->CHCFG & DMA_CHCFG1_CHEN))
    {
        SystemSetActiveFlag(eSystemEventUartTx);
        tx_len = (tx_in > tx_out) ? (tx_in - tx_out) : (sizeof(tx_buff) - tx_out);
        DMA_CH6->TXNUM = tx_len;
        DMA_CH6->MADDR = (uint32_t)tx_buff + tx_out;
        DMA_CH6->CHCFG |= DMA_CHCFG1_CHEN;
    }

    return 1;
}

/**
 * @brief 读取串口数据
 *
 * @param dev 设备号
 * @param buff 要接收的数据地址
 * @param len 要接收的数据长度
 * @return unsigned short 实际接收的数据长度
 */
uint16_t hal_uart_receive(uart_id_t dev, uint8_t buff[], uint16_t len)
{
    if (rx_in == rx_out || len == 0)
        return 0;

    if (len > 1)
    {
        uint16_t l;

        l = (rx_in - rx_out) & (sizeof(rx_buff) - 1);
        len = min(len, l);

        // first get the data from fifo->out until the end of the buffer
        l = min(len, sizeof(rx_buff) - rx_out);
        memcpy(buff, rx_buff + rx_out, l);

        // then get the rest (if any) from the beginning of the buffer
        memcpy(buff + l, rx_buff, len - l);

        rx_out = (rx_out + len) & (sizeof(rx_buff) - 1);
    }
    else
    {
        buff[0] = rx_buff[rx_out];
        rx_out = (rx_out + 1) & (sizeof(rx_buff) - 1);
    }

    return len;
}

void hal_uart_enable(void)
{

}

void hal_uart_disable(void)
{
    is_dma_init = false;
}

/**
 * @brief This function handles USART2 interrupt request.
 */
void USART2_IRQHandler(void)
{
    if (USART_GetIntStatus(USART2, USART_INT_IDLEF) != RESET)
    {
        rx_in = (sizeof(rx_buff) - DMA_CH5->TXNUM) & (sizeof(rx_buff) - 1);
        /* 清中断 */
        USART2->STS;
        USART2->DAT;
    }

    if (USART_GetIntStatus(USART2, USART_INT_TXC) != RESET)
    {
        USART_ClrIntPendingBit(USART2, USART_INT_TXC);

        /* 关闭发送完成中断 */
        USART2->CTRL1 &= ~(0x01 << 6);
        // USART_ConfigInt(USART2, USART_INT_TXC, DISABLE);
        if (!(DMA_CH6->CHCFG & DMA_CHCFG1_CHEN))
        {
            /* 查询到还有需要发送的数据 */
            if (tx_in != tx_out)
            {
                tx_len = (tx_in > tx_out) ? (tx_in - tx_out) : (sizeof(tx_buff) - tx_out);
                DMA_CH6->TXNUM = tx_len;
                DMA_CH6->MADDR = (uint32_t)tx_buff + tx_out;
                DMA_CH6->CHCFG |= DMA_CHCFG1_CHEN;
            }
            else
            {
                SystemClearActiveFlag(eSystemEventUartTx);
            }
        }
    }
}

void DMA_Channel6_IRQHandler(void)
{
    if (DMA_GetIntStatus(DMA_INT_TXC6, DMA))
    {
        DMA_ClrIntPendingBit(DMA_INT_TXC6, DMA);
        /* 关闭DMA通道 */
        DMA_CH6->CHCFG &= (uint16_t)(~DMA_CHCFG1_CHEN);
        tx_out = (tx_out + tx_len) & (sizeof(tx_buff) - 1);
        /* 打开发送完成中断 */
        USART2->CTRL1 |= (0x01 << 6);
        // USART_ConfigInt(USART2, USART_INT_TXC, ENABLE);
    }
}

// Redirect C library printf function to USART port.After rewriting, printf function can be used.
int fputc(int ch, FILE *f)
{
#if defined ENABLE_UART
    uint8_t c = ch;
    while(!hal_uart_write(0, &c, 1));
#endif
    return (ch);
}

// Redirect C library scanf function to USART port.After rewriting, functions such as scanf, getchar can be used.
int fgetc(FILE *f)
{
    /* Waiting for usart port input data */
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXDNE) == RESET)
        ;

    return (int)USART_ReceiveData(USART2);
}
