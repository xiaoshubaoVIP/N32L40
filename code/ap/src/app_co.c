#include "app_co.h"
#include "common.h"
#include "app_main.h"
#include "modules_config.h"
#include "app_smoke.h"

#define SUPPORT_CO_DEBUG
#define FLASH_MAP CoFlashMap
#define FLASH_MAP_ADDR_POS FLASH_MAP_CO_ADDR_POS

#define BSI_CERTIFY_FAIL_PROCESS // quanwu.xu 20181218

// for emc
#define CO_SAMPLE_CHANGE_MAX 50      // 125mv=50ppm
#define CO_SAMPLE_NEXT_CHANGE_MAX 50 // 125mv=50ppm
#define CO_SAMPLE_FAULT_CORRECT 30   // 70mv

#define BATTERY_FACTORY_CHECK (3000)                     // 3.00V
#define BATTERY_CO_FACTORY_CHECK (BATTERY_FACTORY_CHECK) // 3.00V

// co alarm set
#define CO_CALI_STATE (0xa5)

#define CO_ALARM_400PPM_CONCENTRATION (340) // 400
#define CO_ALARM_150PPM_CONCENTRATION (120) // 150
#define CO_ALARM_70PPM_CONCENTRATION (56)   // 70

#define CO_ALARM_300PPM_CONCENTRATION (210) // 300
#define CO_ALARM_100PPM_CONCENTRATION (70)  // 100
#define CO_ALARM_50PPM_CONCENTRATION (40)   // 50
#define CO_ALARM_30PPM_CONCENTRATION (22)   // 30

#define CO_CALIBRATION_STEP0_TIME 6000
#define CO_CALIBRATION_STEP1_TIME 6000
#define CO_CALIBRATION_STEP2_TIME 6000
#define CO_CALIBRATION_WAIT_STABLE 6000
#define CO_CALIBRATION_CHECK_TIME (3000)
#define CO_CALIBRATION_INTERVAL_TIME (3000)
#define CO_BEEP_TIME (200)
#define CO_BEEPDELAY_TIME (4700)
#define CO_ERR_TIME (60)
#define CO_CALIBRATION_ERR_TIME (10)
#define CO_MUTE_TIME (270000)
#define CO_MUTE_TIME_OUT (240000) // 4min
#define CO_ALARM_TIME_OUT (60000)
#define CO_CALIBRATION_DELAY_TIME (15) // weit for stable  6s*15 = 1min30S
#define CO_SELFCHECK_COUNT (6)         // 60s/SLEEP_TIME=6
#define CO_CHECK_COUNT (3)             // 30s/SLEEP_TIME=6
#define CO_KEY_TEST_DELAY_TIME (1000)  // MS
#define CO_TEST_LCD_TIME (450)
#define CO_EXIT_TIME (1000)
#define CO_CALIBRATION_COUNT (16)
#define CO_DIFF_VALUE (5)

#if defined STANDARD_UL2034_UL217
#define COPPMLEVEL0 (30)
#define COPPMLEVEL1 (50)   //(70)
#define COPPMLEVEL2 (100)  //(150)
#define COPPMLEVEL3 (300)  //(400)
#define COPPMLEVEL4 (4500) //(5000)

#define CO_ALARM_70PPM_TIME (120) // 120min    (60-240)
#define CO_ALARM_150PPM_TIME (20) // 20min    (10-50)
#define CO_ALARM_400PPM_TIME (6)  // 6min     (4-15)
#elif defined STANDARD_EN50291_EN14604

#if defined TUV_RENGZHENG
#define COPPMLEVEL0 (24)
#else
#define COPPMLEVEL0 (30)
#endif

#define COPPMLEVEL1 (45)
#define COPPMLEVEL2 (75)  // 100
#define COPPMLEVEL3 (210) //(300)
#define COPPMLEVEL4 (4500)

#define CO_ALARM_30PPM_TIME (150) // 150min    (>120)
#define CO_ALARM_50PPM_TIME (70)  // 70min     (60-90)
#define CO_ALARM_100PPM_TIME (20) // 20min      (10-40)
#define CO_ALARM_300PPM_TIME (1)  // 1min       (<3)
#endif

#define CO_ALARM_COUNT (4)

typedef struct
{
    uint8_t COCaliResult;
    uint8_t COCaliState;         // 0: step 0; 1:step 1; 2:step 2;0xa5 :caliOK
    uint16_t Sample;             // adc voltage
    uint16_t COCaliGradient;     // 	k;  y = k*x + b;
    uint16_t COCaliOffset;       //	b;  y = k*x + b;
    uint16_t COCaliThresholdMin; // calibration threshold min value
    uint16_t COCaliThresholdMax; // calibration threshold max value
} COCaliTag;
typedef struct
{
    uint8_t State;   // CO state
    uint16_t Sample; // CO out adc voltage
    uint16_t Ppm;    // CO PPM
    uint16_t last_Ppm;
    uint16_t PpmPeak;           // CO PPM history peak
    uint16_t COCaliGradient;    //	k;  y = k*x + b;
    uint16_t COCaliOffset;      //  b;  y = k*x + b;
    uint16_t COAlarmThreshold0; //
    uint16_t COAlarmThreshold1; //
    uint16_t COAlarmThreshold2; //
    uint16_t COAlarmThreshold3; //
    uint16_t COAlarmThreshold4; //
    uint8_t COFaultOpenCount;   // CO OPEN ERR COUNTER  >2 ALARM
    uint8_t COFaultShortCount;  // CO SHORT ERR COUNTER >2 ALARM

#if defined TUV_RENGZHENG
    uint8_t Co_5000PPM_Flag;
    uint16_t co_5000ppm_count;
    uint8_t EMC_CHECK_Flag; // 0???normal  1:emc
    uint8_t co_sensor_pre_fault;
    uint8_t Self_Check_Flag; // CO state
#endif
} COTag;

static COTag CoObject;
static COCaliTag CoCaliObject;
static uint8_t CoLevel;

uint16_t getCoAppPpm(void)
{
    return CoObject.Ppm;
}

uint8_t getCoLevel(void)
{
    return CoLevel;
}

BOOL IsCOErr(void)
{
    BOOL Ret = false;
    if ((CoObject.State == eCOOPEN) || (CoObject.State == eCOSHORT))
    {
        Ret = true;
    }
    return Ret;
}

BOOL IsCOPPM(void)
{
    return ((CoObject.Ppm > COPPMLEVEL0) ? true : false);
}

void COSetPeakData(uint16_t PpmPeakValue)
{
    CoObject.PpmPeak = PpmPeakValue;
    CoPeakChange();
}

uint16_t COGetPeakData(void)
{
    return CoObject.PpmPeak;
}

static uint8_t COCalibrationCheck(COCaliTag *pcali, uint16_t COData)
{
    uint8_t ret = 1;
    if ((COData > pcali->COCaliThresholdMin) && (COData < pcali->COCaliThresholdMax))
    {
        CoCaliObject.COCaliResult = eCoCailOk;
        pcali->Sample = COData;
        log_noinfo("avg=%d", COData);
    }
    else
    {
        log_noinfo("avg %d out range(%d<x<%d)", COData, pcali->COCaliThresholdMin, pcali->COCaliThresholdMax);
        CoCaliObject.COCaliResult = eCoCailFail;
        ret = 0;
    }
    SystemPostFIFI(eSystemEvenCalibrationFinish);
    return ret;
}

static uint8_t COCalibrationStartCheck(COCaliTag *pcali, uint16_t COData)
{
    return ((COData > pcali->COCaliThresholdMin) && (COData < pcali->COCaliThresholdMax));
}

/**
 * @brief 获取CObuff的差值
 *
 * @param buff CObuff
 * @param size CObuff大小
 * @return uint16_t 最大最小的差值
 */
uint16_t getCoBuffDifference(uint16_t buff[], uint16_t size)
{
    uint16_t max = 0;
    uint16_t min = ~0;

    for (uint16_t i = 0; i < size; i++)
    {
        if (buff[i] == 0)
            continue;
        if (max < buff[i])
            max = buff[i];

        if (min > buff[i])
            min = buff[i];
    }

    return max - min;
}

/**
 * @brief Co校机 - 等待采样稳定
 *
 */
