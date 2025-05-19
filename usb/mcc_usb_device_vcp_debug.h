#ifndef MCC_USB_DEVICE_VCP_DEBUG_H
#define	MCC_USB_DEVICE_VCP_DEBUG_H

#include "common/libdef.h"
#include "project_cfg.h"

public void VCP_Debug_Write(uint8_t c);
public void VCP_Debug_Init(uint8_t PortIdx);
public void VCP_Debug_SetRxCallback(void (*cbfnc)(uint8_t *, uint8_t));

#endif