/**
 * @file shell.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief letter shell
 * @version 3.0.0
 * @date 2019-12-30
 * 
 * @copyright (c) 2020 Letter
 * 
 */

#ifndef     __SHELL_H__
#define     __SHELL_H__

#include "shell_cfg.h"

#define     SHELL_VERSION               "3.1.2"                 /**< �汾�� */


/**
 * @brief shell ����
 * 
 * @param expr ���ʽ
 * @param action ����ʧ�ܲ���
 */
#define     SHELL_ASSERT(expr, action) \
            if (!(expr)) { \
                action; \
            }

#if SHELL_USING_LOCK == 1
#define     SHELL_LOCK(shell)           shell->lock(shell)
#define     SHELL_UNLOCK(shell)         shell->unlock(shell)
#else
#define     SHELL_LOCK(shell)
#define     SHELL_UNLOCK(shell)
#endif /** SHELL_USING_LOCK == 1 */
/**
 * @brief shell ����Ȩ��
 * 
 * @param permission Ȩ�޼���
 */
#define     SHELL_CMD_PERMISSION(permission) \
            (permission & 0x000000FF)

/**
 * @brief shell ��������
 * 
 * @param type ����
 */
#define     SHELL_CMD_TYPE(type) \
            ((type & 0x0000000F) << 8)

/**
 * @brief ʹ��������δУ������������ʹ��
 */
#define     SHELL_CMD_ENABLE_UNCHECKED \
            (1 << 12)

/**
 * @brief ���÷���ֵ��ӡ
 */
#define     SHELL_CMD_DISABLE_RETURN \
            (1 << 13)

/**
 * @brief ֻ������(���Ա�����Ч)
 */
#define     SHELL_CMD_READ_ONLY \
            (1 << 14)

/**
 * @brief �����������
 */
#define     SHELL_CMD_PARAM_NUM(num) \
            ((num & 0x0000000F)) << 16

#ifndef SHELL_SECTION
    #if defined(__CC_ARM) || defined(__CLANG_ARM)
        #define SHELL_SECTION(x)                __attribute__((section(x)))
    #elif defined (__IAR_SYSTEMS_ICC__)
        #define SHELL_SECTION(x)                @ x
    #elif defined(__GNUC__)
        #define SHELL_SECTION(x)                __attribute__((section(x)))
    #else
        #define SHELL_SECTION(x)
    #endif
#endif

#ifndef SHELL_USED
    #if defined(__CC_ARM) || defined(__CLANG_ARM)
        #define SHELL_USED                      __attribute__((used))
    #elif defined (__IAR_SYSTEMS_ICC__)
        #define SHELL_USED                      __root
    #elif defined(__GNUC__)
        #define SHELL_USED                      __attribute__((used))
    #else
        #define SHELL_USED
    #endif
#endif

/**
 * @brief shell float�Ͳ���ת��
 */
#define     SHELL_PARAM_FLOAT(x)            (*(float *)(&x))

/**
 * @brief shell ��������
 */
#define     SHELL_AGENCY_FUNC_NAME(_func)   agency##_func

/**
 * @brief shell����������
 * 
 * @param _func ������ĺ���
 * @param ... �������
 */
#define     SHELL_AGENCY_FUNC(_func, ...) \
            void SHELL_AGENCY_FUNC_NAME(_func)(int p1, int p2, int p3, int p4, int p5, int p6, int p7) \
            { _func(__VA_ARGS__); }