static void COCalibrationWaitStable(TmrHdl Handle)
{
    uint16_t COTemp;

    static uint8_t out_range_count;
    static uint8_t stable_count;
    static uint8_t timeout_count;

    static uint8_t co_index;
    static uint16_t co_buf[CO_CALIBRATION_DELAY_TIME];

    LedPollingHandle(LED_TYPE_YELLOW, 50);

    timeout_count++;

    /* 超过5分钟就跳出 */
    if (timeout_count > ((3 * 60 * 1000) / CO_CALIBRATION_WAIT_STABLE))
    {
        log_noinfo("stable ok");
        TmrSetCtx(Handle, (void *)eCoCailFinsih);
        TmrSetCurCnt(Handle, TMR_PERIOD(CO_CALIBRATION_INTERVAL_TIME));
        ((TmrInfo *)Handle)->ReloadCnt = TMR_PERIOD(CO_CALIBRATION_INTERVAL_TIME);
        return;
    }

    COTemp = COGetData();
    if (COCalibrationStartCheck(&CoCaliObject, COTemp))
    {
        co_buf[co_index] = COTemp;

        co_index = (co_index < CO_CALIBRATION_DELAY_TIME) ? co_index + 1 : 0;

        if ((COTemp = getCoBuffDifference(co_buf, CO_CALIBRATION_DELAY_TIME)) < 10)
        {
            stable_count++;
        }

        log_noinfo("Dif=%d", COTemp);

        if (stable_count > CO_CALIBRATION_DELAY_TIME)
        {
            timeout_count = 0xf0;
        }
    }
    else
    {
        stable_count = 0;
        co_index = 0;
        out_range_count++;
        log_noinfo("out threshold");
        if (out_range_count > CO_CALIBRATION_DELAY_TIME)
        {
            CoCaliObject.COCaliResult = eCoCailFail;
            SystemPostFIFI(eSystemEvenCalibrationFinish);
            TmrDeleteTimer(Handle);
            log_noinfo("co fail. out range");
        }
    }
}

/**
 * @brief Co校机阈值合格数据
 *
 * @param pcali
 */
static void CoCalibrationValueInit(COCaliTag *pcali)
{
    uint16_t COCaliThresholdMin = PPM_CARIBRATON_STEP0_THRESHOLD_MIN;
    uint16_t COCaliThresholdMax = PPM_CARIBRATON_STEP0_THRESHOLD_MAX;

    switch (pcali->COCaliState)
    {
    case 0: // 0ppm
        COCaliThresholdMin = PPM_CARIBRATON_STEP0_THRESHOLD_MIN;
        COCaliThresholdMax = PPM_CARIBRATON_STEP0_THRESHOLD_MAX;
        break;

    case 1: // 150ppm
        COCaliThresholdMin = PPM_CARIBRATON_STEP1_THRESHOLD_MIN + pcali->COCaliOffset;
        COCaliThresholdMax = PPM_CARIBRATON_STEP1_THRESHOLD_MAX + pcali->COCaliOffset;
        break;

    case 2: // 300ppm/400ppm
        COCaliThresholdMin = pcali->COCaliOffset + (PPM_CARIBRATON_STEP2_THRESHOLD_MIN * pcali->COCaliGradient);
        COCaliThresholdMax = pcali->COCaliOffset + (PPM_CARIBRATON_STEP2_THRESHOLD_MAX * pcali->COCaliGradient);
        break;
    }
    pcali->COCaliThresholdMin = COCaliThresholdMin;
    pcali->COCaliThresholdMax = COCaliThresholdMax;

    printf("COCaliThresholdMin = %d\n", COCaliThresholdMin);
    printf("COCaliThresholdMax = %d\n", COCaliThresholdMax);
}

void COInit(void)
{
    // FlashGetData(COCaliGradient, CoObject.COCaliGradient);
    // FlashGetData(COCaliOffset, CoObject.COCaliOffset);
}
REG_APP_INIT(COInit);

#if defined TUV_RENGZHENG
static uint16_t Test_5000_RenZheng(uint16_t PpmCode) // 50~60
{
    uint16_t ret = PpmCode;

    if (PpmCode >= 10 && PpmCode < 15) // 56~59
    {
        PpmCode += 45;
    }
    else if (PpmCode >= 15 && PpmCode < 20) // 55~59
    {
        PpmCode += 40;
    }
    else if (PpmCode >= 20 && PpmCode < 25) // 55~59
    {
        PpmCode += 35;
    }
    else if (PpmCode >= 25 && PpmCode < 30) // 55~59
    {
        PpmCode += 30;
    }
    else if (PpmCode >= 30 && PpmCode < 35) // 55~59
    {
        PpmCode += 25;
    }
    else if (PpmCode >= 35 && PpmCode < 40) // 55~59
    {
        PpmCode += 20;
    }
    else if (PpmCode >= 40 && PpmCode < 45) // 55~59
    {
        PpmCode += 15;
    }
    else if (PpmCode >= 45 && PpmCode < 50) // 55~59
    {
        PpmCode += 10;
    }
    else if (PpmCode > 50 && PpmCode < 60) //?????????
    {
    }
    else if (PpmCode >= 60 && PpmCode < 65)
    {
        PpmCode -= 5;
    }
    else if (PpmCode >= 65 && PpmCode < 70)
    {
        PpmCode -= 10;
    }
    else if (PpmCode >= 70 && PpmCode < 75)
    {
        PpmCode -= 15;
    }
    else if (PpmCode >= 75 && PpmCode < 80)
    {
        PpmCode -= 20;
    }
    ret = PpmCode;
    return ret;
}

static uint16_t Compensationalgorithm(COTag *pCO, uint16_t Ppm)
{
    static uint16_t co_after_5000ppm_count = 0;

#if defined SUPPORT_CO_DEBUG
    if (IsUartOpen())
    {
        debug("PPM5000Before = %d\n", Ppm);
    }
#endif
    if (pCO->Co_5000PPM_Flag == 1)
    {
        co_after_5000ppm_count++;
        if (co_after_5000ppm_count > 840) // 7h=360*7=2520
        {
            pCO->Co_5000PPM_Flag = 0;
        }
        else
        {
            if (Ppm >= 10)
            {
                if (co_after_5000ppm_count > 720) // 6h=360*6 = 2160
                {
                    Ppm = Test_5000_RenZheng(Ppm);
                }
                else if (co_after_5000ppm_count > 600) // 5h=360*5 = 1800
                {
                    Ppm = Test_5000_RenZheng(Ppm);
                }
                else if (co_after_5000ppm_count > 480) // 4h=360*4= 1440
                {
                    Ppm = Test_5000_RenZheng(Ppm);
                }
                else if (co_after_5000ppm_count > 360) // 3h=360*3=1080
                {
                    Ppm = Test_5000_RenZheng(Ppm);
                }
                else if (co_after_5000ppm_count > 120) // 1h=360*1=360
                {
                    Ppm = Test_5000_RenZheng(Ppm);
                }
                else
                {
                    Ppm = Test_5000_RenZheng(Ppm);
                }
            }
        }
    }
    else
    {
        co_after_5000ppm_count = 0;
    }
#if defined SUPPORT_CO_DEBUG
    if (IsUartOpen())
    {
        debug("PPM5000Last = %d\n", Ppm);
    }
#endif
    return Ppm;
}

static uint16_t RenZheng_Code_70PPM_High(uint16_t PpmCode)
{
    uint16_t ret = PpmCode;

    if (PpmCode >= 25 && PpmCode < 30) // 29~34
    {
        PpmCode += (PpmCode * 20) / 100;
    }
    else if (PpmCode >= 30 && PpmCode < 35) //
    {
    }
    else if (PpmCode >= 35 && PpmCode < 40) // 30~34
    {
        PpmCode -= (PpmCode * 14) / 100;
    }
    else if (PpmCode >= 40 && PpmCode < 45) // 30~33
    {
        PpmCode -= (PpmCode * 25) / 100;
    }
    //////////////////////////////////////////////////////

    else if (PpmCode >= 45 && PpmCode < 50) // 49~55
    {
        PpmCode += (PpmCode * 10) / 100;
    }
    else if (PpmCode >= 50 && PpmCode < 55) // 49~54
    {
    }
    else if (PpmCode >= 55 && PpmCode < 60) // 50~55
    {
        PpmCode -= (PpmCode * 10) / 100;
    }
    else if (PpmCode >= 60 && PpmCode < 65) // 51~55
    {
        PpmCode -= (PpmCode * 15) / 100;
    }
    else if (PpmCode >= 65 && PpmCode < 70) // 50~54
    {
        PpmCode -= (PpmCode * 23) / 100;
    }
    ret = PpmCode;
    return ret;
}

