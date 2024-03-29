#ifndef WDT_H
#define	WDT_H

#include "Common/LibDef.h"
#include "Wdt_Cfg.h"

#ifndef WDT_STT_LED_Toggle
#define WDT_STT_LED_Toggle()
#endif

#ifndef WDT_STT_LED_SetHigh
#define WDT_STT_LED_SetHigh()
#endif

#ifndef WDT_STT_LED_SetLow
#define WDT_STT_LED_SetLow()
#endif

public void SYS_SoftReset(void);
public void WDT_Tasks(void *);
public void WDT_Enable(void);
public void WDT_Disable(void);

#endif