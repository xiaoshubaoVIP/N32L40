#include "radio/inc/radio_protocol.h"
#include "app_rf.h"
#include "common.h"
#include "bsp_config.h"
#include <string.h>

#ifdef SUPPORT_RADIO_FUNCTION
#define RadioQueueLimit ((uint8_t)64)
#define RadioBufLimit ((uint8_t)96)
#define CMD_QUEUE_LENGHT 16
#define is_queue_null() (cmd_queue.in == cmd_queue.out)

#define RadioQueueLimit ((uint8_t)64)
#define RadioBufLimit ((uint8_t)96)

unsigned char volatile RadioQueueBuf[RadioQueueLimit] = {0}; //���л���
unsigned char RadioReadBuf[RadioBufLimit] = {0};             //���ջ���

volatile unsigned char *rf_queue_in = (unsigned char *)RadioQueueBuf;
volatile unsigned char *rf_queue_out = (unsigned char *)RadioQueueBuf;

const uint8_t UniversalID[RfIdLength] = {0xFF, 0xFF, 0xFF, 0xFE};
#pragma anon_unions

void RfHandleMsg(uint8_t DataOffset);

/**
 * @brief ����ģʽ����
 *
 */
typedef struct
{
    unsigned char ID[4];       /* ����ID or UDID */
    unsigned char SubId;       /* Դ�豸Sub ID(˭����) */
    unsigned char Target;      /* Ŀ���豸Sub ID(����˭) */
    unsigned char DevType;     /* �豸���� */
    unsigned char MsgL1;       /* һ����Ϣ���� */
    unsigned char MsgL2;       /* ������Ϣ���� */
    unsigned char MsgProperty; /* ��Ϣ���� */
    unsigned char MsgData[4];  /* ��Ϣ���� */
    unsigned char crc;
} Hub_data;

/**
 * @brief ����������
 *
 */
typedef struct
{
    unsigned char ID[4]; /* ����ID or UDID */
    unsigned char MsgL1;
    unsigned char crc;
} Intranet_data;

/**
 * @brief RF���ݰ�
 */
typedef struct
{
    union
    {
        Hub_data Hub_data;
        Intranet_data Intranet_data;
    };
} protocol_content;

/**
 * @brief ��������
 */
static struct
{
    unsigned char in;
    unsigned char out;

    struct
    {
        unsigned char pack_num;
        unsigned char mode;
        unsigned char len;
        protocol_content dat;
    } queue[CMD_QUEUE_LENGHT];
} cmd_queue; // �������

// unsigned char volatile RadioQueueBuf[RadioQueueLimit] = {0}; //���л���
// unsigned char RadioReadBuf[RadioBufLimit] = {0};             //���ջ���
// volatile unsigned char *rf_queue_in = (unsigned char *)RadioQueueBuf;
// volatile unsigned char *rf_queue_out = (unsigned char *)RadioQueueBuf;

static unsigned char ID[4] = {0};
static unsigned char sub_id;
static unsigned char tar_id;

unsigned char crc8(void *buf1, unsigned char len)
{
    int i;
    char crc = 0;
    unsigned char *buf = (unsigned char *)buf1;
    while (len-- != 0)
    {
        for (i = 0x80; i != 0; i /= 2)
        {
            if ((crc & 0x80) != 0)
            {
                crc *= 2;
                crc ^= 0x07;
            }
            else
            {
                crc *= 2;
            }

            if ((*buf & i) != 0)
                crc ^= 0x07;
        }
        buf++;
    }
    return crc;
}

/**********************************************************************
Description:	CompareRfID
Parameters:
Return:
Date:
Author:		quanwu.xu
**********************************************************************/
BOOL CompareRfID(const uint8_t *src, const uint8_t *dest, uint8_t Length)
{
    uint8_t i = 0;
    BOOL Ret = true;

    for (i = 0; i < Length; i++)
    {
        if (src[i] != dest[i])
        {
            Ret = false;
            break;
        }
    }
    return Ret;
}

static void protocol_transfer(unsigned char RadioMode, unsigned char PackNum, unsigned char dat[], unsigned char len)
{
    Radio.Init((eRadioInitMode)RadioMode);

    if (len != 0)
    {
        printbuff(dat, len, "rf tx:");
        Radio.Send((unsigned char *)dat, len, PackNum, 600); // 600 * 10ms = 6000ms
    }
}

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
    SystemSetActiveFlag(eActiveFlagNetBusy);
    protocol_transfer(cmd_queue.queue[cmd_queue.out].mode,
                      cmd_queue.queue[cmd_queue.out].pack_num,
                      (uint8_t *)&cmd_queue.queue[cmd_queue.out].dat,
                      cmd_queue.queue[cmd_queue.out].len);

    cmd_queue.out = (cmd_queue.out + 1) & (CMD_QUEUE_LENGHT - 1);
}

