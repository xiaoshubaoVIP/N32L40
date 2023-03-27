#ifndef __RADIO_API_H__
#define __RADIO_API_H__

#include "radio/inc/radio_protocol.h"
#include "app_rf.h"

/* 发送Co报警 */
#define CoAlarm()

/* 发送烟雾报警 */
#define SmokeAlarm()

/* 发送取消报警 */
#define CancelAlarm()   do{IntranetSend(NormalIntranetMode, PackNum_8, eIntranetRadioMsgCancleAlarm);}while(0)

/* 广播取消报警 */
#define BroadcastSendCancelAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleAlarm);\
                            }while(0)

/* 转发smoke报警 */
#define ForwardSmokeAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgSmokeAlarm);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/* 转发co报警 */
#define ForwardCoAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgCoAlarm);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/* 转发取消报警 */
#define ForwardCancelAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleAlarm);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/* 广播测试下收到取消测试-发送取消测试 */
#define SendCancelAlarmSync()  do{ IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgCancleAlarm);\
                            }while(0)

/* 开始配对 */
#define SendStartPair() do{IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgPairNewRequest);}while(0)

/* 发送配对应答 */
#define SendPairACK()   do{IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgPairNewPairAck);}while(0)

/* 发送工程宝配对应答 */
#define SendFactoryPairACK() do{IntranetSend(FactoryPairMode, PackNum_1, eIntranetRadioMsgFactoryPairAck);}while(0)

/* 结束配对 */
#define SendStopPair()
/* 发送产测 */
#define SendFactoryPair() do{IntranetSend(FactoryPairMode, PackNum_1, eIntranetRadioMsgFactoryPairRequest);}while(0)
/* 发送自检结果 */
#define SendSelfCheck(res)

/* rf联动测试 */
#define SendStartTest() do{ IntranetSend(NormalIntranetMode, PackNum_32, eIntranetRadioMsgMasterTest);\
                            IntranetSend(NormalIntranetSpecialMode, PackNum_2, eIntranetRadioMsgMasterTest);\
                            IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgMasterTest);\
                            }while(0)

/* rf转发-联动测试 */
#define SendForwardTest() do{ IntranetSend(NormalIntranetMode, PackNum_3, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgForwardTest);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgForwardTest);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/*进入快速取消命令*/
#define SendQuicklyCancelCmd() do{IntranetSend(NormalIntranetMode, PackNum_32, eIntranetRadioMsgCancleMsgSync);}while(0)

/* rf取消联动测试 */
#define SendCancelTest()  do{ IntranetSend(NormalIntranetMode, PackNum_32, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_16, eIntranetRadioMsgCancleTest);\
                            }while(0)

/* 广播测试下收到取消测试-发送取消测试 */
#define SendCancelSyncTest()  do{ IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgCancleTest);\
                            }while(0)

/* 清除配对信息 */
#define SendPairReset()

#define protocol_handle() RadioService()

#define RadioIdle() RadioIdleEnter()

/**
 * @brief Set the Radio Mode
 * 
 * @param mode 
 */
void SetRadioMode(unsigned char mode);
#endif 
