#include <stdint.h>
#include "radio/inc/radio.h"
#include "bsp_config.h"
#include "inc/mac.h"
#include <stdint.h>

#define CMD_QUEUE_LENGHT 16
#define is_queue_null() (cmd_queue.in == cmd_queue.out)
#define transfer(dat, mode, PackNum) protocol_transfer(dat, mode, PackNum)

static unsigned char ID[4] = {0};
static unsigned char pair_status;
static unsigned char sub_id;
static unsigned char tar_id;

/**
 * @brief ����ģʽ����
 *
 */
typedef struct
{
    unsigned char SubId;       /* Դ�豸Sub ID(˭����) */
    unsigned char Target;      /* Ŀ���豸Sub ID(����˭) */
    unsigned char DevType;     /* �豸���� */
    unsigned char MsgL1;       /* һ����Ϣ���� */
    unsigned char MsgL2;       /* ������Ϣ���� */
    unsigned char MsgProperty; /* ��Ϣ���� */
    unsigned char MsgData[4];  /* ��Ϣ���� */
} Hub_data;

typedef struct
{
    unsigned char MsgL1;
} Intranet_data;

typedef struct
{
    unsigned char ID[4]; /* ����ID or UDID */
    union
    {
        Hub_data Hub_data;
        Intranet_data Intranet_data;
    };
} protocol_content;

static struct
{
    unsigned char in;
    unsigned char out;
    protocol_content queue[CMD_QUEUE_LENGHT];
} cmd_queue; // �������

/**
 * @brief ���
 *
 */
static void push_queue(void)
{
    cmd_queue.in = (cmd_queue.in + 1) & (CMD_QUEUE_LENGHT - 1);
}

/**
 * @brief ����
 *
 */
static void pull_queue(void)
{
    if (is_queue_null())
        return;

    cmd_queue.out = (cmd_queue.out + 1) & (CMD_QUEUE_LENGHT - 1);
    transfer(cmd_queue.queue[cmd_queue.out],
             &cmd_queue.queue[cmd_queue.out].mode,
             cmd_queue.queue[cmd_queue.out].packnum);
}

void SendCmd(const unsigned char cmd1, const unsigned char cmd2, unsigned char *data)
{
    protocol_content *send;

    if (RF_PAIR_MODE_NONE == pair_status)
        return;

    send = &cmd_queue.queue[cmd_queue.in];
    memcpy(send->ID, ID, sizeof(ID));

    if (RF_PAIR_MODE_INTRANET == pair_status)
    {
        send->Intranet_data.MsgL1 = cmd1;
    }
    else if (RF_PAIR_MODE_HUB == pair_status)
    {
        send->Hub_data.MsgProperty = 0;

        send->Hub_data.MsgL1 = cmd1;
        send->Hub_data.MsgL2 = cmd2;
        memcpy(send->Hub_data.MsgData, data, sizeof(send->Hub_data.MsgData));

        send->Hub_data.SubId = sub_id;
        send->Hub_data.Target = tar_id;
        send->Hub_data.DevType = Device_Type;
    }

    push_queue();
}

void SetMacID(uint32_t id)
{
    *(uint32_t*)ID = id;
}

void ChangePairStatus(uint8_t )
{

}

void MacInit(void)
{

}