void IntranetSend(unsigned char mode, unsigned char pack_num, unsigned char cmd)
{
    protocol_content *send = &cmd_queue.queue[cmd_queue.in].dat;

    memcpy(send->Intranet_data.ID, ID, sizeof(ID));
    send->Intranet_data.MsgL1 = cmd;
    send->Intranet_data.crc = crc8(&send->Intranet_data, sizeof(send->Intranet_data) - 1);
    cmd_queue.queue[cmd_queue.in].len = Radio.PlayloadLen;
    cmd_queue.queue[cmd_queue.in].pack_num = pack_num;
    cmd_queue.queue[cmd_queue.in].mode = mode;
    push_queue();
}

/**
 * @brief Set the Radio Mode
 * 
 * @param mode 
 */
void SetRadioMode(unsigned char mode)
{
    cmd_queue.queue[cmd_queue.in].len = 0;
    cmd_queue.queue[cmd_queue.in].mode = mode;
    push_queue();
}

void SendHubData(unsigned char mode, unsigned char pack_num, const unsigned char cmd1, const unsigned char cmd2, unsigned char *data)
{
    protocol_content *send = &cmd_queue.queue[cmd_queue.in].dat;

    cmd_queue.queue[cmd_queue.in].len = Radio.PlayloadLen;
    memcpy(send->Hub_data.ID, ID, sizeof(ID));

    send->Hub_data.MsgProperty = 0;

    send->Hub_data.MsgL1 = cmd1;
    send->Hub_data.MsgL2 = cmd2;
    memcpy(send->Hub_data.MsgData, data, sizeof(send->Hub_data.MsgData));

    send->Hub_data.SubId = sub_id;
    send->Hub_data.Target = tar_id;
    send->Hub_data.DevType = Device_Type;

    send->Hub_data.crc = crc8(&send->Hub_data, sizeof(send->Hub_data) - 1);
    cmd_queue.queue[cmd_queue.in].len = sizeof(Hub_data);
    cmd_queue.queue[cmd_queue.in].pack_num = pack_num;
    cmd_queue.queue[cmd_queue.in].mode = mode;

    push_queue();
}

void SetRadioID(const unsigned char id[4])
{
    memcpy(ID, id, sizeof(ID));
}

uint8_t* GetRadioID(void)
{
    return ID;
}

/*****************************************************************************
�������� : get_queue_total_data
�������� : ��ȡ����������
������� : ��
���ز��� : ��
*****************************************************************************/
static unsigned char Rf_get_queue_total_data(void)
{
    if (rf_queue_in != rf_queue_out)
        return 1;
    else
        return 0;
}
/*****************************************************************************
�������� : Queue_Read_Byte
�������� : ��ȡ����1�ֽ�����
������� : ��
���ز��� : ��
*****************************************************************************/
static unsigned char Rf_Queue_Read_Byte(void)
{
    unsigned char value = 0;

    if (rf_queue_out != rf_queue_in)
    {
        //������
        if (rf_queue_out >= (unsigned char *)(RadioQueueBuf + sizeof(RadioQueueBuf)))
        {
            //�����Ѿ���ĩβ
            rf_queue_out = (unsigned char *)(RadioQueueBuf);
        }

        value = *rf_queue_out++;
    }

    return value;
}

/*****************************************************************************
�������� : uart_receive_input
�������� : �����ݴ���
������� : value:�����յ��ֽ�����
���ز��� : ��
ʹ��˵�� : ��MCU���ڽ��պ����е��øú���,�������յ���������Ϊ��������
*****************************************************************************/
void RadioReceiveInput(unsigned char value)
{
    if ((rf_queue_in > rf_queue_out) && ((rf_queue_in - rf_queue_out) >= sizeof(RadioQueueBuf)))
    {
        //���ݶ�����
    }
    else if ((rf_queue_in < rf_queue_out) && ((rf_queue_out - rf_queue_in) == 1))
    {
        //���ݶ�����
    }
    else
    {
        //���в���
        if (rf_queue_in >= (unsigned char *)(RadioQueueBuf + sizeof(RadioQueueBuf)))
        {
            rf_queue_in = (unsigned char *)(RadioQueueBuf);
        }

        *rf_queue_in++ = value;
    }
}