static uint16_t RenZheng_Code_100PPM_High(uint16_t PpmCode)
{
    uint16_t ret = PpmCode;

    if (PpmCode >= 70 && PpmCode < 80) // 99~112
    {
        PpmCode += (PpmCode * 42) / 100;
    }
    else if (PpmCode >= 80 && PpmCode < 90) // 100~112
    {
        PpmCode += (PpmCode * 25) / 100;
    }
    else if (PpmCode >= 90 && PpmCode < 100) // 100~111
    {
        PpmCode += (PpmCode * 11) / 100;
    }
    else if (PpmCode >= 100 && PpmCode < 110) // 99~108
    {
    }
    else if (PpmCode >= 110 && PpmCode < 120) // 99
    {
        PpmCode -= (PpmCode * 9) / 100;
    }
    else if (PpmCode >= 120 && PpmCode < 130) // 101~109
    {
        PpmCode -= (PpmCode * 16) / 100;
    }
    else if (PpmCode >= 130 && PpmCode < 140) // 101~109
    {
        PpmCode -= (PpmCode * 23) / 100;
    }
    else if (PpmCode >= 140 && PpmCode < 150) // 101~108
    {
        PpmCode -= (PpmCode * 28) / 100;
    }

    ret = PpmCode;
    return ret;
}

static uint16_t RenZheng_Code_70PPM(uint16_t PpmCode)
{
    uint16_t ret = PpmCode;

    if (PpmCode >= 14 && PpmCode < 19) // 29~37
    {
        PpmCode += (PpmCode * 110) / 100;
    }
    else if (PpmCode >= 19 && PpmCode < 24) // 29~36
    {
        PpmCode += (PpmCode * 57) / 100;
    }
    else if (PpmCode >= 24 && PpmCode < 29) // 30~35
    {
        PpmCode += (PpmCode * 25) / 100;
    }
    else if (PpmCode >= 29 && PpmCode < 34) // 49~56
    {
        PpmCode += (PpmCode * 72) / 100;
    }
    else if (PpmCode >= 34 && PpmCode < 39) // 49~55
    {
        PpmCode += (PpmCode * 47) / 100;
    }
    else if (PpmCode >= 39 && PpmCode < 44) // 49~54
    {
        PpmCode += (PpmCode * 28) / 100;
    }
    else if (PpmCode >= 44 && PpmCode < 49) // 49~55
    {
        PpmCode += (PpmCode * 13) / 100;
    }
    else if (PpmCode >= 49 && PpmCode < 54)
    {
    }
    else if (PpmCode >= 54 && PpmCode < 60) // 50~54
    {
        PpmCode -= (PpmCode * 8) / 100;
    }
    ret = PpmCode;
    return ret;
}
static uint16_t RenZheng_Code_Olny100(uint16_t PpmCode)
{
    uint16_t ret = PpmCode;

    if (PpmCode >= 60 && PpmCode < 70) // 99~114
    {
        PpmCode += (PpmCode * 66) / 100;
    }
    else if (PpmCode >= 70 && PpmCode < 80) // 100~112
    {
        PpmCode += (PpmCode * 42) / 100;
    }
    else if (PpmCode >= 80 && PpmCode < 90) // 100~111
    {
        PpmCode += (PpmCode * 25) / 100;
    }
    else if (PpmCode >= 90 && PpmCode < 100) // 99~108
    {
        PpmCode += (PpmCode * 10) / 100;
    }
    else if (PpmCode >= 100 && PpmCode < 110)
    {
    }
    else if (PpmCode >= 110 && PpmCode < 120) // 101~109
    {
        PpmCode -= (PpmCode * 9) / 100;
    }
    else if (PpmCode >= 120 && PpmCode < 130) // 101~109
    {
        PpmCode -= (PpmCode * 16) / 100;
    }
    else if (PpmCode >= 130 && PpmCode < 140) // 101~108
    {
        PpmCode -= (PpmCode * 23) / 100;
    }
    else if (PpmCode >= 140 && PpmCode < 150) // 101~108
    {
        PpmCode -= (PpmCode * 28) / 100;
    }
    ret = PpmCode;
    return ret;
}

static uint16_t RenZheng_Code_Olny300(uint16_t PpmCode)
{
    uint16_t ret = PpmCode;

    if (PpmCode >= 150 && PpmCode < 160) // 300~310
    {
        PpmCode += (PpmCode * 100) / 100;
    }
    else if (PpmCode >= 160 && PpmCode < 170) // 297~311
    {
        PpmCode += (PpmCode * 87) / 100;
    }
    else if (PpmCode >= 170 && PpmCode < 180) // 297~311
    {
        PpmCode += (PpmCode * 76) / 100;
    }
    else if (PpmCode >= 180 && PpmCode < 190) // 297~311
    {
        PpmCode += (PpmCode * 65) / 100;
    }
    else if (PpmCode >= 190 && PpmCode < 200) // 298~312
    {
        PpmCode += (PpmCode * 57) / 100;
    }
    else if (PpmCode >= 200 && PpmCode < 210) // 300~313
    {
        PpmCode += (PpmCode * 50) / 100;
    }
    else if (PpmCode >= 210 && PpmCode < 220) // 298~310
    {
        PpmCode += (PpmCode * 42) / 100;
    }
    else if (PpmCode >= 220 && PpmCode < 230) // 299~311
    {
        PpmCode += (PpmCode * 36) / 100;
    }
    else if (PpmCode >= 230 && PpmCode < 240) // 299~310
    {
        PpmCode += (PpmCode * 30) / 100;
    }
    else if (PpmCode >= 240 && PpmCode < 250) // 300~311
    {
        PpmCode += (PpmCode * 25) / 100;
    }
    else if (PpmCode >= 250 && PpmCode < 260) // 300~310
    {
        PpmCode += (PpmCode * 20) / 100;
    }
    else if (PpmCode >= 260 && PpmCode < 270) // 299~309
    {
        PpmCode += (PpmCode * 15) / 100;
    }
    else if (PpmCode >= 270 && PpmCode < 280) // 299~309
    {
        PpmCode += (PpmCode * 11) / 100;
    }
    else if (PpmCode >= 280 && PpmCode < 290) // 299~309
    {
        PpmCode += (PpmCode * 7) / 100;
    }
    else if (PpmCode >= 290 && PpmCode < 300) // 298~307
    {
        PpmCode += (PpmCode * 3) / 100;
    }
    else if (PpmCode >= 300 && PpmCode < 310)
    {
    }
    else if (PpmCode >= 310 && PpmCode < 320) // 301~310
    {
        PpmCode -= (PpmCode * 3) / 100;
    }
    else if (PpmCode >= 320 && PpmCode < 330) // 301~309
    {
        PpmCode -= (PpmCode * 6) / 100;
    }
    else if (PpmCode >= 330 && PpmCode < 340) // 301~309
    {
        PpmCode -= (PpmCode * 9) / 100;
    }
    else if (PpmCode >= 340 && PpmCode < 350) // 300~309
    {
        PpmCode -= (PpmCode * 12) / 100;
    }
    else if (PpmCode >= 350 && PpmCode < 360) // 301~310
    {
        PpmCode -= (PpmCode * 14) / 100;
    }
    else if (PpmCode >= 360 && PpmCode < 370) // 303~312
    {
        PpmCode -= (PpmCode * 16) / 100;
    }
    else if (PpmCode >= 370 && PpmCode < 380) // 300~309
    {
        PpmCode -= (PpmCode * 19) / 100;
    }
    else if (PpmCode >= 380 && PpmCode < 390) // 301~310
    {
        PpmCode -= (PpmCode * 21) / 100;
    }
    else if (PpmCode >= 390 && PpmCode < 400) // 301~310
    {
        PpmCode -= (PpmCode * 23) / 100;
    }
    ret = PpmCode;
    return ret;
}

