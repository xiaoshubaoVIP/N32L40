/**
 * @file net_api.h
 * @brief �����������������ṩ�ӿڣ������¶������º�Ľӿڹ��ܣ�����Ӧ�û����
 * @version 0.1
 * @date 2023-03-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __NET_API_H__
#define __NET_API_H__

/**
 * @brief co����������co���������
 */
#ifndef CoAlarm
#define CoAlarm()
#endif

/**
 * @brief �����������������������
 */
#ifndef SmokeAlarm
#define SmokeAlarm()
#endif

/**
 * @brief ȡ������������״̬�˳������CancelAlarm
 */
#ifndef CancelAlarm
#define CancelAlarm()
#endif

/**
 * @brief ����������״̬���뾲�������EnterMute
 */
// #ifndef EnterMute
// #define EnterMute() __nop()
// #endif

/**
 * @brief ȡ������������״̬�˳��󽫵���CancelMute
 */
#ifndef CancelMute
#define CancelMute()
#endif

/**
 * @brief Co���������󣬼�⵽Co���������󽫵���CoFault
 */
#ifndef CoFault
#define CoFault()
#endif

/**
 * @brief Smoke���������󣬼�⵽smoke���������󽫵���SmokeFault
 */
#ifndef SmokeFault
#define SmokeFault()
#endif

/**
 * @brief ȡ�����󣬴Ӵ���״̬�˳�������CancelFault
 */
#ifndef CancelFault
#define CancelFault()
#endif

/**
 * @brief ��ص����仯�������仯�󴥷�BatteryChagne
 */
#ifndef BatteryChagne
#define BatteryChagne()
#endif

/**
 * @brief ppm��ֵ��ppm��ֵ�ı����CoPeakChange
 */
#ifndef CoPeakChange
#define CoPeakChange()
#endif

/**
 * @brief �����սᣬ����󴥷�EndLife
 */
#ifndef EndLife
#define EndLife()
#endif

/**
 * @brief �Լ�ɹ�����������Լ��������޹��Ͻ������SelfCheckOk���й��Ͻ�����CoFault/SmokeFault�ӿڡ�
 */
#ifndef SelfCheckOk
#define SelfCheckOk(res)
#endif

/**
 * @brief �����ɹ��������eSystemEventNetMute�¼��������������MuteSuccess/MuteFail�ӿڡ�
 */
// #ifndef MuteSuccess
// #define MuteSuccess() __nop()
// #endif

/**
 * @brief ����ʧ�ܣ������eSystemEventNetMute�¼��������������MuteSuccess/MuteFail�ӿڡ�
 */
// #ifndef MuteFail
// #define MuteFail() __nop()
// #endif

/**
 * @brief CO����ppm����coӦ�ü�Ⲣˢ��ʱ��������UpdateCoPPM�ӿڡ�
 */
#ifndef UpdateCoPPM
#define UpdateCoPPM()
#endif
#endif
