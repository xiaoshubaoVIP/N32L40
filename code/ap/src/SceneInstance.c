#include "SceneInstance.h"
#include "common.h"



#define SYSTEMTICKS                   ((uint32_t)0xffffffff)
#define SYSTEMIDLETICKS               SYSTEM_ONE_DAY_COUNT

static TmrHeader gTmrHeader;
static TmrHeader gTmrHeaderIdle;


static uint32_t GetTicks(void)
{
    uint32_t Ret = 0;
    //to be done
    static uint32_t lastTick10ms = 0;
    uint32_t  tick = SystemGetTick(); 
    uint32_t  dTick = (tick >= lastTick10ms) ? (tick - lastTick10ms) : (SYSTEMTICKS - lastTick10ms + tick);
    if (dTick < 1)
    {
        Ret = 0;
    }
    else
    {	//10ms
        Ret = dTick;
        lastTick10ms = tick;
    }

    //to be done
    return (Ret);
}

static uint32_t GetIdleTicks(void)
{
    uint32_t Ret = 1;
    static uint32_t lastTick = 0;
    //to be done
    uint32_t  tick = SystemGetIdleNUm();
    uint32_t  dTick = (tick >= lastTick) ? (tick - lastTick) : (SYSTEMIDLETICKS - lastTick + tick);
    if (dTick < 1)
    {
        Ret = 0;
    }
    else
    {	//10s
        Ret = dTick;
        lastTick = tick;
    }
    //to be done
    return (Ret);
}

void PollingModuleInit(void)
{
    static TmrInfo TimerMbs[TIMER_MAX_NUM];
    static TmrInfo TimerIdleMbs[TIMER_MAX_NUM];

    TmrInitHeader(&gTmrHeader, GetTicks, TimerMbs, GET_ARRAY_NUM(TimerMbs));
    //to be done,
    TmrInitHeader(&gTmrHeaderIdle, GetIdleTicks, TimerIdleMbs, GET_ARRAY_NUM(TimerIdleMbs));
    //to be done
}

TmrHeader* GetTmrHd(void)
{
    return &gTmrHeader;
}

TmrHeader* GetTmrIdleHd(void)
{
    return &gTmrHeaderIdle;
}

void SystemPolling(void)
{
    Polling(GetTmrHd());
}

void SystemPollingIdle(void)
{
    Polling(GetTmrIdleHd());
}




