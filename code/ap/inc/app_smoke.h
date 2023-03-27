#ifndef __APP_SMOKE_H__
#define __APP_SMOKE_H__

#include "config.h"
#ifdef SUPPORT_SMOKE_FUNCTION

/************************************************** include **********************************************************/
#include "inc/smoke.h"
#include "app_main.h"

/************************************************** define ***********************************************************/

/* 
    日志等级
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 3


#define EMC_DARK_BUFFER                             (70)      // for EMC buffer area
#define BATTERY_CHECK_MIN				                (3000)					
#define BATTERY_CHECK_MAX				                (3300)					


#define SMOKE_CALIBRATION_DUST_INCREASE             (1)
#define SMOKE_CALI_STATE                            (0xa5)

//Smoke Check State
#define SMOKE_Check_Cnt                             (3)
#define SMOKE_Cancel_Cnt                            (6)

#define SMOKE_CALIBRATION_SUCESS_MIN                (326) 
#define SMOKE_CALIBRATION_SUCESS_MAX                (2000) 
#define SMOKE_DARK_AVERAGE					        (800)				    			//      1.2V
#define SMOKE_LIGHT_AVERAGE					        (2000)				    			//      2.0V
#define SMOKE_LIGHT_DARK_AVERAGE			        (35)                 					//      0.325V
#define SMOKE_ERR_VALUE        	       		        ((SMOKE_CALIBRATION_SUCESS_MIN>>1))       //

#define SMOKE_EnvironmentCompensation_COUNT          (720)                     // 7200s/SLEEP_TIME=720
#define SMOKE_SENSOR_BOUNDRY_OF_SAFTY                (100) 
#define SYSTEM_LDO_VOL                               (2500) 


//time
#define SMOKE_CALIBRATION_LCD_TIME                  (650)//MS
#define SMOKE_CALIBRATION_ERR_TIME                  (10)
#define SMOKE_CALIBRATION_TIME                      (2000)//MS

#define SMOKE_BEEP_TIME                             (1000)//MS
#define SMOKE_MUTE_TIME                             (5000)//MS
#define SMOKE_MUTE_TIME_NUM                         (108)//5S*108


/************************************************** typedef ***********************************************************/
typedef struct Smoke{

	uint8_t State;
	uint8_t SmokeDetCnt;//smoked num
	uint8_t SmokeInactCnt;//normal num
	uint8_t SmokeTransFaultCnt;// open or short num
	uint8_t SmokeDustFlag;// for dust compensation
	uint16_t SmokingCnt;//smoking num
	uint16_t SmokeSensortive;//smoke threshold
    uint16_t SmokeCailBattery;
	uint16_t AlarmLevel;
	SmokeDataTag SmokeData;
}SmokeTag;



typedef enum{
	eSmokeNO = 0,
	eSmoked,
	eSmoking, //not silence
	eSmokeMute, 
	eSmokeMuteTimeOut,
	eSmokeErr,
	eSmokeCancel,
	eSmokeCheck ,
	eSmokeTest,
	eSmokeTestFinish,
	eSmokeNetAlarm,

    eSmokeCail ,
	eSmokeCailFail = 0x60,
    eSmokeCailTempErr ,
    eSmokeCailBatErr , 
    eSmokeCailOk = 0xA5,
    eSmokeCailEnumMax ,
}eSmokeState;



/************************************************** interface ***********************************************************/
SmokeTag * SmokeInit(void);
void SystemSmokeAlarm(SystemTag  *me,QEvent *e);
uint16_t SmokeCheckEvt(void);
void SystemSmokeError(SystemTag  *me,QEvent *e);
void SmokeKeyTestHandle(uint8_t handle);
void SmokeFactoryMode(SystemTag  *me,QEvent *e);
#endif /* end of SUPPORT_SMOKE_FUNCTION */
#endif /* end of File */
