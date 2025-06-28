#ifndef MCC_USB_DEVICE_VCP0_H
#define	MCC_USB_DEVICE_VCP0_H

#include "Common/LibDef.h"
#include "mcc_usb_device_vcp.h"

/* ******************************************************* EXTERNAL PROTOTYPE */
port_rxbuf_t *BridgePort0_Init(void);
void BridgePort0_TxdLedSetState(bool logic);
void BridgePort0_RxdLedSetState(bool logic);
void BridgePort0_DtrPinSetState(bool logic);
void BridgePort0_Enable(uint32_t dwDTERate);
void BridgePort0_Disable(void);
bool BridgePort0_IsTxReady(void);
bool BridgePort0_IsTxDone(void);
bool BridgePort0_IsRxReady(void);
void BridgePort0_WriteByte(uint8_t b);
uint8_t BridgePort0_ReadByte(void);
/* ************************************************************************** */
public void VCP0_Init(uint8_t portIdx);
public bool VCP0_IsTxReady(void);
public bool VCP0_IsTxDone(void);
public bool VCP0_IsRxReady(void);
public void VCP0_WriteByte(uint8_t b);
public uint8_t VCP0_ReadByte(void);

#endif