void TUV_RENGZHENG_SelfCheck(void)
{

    static uint16_t temp_sample = 0;
    static uint16_t first_sample = 0;
    static uint16_t second_sample = 0;
    static uint16_t third_sample = 0;
    uint32_t SlpTimeCnt = SystemGetIdleNUm();

    if (CoObject.EMC_CHECK_Flag)
    {
        if (SlpTimeCnt % CO_CHECK_COUNT == 0) // first
        {
            first_sample = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);
            if (SlpTimeCnt % (CO_SELFCHECK_COUNT) == 0) // FOR SELF CHECK
            {
                if (first_sample >= CO_SAMPLE_FAULT_CORRECT)
                {
                    first_sample -= CO_SAMPLE_FAULT_CORRECT;
                }
            }
            if ((first_sample > (temp_sample + CO_SAMPLE_NEXT_CHANGE_MAX)) || (temp_sample > (first_sample + CO_SAMPLE_NEXT_CHANGE_MAX)))
            {
                // invalid data
            }
            else
            {
                ;
            }
            CoObject.Ppm = first_sample;
        }
        else if (SlpTimeCnt % CO_CHECK_COUNT == 1) // second
        {
            second_sample = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);
            if ((second_sample > (first_sample + CO_SAMPLE_NEXT_CHANGE_MAX)) || (first_sample > (second_sample + CO_SAMPLE_NEXT_CHANGE_MAX)))
            {
                // invalid data
            }
            else
            {
                CoObject.EMC_CHECK_Flag = 0;
            }
            CoObject.Ppm = second_sample;
        }
        else // third
        {
            third_sample = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);
            if ((third_sample > (second_sample + CO_SAMPLE_NEXT_CHANGE_MAX)) || (second_sample > (third_sample + CO_SAMPLE_NEXT_CHANGE_MAX)))
            {
                // invalid data
            }
            else
            {
                CoObject.EMC_CHECK_Flag = 0; // valid data
            }
            CoObject.Ppm = third_sample;
        }
        temp_sample = CoObject.Ppm;
    }

    if (CoObject.Self_Check_Flag)
    {
        if (SlpTimeCnt % CO_CHECK_COUNT == 0)
        {
        }
        else if (SlpTimeCnt % CO_CHECK_COUNT == 1) // second
        {
        }
        else
        {
            CoObject.Self_Check_Flag = 0;
        }
    }

    if (((SlpTimeCnt % CO_CHECK_COUNT) == (CO_CHECK_COUNT - 1)) || (CoObject.Ppm > 0)) // 30s time  check 2???
    {
        if (CoObject.EMC_CHECK_Flag == 0 && CoObject.Self_Check_Flag == 0)
        {
            COAlarmCheck(&CoObject);
        }

        if ((temp_sample > (CoObject.Ppm + CO_SAMPLE_CHANGE_MAX)) || (CoObject.Ppm > (temp_sample + CO_SAMPLE_CHANGE_MAX)))
        {
            CoObject.EMC_CHECK_Flag = 1;
            CoObject.co_sensor_pre_fault = 1;
        }
        else
        {
            CoObject.EMC_CHECK_Flag = 0;
        }

        temp_sample = CoObject.Ppm;

        {
#if defined SUPPORT_LCD_FUNCTION
            LCD_DisplayNum(CoObject.Ppm);
#endif
            UpdateCoPPM();
        }
    }

    if (((SlpTimeCnt % CO_SELFCHECK_COUNT) == (CO_SELFCHECK_COUNT - 1))) // 60s same time check 5???
    {
        CoObject.Self_Check_Flag = 1;
        COSelfCheckHandle(&CoObject);
    }

    if (CoObject.EMC_CHECK_Flag)
    {
        if (SlpTimeCnt % CO_CHECK_COUNT == 0) // first
        {
            first_sample = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);
            if (SlpTimeCnt % (CO_SELFCHECK_COUNT) == 0) // FOR SELF CHECK
            {
                if (first_sample >= CO_SAMPLE_FAULT_CORRECT)
                {
                    first_sample -= CO_SAMPLE_FAULT_CORRECT;
                }
            }

            if ((first_sample > (temp_sample + CO_SAMPLE_NEXT_CHANGE_MAX)) || (temp_sample > (first_sample + CO_SAMPLE_NEXT_CHANGE_MAX)))
            {
                // invalid data
            }
            else
            {
                ;
            }
            CoObject.Ppm = first_sample;
        }
        else if (SlpTimeCnt % CO_CHECK_COUNT == 1) // second
        {
            second_sample = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);
            if ((second_sample > (first_sample + CO_SAMPLE_NEXT_CHANGE_MAX)) || (first_sample > (second_sample + CO_SAMPLE_NEXT_CHANGE_MAX)))
            {
                // invalid data
            }
            else
            {
                CoObject.EMC_CHECK_Flag = 0;
            }
            CoObject.Ppm = second_sample;
        }
        else // third
        {
            third_sample = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);
            if ((third_sample > (second_sample + CO_SAMPLE_NEXT_CHANGE_MAX)) || (second_sample > (third_sample + CO_SAMPLE_NEXT_CHANGE_MAX)))
            {
                // invalid data
            }
            else
            {
                CoObject.EMC_CHECK_Flag = 0; // valid data
            }
            CoObject.Ppm = third_sample;
        }
        temp_sample = CoObject.Ppm;
    }

    if ((SlpTimeCnt % CO_CHECK_COUNT) == (CO_CHECK_COUNT - 1)) // 30s time  check 2???
    {
        if (CoObject.EMC_CHECK_Flag == 0)
        {
            COAlarmCheck(&CoObject);
        }

        if ((temp_sample > (CoObject.Ppm + CO_SAMPLE_CHANGE_MAX)) || (CoObject.Ppm > (temp_sample + CO_SAMPLE_CHANGE_MAX)))
        {
            CoObject.EMC_CHECK_Flag = 1;
        }
        else
        {
            CoObject.EMC_CHECK_Flag = 0;
        }

        temp_sample = CoObject.Ppm;

#if defined SUPPORT_LCD_FUNCTION
        LCD_DisplayNum(CoObject.Ppm);
#endif
    }

    if (((SlpTimeCnt % CO_SELFCHECK_COUNT) == (CO_SELFCHECK_COUNT - 1))) // 60s same time check 5???
    {
        COSelfCheckHandle(&CoObject);
    }
}

#endif

#if defined BSI_CERTIFY_FAIL_PROCESS
static uint16_t COEnvironmentCompensation(uint16_t Ppm)
{
    uint16_t ret = Ppm;
    uint8_t TempValue = SensorGetTemperature();

    if(TempValue < 10)                  //<-10
    {
        ret += (ret * 30) / 100;
    }
    else if(TempValue < 20)             //-10 ~ 0
    {
        ret += (ret * 20) / 100;
    }
    else if(TempValue < 30)             //0 ~ 10
    {
        ret += (ret * 10) / 100;
    }
    else if(TempValue < 55)             //10 ~ 35
    {

    }
    else if(TempValue < 65)             //35 ~ 45
    {
        ret -= (ret * 10) / 100;
    }
    else if(TempValue < 80)             //45 ~ 60
    {
        ret -= (ret * 15) / 100;
    }
    else                                // > 60
    {
        ret -= (ret * 30) / 100;
    }

    debug_uart("\nT:%d Aft Ppm:%d\n", TempValue - 20, Ppm);

    ret = Ppm;
    return ret;
}

#endif