/**********************************************************************
Description:	RadioService()
Parameters:
Return:
Date:
Author:		quanwu.xu
**********************************************************************/
void RadioService(void)
{
    static uint16_t rx_in = 0;
    uint8_t i = 0;
    uint8_t PlayloadBuf[16] = {0};
    uint8_t offset = 0;
    uint8_t rx_value_len = 0; //����֡����

    Radio.Process();

    if (RF_GetStatus() == RF_STATE_IDLE && !is_queue_null())
    {
        pull_queue();
    }
    else if ((RF_GetStatus() == RF_STATE_IDLE) && (GetRadioState() == eSystemState_Idle))
    {
        SystemClearActiveFlag(eActiveFlagNetBusy);
        if (Radio.InitMode != LowPowerIntranetMode)
        {
            printf("rf into ide\n");
            Radio.Init(LowPowerIntranetMode);
        }
    }

    if (Radio.Rx(PlayloadBuf, Radio.PlayloadLen) == true)
    {
        for (i = 0; i < Radio.PlayloadLen; i++)
        {
            RadioReceiveInput(PlayloadBuf[i]);
        }
    }

    while ((rx_in < sizeof(RadioReadBuf)) && (Rf_get_queue_total_data() > 0))
    {
        RadioReadBuf[rx_in++] = Rf_Queue_Read_Byte();
    }

    if (rx_in < RfIdLength)
    {
        return;
    }

    // check real  data
    // printflog("%d,%d\n", rx_in, offset);

    while ((rx_in - offset) >= RfIdLength)
    {
        // rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100 + wifi_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD;
        rx_value_len = Radio.PlayloadLen;
        // data not receive over
        if ((rx_in - offset) < rx_value_len)
        {
            break;
        }

        // ���ݽ������,and compare check sum
        if (crc8((unsigned char *)RadioReadBuf + offset, rx_value_len - 1) == RadioReadBuf[offset + rx_value_len - 1])
        {
#if 1 // def DEBUG_MODE
      //  printbuff(PlayloadBuf, Radio.PlayloadLen, "Rx:");
#endif
            RfHandleMsg(offset);
        }
        else
        {
            // debug_error("Crc Err\n");
        }
        offset += rx_value_len;
    } // end while

    rx_in -= offset;
    // copy remain data to first position
    if (rx_in > 0)
    {
        memcpy(RadioReadBuf, RadioReadBuf + offset, rx_in);
    }
}
REG_APP_TASK(RadioService);

/**
 * @brief �������ݴ���-����Э��
 *
 * @param DataOffset
 */
void HubRfHandleMsg(uint8_t DataOffset)
{
    //	uint8_t RadioMsgLevel = 0;
    //	RadioMsgTag RfMsgTagObj = {0};
    //	RadioDevTag* pRadioApi = GetRfApiTag();

    //	memcpy(&RfMsgTagObj, (void *)&RadioReadBuf[DataOffset], HUB_PLAYLOAD_LEN);

    //	RadioMsgLevel = RfMsgTagObj.MsgLevel1;

    //	if (RadioStatus == eSystemState_Pair)
    //	{
    //		HubRfPairMsgHandle(&RfMsgTagObj,RadioStatus);
    //	}
    //	else if(CompareRfID(pRadioApi->RadioDevId, RfMsgTagObj.DeviceId, RfIdLength) &&
    //								((RfMsgTagObj.TargetNumber == pRadioApi->SelfDevNum) ||
    //											(RfMsgTagObj.TargetNumber == TxToAllFireAlarms)))
    //	{
    //		switch(RadioMsgLevel)
    //		{
    //			case eRfMsgWakeup:
    //				HubRfWakeupDevMsgHandle(&RfMsgTagObj);
    //				break;
    //
    //			case eRfMsgTraverse:
    //				break;
    //
    //			case eRfMsgDeleteDev:
    //				HubRfDeleteDevMsgHandle(&RfMsgTagObj);
    //				break;
    //
    //			case eRfMsgSearchDev:
    //				HubRfSearchDevMsgHandle(&RfMsgTagObj);
    //				break;
    //
    //			case eRfMsgHeartBeat:
    //				HubRfHeartbeatMsgHandle(&RfMsgTagObj);
    //				break;
    //
    //			case eRfMsgFireAlarm:
    //				HubRfFireAlarmMsgHandle(&RfMsgTagObj);
    //				break;
    //
    //			default:
    //				break;
    //		}
    //	}
}

