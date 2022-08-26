#include "Bootloader.h"
#include "libcomp.h"

#ifdef USE_DEFAULT_BUTTON
#define BLD_Trigger_GetState() MOD_Button_GetState()
#elif !defined(BLD_Trigger_GetState)
#error "Please define BLD_Trigger_GetState() macro"
#else
#warning "Use custom macro BLD_Trigger_GetState()"
#endif

void Bootloader_Tasks(void *pvParameters)
{
#ifdef USE_RTOS
    while(1)
    {
#endif
        WATCHDOG_TimerClear();
        
#ifdef BLD_Custom_Tasks
        BLD_Custom_Tasks();
#endif
        
        if(BLD_Trigger_GetState()==HOLD_PRESS)
        {
            SYSKEY=0x0; //write invalid key to force lock
            SYSKEY=0xAA996655; //write Key1 to SYSKEY
            SYSKEY=0x556699AA; //write Key2 to SYSKEY
            RSWRSTSET=1;
            unsigned int dummy=RSWRST;
        }

#ifdef USE_RTOS
        vTaskDelay(1/portTICK_PERIOD_MS);
    }
#endif
}