#if defined SUPPORT_CO_TEMP_TEST
void CO_Temperature_Test(uint16_t co_ppm)
{
    uint8_t i = 0;

    static uint8_t CoTempFlag = 0;

#if defined STANDARD_UL2034_UL217
    static uint32_t Ppm70_Count = 0;
    static uint32_t Ppm150_Count = 0;
    static uint32_t Ppm400_Count = 0;
#else
    static uint32_t Ppm30_Count = 0;
    static uint32_t Ppm50_Count = 0;
    static uint32_t Ppm100_Count = 0;
    static uint32_t Ppm300_Count = 0;
#endif
    static uint32_t TempPpm = 0;
    static uint16_t AvrPpm = 0;

    uint8_t temperature_data = SensorGetTemperature();

#if defined STANDARD_UL2034_UL217
    if ((CoTempFlag == 0) && (co_ppm > 300) && (co_ppm < 500)) // 400ppm
    {
        Ppm70_Count = 0;
        Ppm150_Count = 0;
        Ppm400_Count++;
        if ((Ppm400_Count >= 60) && (Ppm400_Count < 90)) // 10min
        {
            TempPpm += co_ppm;
        }
        else if (Ppm400_Count == 90) // 15min
        {
            CoTempFlag = 1;
            AvrPpm = TempPpm / 30;
            if (temperature_data < 30) // < 10C
            {
                i = 6;
            }
            else
            {
                i = 0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x10 + i, AvrPpm >> 8);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x11 + i, AvrPpm & 0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm = 0;
        }
    }
    else if ((CoTempFlag == 0) && (co_ppm > 110) && (co_ppm < 190)) // 150ppm
    {
        Ppm70_Count = 0;
        Ppm400_Count = 0;
        Ppm150_Count++;
        if ((Ppm150_Count >= 60) && (Ppm150_Count < 90)) // 10min
        {
            TempPpm += co_ppm;
        }
        else if (Ppm150_Count == 90) // 15min
        {
            AvrPpm = TempPpm / 30;
            if (temperature_data < 30) // < 10C
            {
                i = 6;
            }
            else
            {
                i = 0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x12 + i, AvrPpm >> 8);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x13 + i, AvrPpm & 0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm = 0;
        }
    }
    else if ((CoTempFlag == 0) && (co_ppm > 50) && (co_ppm < 90)) // 70ppm
    {
        Ppm400_Count = 0;
        Ppm150_Count = 0;
        Ppm70_Count++;
        if ((Ppm70_Count >= 60) && (Ppm70_Count < 90)) // 10min
        {
            TempPpm += co_ppm;
        }
        else if (Ppm70_Count == 90) // 15min
        {
            AvrPpm = TempPpm / 30;
            if (temperature_data < 30) // < 10C
            {
                i = 6;
            }
            else
            {
                i = 0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x14 + i, AvrPpm >> 8);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x15 + i, AvrPpm & 0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm = 0;
        }
    }
    else
    {
        Ppm70_Count = 0;
        Ppm150_Count = 0;
        Ppm400_Count = 0;
        TempPpm = 0;
    }
#elif defined STANDARD_EN50291_EN14604
    if ((CoTempFlag == 0) && (co_ppm > 200) && (co_ppm < 400)) // 300ppm
    {
        Ppm30_Count = 0;
        Ppm50_Count = 0;
        Ppm100_Count = 0;
        Ppm300_Count++;
        if ((Ppm300_Count >= 60) && (Ppm300_Count < 90)) // 10min
        {
            TempPpm += co_ppm;
        }
        else if (Ppm300_Count == 90) // 15min
        {
            CoTempFlag = 1;
            AvrPpm = TempPpm / 30;
            if (temperature_data < 30) // < 10C
            {
                i = 8;
            }
            else
            {
                i = 0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x10 + i, AvrPpm >> 8);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x11 + i, AvrPpm & 0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm = 0;
        }
    }
    else if ((CoTempFlag == 0) && (co_ppm > 70) && (co_ppm < 130)) // 100ppm
    {
        Ppm30_Count = 0;
        Ppm50_Count = 0;
        Ppm300_Count = 0;
        Ppm100_Count++;
        if ((Ppm100_Count >= 60) && (Ppm100_Count < 90)) // 10min
        {
            TempPpm += co_ppm;
        }
        else if (Ppm100_Count == 90) // 15min
        {
            AvrPpm = TempPpm / 30;
            if (temperature_data < 30) // < 10C
            {
                i = 8;
            }
            else
            {
                i = 0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x12 + i, AvrPpm >> 8);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x13 + i, AvrPpm & 0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm = 0;
        }
    }
    else if ((CoTempFlag == 0) && (co_ppm > 40) && (co_ppm < 70)) // 50ppm
    {
        Ppm300_Count = 0;
        Ppm100_Count = 0;
        Ppm30_Count = 0;
        Ppm50_Count++;
        if ((Ppm50_Count >= 60) && (Ppm50_Count < 90)) // 10min
        {
            TempPpm += co_ppm;
        }
        else if (Ppm50_Count == 90) // 15min
        {
            AvrPpm = TempPpm / 30;
            if (temperature_data < 30) // < 10C
            {
                i = 8;
            }
            else
            {
                i = 0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x14 + i, AvrPpm >> 8);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x15 + i, AvrPpm & 0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm = 0;
        }
    }
    else if ((CoTempFlag == 0) && (co_ppm > 20) && (co_ppm < 40)) // 30ppm
    {
        Ppm300_Count = 0;
        Ppm100_Count = 0;
        Ppm50_Count = 0;
        Ppm30_Count++;
        if ((Ppm30_Count >= 60) && (Ppm30_Count < 90)) // 10min
        {
            TempPpm += co_ppm;
        }
        else if (Ppm30_Count == 90) // 15min
        {
            AvrPpm = TempPpm / 30;
            if (temperature_data < 30) // < 10C
            {
                i = 8;
            }
            else
            {
                i = 0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x16 + i, AvrPpm >> 8);
            FLASH_ProgramByte(flash_data_start_physical_address() + 0x17 + i, AvrPpm & 0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm = 0;
        }
    }
    else
    {
        Ppm30_Count = 0;
        Ppm50_Count = 0;
        Ppm100_Count = 0;
        Ppm300_Count = 0;
        TempPpm = 0;
    }
#endif
}
#endif

#if defined STANDARD_UL2034_UL217
static uint16_t COPpmULCheck(COTag *pCO, uint16_t Ppm)
{
    static uint8_t CoCancelAlarmNum = 0;
    uint16_t ret = eCONO; // 0 :normal 1:smoke 2:err

    if (Ppm <= COPPMLEVEL1) // <70ppm
    {
        if (CoCancelAlarmNum >= 3)
        {
            pCO->COAlarmThreshold0 = 0;
            pCO->COAlarmThreshold1 = 0;
            pCO->COAlarmThreshold2 = 0;
            pCO->COAlarmThreshold3 = 0;

            if (pCO->State != eCONO)
            {
                pCO->State = eCONO;
                SystemPostFIFI(eSystemEventCOAlarmCancel);
            }
        }
        else
        {
            CoCancelAlarmNum++;
        }
    }
    else //>=70 ppm
    {
        CoCancelAlarmNum = 0;
        if (Ppm > COPPMLEVEL1 && Ppm <= COPPMLEVEL2) // 70ppm <= x < 150ppm
        {
            goto CO_ALARM_1;
        }
        else if (Ppm > COPPMLEVEL2 && Ppm <= COPPMLEVEL3) // 150ppm <= x <450ppm
        {
            goto CO_ALARM_2;
        }
        else if (Ppm > COPPMLEVEL3 && Ppm <= COPPMLEVEL4) // 450ppm <= x <5000ppm
        {
            goto CO_ALARM_3;
        }
        else // >5000ppm
        {
            goto CO_ALARM_4;
        }

    CO_ALARM_4:
        pCO->COAlarmThreshold3++;
        debug_uart("Co T3 = %d\n", pCO->COAlarmThreshold3);
    CO_ALARM_3:
        pCO->COAlarmThreshold2++;
        debug_uart("Co T2 = %d\n", pCO->COAlarmThreshold2);
    CO_ALARM_2:
        pCO->COAlarmThreshold1++;
        debug_uart("Co T1 = %d\n", pCO->COAlarmThreshold1);
    CO_ALARM_1:
        pCO->COAlarmThreshold0++;
        debug_uart("Co T0 = %d\n", pCO->COAlarmThreshold0);

        if ((pCO->COAlarmThreshold0 >= (CO_ALARM_70PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))) ||
            (pCO->COAlarmThreshold1 >= (CO_ALARM_150PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))) ||
            (pCO->COAlarmThreshold2 >= (CO_ALARM_400PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))) ||
            (pCO->COAlarmThreshold3 >= (CO_ALARM_400PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))))
        {
            if (pCO->State != eCOed)
            {
                SystemPostFIFI(eSystemEventCOAlarm);
                pCO->State = eCOed;
            }
            else
            {
                pCO->COAlarmThreshold0 = 0;
                pCO->COAlarmThreshold1 = 0;
                pCO->COAlarmThreshold2 = 0;
                pCO->COAlarmThreshold3 = 0;
            }
        }
    }

    ret = pCO->State;
    return ret;
}
#else

static uint16_t COPpmBSICheck(COTag *pCO, uint16_t Ppm)
{
    uint16_t ret = eCONO; // 0 :normal 1:smoke 2:err
    static uint8_t CoCancelAlarmNum = 0;

    if (Ppm <= COPPMLEVEL0) // <30ppm
    {
        if (CoCancelAlarmNum >= 3)
        {
            pCO->COAlarmThreshold0 = 0;
            pCO->COAlarmThreshold1 = 0;
            pCO->COAlarmThreshold2 = 0;
            pCO->COAlarmThreshold3 = 0;
            CoCancelAlarmNum = 0;
            if (pCO->State != eCONO)
            {
                pCO->State = eCONO;
                SystemPostFIFI(eSystemEventCOAlarmCancel);
            }
        }
        else
        {
            CoCancelAlarmNum++;
        }
    }
    else //>=30 ppm
    {
        CoCancelAlarmNum = 0;

        if (Ppm > COPPMLEVEL3)
        {
            goto CO_ALARM_4;
        }
        else if (Ppm > COPPMLEVEL2)
        {
            goto CO_ALARM_3;
        }
        else if (Ppm > COPPMLEVEL1)
        {
            goto CO_ALARM_2;
        }
        else if (Ppm > COPPMLEVEL0)
        {
            goto CO_ALARM_1;
        }

    CO_ALARM_4:
        pCO->COAlarmThreshold3++;
        debug_uart("Co T3 = %d\n", pCO->COAlarmThreshold3);
    CO_ALARM_3:
        pCO->COAlarmThreshold2++;
        debug_uart("Co T2 = %d\n", pCO->COAlarmThreshold2);
    CO_ALARM_2:
        pCO->COAlarmThreshold1++;
        debug_uart("Co T1 = %d\n", pCO->COAlarmThreshold1);
    CO_ALARM_1:
        pCO->COAlarmThreshold0++;
        debug_uart("Co T0 = %d\n", pCO->COAlarmThreshold0);

        if ((pCO->COAlarmThreshold0 > (CO_ALARM_30PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))) ||
            (pCO->COAlarmThreshold1 > (CO_ALARM_50PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))) ||
            (pCO->COAlarmThreshold2 > (CO_ALARM_100PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))) ||
            (pCO->COAlarmThreshold3 > (CO_ALARM_300PPM_TIME * 60 / (SLEEP_TIME * CO_CHECK_COUNT))))
        {
            if (pCO->State != eCOed)
            {
                SystemPostFIFI(eSystemEventCOAlarm);
                pCO->State = eCOed;
            }
            else
            {
                pCO->COAlarmThreshold0 = 0;
                pCO->COAlarmThreshold1 = 0;
                pCO->COAlarmThreshold2 = 0;
                pCO->COAlarmThreshold3 = 0;
            }
        }
    }

    ret = pCO->State;
    return ret;
}
#endif