#if SHELL_USING_CMD_EXPORT == 1

    /**
     * @brief shell �����
     * 
     * @param _attr ��������
     * @param _name ������
     * @param _func �����
     * @param _desc ��������
     */
    #define SHELL_EXPORT_CMD(_attr, _name, _func, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = _desc; \
            SHELL_USED const ShellCommand \
            shellCommand##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr, \
                .data.cmd.name = shellCmd##_name, \
                .data.cmd.function = (int (*)())_func, \
                .data.cmd.desc = shellDesc##_name \
            }

    /**
     * @brief shell ���������
     * 
     * @param _attr ��������
     * @param _name ������
     * @param _func �����
     * @param _desc ��������
     * @param ... �������
     */
    #define SHELL_EXPORT_CMD_AGENCY(_attr, _name, _func, _desc, ...) \
            SHELL_AGENCY_FUNC(_func, ##__VA_ARGS__) \
            SHELL_EXPORT_CMD(_attr, _name, SHELL_AGENCY_FUNC_NAME(_func), _desc)

    /**
     * @brief shell ��������
     * 
     * @param _attr ��������
     * @param _name ������
     * @param _value ����ֵ
     * @param _desc ��������
     */
    #define SHELL_EXPORT_VAR(_attr, _name, _value, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = _desc; \
            SHELL_USED const ShellCommand \
            shellVar##_name SHELL_SECTION("shellVar") =  \
            { \
                .attr.value = _attr, \
                .data.var.name = shellCmd##_name, \
                .data.var.value = (void *)_value, \
                .data.var.desc = shellDesc##_name \
            }

    /**
     * @brief shell �û�����
     * 
     * @param _attr �û�����
     * @param _name �û���
     * @param _password �û�����
     * @param _desc �û�����
     */
    #define SHELL_EXPORT_USER(_attr, _name, _password, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellPassword##_name[] = #_password; \
            const char shellDesc##_name[] = #_desc; \
            SHELL_USED const ShellCommand \
            shellUser##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                .data.user.name = shellCmd##_name, \
                .data.user.password = shellPassword##_name, \
                .data.user.desc = shellDesc##_name \
            }

    /**
     * @brief shell ��������
     * 
     * @param _attr ��������
     * @param _value ������ֵ
     * @param _func ��������
     * @param _desc ��������
     */
    #define SHELL_EXPORT_KEY(_attr, _value, _func, _desc) \
            const char shellDesc##_value[] = #_desc; \
            SHELL_USED const ShellCommand \
            shellKey##_value SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                .data.key.value = _value, \
                .data.key.function = (void (*)(Shell *))_func, \
                .data.key.desc = shellDesc##_value \
            }

    /**
     * @brief shell ����������
     * 
     * @param _attr ��������
     * @param _value ������ֵ
     * @param _func ��������
     * @param _desc ��������
     * @param ... �������
     */
    #define SHELL_EXPORT_KEY_AGENCY(_attr, _value, _func, _desc, ...) \
            SHELL_AGENCY_FUNC(_func, ##__VA_ARGS__) \
            SHELL_EXPORT_KEY(_attr, _value, SHELL_AGENCY_FUNC_NAME(_func), _desc)
#else
    /**
     * @brief shell ����item����
     * 
     * @param _attr ��������
     * @param _name ������
     * @param _func �����
     * @param _desc ��������
     */
    #define SHELL_CMD_ITEM(_attr, _name, _func, _desc) \
            { \
                .attr.value = _attr, \
                .data.cmd.name = #_name, \
                .data.cmd.function = (int (*)())_func, \
                .data.cmd.desc = #_desc \
            }

    /**
     * @brief shell ����item����
     * 
     * @param _attr ��������
     * @param _name ������
     * @param _value ����ֵ
     * @param _desc ��������
     */
    #define SHELL_VAR_ITEM(_attr, _name, _value, _desc) \
            { \
                .attr.value = _attr, \
                .data.var.name = #_name, \
                .data.var.value = (void *)_value, \
                .data.var.desc = #_desc \
            }

    /**
     * @brief shell �û�item����
     * 
     * @param _attr �û�����
     * @param _name �û���
     * @param _password �û�����
     * @param _desc �û�����
     */
    #define SHELL_USER_ITEM(_attr, _name, _password, _desc) \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                .data.user.name = #_name, \
                .data.user.password = #_password, \
                .data.user.desc = #_desc \
            }

    /**
     * @brief shell ����item����
     * 
     * @param _attr ��������
     * @param _value ������ֵ
     * @param _func ��������
     * @param _desc ��������
     */
    #define SHELL_KEY_ITEM(_attr, _value, _func, _desc) \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                .data.key.value = _value, \
                .data.key.function = (void (*)(Shell *))_func, \
                .data.key.desc = #_desc \
            }

    #define SHELL_EXPORT_CMD(_attr, _name, _func, _desc)
    #define SHELL_EXPORT_CMD_AGENCY(_attr, _name, _func, _desc, ...)
    #define SHELL_EXPORT_VAR(_attr, _name, _value, _desc)
    #define SHELL_EXPORT_USER(_attr, _name, _password, _desc)
    #define SHELL_EXPORT_KEY(_attr, _value, _func, _desc)
    #define SHELL_EXPORT_KEY_AGENCY(_attr, _name, _func, _desc, ...)
#endif /** SHELL_USING_CMD_EXPORT == 1 */

/**
 * @brief shell command����
 */
typedef enum
{
    SHELL_TYPE_CMD_MAIN = 0,                                    /**< main��ʽ���� */
    SHELL_TYPE_CMD_FUNC,                                        /* *< C������ʽ���� */ /* ��ʹ�ø���ʽ*/
    SHELL_TYPE_VAR_INT,                                         /**< int�ͱ��� */
    SHELL_TYPE_VAR_SHORT,                                       /**< short�ͱ��� */
    SHELL_TYPE_VAR_CHAR,                                        /**< char�ͱ��� */
    SHELL_TYPE_VAR_STRING,                                      /**< string�ͱ��� */
    SHELL_TYPE_VAR_POINT,                                       /**< ָ���ͱ��� */
    SHELL_TYPE_VAR_NODE,                                        /**< �ڵ���� */
    SHELL_TYPE_USER,                                            /**< �û� */
    SHELL_TYPE_KEY,                                             /**< ���� */
} ShellCommandType;


/**
 * @brief Shell����
 */