/**********************************************************************
Description:	IntranetRfHandleMsg()
Parameters:
Return:
Date:
Author:		quanwu.xu
**********************************************************************/
void IntranetPairMsgHandle(IntranetRadioMsgTag *pIntranetMsgTagObj)
{
    eInternatMsgType RadioMsgType = eIntranetRadioMsgNull;
    RadioMsgType = pIntranetMsgTagObj->MsgType;
    printf("pairmsg = %x\n", RadioMsgType);

    if ((RadioMsgType == eIntranetRadioMsgPairAck) ||
        (RadioMsgType == eIntranetRadioMsgPairNewPairAck) ||
        (RadioMsgType == eIntranetRadioMsgFactoryPairAck))
    {
        if ((pIntranetMsgTagObj->DeviceId[0] == 0) && (pIntranetMsgTagObj->DeviceId[1] == 0) &&
            (pIntranetMsgTagObj->DeviceId[2] == 0) && (pIntranetMsgTagObj->DeviceId[3] == 0))
        {
            printf("ID is 0\n");
        }
        else
        {
            // memcpy((void *)pRadioApi->RadioDevId, (void *)pIntranetMsgTagObj->DeviceId, RfIdLength);
            SetRadioID(pIntranetMsgTagObj->DeviceId);
            SystemPostFIFI(Q_eIntranetEventPairAnswer);
        }
    }
    else if (RadioMsgType == eIntranetRadioMsgPairNewRequest)
    {
        SystemPostFIFI(Q_eIntranetEventPairRequest);
    }
    else if (RadioMsgType == eIntranetRadioMsgPairRequest)
    {
        // memcpy((void *)pRadioApi->RadioDevId, (void *)pIntranetMsgTagObj->DeviceId, RfIdLength);
        SystemPostFIFI(Q_eIntranetEventPairOldRequest);
    }
}

void IntranetRfHandleMsg(uint8_t DataOffset)
{
    eInternatMsgType RadioMsgType = eIntranetRadioMsgNull;
    IntranetRadioMsgTag IntranetMsgTagObj = {0};

    memcpy(&IntranetMsgTagObj, (void *)&RadioReadBuf[DataOffset], INTRANET_PLAYLOAD_LEN);

    RadioMsgType = IntranetMsgTagObj.MsgType;

    if (GetRadioState() == eSystemState_Pair)
    {
        IntranetPairMsgHandle(&IntranetMsgTagObj);
    }
    else if ((GetRadioPairMode() != RF_PAIR_MODE_NONE) &&
             (CompareRfID(ID, IntranetMsgTagObj.DeviceId, RfIdLength)))
    {
        printf("Msg=%x\n", RadioMsgType);
        // debug_printbuff(RadioReadBuf,INTRANET_PLAYLOAD_LEN, "rx:");
        switch (RadioMsgType)
        {
        case eIntranetRadioMsgMasterTest:
            SystemPostFIFI(Q_eIntranetEventMasterTest);
            break;

        case eIntranetRadioMsgForwardTest:
            SystemPostFIFI(Q_eIntranetEventForwardTest);
            break;

        case eIntranetRadioMsgCancleTest:
            SystemPostFIFI(Q_eIntranetEventCancelTest);
            break;

        case eIntranetRadioMsgSync:
            SystemPostFIFI(Q_eIntranetEventSync);
            break;

        case eIntranetRadioMsgCancleMsgSync:
            SystemPostFIFI(Q_eIntranetEventCancelSync);
            break;

        case eIntranetRadioMsgSmokeAlarm:
            SystemPostFIFI(Q_eIntranetEventSmokeAlarm);
            break;

        case eIntranetRadioMsgCoAlarm:
            SystemPostFIFI(Q_eIntranetEventCoAlarm);
            break;

        case eIntranetRadioMsgCancleAlarm:
            SystemPostFIFI(Q_eIntranetEventCancelAlarm);
            break;

        case eIntranetRadioMsgMute:
            SystemPostFIFI(Q_eIntranetEventMute);
            break;

        case eIntranetRadioMsgCancleMute:
            SystemPostFIFI(Q_eIntranetEventCancelMute);
            break;

        case eIntranetRadioMsgStopMasterAlarm:
            SystemPostFIFI(Q_eIntranetEventMuteMasterAlarm);
            break;

        default:
            break;
        }
    }
    else if (CompareRfID(UniversalID, IntranetMsgTagObj.DeviceId, RfIdLength))
    {
        // if (RadioMsgType == eIntranetRadioMsgMasterTest)
        // {
        //     UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgTest, DevTestStart);
        // }

        SystemPostFIFI(Q_eIntranetEventRadioRxTest);
    }
}

/**
 * @brief �������ݴ���
 *
 * @param DataOffset
 */
void RfHandleMsg(uint8_t DataOffset)
{
    if (Radio.PlayloadLen == HUB_PLAYLOAD_LEN)
    {
        HubRfHandleMsg(DataOffset);
    }
    else if (Radio.PlayloadLen == INTRANET_PLAYLOAD_LEN)
    {
        IntranetRfHandleMsg(DataOffset);
    }
}
#endif
