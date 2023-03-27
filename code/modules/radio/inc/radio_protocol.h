#ifndef __RADIO_PROTOCOL_H__
#define __RADIO_PROTOCOL_H__
#include <stdint.h>
#include "radio/inc/radio_2300a.h"

/* 
    日志等级 LOG_LEVEL
    0:ERROR
    1:WARN
    2:INFO
    3:DEBUG
 */
#undef LOG_LEVEL
#define LOG_LEVEL 3

#define RfIdLength 4

#define Device_Type 0




//产品代号Production type
typedef enum _eProductType
{
    eProductType_NULL = 0x00,
    eProductType_Hub1,
    eProductType_Hub2,
    eProductType_Hub3,
    eProductType_Repeater1_Smart,
    eProductType_Repeater2_Smart,
    eProductType_Repeater3_Smart,
    eProductType_SC01_Smart,
    eProductType_SC03_Smart,
    eProductType_SD01_Smart,
    eProductType_SD03_Smart,
    eProductType_CD01_Smart,
    eProductType_CD03_Smart,
    eProductType_CD14_Smart,
    eProductType_XC01_Smart,
    eProductType_XC02_Smart,
    eProductType_XC03_Smart,
    eProductType_XP01_Smart, // 17
    eProductType_XP01_S_Smart,
    eProductType_XP02_Smart,
    eProductType_XP02_S_Smart,
    eProductType_XP03_Smart,
    eProductType_XP03_S_Smart,
    eProductType_XS01_Smart,
    eProductType_XS02_Smart,
    eProductType_XS03_Smart,
    eProductType_HC01_Smart,
    eProductType_HC02_Smart,
    eProductType_HC03_Smart,
    eProductType_GC01_Smart,
    eProductType_GC02_Smart,
    eProductType_GC03_Smart,
    eProductType_Heat01_Smart,
    eProductType_Heat02_Smart,
    eProductType_Heat03_Smart,
    eProductType_Gas01_Smart,
    eProductType_Gas02_Smart,
    eProductType_Gas03_Smart,
    eProductType_Water01_Smart,
    eProductType_Water02_Smart,
    eProductType_Water03_Smart,
    eProductType_Plug01_Smart,
    eProductType_Plug02_Smart,
    eProductType_Plug03_Smart,
    eProductType_Romote01_Smart,
    eProductType_Romote02_Smart,
    eProductType_Romote03_Smart,
    eProductType_PIR01_Smart,
    eProductType_PIR02_Smart,
    eProductType_PIR03_Smart,
    eProductType_Magnets01_Smart,
    eProductType_Magnets02_Smart,
    eProductType_Magnets03_Smart,
    eProductType_DriveWay01_Smart,
    eProductType_DriveWay02_Smart,
    eProductType_DriveWay03_Smart,
    eProductType_GlassAlarm01_Smart,
    eProductType_GlassAlarm02_Smart,
    eProductType_GlassAlarm03_Smart,
    eProductType_SOS01_Smart,
    eProductType_SOS02_Smart,
    eProductType_SOS03_Smart,
    eProductType_Actuator01_Smart,
    eProductType_Actuator02_Smart,
    eProductType_Actuator03_Smart,
    eProductType_KeyBoard01_Smart,
    eProductType_KeyBoard02_Smart,
    eProductType_KeyBoard03_Smart,
    eProductType_DoorBell01_Smart,
    eProductType_DoorBell02_Smart,
    eProductType_DoorBell03_Smart,
    eProductType_Horn01_Smart,
    eProductType_Horn02_Smart,
    eProductType_Horn03_Smart,
    eProductType_Camera01_Smart,
    eProductType_Camera02_Smart,
    eProductType_Camera03_Smart,
    eProductType_WeatherStation01_Smart,
    eProductType_WeatherStation02_Smart,
    eProductType_WeatherStation03_Smart,
} eProductType;

//设备类型
typedef enum _eRfDeviceType
{
    eTypeNull = 0x00,
    eTypeHub,              // Hub设备
    eTypeRepeater,         //中继器
    eTypeSmoke,            //烟感报警器
    eTypeCo,               // Co报警器
    eTypeHeat,             //热报警器
    eTypeGas,              //燃气
    eTypeSmokeAndCo,       // SC复合型报警器
    eTypeHeatAndCo,        // HC复合型报警器
    eTypeGasAndCo,         // GC复合报警器
    eTypeWaterSensor,      //水浸
    eTypePlug,             //插座
    eTypeRomote,           //遥控器
    eTypePir,              // Pir
    eTypeMagnet,           //门窗磁
    eTypeDriveWay,         //车道报警器
    eTypeGlassBrokenAlarm, //玻璃碎探测报警器
    eTypeSos,              //求救器
    eTypeActuator,         //执行器
    eTypeKeyBoard,         //键盘设备
    eTypeDoorBell,         //门铃
    eTypeHorn,             //喇叭
    eTypeCamera,           //摄像头
    eTypeWeatherStation,   //温湿计
} eRfDeviceType;

