#ifndef SYSTEMTICK_H
#define SYSTEMTICK_H

#include <stdint.h>
#include <stdbool.h>

#ifndef TICK_PER_MS
#define TICK_PER_MS                         12000U
#endif

#ifndef TICK_PER_US
#define TICK_PER_US                         12U
#endif

#define Tick_Reset(cxt)                     cxt.Over=1
#define Tick_Timer_Reset(cxt)               cxt.Over=1

typedef __UINT32_TYPE__ tick_t;

typedef struct {
    bool Over;
    tick_t Begin;
    tick_t TickCount;
} tick_timer_t;

void __delay_us(tick_t us);
void __delay_ms(tick_t ms);
bool Tick_Is_Over(tick_timer_t *pTick, tick_t us);

#define Tick_Get()                          (_CP0_GET_COUNT())
#define Tick_Dif(tk1, tk2)                  ((tk1)-(tk2))
#define Tick_Dif_Us(tk1, tk2)               (((tk1)-(tk2))/TICK_PER_US)
#define Tick_Dif_Ms(tk1, tk2)               (((tk1)-(tk2))/TICK_PER_MS)
#define Tick_Is_Over_Us(pTick, us)          Tick_Is_Over(pTick, us)
#define Tick_Is_Over_Ms(pTick, ms)          Tick_Is_Over(pTick, 1000*(ms))
#define Tick_Is_Over_Sec(pTick, sec)        Tick_Is_Over(pTick, 1000000*(sec))

#define Tick_Timer_Read()                   (_CP0_GET_COUNT())
#define Tick_Timer_Is_Over_Us(pTick, us)    Tick_Is_Over(&pTick, us)
#define Tick_Timer_Is_Over_Ms(pTick, ms)    Tick_Is_Over(&pTick, 1000*(ms))
#define Tick_Timer_Is_Over_Sec(pTick, sec)  Tick_Is_Over(&pTick, 1000000*(sec))

#endif