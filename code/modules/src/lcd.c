/****************************************************************************/
/*	File   :             F:\Smoker\SC01study\trunks\src\LCD\lcd.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/6/10,14:48:49                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "inc/lcd.h"
#include "common.h"
#include "bsp_config.h"
#include "inc/PwrManager.h"
#include <string.h>

#if defined SUPPORT_LCD_FUNCTION

/* Private define ------------------------------------------------------------*/

#define LCD_MAX_COM (4u)
#define LCD_ONE_CHAR_MAX_SEG (2u)
#define LCD_MAX_RAM (8u)

#define SignalMax (21u)
#define NumberMax (3u)

/* Private typedef -----------------------------------------------------------*/

// struct Lcd
// {
//     uint8_t State;
//     uint8_t LcdMem[LCD_MAX_COM];
// };
// typedef struct Lcd LcdTag;
static uint8_t LcdMem[LCD_MAX_COM];
static uint8_t last_LcdMem[LCD_MAX_COM];

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
// static LcdTag LcdObject = {eLcdNULL, {0x00, 0x00, 0x00, 0x00}};

// SEG_combination{ 0,A ,F ,E ,D ,B,G ,C}
const uint8_t NumberMap[SignalMax] = {
    /*	0  1		2		 3			   4   */
    0x7d, 0x05, 0x5e, 0x4f, 0x27,
    /*	5  6		 7		  8 		   9   */
    0x6b, 0x7b, 0x45, 0x7f, 0x6f,
    /*null	-	 A		B			 L */
    0x00, 0x02, 0x77, 0x3b, 0x38,
    /*E   R 		 P		S			 n        d */
    0x7a, 0x12, 0x76, 0x6b, 0x13, 0x1f};

//  SEG NUM  ARRAY
static uint8_t PositionColumn[NumberMax] = {4, 2, 0};

#define LcdGlassWriteMatrixPixel(PixelColumn, PixelRow) (LcdMem[PixelColumn] |= ((uint8_t)0x01 << PixelRow))

#define LcdGlassClearMatrixPixel(PixelColumn, PixelRow) (LcdMem[PixelColumn] &= ~((uint8_t)0x01 << PixelRow))

#define LcdClearRam() memset(LcdMem, 0, sizeof(LcdMem));

/* Private function prototypes -----------------------------------------------*/

void LCD_Clear(void)
{
    memset(LcdMem, 0, sizeof(LcdMem));
    LcdStoreReg();
}

/**
 * @brief  display all seg
 * @note
 *
 * @param  data:display data
 * @retval None
 */
void LcdStoreReg(void)
{
    uint64_t temp[LCD_MAX_COM];
    uint8_t is_change = 0;

    for (uint8_t i = 0; i < sizeof(LcdMem); i++)
    {
        if (LcdMem[i] != last_LcdMem[i])
        {
            is_change = 1;
            last_LcdMem[i] = LcdMem[i];
        }
    }

    if (is_change == 0)
        return;

    // if (memcmp(LcdMem, last_LcdMem, sizeof(LcdMem)) != 0)
    // {
    //     memcpy(last_LcdMem, LcdMem, sizeof(LcdMem));
    // }
    // else
    // {
    //     return;
    // }

    for (uint8_t i = 0; i < LCD_MAX_COM; i++)
        temp[lcd_com_conversion(i)] = lcd_ram_conversion(LcdMem[i]);
    hal_lcd_write((uint32_t*)temp, 0, LCD_MAX_COM << 1);
}

void LCD_BACK_LIGHT(unsigned char state)
{
    hal_gpio_write(PIN_BACKLIGHT, (pin_status_t)state);
}

/**
 * @brief  display string
 * @note   display special string
 *
 * @param  string_index:string index
 * @retval None
 */
