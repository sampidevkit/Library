#ifndef BUTTONS_H
#define BUTTONS_H

#include "System/TickTimer.h"
#include "System/TickCounter.h"
#include "Common/LibDef.h"

#ifdef COMMON_LIB_CFG
#include "Common_Lib_Cfg.h"
#else
#include "Buttons_Cfg.h"
#endif

typedef enum BUTTON_MODE {
    TIME_MODE_EDGE_L2H = 0,
    TIME_MODE_EDGE_H2L,
    COUNT_MODE_EDGE_L2H,
    COUNT_MODE_EDGE_H2L
} detect_mode_t;

typedef enum BUTTON_TASKS {
    BT_UNPRESSED = 0,
    BT_CHECK_AGAIN,
    BT_CHECK_NOISE,
    BT_PRESSED,
} button_task_t;

typedef bool (*signal_handler_t)(void);

typedef struct {
    detect_mode_t Mode;
    button_task_t BtTask;
    uint16_t DetectTime;
    signal_handler_t SignalFunc;
    tick_timer_t TickTimer;
    tick_counter_t TickCounter;
} button_t;

#define Button_ChangeMode(pBt, Mode) pBt.Mode=Mode
#define Button_ChangeDetectTime(pBt, Time) pBt.DetectTime=Time
#define Button_ChangeDetect(pBt, Time) pBt.DetectTime=Time

public bool Button_Is_Pressed(button_t *pBt);
public void Button_Reset(button_t *pBt);
public void Button_Init(button_t *pBt, signal_handler_t SignalFunc,
        detect_mode_t Mode, uint16_t DetectTime);

#endif