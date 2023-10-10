#ifndef SYSTEMTICK_H
#define SYSTEMTICK_H

#include "Common/LibDef.h"
#include "SystemTick_Cfg.h"

#ifndef SYS_TMR_BIT_LEN
#define SYS_TMR_BIT_LEN         32
#endif

#ifndef TICK_PER_SEC
#warning "TICK_PER_SEC is 12000000U"
#define TICK_PER_SEC            12000000U
#endif

#ifndef TICK_PER_MS
#warning "TICK_PER_MS is 12000U"
#define TICK_PER_MS             12000U
#endif

#ifndef TICK_PER_US
#warning "TICK_PER_US is 12U"
#define TICK_PER_US             12U
#endif

#if(SYS_TMR_BIT_LEN==32)
typedef __UINT32_TYPE__ tick_t;
#elif(SYS_TMR_BIT_LEN==16)
#ifdef USE_SYS_TMR_INTERRUPT
typedef __UINT32_TYPE__ tick_t;
#else
typedef __UINT16_TYPE__ tick_t;
#endif
#else
#error "Library doesn't support your SYS_TMR_BIT_LEN"
#endif

#define Tick_Reset(cxt)         cxt.Over=1
#define Tick_Timer_Reset(cxt)   cxt.Over=1

typedef struct {
    bool Over;
    tick_t Begin;
    tick_t TickCount;
} tick_timer_t;

bool Tick_Is_Over(tick_timer_t *pTick, tick_t t);
void __delay_sec(tick_t sec);
void __delay_ms(tick_t ms);

#ifndef SysTmr_Read
#warning "Default SysTmr_Read() is _CP0_GET_COUNT()"
#define SysTmr_Read()                       (_CP0_GET_COUNT())
#endif

#if(SYS_TMR_BIT_LEN==32)
#define Tick_Get()                          SysTmr_Read()
#define SystemTick_Init()
#elif defined(USE_SYS_TMR_INTERRUPT)
tick_t Tick_Get(void);
void SystemTick_Init(void);
#else
#define Tick_Get()                          SysTmr_Read()
#define SystemTick_Init()
#endif

#define Tick_Dif(tk1, tk2)                  ((tick_t)((tk1)-(tk2)))
#define Tick_Timer_Read()                   Tick_Get()

#if(TICK_PER_US>0)
void __delay_us(tick_t us);

#define Tick_Dif_Us(tk1, tk2)               ((tick_t)((tk1)-(tk2))/TICK_PER_US)
#define Tick_Dif_Ms(tk1, tk2)               ((tick_t)((tk1)-(tk2))/TICK_PER_MS)
#define Tick_Dif_Sec(tk1, tk2)              ((tick_t)((tk1)-(tk2))/TICK_PER_SEC)

#define Tick_Is_Over_Us(pTick, us)          Tick_Is_Over(pTick, us)
#define Tick_Is_Over_Ms(pTick, ms)          Tick_Is_Over(pTick, 1000U*(ms))
#define Tick_Is_Over_Sec(pTick, sec)        Tick_Is_Over(pTick, 1000000U*(sec))

#define Tick_Timer_Is_Over_Us(pTick, us)    Tick_Is_Over(&pTick, us)
#define Tick_Timer_Is_Over_Ms(pTick, ms)    Tick_Is_Over(&pTick, 1000U*(ms))
#define Tick_Timer_Is_Over_Sec(pTick, sec)  Tick_Is_Over(&pTick, 1000000U*(sec))
#else

#define Tick_Dif_Ms(tk1, tk2)               ((tick_t)((tk1)-(tk2))/TICK_PER_MS)
#define Tick_Dif_Sec(tk1, tk2)              ((tick_t)((tk1)-(tk2))/TICK_PER_SEC)

#define Tick_Is_Over_Ms(pTick, ms)          Tick_Is_Over(pTick, ms)
#define Tick_Is_Over_Sec(pTick, sec)        Tick_Is_Over(pTick, 1000U*(sec))

#define Tick_Timer_Is_Over_Ms(pTick, ms)    Tick_Is_Over(&pTick, ms)
#define Tick_Timer_Is_Over_Sec(pTick, sec)  Tick_Is_Over(&pTick, 1000U*(sec))

#endif

#endif