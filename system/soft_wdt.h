#ifndef SOFTWDT_H
#define	SOFTWDT_H

#include "common/libdef.h"
#include "project_cfg.h"
#include "gpio.h"
#include "task_manager.h"

extern volatile uint32_t softWdtCount;
extern volatile uint32_t softWdtCountMax;

/* **************************************************************** PROTOTYPE */
void softWDT_TmrSetInterruptHandler(simple_fnc_t fnc);
void softWDT_HwWdtClear(void);
/* ************************************************************************** */
void softWDT_Init(uint32_t tout, gpio_pin_t led, simple_fnc_t cb);
public void SYS_SoftReset(void);
public void softWDT_Tasks(void *);
public void softWDT_Enable(void);
public void softWDT_Disable(void);
public void softWDT_SetLedIndicator(gpio_pin_t pin);
public void softWDT_SetCbBeforeReset(simple_fnc_t fnc);

public void softWDT_Isr(void); // Call this function in timer interrupt

#define NO_WDT      0xFFFFFFFF
#define ClrWdt()    TaskManager()

#endif