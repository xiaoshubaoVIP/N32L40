/****************************************************************************/
/*	File   :             Beep.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:15:10                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __BEEP_H__
#define __BEEP_H__

#include <stdint.h>

typedef enum{
  	eBeepOn,
  	eBeepOff,
}eBeepState;

void BeepSetState(eBeepState State);
void BeepPollingHandle(uint32_t Time);
#endif