int LcdDisplayString(uint8_t *pstr, uint8_t start, uint8_t end)
{
    int ret = -1;
    uint8_t i = start;
    int8_t *ptr = (int8_t *)pstr;

    while (i <= end)
    {
        int8_t *ch = ptr;
        uint8_t Num = (uint8_t)(*ch - '0');
        uint8_t Position = i;
        uint8_t p = 0;              // mem position
        uint8_t c = NumberMap[Num]; // for codec

        c = c << 1; // for first position

        if ((*ch <= LCD_DIS_d) && (*ch >= ASCII_NUM_0))
        {
            uint8_t temp = 0;
            uint8_t x = 0;
            uint8_t y = 0;
            p = PositionColumn[Position];
            for (y = 0; y < LCD_ONE_CHAR_MAX_SEG; ++y)
            {
                for (x = 0; x < LCD_MAX_COM; ++x)
                {
                    // clear bit no first position
                    if (y == 0 && x == 0)
                    {
                        ;
                    }
                    else
                    {
                        temp = ~((uint8_t)0x01 << (p + y)); // GET BIT POSITION
                        LcdMem[x] &= temp;                  // CLEAR BIT SEG
                    }

                    // get bit
                    temp = ((uint8_t)((c & (0x01 << x)) >> x) << (p + y));
                    LcdMem[x] |= temp;
                    // printf("LcdMem[%d] = 0x%x\n",x,LcdObject.LcdMem[x]);
                }
                c = c >> 4; // for second seg
            }
        }
        ptr++;
        i++;
    }
    LcdStoreReg();
    return ret;
}

/**
 * @brief  display point
 * @note
 *
 * @param
 * @retval None
 */
void LcdDisplayPoint(eLcdPointState eType, uint8_t com, uint8_t seg)
{
    switch (eType)
    {
    case eLcdPointOFF:
        LcdGlassClearMatrixPixel(com, seg);
        break;

    case eLcdPointON:
        LcdGlassWriteMatrixPixel(com, seg);
        break;

    default:
        break;
    }
    LcdStoreReg();
}

void SetLcdPointData(eLcdPointState eType, uint8_t com, uint8_t seg)
{
    switch (eType)
    {
    case eLcdPointOFF:
        LcdGlassClearMatrixPixel(com, seg);
        break;

    case eLcdPointON:
        LcdGlassWriteMatrixPixel(com, seg);
        break;

    default:
        break;
    }
}

int SetLcdString(uint8_t *pstr, uint8_t start, uint8_t end)
{
    int ret = -1;
    uint8_t i = start;
    int8_t *ptr = (int8_t *)pstr;

    while (i <= end)
    {
        int8_t *ch = ptr;
        uint8_t Num = (uint8_t)(*ch - '0');
        uint8_t Position = i;
        uint8_t p = 0;              // mem position
        uint8_t c = NumberMap[Num]; // for codec

        c = c << 1; // for first position

        if ((*ch <= LCD_DIS_d) && (*ch >= ASCII_NUM_0))
        {
            uint8_t temp = 0;
            uint8_t x = 0;
            uint8_t y = 0;
            p = PositionColumn[Position];
            for (y = 0; y < LCD_ONE_CHAR_MAX_SEG; ++y)
            {
                for (x = 0; x < LCD_MAX_COM; ++x)
                {
                    // clear bit no first position
                    if (y == 0 && x == 0)
                    {
                        ;
                    }
                    else
                    {
                        temp = ~((uint8_t)0x01 << (p + y)); // GET BIT POSITION
                        LcdMem[x] &= temp;                  // CLEAR BIT SEG
                    }

                    // get bit
                    temp = ((uint8_t)((c & (0x01 << x)) >> x) << (p + y));
                    LcdMem[x] |= temp;
                    // printf("LcdMem[%d] = 0x%x\n",x,LcdObject.LcdMem[x]);
                }
                c = c >> 4; // for second seg
            }
        }
        ptr++;
        i++;
    }

    return ret;
}

void LCD_DisplayNum(uint16_t num)
{
    char buf[10] = {0};
    uint8_t pos = 0;
    char strNull[4] = {LCD_DIS_NULL, LCD_DIS_NULL, LCD_DIS_NULL, LCD_DIS_NULL};
    SetLcdPointData(eLcdPointON, LCD_CO_COM, LCD_CO_SEG);         // CO鍥炬爣
    SetLcdPointData(eLcdPointON, LCD_CO_PPM_COM, LCD_CO_PPM_SEG); // PPM鍥炬爣
    SetLcdString((uint8_t *)strNull, 0, 2);

    if (num > 999)
    {
        num = 999;
    }

    if (num < 10)
    {
        pos = 2;
    }
    else if ((num >= 10) && (num < 100))
    {
        pos = 1;
    }
    else if ((num >= 100) && (num < 1000))
    {
        pos = 0;
    }
    sprintf(buf, "%d", num);

    SetLcdString((uint8_t *)buf, pos, 2); //鏄剧ず瀛楋拷??
    LcdStoreReg();
}

