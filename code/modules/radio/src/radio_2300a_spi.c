#include "radio/inc/radio_2300a_spi.h"
#include "bsp_config.h"
#include "n32l40x.h"

#ifdef SUPPORT_RADIO_FUNCTION 

#define	cmt_spi3_csb_out()		hal_gpio_mode(PIN_CSB, PIN_MODE_OUTPUT)
#define	cmt_spi3_fcsb_out()		hal_gpio_mode(PIN_FCSB, PIN_MODE_OUTPUT)
#define	cmt_spi3_sclk_out()		hal_gpio_mode(PIN_SCLK, PIN_MODE_OUTPUT)
#define	cmt_spi3_sdio_out()		hal_gpio_mode(PIN_SDIO, PIN_MODE_OUTPUT)
#define	cmt_spi3_sdio_in()		hal_gpio_mode(PIN_SDIO, PIN_MODE_INPUT_PULLUP)

#define	cmt_spi3_csb_1()		hal_gpio_write(PIN_CSB,PIN_SET) //GPIO_WriteBit(Subghz_Csb_Port,Subghz_Csb_Pin,Bit_SET)
#define	cmt_spi3_csb_0()		hal_gpio_write(PIN_CSB,PIN_RESET)

#define	cmt_spi3_fcsb_1()		hal_gpio_write(PIN_FCSB,PIN_SET)
#define	cmt_spi3_fcsb_0()		hal_gpio_write(PIN_FCSB,PIN_RESET)

#define	cmt_spi3_sclk_1()		hal_gpio_write(PIN_SCLK,PIN_SET)
#define	cmt_spi3_sclk_0()		hal_gpio_write(PIN_SCLK,PIN_RESET)

#define	cmt_spi3_sdio_1()		hal_gpio_write(PIN_SDIO,PIN_SET)
#define	cmt_spi3_sdio_0()		hal_gpio_write(PIN_SDIO,PIN_RESET)

#define	cmt_spi3_sdio_read()	hal_gpio_read(PIN_SDIO)


/* ************************************************************************ */
    
void cmt_spi3_delay(void)
{
	unsigned short n = 28;
	while(n--);
}

void cmt_spi3_delay_us(void)
{
	unsigned short n = 32;
	while(n--);
}

void cmt_spi3_init(void)
{
	cmt_spi3_csb_1();
	cmt_spi3_csb_out();
	cmt_spi3_csb_1();   /* CSB has an internal pull-up resistor */

	cmt_spi3_sclk_0();
	cmt_spi3_sclk_out();
	cmt_spi3_sclk_0();   /* SCLK has an internal pull-down resistor */

	cmt_spi3_sdio_1();
	cmt_spi3_sdio_out();
	cmt_spi3_sdio_1();

	cmt_spi3_fcsb_1();
	cmt_spi3_fcsb_out();
	cmt_spi3_fcsb_1();  /* FCSB has an internal pull-up resistor */

	cmt_spi3_delay();
}

void cmt_spi3_send(uint8_t data8)
{
    uint8_t i;

    for(i=0; i<8; i++)
    {
        cmt_spi3_sclk_0();

        /* Send byte on the rising edge of SCLK */
        if(data8 & 0x80)
            cmt_spi3_sdio_1();
        else            
            cmt_spi3_sdio_0();

        cmt_spi3_delay();

        data8 <<= 1;
        cmt_spi3_sclk_1();
        cmt_spi3_delay();
    }
}

uint8_t cmt_spi3_recv(void)
{
    uint8_t i;
    uint8_t data8 = 0xFF;

    for(i=0; i<8; i++)
    {
        cmt_spi3_sclk_0();
        cmt_spi3_delay();
        data8 <<= 1;

        cmt_spi3_sclk_1();

        /* Read byte on the rising edge of SCLK */
        if(cmt_spi3_sdio_read())
            data8 |= 0x01;
        else
            data8 &= ~0x01;

        cmt_spi3_delay();
    }

    return data8;
}

void cmt_spi3_write(uint8_t addr, uint8_t dat)
{
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_out();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0(); 

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    /* r/w = 0 */
    cmt_spi3_send(addr&0x7F);

    cmt_spi3_send(dat);

    cmt_spi3_sclk_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    cmt_spi3_csb_1();
    
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();    
}

