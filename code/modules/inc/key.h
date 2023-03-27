/****************************************************************************/
/*	File   :             Key.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:13:12                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

/* 
    日志等级
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 2


typedef enum{
	TYPE_LONG_CLICK = 0x00,
	TYPE_SHORT_CLICK,
	TYPE_DOUBLE_CLICK,
	TYPE_THREE_CLICK,
	TYPE_FOUR_CLICK,
	
}KEYType;

typedef struct Key KeyTag;

KeyTag * KeyInit(void);
void KeyCheck(void);
void KeyProcess(void);
#endif

