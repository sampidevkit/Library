#ifndef SYSTEM_TICK_H
#define SYSTEM_TICK_H

#include "common/libdef.h"
#include "project_cfg.h"

#if defined(__XC32__) || defined(USE_SYSTEM_TICK_32BIT)
#define tick_t  uint32_t

typedef enum {
    US,
    MS,
    SEC
} tick_timer_type_t;
#else
#define tick_t  uint16_t

typedef enum {
    MS,
    SEC
} tick_timer_type_t;
#endif

typedef struct {
    tick_t Start;
    tick_t Duration;
} __attribute_packed__ elapse_t;

typedef struct {
    volatile uint8_t Timeout;
    tick_t Start;
    tick_t Duration;
} __attribute_packed__ tick_timer_t;

extern const tick_t TICK_PER_SEC;
extern const tick_t TICK_PER_MS;

#if defined(__XC32__) || defined(USE_SYSTEM_TICK_US)
extern const tick_t TICK_PER_US;
#endif

/* **************************************************************** PROTOTYPE */
tick_t Tick_Timer_Get_TickVal(void);
/* ************************************************************************** */
public void Tick_Timer_SetFncCallInDelay(simple_fnc_t pFnc);
public void Delay(tick_t Time, tick_timer_type_t TickType); // Do not use in multi-task mode
public bool Tick_Timer_Is_Over(tick_timer_t *pTick, tick_t Time, tick_timer_type_t TickType);

static inline void Tick_Reset(tick_timer_t *pTick) {
    pTick->Timeout = 2;
}

#define Tick_Timer_Init()                   Tick_Timer_SetFncCallInDelay(NULL)
#define Tick_Timer_Reset(x)                 Tick_Reset(&(x))
#define Delay_Us(Time)                      Delay(Time, US)
#define delay_us(t)                         Delay_Us(t)

#ifndef __XC8__
#define __delay_us(t)                       Delay_Us(t)
#endif

#define Tick_GetTimeUs()                    (Tick_Timer_Get_TickVal() / TICK_PER_US)
#define Tick_Timer_Is_Over_Us(pTick, Time)  Tick_Timer_Is_Over(&pTick, Time, US)

#define Delay_Ms(Time)                      Delay(Time, MS)
#define Delay_Sec(Time)                     Delay(Time, SEC)
#define delay_ms(t)                         Delay_Ms(t)
#define delay_sec(t)                        Delay_Sec(t)

#ifndef __XC8__
#define __delay_ms(t)                       Delay_Ms(t)
#endif

#define __delay_sec(t)                      Delay_Sec(t)

#define Tick_Timer_Read()                   Tick_Timer_Get_TickVal()
#define Tick_GetTimeMs()                    (Tick_Timer_Get_TickVal() / TICK_PER_MS)
#define Tick_GetTimeSec()                   (Tick_Timer_Get_TickVal() / TICK_PER_SEC)
#define Tick_Timer_Is_Over_Ms(pTick, Time)  Tick_Timer_Is_Over(&pTick, Time, MS)
#define Tick_Timer_Is_Over_Sec(pTick, Time) Tick_Timer_Is_Over(&pTick, Time, SEC)
#define Elapse_Create(name, t)              do{name.Start=Tick_Timer_Get_TickVal(); name.Duration=t*TICK_PER_MS;}while(0)
#define Elapse_Get(name)                    ((Tick_Timer_Get_TickVal()-name.Start)>name.Duration?1:0)
#define Elapse_Update(name)                 name.Start=Tick_Timer_Get_TickVal()

#define Tick_Get()                          Tick_Timer_Get_TickVal()
#define Tick_GetUs(ms)                      (ms*TICK_PER_US)
#define Tick_Dif_Us(tk1, tk2)               (((tk1)-(tk2))/TICK_PER_US)

#define Tick_GetMs(ms)                      (ms*TICK_PER_MS)
#define Tick_GetSec(ms)                     (ms*TICK_PER_SEC)
#define Tick_Dif(tk1, tk2)                  (tick_t)((tk1)-(tk2))
#define Tick_Dif_Ms(tk1, tk2)               (tick_t)(((tk1)-(tk2))/TICK_PER_MS)
#define Tick_Dif_Sec(tk1, tk2)              (tick_t)(((tk1)-(tk2))/TICK_PER_SEC)

#endif