void cmt_spi3_read(uint8_t addr, uint8_t* p_dat)
{
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_out();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0(); 

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    /* r/w = 1 */
    cmt_spi3_send(addr|0x80);

    /* Must set SDIO to input before the falling edge of SCLK */
    cmt_spi3_sdio_in();
    
    *p_dat = cmt_spi3_recv();

    cmt_spi3_sclk_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    cmt_spi3_csb_1();
    
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

void cmt_spi3_write_fifo(const uint8_t* p_buf, uint16_t len)
{
    uint8_t i;

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_1();
    cmt_spi3_csb_out();
    cmt_spi3_csb_1();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0();

    cmt_spi3_sdio_out();

    for(i=0; i<len; i++)
    {
        cmt_spi3_fcsb_0();

        /* > 1 SCLK cycle */
        cmt_spi3_delay();
        cmt_spi3_delay();

        cmt_spi3_send(p_buf[i]);

        cmt_spi3_sclk_0();

        /* > 2 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();

        cmt_spi3_fcsb_1();

        /* > 4 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
    }

    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

void cmt_spi3_read_fifo(uint8_t* p_buf, uint16_t len)
{
    uint16_t i;

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_1();
    cmt_spi3_csb_out();
    cmt_spi3_csb_1();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0();

    cmt_spi3_sdio_in();

    for(i=0; i<len; i++)
    {
        cmt_spi3_fcsb_0();

        /* > 1 SCLK cycle */
        cmt_spi3_delay();
        cmt_spi3_delay();

        p_buf[i] = cmt_spi3_recv();

        cmt_spi3_sclk_0();

        /* > 2 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();

        cmt_spi3_fcsb_1();

        /* > 4 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
    }

    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

/**
 * @brief 2300ªΩ–—mcu÷–∂œ ioø⁄≈‰÷√
 * 
 */
void RadioWakeUpPinCfg(void)
{
    /**
     * @brief 2300 …‰∆µ≤‚ ‘∞¥º¸ - ∏¥”√…’¬ºSWCLK
     * 
     */
    /*…‰∆µ≤‚ ‘∞¥º¸ ÷–∂œ ‰»Î“˝Ω≈≥ı ºªØ */
    hal_gpio_mode(PIN_RADIOTEST,PIN_MODE_INPUT_PULLUP);
    hal_gpio_setirq(PIN_RADIOTEST, IT_TRIGGER_FALLING);

    /**
     * @brief 2300 ªΩ–—mcu÷–∂œ
     * 
     */
    /*…‰∆µªΩ–—mcu ÷–∂œ ‰»Î“˝Ω≈≥ı ºªØ */   
    hal_gpio_mode(PIN_WAKEUP,PIN_MODE_INPUT_PULLDOWN);
    hal_gpio_setirq(PIN_WAKEUP, IT_TRIGGER_RISING);

}


void CMT2300A_InitGpio(void)
{
	cmt_spi3_init();
}

/*! ********************************************************
* @name    CMT2300A_ReadReg
* @desc    Read the CMT2300A register at the specified address.
* @param   addr: register address
* @return  Register value
* *********************************************************/
uint8_t CMT2300A_ReadReg(uint8_t addr)
{
    uint8_t dat = 0xFF;
    cmt_spi3_read(addr, &dat);
	
    return dat;
}

/*! ********************************************************
* @name    CMT2300A_WriteReg
* @desc    Write the CMT2300A register at the specified address.
* @param   addr: register address
*          dat: register value
* *********************************************************/
void CMT2300A_WriteReg(uint8_t addr, uint8_t dat)
{
    cmt_spi3_write(addr, dat);
}

/*! ********************************************************
* @name    CMT2300A_ReadFifo
* @desc    Reads the contents of the CMT2300A FIFO.
* @param   buf: buffer where to copy the FIFO read data
*          len: number of bytes to be read from the FIFO
* *********************************************************/
void CMT2300A_ReadFifo(uint8_t buf[], uint16_t len)
{
    cmt_spi3_read_fifo(buf, len);
}

/*! ********************************************************
* @name    CMT2300A_WriteFifo
* @desc    Writes the buffer contents to the CMT2300A FIFO.
* @param   buf: buffer containing data to be put on the FIFO
*          len: number of bytes to be written to the FIFO
* *********************************************************/
void CMT2300A_WriteFifo(const uint8_t buf[], uint16_t len)
{
    cmt_spi3_write_fifo(buf, len);
}

#endif /* end of SUPPORT_RADIO_FUNCTION */