void LCD_DisplayStr(eLcdStr str)
{
    switch (str)
    {
    case eStr_End:
    {
        uint8_t str_E[3] = {LCD_DIS_E, LCD_DIS_n, LCD_DIS_d}; //"End"
        LCD_DsiplayCoFlg(eLcdON);
        BatteryLcdDisplay(BatteryState);
        LcdDisplayString(str_E, 0, 2);                        // End
    }
    break;

    case eStr_Err:
    {
        uint8_t str_Err[3] = {LCD_DIS_E, LCD_DIS_R, LCD_DIS_R}; //"Err"
        LCD_DsiplayCoFlg(eLcdON);
        BatteryLcdDisplay(BatteryState);
        LcdDisplayString(str_Err, 0, 2);                        // Err
    }
    break;

        // case eStr_LINE:
        // 	{
        // 		uint8_t str_LINE[3] = {LCD_DIS_LINE,LCD_DIS_LINE,LCD_DIS_LINE};//"-"
        // 		LcdDisplayString(str_LINE,start,end);//-
        // 	}
        // 	break;

    case eStr_Lb:
    {
        uint8_t str_Lb[3] = {LCD_DIS_L, LCD_DIS_B, LCD_DIS_NULL}; //"Lb "
        LCD_DsiplayCoFlg(eLcdON);
        BatteryLcdDisplay(BatteryState);
        LcdDisplayString(str_Lb, 0, 2);                           // Lb
    }
    break;

    case eStr_PAS:
    {
        uint8_t str_PAS[3] = {LCD_DIS_P, LCD_DIS_A, LCD_DIS_S}; //"PAS"
        LcdDisplayString(str_PAS, 0, 2);                        // PAS
    }
    break;

    case eStr_E03:
    {
        uint8_t str_E03[3] = {LCD_DIS_E, ASCII_NUM_0, NUM_3}; // E03
        LcdDisplayString(str_E03, 0, 2);
    }
    break;

    case eStr_E04:
    {
        uint8_t str_E04[3] = {LCD_DIS_E, ASCII_NUM_0, NUM_4}; // E04
        LcdDisplayString(str_E04, 0, 2);
    }
    break;

    case eStr_E06:
    {
        uint8_t str_E06[3] = {LCD_DIS_E, ASCII_NUM_0, NUM_6}; // E06
        LcdDisplayString(str_E06, 0, 2);
    }
    break;

    case eStr_E07:
    {
        uint8_t str_E07[3] = {LCD_DIS_E, ASCII_NUM_0, NUM_7}; // E07
        LcdDisplayString(str_E07, 0, 2);
    }
    break;

    case eStr_E08:
    {
        uint8_t str_E08[3] = {LCD_DIS_E, ASCII_NUM_0, NUM_8}; // E08
        LcdDisplayString(str_E08, 0, 2);
    }
    break;

    case eStr_E09:
    {
        uint8_t str_E09[3] = {LCD_DIS_E, ASCII_NUM_0, NUM_9}; // E09
        LcdDisplayString(str_E09, 0, 2);
    }
    break;

    case eStr_E10:
    {
        uint8_t str_E10[3] = {LCD_DIS_E, NUM_1, ASCII_NUM_0}; // E10
        LcdDisplayString(str_E10, 0, 2);
    }
    break;

    case eStr_E11:
    {
        uint8_t str_E11[3] = {LCD_DIS_E, NUM_1, NUM_1}; // E11
        LcdDisplayString(str_E11, 0, 2);
    }
    break;

    case eStr_E12:
    {
        uint8_t str_E12[3] = {LCD_DIS_E, NUM_1, NUM_2}; // E12
        LcdDisplayString(str_E12, 0, 2);
    }
    break;
    }
}

void LCD_DsiplayCoFlg(eLcdState control)
{
    switch (control)
    {
    case eLcdON: //
        SetLcdPointData(eLcdPointON, LCD_CO_COM, LCD_CO_SEG);
        SetLcdPointData(eLcdPointON, LCD_CO_PPM_COM, LCD_CO_PPM_SEG);
        break;

    case eLcdOFF: //
        SetLcdPointData(eLcdPointOFF, LCD_CO_COM, LCD_CO_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_CO_PPM_COM, LCD_CO_PPM_SEG);
        break;

    default:
        return;
    }

    LcdStoreReg();
}