uint16_t COAlarmCheck(void)
{
#ifdef SUPPORT_LCD_FUNCTION
    static uint8_t state = 0;
#endif

    uint16_t ret = eCONO; // 0 :normal 1:smoke 2:err
    uint16_t Ppm = 0;

    Ppm = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);

#if defined BSI_CERTIFY_FAIL_PROCESS
    Ppm = COEnvironmentCompensation(Ppm);
#endif

#if defined TUV_RENGZHENG
    Ppm = Compensationalgorithm(pCO, Ppm);
#endif
    if (Ppm < COPPMLEVEL0) // display from COPPMLEVEL0
    {
        Ppm = 0;
#ifdef SUPPORT_LCD_FUNCTION
        if (state != 0)
        {
            state = 0;
            LCDStandyDisplay(0, false);
        }
#endif
    }
    else
    {
#if defined SUPPORT_LCD_FUNCTION
        debug_uart("display co = %d\n", state);
        if (state >= 2)
        {
            state = 2;
            LCDStandyDisplay(Ppm, false);
        }
        else
        {
            state++;
        }
#endif
    }
    // set ppm value
    CoObject.Ppm = Ppm;

#if !defined TEMPERATURE_HIGH_LOW_TEST
#if defined SUPPORT_CO_TEMP_TEST
    CO_Temperature_Test(Ppm);
#else
#if defined STANDARD_UL2034_UL217
    ret = COPpmULCheck(&CoObject, Ppm);
#else
    ret = COPpmBSICheck(&CoObject, Ppm);
#endif
#endif
#else
#warning CO alarm
#endif

#ifdef SUPPORT_PEAK_FUNCTION
    if (COGetPeakData() < CoObject.Ppm) // peak value >30PPM
    {
        COSetPeakData(CoObject.Ppm);
    }
#endif

    return ret;
}

uint16_t COSelfCheckHandle(COTag *pCO)
{
    uint16_t ret = eCONO; // 0 :normal 1:CO 2:err
    uint16_t COData;
    uint16_t COShortData;

    int16_t Temp;
    uint8_t TempC = SensorGetTemperature();

    /*触发报警后不进行co自检*/
    if (CoObject.State == eCOed)
    {
        return 0;
    }

    COData = COAdcGetSample(CO_NORMAL_CHECK);
    COShortData = COAdcGetSample(CO_SHORT_FAULT_CHECK);
    debug_uart("co sample:%d Fault:%d\n", COData, COShortData);

    Temp = COShortData - COData;
    if ((COShortData > SENSOR_OPEN_FAULT_SAMPLE) && (TempC < 74))
    {
        DelayMs(20);
        COData = COAdcGetSample(CO_NORMAL_CHECK);
        debug_uart("co sample:%d", COData);

        if (COData < SENSOR_OPEN_SAMPLE)
        {
            pCO->COFaultOpenCount++;
            pCO->COFaultShortCount = 0;
            pCO->State = eCOErr;
        }
    }
    else if (((COShortData > SENSOR_SHORT_SAMPLE_MIN) &&
              (COShortData < SENSOR_SHORT_SAMPLE_MAX)) &&
             ((Temp < 100) && (Temp > -100)) &&
             (TempC < 74))
    {
        pCO->COFaultShortCount++;
        pCO->COFaultOpenCount = 0;
        pCO->State = eCOErr;
    }
    else
    {
        if (IsCOErr())
        {
            SystemPostFIFI(eSystemEventCOAlarmCancel);
        }
        pCO->COFaultOpenCount = 0;
        pCO->COFaultShortCount = 0;
        pCO->State = eCONO;
    }

    if (pCO->COFaultOpenCount >= 2)
    {
        if (!IsCOErr())
        {
            SystemPostFIFI(eSystemEventCOErr);
        }
        pCO->COFaultOpenCount = 2;
        pCO->State = eCOOPEN;
    }
    else if (pCO->COFaultShortCount >= 2)
    {
        if (!IsCOErr())
        {
            SystemPostFIFI(eSystemEventCOErr);
        }
        pCO->COFaultShortCount = 2;
        pCO->State = eCOSHORT;
    }

    debug_uart("OpenCnt:%d,ShortCnt:%d,pCO->State:%d\n", pCO->COFaultOpenCount, pCO->COFaultShortCount, pCO->State);

    ret = pCO->State;
    return ret;
}

#if defined STANDARD_UL2034_UL217 && defined BOARD_SC200
#define HIGHCONCENTRATION (500)            // 500ppm
#define HIGHCONCENTRATION_CHECKTIME (5000) // 5s
/**
 * @brief co高浓度时进行定时5s检测
 *
 * @param Handle
 * @param eCmd
 */
static void PollingCoCheck(TmrHdl Handle, eTmrCmd eCmd)
{
    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagCO);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagCO);
        break;

    case eTimeOut:
        COAlarmCheck();
        COSelfCheckHandle(&CoObject);
        if (CoObject.Ppm >= HIGHCONCENTRATION && (CoObject.COFaultShortCount == 0) && (CoObject.COFaultOpenCount == 0))
        {
            LCD_DisplayNum(CoObject.Ppm);
            printf("High Concentration Co to Alarm\n");
            CoObject.State = eCOed;
            SystemPostFIFI(eSystemEventCOAlarm);
            // clear time cnt
            CoObject.COAlarmThreshold0 = 0;
            CoObject.COAlarmThreshold1 = 0;
            CoObject.COAlarmThreshold2 = 0;
            CoObject.COAlarmThreshold3 = 0;
        }
        TmrDeleteTimer(Handle);
        break;

    default:
        break;
    }
}

/**
 * @brief 高浓度报警检测
 *
 * @return unsigned char true触发快速检测 false没触发快速检测
 */
static bool HighConcentrationCheck(void)
{
    if (CoObject.Ppm >= HIGHCONCENTRATION)
    {
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(HIGHCONCENTRATION_CHECKTIME), PollingCoCheck, ((void *)0));
        return true;
    }

    return false;
}
#endif

void COSelfCheck(void)
{
    uint32_t SlpTimeCnt = SystemGetIdleNUm();

    if ((CoObject.COFaultShortCount) || (CoObject.COFaultOpenCount)) // error
    {
        COSelfCheckHandle(&CoObject);
    }
    else // OK
    {
#if defined TUV_RENGZHENG
        TUV_RENGZHENG_SelfCheck();
#endif

        if (((SlpTimeCnt % CO_CHECK_COUNT) == (CO_CHECK_COUNT - 1))) // 30s time  check
        {
            COAlarmCheck();
            UpdateCoPPM();
#if defined STANDARD_UL2034_UL217 && defined BOARD_SC200
            if (HighConcentrationCheck() == true)
                return;
#endif
        }

        if (((SlpTimeCnt % CO_SELFCHECK_COUNT) == (CO_SELFCHECK_COUNT - 1))) // 60s same time check 5???
        {
            COSelfCheckHandle(&CoObject);
        }
    }
}
REG_APP_WAKE_PROC(COSelfCheck);

/**
 * @brief 时间控制
 *
 * @param Handle
 * @param eCmd
 */