//默认设备号
#ifdef SUPPORT_TYPE_HUB
#define DEFAULT_DEV_NUM	((uint8_t)0x0)
#else
#define DEFAULT_DEV_NUM	((uint8_t)0xFF)
#endif

//消息属性
//(应用GateWay 协议)
#define RfMsgPropertyNull					((uint8_t)0)
#define RfMsgPropertyNeedAck				((uint8_t)0x01)
#define RfMsgPropertyNeedUpTransfer			((uint8_t)0x02)
#define RfMsgPropertyNeedDownTransfer		((uint8_t)0x04)
#define RfMsgPropertyIsUpTransfer			((uint8_t)0x08)
#define RfMsgPropertyIsDownTransfer			((uint8_t)0x10)

//特定目标设备号
#define  HubNumber						((uint8_t)0)		//Hub
#define  TxToAllDevices 				((uint8_t)0xFF)		//all Devices
#define  TxToAllRepeaters				((uint8_t)0xFE)		//all Repeater
#define  TxToAllFireAlarms				((uint8_t)0xFD)		//all FireAlarm


#define PackNum_1 	((uint8_t)0x00)
#define PackNum_2 	((uint8_t)0x01)
#define PackNum_3 	((uint8_t)0x02)
#define PackNum_4	((uint8_t)0x03)
#define PackNum_8	((uint8_t)0x07)
#define PackNum_16  ((uint8_t)0x0F)
#define PackNum_32  ((uint8_t)0x1F)

//一级消息类型
typedef enum _eHubMsgType{
	eRfMsgNull = 0x00,
	eRfMsgWakeup, 
	eRfMsgTraverse,
	eRfMsgPair,
	eRfMsgSync,
	eRfMsgDeleteDev,
	eRfMsgSearchDev,
	eRfMsgHeartBeat,
	eRfMsgFireAlarm,		//0x08
	eRfMsgWaterSensor,
	eRfMsgPlug,
	eRfMsgRomote,
	eRfMsgPir,
	eRfMsgMagnet,
	eRfMsgDriveWay,
	eRfMsgGlassBrokenAlarm,
	eRfMsgSos,
	eRfMsgActuator,
	eRfMsgKeyBoard,
	eRfMsgDoorBell,
	eRfMsgHorn,
	eRfMsgCamera,
	eRfMsgWeatherStation,

	eRfMsgFactoyTest = 0xFF,
}eHubMsgType;

//一级消息类型
typedef enum _eInternatMsgType{
	eIntranetRadioMsgNull = 0x00,
	eIntranetRadioMsgAlarm,
	eIntranetRadioMsgCancleAlarm,
	eIntranetRadioMsgMute,
	eIntranetRadioMsgCancleMute,			//0x04
	eIntranetRadioMsgForwardTest,			//0x05
	eIntranetRadioMsgCancleTest,			//0x06
	eIntranetRadioMsgPairRequest,			//0x07
	eIntranetRadioMsgPairAck,				//0x08
	eIntranetRadioMsgAck,					//0x09	
	eIntranetRadioMsgOnlineCheck,			//0x0A
	eIntranetRadioMsgSync,					//0x0B	
	eIntranetRadioMsgCancleMsgSync,			//0x0C
	eIntranetRadioMsgFactoryPairRequest,	//0x0D
	eIntranetRadioMsgFactoryPairAck, 		//0x0E	
	eIntranetRadioMsgMasterTest,			//0x0F
	eIntranetRadioMsgStopMasterAlarm,		//0x10,
	eIntranetRadioMsgPairNewRequest,
	eIntranetRadioMsgPairNewPairAck,

	eIntranetRadioMsgSmokeAlarm = 0x41,
	eIntranetRadioMsgCoAlarm = 0x81,
	eIntranetRadioMsgGasAlarm = 0xC1,

#ifdef SUPPORT_SPEAK_LOCATION
	eIntranetRadioMsgLocationEntryway = 0xD1,//0xD1
	eIntranetRadioMsgLocationBasement,
	eIntranetRadioMsgLocationLivingroom,
	eIntranetRadioMsgLocationDiningroom,
	eIntranetRadioMsgLocationKitchen,
	eIntranetRadioMsgLocationHallway,
	eIntranetRadioMsgLocationMasterbedroom,
	eIntranetRadioMsgLocationKidsroom,
	eIntranetRadioMsgLocationGuestroom,
	eIntranetRadioMsgLocationDen,
	eIntranetRadioMsgLocationFamilyroom,
	eIntranetRadioMsgLocationStudy,
	eIntranetRadioMsgLocationOffice,
	eIntranetRadioMsgLocationWorkshop,
	eIntranetRadioMsgLocationUtilityroom,
	eIntranetRadioMsgLocationAttic,
#endif

}eInternatMsgType;