void LCD_SystemStartAnimation(uint8_t line)
{
    switch (line)
    {
    case 0:
        SetLcdPointData(eLcdPointON, LCD_DIS_LINE_COM, LCD_DIS_LINE1_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_DIS_LINE_COM, LCD_DIS_LINE2_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_DIS_LINE_COM, LCD_DIS_LINE3_SEG);

        break;

    case 1:
        SetLcdPointData(eLcdPointON, LCD_DIS_LINE_COM, LCD_DIS_LINE1_SEG);
        SetLcdPointData(eLcdPointON, LCD_DIS_LINE_COM, LCD_DIS_LINE2_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_DIS_LINE_COM, LCD_DIS_LINE3_SEG);
        break;

    case 2:
        SetLcdPointData(eLcdPointON, LCD_DIS_LINE_COM, LCD_DIS_LINE1_SEG);
        SetLcdPointData(eLcdPointON, LCD_DIS_LINE_COM, LCD_DIS_LINE2_SEG);
        SetLcdPointData(eLcdPointON, LCD_DIS_LINE_COM, LCD_DIS_LINE3_SEG);
        break;

    default:
        return;
    }

    LcdStoreReg();
}

/**
 * @brief 电量显示格数
 * 
 * @param BatteryState 
 */
void BatteryLcdDisplay(uint8_t BatteryState)
{
    uint8_t str1[3] = {LCD_DIS_L, LCD_DIS_B, LCD_DIS_NULL}; //"LB "

    LCD_DsiplayCoFlg(eLcdON);
    SetLcdPointData(eLcdPointON, LCD_BATTERY_COM, LCD_BATTERY_SEG);
    switch (BatteryState)
    {
    case 1:
        SetLcdPointData(eLcdPointOFF, LCD_BATTERY_LEVEL1_COM, LCD_BATTERY_LEVEL1_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_BATTERY_LEVEL2_COM, LCD_BATTERY_LEVEL2_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_BATTERY_LEVEL3_COM, LCD_BATTERY_LEVEL3_SEG);
        LcdDisplayString(str1, 0, 2);
        break;

    case 2:
        SetLcdPointData(eLcdPointON, LCD_BATTERY_LEVEL1_COM, LCD_BATTERY_LEVEL1_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_BATTERY_LEVEL2_COM, LCD_BATTERY_LEVEL2_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_BATTERY_LEVEL3_COM, LCD_BATTERY_LEVEL3_SEG);
        break;

    case 3:
        SetLcdPointData(eLcdPointON, LCD_BATTERY_LEVEL1_COM, LCD_BATTERY_LEVEL1_SEG);
        SetLcdPointData(eLcdPointON, LCD_BATTERY_LEVEL2_COM, LCD_BATTERY_LEVEL2_SEG);
        SetLcdPointData(eLcdPointOFF, LCD_BATTERY_LEVEL3_COM, LCD_BATTERY_LEVEL3_SEG);
        break;

    case 4:
        SetLcdPointData(eLcdPointON, LCD_BATTERY_LEVEL1_COM, LCD_BATTERY_LEVEL1_SEG);
        SetLcdPointData(eLcdPointON, LCD_BATTERY_LEVEL2_COM, LCD_BATTERY_LEVEL2_SEG);
        SetLcdPointData(eLcdPointON, LCD_BATTERY_LEVEL3_COM, LCD_BATTERY_LEVEL3_SEG);
        break;
    default:
        return;
    }
    LcdStoreReg();
}

/**
 * @brief lcd待机显示
 * 
 * @param ppm 具体ppm 
 * @param ClrFlg true清屏 false不清屏
 */
void LCDStandyDisplay(uint16_t ppm,BOOL ClrFlg)
{
    if(ClrFlg)
    {
        LcdClearRam();    
    }
    LCD_Control(eLcdON);
    BatteryLcdDisplay(GetBatterLevel());
    LCD_DsiplayCoFlg(eLcdON);
    (ppm>0)?LCD_DisplayNum(ppm):LCD_DisplayNum(0);
    
}

void LCD_Control(eLcdState state)
{
    switch (state)
    {
    case eLcdON:
        hal_lcd_enble();
        break;

    case eLcdOFF:
        LCD_Clear();
        hal_lcd_disenble();
        break;
    }
}

REG_APP_INIT(hal_lcd_enble);

#endif //#if defined SUPPORT_LCD_FUNCTION

/********************END OF FILE**********************/