static void PollingCOTimeOut(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t State = (uint32_t)TmrGetCtx(Handle);
    static uint16_t i = 0;

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagCoMute);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagCoMute);
        i = 0;
        break;

    case eTimeOut:
        switch (State)
        {
        case eCOAlarmCancel:
            if (i < 5)
            {
                LedPollingHandle(LED_TYPE_GREEN, 500);
                i++;
            }
            else
            {
                SystemPostFIFI(eSystemEventCoAlarmCancelExit);
                TmrDeleteTimer(Handle);
            }
            break;

        case eCOMuteCancel:
            if (i < 3)
            {
                LedPollingHandle(LED_TYPE_GREEN, 500);
                i++;
            }
            else
            {
                SystemPostFIFI(eSystemEventCoMuteExit);
                TmrDeleteTimer(Handle);
            }
            break;

        case eCONetAlarm:
            LedPollingHandle(LED_TYPE_GREEN, 200);
            TmrDeleteTimer(Handle);
            break;

        case eCOMute:
            i++;
            debug_uart("co mute = %d\n", i);
            if (i >= 2)
            {
                i = 0;
                TmrDeleteTimer(Handle);
                SystemPostFIFI(eSystemEventCOMuteTimeout);
            }
            else
            {
                COAlarmCheck();
                if (CoObject.Ppm > COPPMLEVEL1)
                {
                    SystemPostFIFI(eSystemEventCOAlarm);
                }
            }
            break;

        case eCoCailStart:
            COCalibrationWaitStable(Handle);
            break;

        case eCoCailFinsih:
            if (SensorTemperatureCheck())
            {
                static uint32_t COData = 0;
                if (i < CO_CALIBRATION_COUNT)
                {
                    LedPollingHandle(LED_TYPE_YELLOW, 50);
                    COData += COGetData();
                    i++;
                }
                else
                {
                    i = 0;
                    COData = COData >> 4;
                    COCalibrationCheck(&CoCaliObject, (uint16_t)COData);
                    COData = 0;
                    TmrDeleteTimer(Handle);
                }
            }
            else
            {
                log_noinfo("temp err=%d", (int8_t)SensorGetTemperature() - 20);
                CoCaliObject.COCaliResult = eCoCailTempErr;
                SystemPostFIFI(eSystemEvenCalibrationFinish);
                TmrDeleteTimer(Handle);
            }
            break;
        }
        break;
    default:
        break;
    }
}

static void PollingCOBeep(TmrHdl Handle, eTmrCmd eCmd)
{
    uint32_t State = (uint32_t)TmrGetCtx(Handle);
    static uint8_t i = 0;
    static uint8_t AlarmFlg = 0;

    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagCOBeep);
        break;

    case eDelTimer:
        i = 0;
        AlarmFlg = 0;
        SystemClearActiveFlag(eActiveFlagCOBeep);
        break;

    case eTimeOut:
        if ((!AlarmFlg) && ((i++ < CO_ALARM_COUNT)))
        {
            LedPollingHandle(LED_TYPE_RED, 100);
            switch (State)
            {
            case eCOed:
                BeepPollingHandle(100);
                break;
            }
        }
        else
        {
            i = 0;
            AlarmFlg = !AlarmFlg;
            if (AlarmFlg)
            {
                SystemPostFIFI(eSystemEventCOBeepOK);
                TmrSetCurCnt(Handle, TMR_PERIOD(4700));
            }
            else
            {
                TmrSetCurCnt(Handle, TMR_PERIOD(200));
            }
        }
        break;

    default:
        break;
    }
}

#if defined SUPPORT_LCD_FUNCTION
static void CODisplayErrType(void)
{
    LCD_Clear();
    if (CoObject.COFaultOpenCount)
    {
        LCD_DisplayStr(eStr_E03);
    }
    else
    {
        LCD_DisplayStr(eStr_E04);
    }
}

#endif

static void PollingCOCalibrationOK(TmrHdl Handle, eTmrCmd eCmd)
{
    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagCO);
        break;

    case eDelTimer:
        SystemClearActiveFlag(eActiveFlagCO);
        break;

    case eTimeOut:
    {
        // COGetData();
        switch (CoCaliObject.COCaliResult)
        {
        case eCoCailOk:
            LedPollingHandle(LED_TYPE_GREEN, 50);
#if defined SUPPORT_LCD_FUNCTION
            {
                static uint8_t t;
                uint8_t str[] = {LCD_DIS_P, LCD_DIS_NULL, ASCII_NUM_0 + CoCaliObject.COCaliState + 1};

                t = !t;
                if (t)
                    LcdDisplayString(str, 0, 2);
                else
                    LCD_DisplayNum((uint32_t)TmrGetCtx(Handle));
            }
#endif
            break;

        default:
            LedPollingHandle(LED_TYPE_RED, 50);
            break;
        }

#if defined SUPPORT_CO_DEBUG
        printf("setp %d b:%d k:%d\n", CoCaliObject.COCaliState + 1, CoCaliObject.COCaliOffset,
               CoCaliObject.COCaliGradient);
#endif
    }
    break;

    default:
        break;
    }
}

static void PollingCOBeepDelay(TmrHdl Handle, eTmrCmd eCmd)
{
    Handle = Handle;
    switch (eCmd)
    {
    case eInsertTimer:
        SystemSetActiveFlag(eActiveFlagCOBeepDelay);
        break;

    case eTimeOut:
        TmrDeleteTimer(Handle);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOBeep, (void *)eCOed);
        break;

    case eDelTimer:
        // to be done
        SystemClearActiveFlag(eActiveFlagCOBeepDelay);
        break;
    default:
        break;
    }
}

void COKeyTestDelay(uint8_t Flag) // flag =1  start delay other is beepdely
{
    uint16_t COState = eCONO;
    switch (Flag)
    {
    case 0:
        COState = COSelfCheckHandle(&CoObject);
        if (IsCOErr() || (eCOErr == COState))
        {
            SystemPostFIFI(eSystemEventCOErr);
            CoObject.State = eCOOPEN;
        }
        else
        {
            // COLcdDisplay();
#ifdef SUPPORT_SMOKE_FUNCTION
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOBeep, (void *)eCOed);
#if defined SUPPORT_LCD_FUNCTION
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_TEST_LCD_TIME), PollingLineDisplay, NULL);
#endif
#else
            TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingCOBeepDelay, (void *)eCOed);
#endif
        }
        break;

    case 1:
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeep));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeepDelay));
#if defined SUPPORT_LCD_FUNCTION
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingLineDisplay));
#endif
#ifdef SUPPORT_SMOKE_FUNCTION
        SystemPostFIFI(eSystemEventCOTestOver);
#else
        SystemPostFIFI(eSystemEventTestCoOverWait);
#endif
        debug_uart("co test over\n");
        break;
    }
}
/******************************************************************************
**Description:
** Parameters:
**     Return:
*******************************************************************************/
void SystemCOAlarm(SystemTag *me, QEvent *e)
{
    static uint8_t update_co_count;

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        update_co_count = 0;
        printf("%s,in\n", __FUNCTION__);
        CoObject.State = eCOed;
        CoAlarm();
#if defined SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_ON();
#endif
        SystemPostFIFI(Q_AppRfCoAlarm);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOBeep, (void *)eCOed);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        CoObject.State = eCONO;
#if defined SUPPORT_LCD_FUNCTION
        LCD_BACK_LIGHT_OFF();
#endif
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeep));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOTimeOut));
        break;

    case eSystemEventCOBeepOK:
#ifdef SUPPORT_SMOKE_FUNCTION
        SmokeCheckEvt();
#endif
        if (++update_co_count > 5 || CoObject.Ppm == 0)
        {
            update_co_count = 0;
            COAlarmCheck();
            UpdateCoPPM();
        }
        break;

    case eSystemEventKey:
    case eSystemEventNetMute:
#if defined STANDARD_EN50291_EN14604
        if (COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient) < COPPMLEVEL3)
        {
            #if defined EnterMute || defined MuteSuccess
                (e->sig == eSystemEventKey) ? (EnterMute()) : (MuteSuccess());
            #endif
            SystemPostFIFI(Q_AppCancelRfCoAlarm);
            Q_TRAN(SystemCOMute);
        }
        else
        {
            #if defined MuteFail
            if (e->sig == eSystemEventNetMute)
                MuteFail();
            #endif
        }
#elif defined STANDARD_UL2034_UL217
        #if defined EnterMute || defined MuteSuccess
            (e->sig == eSystemEventKey) ? (EnterMute()) : (MuteSuccess());
        #endif
        SystemPostFIFI(Q_AppCancelRfCoAlarm);
        Q_TRAN(SystemCOMute);
#endif
        break;

    case eSystemEventCOAlarmCancel:
        SystemPostFIFI(Q_AppCancelRfCoAlarm);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeep));
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_EXIT_TIME), PollingCOTimeOut, (void *)eCOAlarmCancel);
        break;

    case eSystemEventCoAlarmCancelExit:
        CancelAlarm();
        Q_TRAN(SystemIdle);
        break;

    case eSystemEventCOErr:
        Q_TRAN(SystemCOError);
        break;

    case eSystemEventSmokeAlarm:
#if defined SUPPORT_SMOKE_FUNCTION
        Q_TRAN(SystemSmokeAlarm);
#endif
        break;

    default:
        break;
    }
}

