/****************************************************************************/
/*	File   :                        Key.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:13:50                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "inc/Key.h"
#include "common.h"
#include "bsp_config.h"
#include "inc/KeyAnalysis.h"

#define SetSysSleepFlag(s) SystemSetActiveFlag(s)
#define ResetSysSleepFlag(s) SystemClearActiveFlag(s)


#define KEYCOUNTERNUMLONG (200) // 2000MS
#define KEYCOUNTERNUMSHORT (5)  // 50MS
#define KEYCOUNTERREPEAT (300)  // 2S

#define KEY_CHECK_TIME (10) // 200MS

typedef enum
{
    RESET = 0,
    SET = !RESET
} FlagStatus;

struct Key
{

    FlagStatus keyClickFlag; // key press flag
    uint8_t CurrentState;    // 0：short press 1：long press 2:double press   3:three 4: four
    uint8_t KeyValue;        // key value
    uint16_t keyCounter;     // press counter every 10ms
    uint8_t KeyClickState;   // Key Click State flag
};

static KeyTag KeyObject = {.keyClickFlag = RESET, .KeyValue = eKeyNameMax, .CurrentState = 0, .KeyClickState = 0};
static KeyTag *pKeyObject = NULL;

static eKeyName GetKey(void)
{
    eKeyName Key = eKeyNameMax;

    if (hal_gpio_read(PIN_TEST) == PIN_RESET)
    {
        Key = eKeyTest;
    }
#ifdef SUPPORT_RADIO_FUNCTION
    else  if (hal_gpio_read(PIN_RADIOTEST) == PIN_RESET)
    {
        Key = eKeyRadioTest;
    }
#endif
#ifdef PAIR_KEY
    else  if (hal_gpio_read(PIN_PAIR) == PIN_RESET)
    {
        Key = eKeyPair;
    }
#endif
    return (Key);
}

/**
 * @brief  KEY inital
 * @param  None
 * @retval None
 */
KeyTag *KeyInit(void)
{
    if (NULL == pKeyObject)
    {
        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(KEY_CHECK_TIME), PollingKey, (void *)GetKey);
        pKeyObject = &KeyObject;
    }

    return pKeyObject;
}

void KeyCheck(void)
{
    if(KeyObject.keyClickFlag == RESET)
    {
        if( (hal_gpio_read(PIN_TEST) == PIN_RESET) )
        {
            KeyObject.keyClickFlag = SET;     
            SetSysSleepFlag(eActiveFlagKey);
        }
#ifdef SUPPORT_RADIO_FUNCTION
        else if( (hal_gpio_read(PIN_RADIOTEST) == PIN_RESET) )
        {
            KeyObject.keyClickFlag = SET;     
            SetSysSleepFlag(eActiveFlagKey);
        }
#endif 
#ifdef PAIR_KEY    
        else if( (hal_gpio_read(PIN_PAIR) == PIN_RESET) )
        {
            KeyObject.keyClickFlag = SET;
            SetSysSleepFlag(eActiveFlagKey);
        }
#endif    
    }  
}

static void KeyExitScan(void)
{
    KeyObject.keyClickFlag = RESET;
    ResetSysSleepFlag(eActiveFlagKey);
}
/**********************************************************************/
// Description:		KeyDoubleCheck for polling double click
// Parameters:       eHandle   for key value
// Return:
// Date:
// author:		    song
/**********************************************************************/

static void KeyDoubleCheck(TmrHdl Handle, eTmrCmd eCmd)
{
    uint8_t KeyValue;
    KeyTag *pstKey = &KeyObject;
    ;
    KeyValue = (uint32_t)TmrGetCtx(Handle) & 0xff;
    switch (eCmd)
    {
    case eTimeOut:
        switch ((pstKey->KeyClickState))
        {
        case 1:
            pstKey->CurrentState = TYPE_SHORT_CLICK;
            pstKey->KeyValue = KeyValue;
            break;

        case 2:
            pstKey->CurrentState = TYPE_DOUBLE_CLICK;
            pstKey->KeyValue = KeyValue;
            break;

        case 3:
            pstKey->CurrentState = TYPE_THREE_CLICK;
            pstKey->KeyValue = KeyValue;
            break;

        case 4:
            pstKey->CurrentState = TYPE_FOUR_CLICK;
            pstKey->KeyValue = KeyValue;
            break;
        default:
            pstKey->KeyValue = eKeyNameMax;
            break;
        }
        pstKey->KeyClickState = 0;

        TmrDeleteTimer(Handle);
        ResetSysSleepFlag(eActiveFlagKeyDouble);
        break;

    default:
        break;
    }
}

