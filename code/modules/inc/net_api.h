/**
 * @file net_api.h
 * @brief 用于联网发送数据提供接口，请重新定义以下宏的接口功能，其他应用会调用
 * @version 0.1
 * @date 2023-03-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __NET_API_H__
#define __NET_API_H__

/**
 * @brief co报警，进入co报警后调用
 */
#ifndef CoAlarm
#define CoAlarm()
#endif

/**
 * @brief 烟雾报警，进入烟雾报警后调用
 */
#ifndef SmokeAlarm
#define SmokeAlarm()
#endif

/**
 * @brief 取消报警，报警状态退出后调用CancelAlarm
 */
#ifndef CancelAlarm
#define CancelAlarm()
#endif

/**
 * @brief 静音，报警状态进入静音后调用EnterMute
 */
// #ifndef EnterMute
// #define EnterMute() __nop()
// #endif

/**
 * @brief 取消静音，静音状态退出后将调用CancelMute
 */
#ifndef CancelMute
#define CancelMute()
#endif

/**
 * @brief Co传感器错误，检测到Co传感器错误将调用CoFault
 */
#ifndef CoFault
#define CoFault()
#endif

/**
 * @brief Smoke传感器错误，检测到smoke传感器错误将调用SmokeFault
 */
#ifndef SmokeFault
#define SmokeFault()
#endif

/**
 * @brief 取消错误，从错误状态退出将调用CancelFault
 */
#ifndef CancelFault
#define CancelFault()
#endif

/**
 * @brief 电池电量变化，电量变化后触发BatteryChagne
 */
#ifndef BatteryChagne
#define BatteryChagne()
#endif

/**
 * @brief ppm峰值，ppm峰值改变调用CoPeakChange
 */
#ifndef CoPeakChange
#define CoPeakChange()
#endif

/**
 * @brief 寿命终结，进入后触发EndLife
 */
#ifndef EndLife
#define EndLife()
#endif

/**
 * @brief 自检成功结果，进入自检后检测结束无故障将会调用SelfCheckOk，有故障将调用CoFault/SmokeFault接口。
 */
#ifndef SelfCheckOk
#define SelfCheckOk(res)
#endif

/**
 * @brief 静音成功，如果是eSystemEventNetMute事件触发，将会调用MuteSuccess/MuteFail接口。
 */
// #ifndef MuteSuccess
// #define MuteSuccess() __nop()
// #endif

/**
 * @brief 静音失败，如果是eSystemEventNetMute事件触发，将会调用MuteSuccess/MuteFail接口。
 */
// #ifndef MuteFail
// #define MuteFail() __nop()
// #endif

/**
 * @brief CO更新ppm，在co应用检测并刷新时，将调用UpdateCoPPM接口。
 */
#ifndef UpdateCoPPM
#define UpdateCoPPM()
#endif
#endif