static void SystemCOMute(SystemTag *me, QEvent *e)
{
    static uint8_t update_co_count;

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        update_co_count = 0;
        printf("%s,in\n", __FUNCTION__);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_MUTE_TIME), PollingCOTimeOut, (void *)eCOMute);
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOBeep, (void *)eCOMute);
        break;

    case Q_EXIT_SIG:
        printf("%s,out\n", __FUNCTION__);
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOTimeOut));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeep));
        break;

#ifdef SUPPORT_NET_FUNCTION
    case eNETEventCancleMute:
        SystemPostFIFI(eSystemEventLcdUpdate);
        Q_TRAN(SystemIdle);
        break;
#endif

    case eSystemEventCOBeepOK:
#ifdef SUPPORT_SMOKE_FUNCTION
        SmokeCheckEvt();
#endif
        if (++update_co_count > 5)
        {
            update_co_count = 0;
            CoObject.Ppm = COGetPPM(CoObject.COCaliOffset, CoObject.COCaliGradient);
            if (CoObject.Ppm < COPPMLEVEL0)
            {
                CoObject.Ppm = 0;
            }
#if defined SUPPORT_LCD_FUNCTION
            LCD_DisplayNum(CoObject.Ppm);
#endif

#ifdef SUPPORT_PEAK_FUNCTION
            if (COGetPeakData() < CoObject.Ppm) // peak value >30PPM
            {
                COSetPeakData(CoObject.Ppm);
            }
#endif
            UpdateCoPPM();
        }
        break;

    case eSystemEventCOMuteTimeout:
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeep));
        TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOTimeOut));
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_EXIT_TIME), PollingCOTimeOut, (void *)eCOMuteCancel);
        break;

    case eSystemEventCoMuteExit:
        CancelMute();
        Q_TRAN(SystemIdle);
        break;

    case eSystemEventCOAlarm:
        Q_TRAN(SystemCOAlarm);
        break;

    case eSystemEventCOErr:
        Q_TRAN(SystemIdle);
        break;

#if defined SUPPORT_RADIO_FUNCTION
    case Q_eIntranetEventSmokeAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            NetSetSmokeAlarmParm();
            Q_TRAN(AppNetAlarmFunc);
        }
        break;

    case Q_eIntranetEventCoAlarm:
        if (GetRadioPairMode() != RF_PAIR_MODE_NONE)
        {
            NetSetCoAlarmParm();
            Q_TRAN(AppNetAlarmFunc);
        }
        break;
#endif

    default:

        break;
    }
}

/**
 * @brief Co 故障处理函数
 *
 * @param me
 * @param e
 */
void SystemCOError(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case eSystemEventWakeup:
        SECTION_CALL(APP_WAKE_PROC);
        SysTimeUpdate();
        SystemPollingIdle();
        SystemLedFlicker(SysLedErr);
        break;

    case Q_ENTRY_SIG:
        CoFault();
#if defined SUPPORT_LCD_FUNCTION
        LCD_Control(eLcdON);
        SetLcdSwitchFlag(true);
        CODisplayErrType();
#endif
        ResetLedTick();
        EndLifeIndex(1, LED_TYPE_YELLOW, 1, 2, 50);
        break;

    case Q_EXIT_SIG:
        CancelFault();
        break;

    case eSystemEventCOAlarmCancel:
        Q_TRAN(SystemIdle);
        break;

#if defined SUPPORT_SMOKE_FUNCTION
    case eSystemEventSmokeAlarm:
        Q_TRAN(SystemSmokeAlarm);
        break;
#endif

    case eSystemEventKey:
        EndLifeIndex(1, LED_TYPE_YELLOW, 1, 2, 50);
        break;

    default:
        break;
    }
}

/*
***********************************************************************************************************************
*  功能: co校机结果处理
*
*  描述:
*
*  返回:
*
***********************************************************************************************************************
*/
void CoCailOKHandle(void)
{
    uint16_t TempPPM = 0;
    uint8_t co_state = CoCaliObject.COCaliState;

    switch (co_state)
    {
    case 0:
        FlashSetData(COCaliOffset, CoCaliObject.Sample);
        TempPPM = CoCaliObject.Sample;
        break;

    case 1:
        TempPPM = CoCaliObject.COCaliOffset > CoCaliObject.Sample
                      ? CoCaliObject.COCaliOffset - CoCaliObject.Sample
                      : CoCaliObject.Sample - CoCaliObject.COCaliOffset;

        CoCaliObject.COCaliGradient = TempPPM / 15;
        FlashSetData(COCaliGradient, CoCaliObject.COCaliGradient);

        TempPPM = CoCaliObject.COCaliGradient;
        break;

    case 2:
        co_state = CO_CALI_STATE - 1;
        TempPPM = COGetPPM(CoCaliObject.COCaliOffset, CoCaliObject.COCaliGradient);
        break;
    }

    co_state++;
    FlashSetData(COCariFlag, co_state);
    TmrSetCtx(TmrGetHandleByEvent(GetTmrHd(), PollingCOCalibrationOK), (void *)TempPPM);
}

/*
***********************************************************************************************************************
*  功能: co校机结果处理
*
*  描述:
*
*  返回:
*
***********************************************************************************************************************
*/
extern void hal_rtc_write(unsigned short value);
void CoCailResHandle(void)
{
    printf("Co CailRes = %d\n", CoCaliObject.COCaliResult);
    hal_rtc_write(2);
    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(2000), PollingCOCalibrationOK, (void *)CoCaliObject.COCaliResult);

    switch (CoCaliObject.COCaliResult)
    {
    case eCoCailOk:
        CoCailOKHandle();
        break;

#ifdef SUPPORT_LCD_FUNCTION
    case eCoCailTempErr:
        LCD_DisplayStr(eStr_E06); // temp err
        break;

    case eCoCailBatErr:
        LCD_DisplayStr(eStr_E11); // bat err
        break;

    case eCoCailFail:
        LCD_Clear();
        switch (CoCaliObject.COCaliState)
        {
        case 0:
            LCD_DisplayStr(eStr_E08);
            break;

        case 1:
            LCD_DisplayStr(eStr_E09);
            break;

        case 2:
            LCD_DisplayStr(eStr_E10);
            break;
        }
        break;
#endif
    }
}

static void COCalibrationCheckStart(COCaliTag *pcali)
{
    CoCalibrationValueInit(pcali);
    log_noinfo("co calibration setp %d start", pcali->COCaliState + 1);

#if defined SUPPORT_LCD_FUNCTION
    {
        uint8_t str[] = {LCD_DIS_L, LCD_DIS_NULL, ASCII_NUM_0 + pcali->COCaliState + 1};
        LcdDisplayString(str, 0, 2);
    }
#endif
    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(6000), PollingCOTimeOut, (void *)eCoCailStart);
}

void COFactoryMode(SystemTag *me, QEvent *e)
{
    switch (e->sig)
    {
    case eSystemEventWakeup:
        SysTimeUpdate();
        SystemPollingIdle();
        break;

    case Q_ENTRY_SIG:
        FlashGetData(COCariFlag, CoCaliObject.COCaliState);
        FlashGetData(COCaliGradient, CoCaliObject.COCaliGradient);
        FlashGetData(COCaliOffset, CoCaliObject.COCaliOffset);

        if (CoCaliObject.COCaliState == CO_CALI_STATE)
        // if(1)
        {
            CoObject.COCaliGradient = CoCaliObject.COCaliGradient;
            CoObject.COCaliOffset = CoCaliObject.COCaliOffset;
            SystemPostFIFI(eSystemEvenCalibrationSmoke);
            printf("\nCoCailData:\n");
            printf("k = %d,b = %d\n", CoObject.COCaliGradient, CoObject.COCaliOffset);
        }
        else
        {
            SystemPostFIFI(eSystemEvenCalibrationCO);
        }

        break;
    case Q_EXIT_SIG:
        break;

    case eSystemEvenCalibrationCO:
        if (BatteryGetData() > BATTERY_FACTORY_CHECK)
        {
            COCalibrationCheckStart(&CoCaliObject);
        }
        else
        {
            log_noinfo("bat low. v=%d", getBatteryVol());
            CoCaliObject.COCaliResult = eCoCailBatErr;
            SystemPostFIFI(eSystemEvenCalibrationFinish);
        }
        break;

    case eSystemEvenCalibrationFinish:
        CoCailResHandle();
        break;

    case eSystemEvenCalibrationSmoke:
        Q_TRAN(SystemStartHandle);
        break;

    default:
#if defined SUPPORT_CO_DEBUG
        debug("%s ,%d,%d\n", __FUNCTION__, __LINE__, e->sig);
#endif
        break;
    }
}