static void KeyScanValue(void)
{
    KeyInfo Key = GetAndCleanSysKeyInfo();
    KeyTag *pstKey = &KeyObject;
    if (pstKey->keyClickFlag)
    {
        pstKey->keyCounter = Key.Counter;

        if (Key.Counter >= KEYCOUNTERNUMLONG)
        {
            if ((Key.Counter - KEYCOUNTERNUMLONG) % KEYCOUNTERREPEAT == 0)
            {
                pstKey->KeyValue = Key.KeyName;
            }
            else
            {
                pstKey->KeyValue = eKeyNameMax;
            }
            pstKey->CurrentState = TYPE_LONG_CLICK;
        }
        else
        {
            if ((Key.Counter >= KEYCOUNTERNUMSHORT) &&
                (Key.KeyStat == eKeyStatRelease) &&
                (Key.Counter < KEYCOUNTERNUMLONG))
            {
                pstKey->KeyValue = Key.KeyName;
                pstKey->KeyClickState++;

                if (NULL == TmrGetHandleByEvent(GetTmrHd(), KeyDoubleCheck))
                {
                    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), KeyDoubleCheck, (void *)(pstKey->KeyValue));
                    SetSysSleepFlag(eActiveFlagKeyDouble);
                }
                else
                {
                    TmrResetCurCnt(TmrGetHandleByEvent(GetTmrHd(), KeyDoubleCheck));
                }
                pstKey->KeyValue = eKeyNameMax;
            }
            else
            {
                pstKey->KeyValue = eKeyNameMax;
            }
        }

        if ((Key.KeyStat == eKeyStatRelease))
        {
            KeyExitScan(); // next track
        }
    }
}

void KeyProcess(void)
{
    KeyTag *pstKey = &KeyObject;
    static uint8_t CurrentStatelast = 0; // for key long press release  0:no long press 1:long press

    KeyCheck();
    KeyScanValue();

    if (pstKey->KeyValue != eKeyNameMax)
    {
        debug("key:%d, clip=%d\n", pstKey->KeyValue, pstKey->CurrentState);
        switch (pstKey->KeyValue)
        {           
            case eKeyTest:
            {               
                switch ((pstKey->CurrentState))
                {
                case TYPE_LONG_CLICK: 
                    if (CurrentStatelast != 1)
                    {
                        CurrentStatelast = 1;
                        SystemPostFIFI(eSystemEventLongPressKey);
                    }
                    break;

                case TYPE_SHORT_CLICK: 
                    SystemPostFIFI(eSystemEventKey);
                    break;

                case TYPE_DOUBLE_CLICK:
                    SystemPostFIFI(eSystemEventDoubleKey);
                    break;

                case TYPE_FOUR_CLICK: 
                    SystemPostFIFI(eSystemEventFourKey);
                    break;

                default:
                    break;
                }
            }
            break;

            case eKeyRadioTest:
            {   
                switch ((pstKey->CurrentState)) 
                {
                case TYPE_SHORT_CLICK: 
                    SystemPostFIFI(eSystemEventRadioKey);
                    break;      
                }                                           
            }
            break;
#ifdef PAIR_KEY
            case eKeyPair:
            {
                switch ((pstKey->CurrentState))
                {
                    case TYPE_LONG_CLICK:						
                        if(CurrentStatelast!=1)
                        {                           
                            CurrentStatelast = 1;
                            SystemPostFIFI(eSystemEventPairLongPressKey);
                        }
                        break;

                    case TYPE_SHORT_CLICK:                      
                        SystemPostFIFI(eSystemEventPairClik);
                        break;

                     case TYPE_FOUR_CLICK:                      
                        SystemPostFIFI(eSystemEventPairFourKey);
                        break;

                    default:
                    break;
                }

            }
            break;
#endif
            default:
                break;
            }
        // printf("process KeyValue : %d\n",pstKey->KeyValue);
        KeyObject.KeyValue = eKeyNameMax;
    }
    else
    {
        if ((pstKey->keyClickFlag == RESET) && (CurrentStatelast == 1)) // long press release
        {
            CurrentStatelast = 0;
            SystemPostFIFI(eSystemEventReleaseKey);
        }
       
    }
}