typedef struct _IntranetRadioMsgTag{
	uint8_t DeviceId[RfIdLength];
	eInternatMsgType MsgType;
	uint8_t MsgCrc;
}IntranetRadioMsgTag;

typedef enum _ePairedMode
{
	ePairedNull = 0,
	ePairedHub = 0xA1,
	ePairedIntranet = 0xC2,
	ePairedResetId = 0xB3,
}ePairedMode;


/**
 * @brief 射频参数
 * 
 */
typedef struct _RadioDevTag{
	uint8_t RadioDevId[RfIdLength];
	uint8_t RadioUdid[RfIdLength];
	eRfDeviceType DevType;
	eRfDeviceType FatherDevType;
	eProductType ProductType;
	uint8_t SelfDevNum;
	uint8_t FatherDevNum;
	ePairedMode PairMode;
	// eRandSendMsgProperty RandSendProperty;
	// eRandSendMsgProperty RxRandSendProperty;
	// eRandSendMsgState RandSendState;
}RadioDevTag;

//playload帧结构
typedef struct _RadioMsgTag{
	uint8_t DeviceId[RfIdLength];
	uint8_t DeviceNumber;
	uint8_t TargetNumber;
	eRfDeviceType DeviceType;
	eHubMsgType MsgLevel1;
	uint8_t MsgLevel2;
	uint8_t MsgProperty;		//9
	uint8_t MsgData1;
	uint8_t MsgData2;	
	uint8_t MsgData3;
	uint8_t MsgData4;	
	uint8_t MsgCrc;
}RadioMsgTag;

//Send Rf Msg参数结构
typedef struct _RfSendMsgParamer
{
	uint8_t MsgTargetNum;
	eHubMsgType MsgLevel1Type;
	uint8_t MsgLevel2Type;
	uint8_t Property;
	uint8_t Data1;
	uint8_t Data2;
	uint8_t Data3;
	uint8_t Data4;
	eRadioInitMode Mode;
	uint8_t PackNum;
}RfSendMsgParamer;

//Send Rf Msg States结构体
typedef enum _eRfSendMsgStates
{
	eSendRfMsgIdle = 0x00,
	eSendRfMsgStart,
	eSendRfMsgSuccess,
	eSendRfMsgFail,
}eRfSendMsgStates;

//Search father node States结构体
typedef enum _eSearchFatherNodeStates
{
	eSearchNodeIdle = 0x00,
	eSearchNodeStart,
	eSearchNodeWait,
	eSearchNodeSuccess,
	eSearchNodeFail,
}eSearchFatherNodeStates;

//Rand Send Msg Property
typedef enum _eRandSendMsgProperty{
	eRandSendProperty_Null = 0,
	eRandSendProperty_Level_1,
	eRandSendProperty_Level_2,
	eRandSendProperty_Level_3,
	eRandSendProperty_Level_4,
	eRandSendProperty_Level_5,
	eRandSendProperty_Level_6,
	eRandSendProperty_Level_7,
	eRandSendProperty_Level_8,
	eRandSendProperty_Level_9,
	eRandSendProperty_Level_10,
	eRandSendProperty_Level_11,
	eRandSendProperty_Level_12,
	

	eRandSendProperty_Level_Max = 0xFF,
}eRandSendMsgProperty;

//Rand Send Msg State
typedef enum _eRandSendMsgState{
	eRandSend_Valid = 0,
	eRandSend_Invalid,
	eRandSend_Wait,
	eRandSend_Busy,
}eRandSendMsgState;



//子设备数据结构体
typedef struct _ChildDev{
	uint8_t SelfDevNum;
	uint8_t FatherDevNum;
	eRfDeviceType DevType;
	eProductType ProductType;
}ChildDev;





void RadioService(void);
uint8_t* GetRadioID(void);
void SetRadioID(const unsigned char id[4]);
void IntranetSend(unsigned char mode, unsigned char pack_num, unsigned char cmd);
void SendHubData(unsigned char mode, unsigned char pack_num, const unsigned char cmd1, const unsigned char cmd2, unsigned char *data);
#endif
