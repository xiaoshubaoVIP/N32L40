#include "common.h"
#include "bsp_config.h"
#include "modules_config.h"
#include "app_main.h"

const unsigned char mcu_version[] = VERSION;

static void application_init(void)
{
    PollingModuleInit();
    KeyInit();
    QEP_Init(SystemObjectInit);
#ifdef SUPPORT_RADIO_FUNCTION
    QEP_RfInit(RfObjectInit);
#endif
}

INSER_FUNC(StartHandle)
{
    printf("StartHandleA\n");
    SOURCE_FUNC_CALL(StartHandle);
}

int main(void)
{
    board_init();
    application_init();

    while (1)
    {
        SystemPolling();
        KeyProcess();
        SECTION_CALL(APP_TASK);
        QEP_Handler();
        EnteryLowPower();
    }
}
