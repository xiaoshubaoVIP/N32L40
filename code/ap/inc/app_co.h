#ifndef __APP_CO_H__
#define __APP_CO_H__

#include "common.h"
/************************************************** typedef *************************************************************/

typedef enum{
	eCoCalibratioinStepOne = 0x01,
	eCoCalibratioinStepTwo = (0x01<<1),
	eCoCalibratioinStepThree = (0x01<<2),
}eCoCalibratioinStep;

typedef enum{
	eCONO = 0,
	eCOed,// co alarm
	eCOMute,// co alarm
	eCOErr,
	eCOOPEN,
	eCOSHORT,
	eCONetAlarm,
	eCOAlarmCancel,
	eCOMuteCancel,

    //for cail
    eCoCailStart,
    eCoCailFinsih,
    eCoCailFail,
    eCoCailTempErr ,
    eCoCailBatErr , 
    eCoCailOk = 0xA5,
    eCoCailEnumMax ,
}eCOState;



/************************************************** interface ***********************************************************/
void COInit(void);
uint16_t COBatteryGetData(void);
void COFactoryMode(SystemTag  *me,QEvent *e);
void SystemCOAlarm(SystemTag  *me,QEvent *e);
void SystemCOMute(SystemTag  *me,QEvent *e);
void COKeyTest(void);
void COKeyTestDelay(uint8_t Flag);
void COSelfCheck(void);
uint16_t COAlarmCheck(void);
BOOL IsCOErr(void);
BOOL IsCOPPM(void);
void SystemCOError(SystemTag  *me,QEvent *e);
void COSetPeakData(uint16_t PpmPeakValue);
uint16_t COGetPeakData(void);
uint16_t getCoAppPpm(void);
uint32_t GetCOCaliOffset(void);
uint32_t GetCOCaliGradient(void);
#endif
