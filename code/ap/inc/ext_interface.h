#ifndef __EXT_INTERFACE_H__
#define __EXT_INTERFACE_H__
/**
 * @file ext_interface.h
 * @brief ���ļ�����app�䴥���¼�ʱ�ṩ�ӿ�
 *
 * @date 2023-03-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#define SECTION(x) __attribute__((section(x)))
#define SECTION_CALL(s)                                                         \
    do                                                                          \
    {                                                                           \
        extern unsigned int s##$$Base;                                          \
        extern unsigned int s##$$Limit;                                         \
        for (unsigned int *fn_ptr = &s##$$Base; fn_ptr < &s##$$Limit; fn_ptr++) \
            ((void (*)(void))(*fn_ptr))();                                      \
    } while (0)

/* ע��APP���� */
#define REG_APP_TASK(func) void *const task##func SECTION("APP_TASK") = func

/************************************************** app_mian interface ***********************************************************/
/* APP��ʼ������ */
#define REG_APP_INIT(func) void *const app_init##func SECTION("APP_INIT") = func

/* APP���ѵ��� */
#define REG_APP_WAKE_PROC(func) void *const app_wake_proc##func SECTION("APP_WAKE_PROC") = func



/**************************************************Insert interface**************************************************/
#if defined(__CC_ARM)
#define INSER_FUNC(f) void $Sub$$##f(void)
#define SOURCE_FUNC_CALL(f) {extern void $Super$$##f(void); $Super$$##f();}
#elif defined(__ICCARM__) || defined(__ICCRX__)
#define INSER_FUNC(f)
#define SOURCE_FUNC_CALL(f)
#elif defined(__GNUC__)
#define INSER_FUNC(f) void $Sub$$##f(void)
#define SOURCE_FUNC_CALL(f) {extern void $Super$$##f(void); $Super$$##f();}
#endif

#endif
