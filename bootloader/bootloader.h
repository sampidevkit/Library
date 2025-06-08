#ifndef BOOTLOADER_H
#define	BOOTLOADER_H

#include "common/libdef.h"
#include "project_cfg.h"

#ifndef BLD_BUFFER_SIZE
#define BLD_BUFFER_SIZE         48
#endif

/* ********************************************************* CONFIG PROTOTYPE */
void BLD_SystemReboot(void);
bool BLD_Trigger_GetState(void);
void BLD_Comm_Init(void);
bool BLD_IsRxReady(void);
bool BLD_IsTxReady(void);
bool BLD_IsTxDone(void);
void BLD_Write(uint8_t c);
uint8_t BLD_Read(void);

#define BLD_LED_LOW     0
#define BLD_LED_HIGH    1
#define BLD_LED_TOGGLE  2
void BLD_DownloadLed_SetState(uint8_t state);
void BLD_UpgradeLed_SetState(uint8_t state);
/* ***************************************************** TARGET MCU PROTOTYPE */
void Jump2App(void);
/* ************************************************************************** */
public void BootLoader_Initialize(void);
public void BootLoader_Deinitialize(void);

#endif