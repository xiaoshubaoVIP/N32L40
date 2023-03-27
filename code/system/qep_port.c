#include "qep_port.h"
#include "common.h"
#define MainTaskQueue_Length            (10)

static QFsm  super_;  //QP system
static QFsm  RFsuper_;  //RF QP system
static QEvent const *SystemQueueSto[MainTaskQueue_Length];
static QEQueue eQueue__;



void QEP_Init(QState init)
{
    QFsm_ctor_(&super_, init);
    QEQueue_init(&eQueue__, SystemQueueSto, Q_DIM(SystemQueueSto)); //QUEUE FOR EVENT
    QFsm_init(&super_, (QEvent const *)0);
}

void QEP_RfInit(QState init)
{
    QFsm_ctor_(&RFsuper_, init);
    QFsm_init(&RFsuper_, (QEvent const *)0);
}

void QEP_Handler(void)
{
    QEvent *e;

    while ((e = (QEvent *)QEQueue_get(&eQueue__)) != 0)
    {
        QFsm_dispatch(&super_, e);
#ifdef SUPPORT_RADIO_FUNCTION
        QFsm_dispatch(&RFsuper_, e);
#endif
    }
}

void QEP_Post(QSignal sig)
{
    static QEvent des[MainTaskQueue_Length];
    static uint8_t des_count;
    
    des[des_count].sig = sig;

    QEQueue_postFIFO(&eQueue__, &des[des_count]);

    des_count++;
    if (des_count >= MainTaskQueue_Length)
        des_count = 0;
}

