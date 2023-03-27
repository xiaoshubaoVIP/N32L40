#ifndef __SMOKE_H__
#define __SMOKE_H__


/************************************************** include **********************************************************/
#include <stdint.h>
#include "config.h"



#ifdef SUPPORT_SMOKE_FUNCTION
/************************************************** define **********************************************************/
/* 
    日志等级
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 2



#define IR_ALL_ON                                   ((uint8_t)0)
#define IR_ALL_OFF                                  ((uint8_t)1)
#define IRCTR_ON_TEST_OFF                           ((uint8_t)2)
#define IR_TEST_ON_IR_OFF                           ((uint8_t)3)







/************************************************** Typedef **********************************************************/

typedef struct _SmokeTag{
	uint16_t Dark;
	uint16_t Light;
	uint16_t Diff;
}SmokeDataTag;




/************************************************** Interface **********************************************************/
SmokeDataTag SmokeGetLdData(uint8_t State);




#endif /* end of SUPPORT_SMOKE_FUNCTION */

#endif /* end of file */
