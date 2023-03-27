#ifndef __RADIO_API_H__
#define __RADIO_API_H__

#include "radio/inc/radio_protocol.h"
#include "app_rf.h"

/* ����Co���� */
#define CoAlarm()

/* ���������� */
#define SmokeAlarm()

/* ����ȡ������ */
#define CancelAlarm()   do{IntranetSend(NormalIntranetMode, PackNum_8, eIntranetRadioMsgCancleAlarm);}while(0)

/* �㲥ȡ������ */
#define BroadcastSendCancelAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleAlarm);\
                            }while(0)

/* ת��smoke���� */
#define ForwardSmokeAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgSmokeAlarm);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/* ת��co���� */
#define ForwardCoAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgCoAlarm);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/* ת��ȡ������ */
#define ForwardCancelAlarm()  do{ IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleAlarm);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/* �㲥�������յ�ȡ������-����ȡ������ */
#define SendCancelAlarmSync()  do{ IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgCancleAlarm);\
                            }while(0)

/* ��ʼ��� */
#define SendStartPair() do{IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgPairNewRequest);}while(0)

/* �������Ӧ�� */
#define SendPairACK()   do{IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgPairNewPairAck);}while(0)

/* ���͹��̱����Ӧ�� */
#define SendFactoryPairACK() do{IntranetSend(FactoryPairMode, PackNum_1, eIntranetRadioMsgFactoryPairAck);}while(0)

/* ������� */
#define SendStopPair()
/* ���Ͳ��� */
#define SendFactoryPair() do{IntranetSend(FactoryPairMode, PackNum_1, eIntranetRadioMsgFactoryPairRequest);}while(0)
/* �����Լ��� */
#define SendSelfCheck(res)

/* rf�������� */
#define SendStartTest() do{ IntranetSend(NormalIntranetMode, PackNum_32, eIntranetRadioMsgMasterTest);\
                            IntranetSend(NormalIntranetSpecialMode, PackNum_2, eIntranetRadioMsgMasterTest);\
                            IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgSync);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgMasterTest);\
                            }while(0)

/* rfת��-�������� */
#define SendForwardTest() do{ IntranetSend(NormalIntranetMode, PackNum_3, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_1, eIntranetRadioMsgForwardTest);\
                            IntranetSend(LowPowerIntranetMode, PackNum_1, eIntranetRadioMsgForwardTest);\
                            IntranetSend(NormalIntranetMode, PackNum_2, eIntranetRadioMsgCancleMsgSync);\
                            }while(0)

/*�������ȡ������*/
#define SendQuicklyCancelCmd() do{IntranetSend(NormalIntranetMode, PackNum_32, eIntranetRadioMsgCancleMsgSync);}while(0)

/* rfȡ���������� */
#define SendCancelTest()  do{ IntranetSend(NormalIntranetMode, PackNum_32, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_16, eIntranetRadioMsgCancleTest);\
                            }while(0)

/* �㲥�������յ�ȡ������-����ȡ������ */
#define SendCancelSyncTest()  do{ IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgSync);\
                            IntranetSend(NormalIntranetMode, PackNum_4, eIntranetRadioMsgCancleTest);\
                            }while(0)

/* ��������Ϣ */
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