typedef struct shell_def
{
    struct
    {
        const struct shell_command *user;                       /**< ��ǰ�û� */
        int activeTime;                                         /**< shell����ʱ�� */
        // char *path;                                             /**< ��ǰshell·�� */
    #if SHELL_USING_COMPANION == 1
        struct shell_companion_object *companions;              /**< �������� */
    #endif
    #if SHELL_KEEP_RETURN_VALUE == 1
        int retVal;                                             /**< ����ֵ */
    #endif
    } info;
    struct
    {
        unsigned short length;                                  /**< �������ݳ��� */
        unsigned short cursor;                                  /**< ��ǰ���λ�� */
        char *buffer;                                           /**< ���뻺�� */
        char *param[SHELL_PARAMETER_MAX_NUMBER];                /**< ���� */
        unsigned short bufferSize;                              /**< ���뻺���С */
        unsigned short paramCount;                              /**< �������� */
        int keyValue;                                           /**< ���밴����ֵ */
    } parser;
#if SHELL_HISTORY_MAX_NUMBER > 0
    struct
    {
        char *item[SHELL_HISTORY_MAX_NUMBER];                   /**< ��ʷ��¼ */
        unsigned short number;                                  /**< ��ʷ��¼�� */
        unsigned short record;                                  /**< ��ǰ��¼λ�� */
        signed short offset;                                    /**< ��ǰ��ʷ��¼ƫ�� */
    } history;
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */
    struct CmdList
    {
        void *base;                                             /**< ������ַ */
        unsigned short count;                                   /**< �������� */
    } commandList, VarList;
    struct
    {
        unsigned char isChecked : 1;                            /**< ����У��ͨ�� */
        unsigned char isActive : 1;                             /**< ��ǰ�Shell */
        unsigned char tabFlag : 1;                              /**< tab��־ */
    } status;
    signed short (*read)(char *, unsigned short);               /**< shell������ */
    signed short (*write)(char *, unsigned short);              /**< shellд���� */
#if SHELL_USING_LOCK == 1
    int (*lock)(struct shell_def *);                              /**< shell ���� */
    int (*unlock)(struct shell_def *);                            /**< shell ���� */
#endif
} Shell;


/**
 * @brief shell command����
 */
typedef struct shell_command
{
    union
    {
        struct
        {
            unsigned char permission : 8;                       /**< commandȨ�� */
            ShellCommandType type : 4;                          /**< command���� */
            unsigned char enableUnchecked : 1;                  /**< ��δУ�����������¿��� */
            unsigned char disableReturn : 1;                    /**< ���÷���ֵ��� */
            unsigned char readOnly : 1;                        /**< ֻ�� */
            unsigned char reserve : 1;                          /**< ���� */
            unsigned char paramNum : 4;                         /**< �������� */
        } attrs;
        int value;
    } attr;                                                     /**< ���� */
    union
    {
        struct
        {
            const char *name;                                   /**< ������ */
            int (*function)();                                  /**< ����ִ�к��� */
            const char *desc;                                   /**< �������� */
        } cmd;                                                  /**< ����� */
        struct
        {
            const char *name;                                   /**< ������ */
            void *value;                                        /**< ����ֵ */
            const char *desc;                                   /**< �������� */
        } var;                                                  /**< �������� */
        struct
        {
            const char *name;                                   /**< �û��� */
            const char *password;                               /**< �û����� */
            const char *desc;                                   /**< �û����� */
        } user;                                                 /**< �û����� */
        struct
        {
            int value;                                          /**< ������ֵ */
            void (*function)(Shell *);                          /**< ����ִ�к��� */
            const char *desc;                                   /**< �������� */
        } key;                                                  /**< �������� */
    } data; 
} ShellCommand;

/**
 * @brief shell�ڵ��������
 */
typedef struct
{
    void *var;                                                  /**< �������� */
    int (*get)();                                               /**< ����get���� */
    int (*set)();                                               /**< ����set���� */
} ShellNodeVarAttr;


#define shellSetPath(_shell, _path)     (_shell)->info.path = _path
#define shellGetPath(_shell)            ((_shell)->info.path)

#define shellDeInit(shell)              shellRemove(shell)

void shellInit(Shell *shell, char *buffer, unsigned short size);
void shellRemove(Shell *shell);
unsigned short shellWriteString(Shell *shell, const char *string);
void shellPrint(Shell *shell, const char *fmt, ...);
void shellScan(Shell *shell, char *fmt, ...);
Shell* shellGetCurrent(void);
void shellHandler(Shell *shell, char data);
void shellWriteEndLine(Shell *shell, char *buffer, int len);
void shellTask(void *param);
int shellRun(Shell *shell, const char *cmd);



#if SHELL_USING_COMPANION == 1
/**
 * @brief shell����������
 */
typedef struct shell_companion_object
{
    int id;                                                     /**< ��������ID */
    void *obj;                                                  /**< �������� */
    struct shell_companion_object *next;                        /**< ��һ���������� */
} ShellCompanionObj;


signed char shellCompanionAdd(Shell *shell, int id, void *object);
signed char shellCompanionDel(Shell *shell, int id);
void *shellCompanionGet(Shell *shell, int id);
#endif

#endif


