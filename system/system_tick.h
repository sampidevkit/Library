#ifndef SYSTEM_TICK_H
#define SYSTEM_TICK_H

#include "common/libdef.h"
#include "project_cfg.h"

#if defined(__XC32__) || defined(USE_SYSTEM_TICK_32BIT)
#define TICK_32BIT
#define tick_t  uint32_t
#else
#define tick_t  uint16_t
#endif

typedef enum {
    SEC,
    MS,
#ifdef TICK_32BIT
    US
#endif
} tick_timer_type_t;

typedef struct {
    tick_t TICK_PER_SEC;
    tick_t TICK_PER_MS;
#ifdef TICK_32BIT
    tick_t TICK_PER_US;
#endif
} tick_para_t;

typedef struct {
    volatile uint8_t Timeout;
    tick_t Start;
    tick_t Duration;
} __attribute_packed__ tick_timer_t;

/* **************************************************************** PROTOTYPE */
const tick_para_t *Tick_Timer_Hal_Init(void);
tick_t Tick_Timer_Get_TickVal(void);
/* ************************************************************************** */
public void Tick_Timer_Init(simple_fnc_t pFnc);
public void Tick_Timer_SetFncCallInDelay(simple_fnc_t pFnc);
public void Delay(tick_t Time, tick_timer_type_t TickType); // Do not use in multi-task mode
public bool Tick_Timer_Is_Over(tick_timer_t *pTick, tick_t Time, tick_timer_type_t TickType);

void Tick_Reset(tick_timer_t *pTick);
tick_t Tick_Per_Sec(void);
tick_t Tick_Per_Ms(void);
tick_t Tick_GetTimeMs(void);
tick_t Tick_GetTimeSec(void);
tick_t Tick_Dif(tick_t Tk1, tick_t Tk0, tick_timer_type_t TickType);

#define Tick_Timer_Reset(x)                 Tick_Reset(&(x))
#define Tick_Get()                          Tick_Timer_Get_TickVal()
#define Tick_DifMs(Tk0, Tk1)                Tick_Dif(Tk0, Tk1, MS)
#define Tick_DifSec(Tk0, Tk1)               Tick_Dif(Tk0, Tk1, SEC)
#define __delay_ms(t)                       Delay_Ms(t)
#define __delay_sec(t)                      Delay_Sec(t)
#define Tick_Timer_Is_Over_Sec(pTick, Time) Tick_Timer_Is_Over(&pTick, Time, SEC)
#define Tick_Timer_Is_Over_Ms(pTick, Time)  Tick_Timer_Is_Over(&pTick, Time, MS)

#ifdef TICK_32BIT
tick_t Tick_Per_Us(void);
tick_t Tick_GetTimeUs(void);
#define Tick_DifUs(Tk0, Tk1)                Tick_Dif(Tk0, Tk1, US)
#define __delay_us(t)                       Delay_Us(t)
#define Tick_Timer_Is_Over_Us(pTick, Time)  Tick_Timer_Is_Over(&pTick, Time, US)
#endif

#endif