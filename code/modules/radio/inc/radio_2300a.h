#ifndef _RADIO_2300A_H_
#define _RADIO_2300A_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "system.h"

#define HUB_PLAYLOAD_LEN ((uint8_t)15)
#define INTRANET_PLAYLOAD_LEN ((uint8_t)6)

#define INFINITE 0xFFFFFFFF

/* RF state machine */
typedef enum
{
    RF_STATE_IDLE = 0,
    RF_STATE_RX_START,
    RF_STATE_RX_WAIT,
    RF_STATE_RX_DONE,
    RF_STATE_RX_TIMEOUT,
    RF_STATE_TX_START,
    RF_STATE_TX_WAIT,
    RF_STATE_TX_DONE,
    RF_STATE_TX_TIMEOUT,
    RF_STATE_ERROR,
} eRFStatus;

/* RF process function results */
typedef enum
{
    RF_IDLE = 0,
    RF_BUSY,
    RF_RX_DONE,
    RF_RX_TIMEOUT,
    RF_TX_DONE,
    RF_TX_TIMEOUT,
    RF_ERROR,
} eRFResult;

typedef enum
{
    eRadioCfgTxMode = 0,
    eRadioCfgRxMode,
    eRadioCfgTestFskTxMode,
    eRadioCfgTestOokTxMode,
    eRadioCfgTestRxMode,
} eRadioCfgMode;

// radio rf mode
typedef enum _eRadioInitMode
{
    LowPowerHubMode = 0,
    NormalHubMode,
    LowPowerIntranetMode,
    NormalIntranetMode,
    NormalIntranetSpecialMode,
    FactoryPairMode,

    FskTestMode,
    OokTestMode,
    RxTestMode,
} eRadioInitMode;

struct strRadio
{
    uint8_t PlayloadLen;
    eRadioInitMode InitMode;
    void (*Init)(eRadioInitMode InitMode);
    void (*Config)(eRadioCfgMode CfgMode);
    void (*SetStates)(eRFStatus States);
    eRFStatus (*GetStates)(void);
    uint8_t (*GetIrqFlag)(void);
    void (*Send)(uint8_t buf[], uint16_t len, uint16_t packnum, uint32_t timeout);
    void (*WaitSendDone)(void);
    BOOL (*Rx)(uint8_t buf[], uint16_t len);
    eRFResult (*Process)(void);
};

eRFStatus RF_GetStatus(void);
extern struct strRadio Radio;

#ifdef __cplusplus
}
#endif

#endif
