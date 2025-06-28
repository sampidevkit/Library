#ifndef CELL_PWRCTRL_H
#define	CELL_PWRCTRL_H

#include "common/libdef.h"
#include "project_cfg.h"

#define DEFAULT_CELL_VCEL_STARTUP       3200 // mV
#define DEFAULT_CELL_VCEL_PWRDN         2700 // mV
#define DEFAULT_CELL_VAUX_MIN           1350 // mV
#define DEFAULT_CELL_VAUX_TIMEOUT       10000 // ms
#define DEFAULT_CELL_ONOFF_PULSE        5500 // ms
#define DEFAULT_CELL_ONOFF_TIMEOUT      3500 // ms
#define DEFAULT_CELL_PWR_DISCHR_WAIT    3000 // ms

typedef __PACKED_STRUCT
{
    uint16_t CELL_VCEL_STARTUP;
    uint16_t CELL_VCEL_PWRDN;
    uint16_t CELL_VAUX_MIN;
    uint16_t CELL_VAUX_TIMEOUT;
    uint16_t CELL_ONOFF_PULSE;
    uint16_t CELL_ONOFF_TIMEOUT;
    uint16_t CELL_PWR_DISCHR_WAIT;
}
cell_para_t;

typedef union {
    uint8_t val;

    struct {
        unsigned dischrErr : 1; // VCEL discharge error
        unsigned vcelRdy : 1; // VCEL ready
        unsigned vauxRdy : 1; // VAUX ready
        unsigned swRdy : 1; // Software ready
        unsigned reboot : 1; // Reboot state
        unsigned rfu : 2; // Unused
        unsigned busy : 1; // Internal process is in progress
    };
} cell_stt_t;

#define Cell_Ready()               ((CellState.val&0b10011111)==0b10001110)
#define Cell_IsBusy()              (CellState.busy==1)
#define Cell_PwrCtrl_GetState()    (CellState)

void Cell_PwrCtrl_Init(void);
void Cell_PwrCtrl_Tasks(void*);
void Cell_TurnOn(bool dischrFirst, uint8_t Retry);
void Cell_TurnOff(void);
bool Cell_PwrCtrl_IsError(void);

/* ************************************************** CONFIGURATION PROTOTYPE */
const cell_para_t *CELL_HAL_Init(void);
void CELL_PWREN_SetState(bool);
void CELL_ONOFF_SetState(bool);
uint16_t CELL_VCEL_Get(void);
uint16_t CELL_VAUX_Get(void);

#endif
