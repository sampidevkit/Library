#ifndef MCC_USB_DEVICE_VCP1_H
#define	MCC_USB_DEVICE_VCP1_H

#include "Common/LibDef.h"
#include "mcc_usb_device_vcp.h"

extern port_rxbuf_t Vcp1RxBuf;
/* ******************************************************* EXTERNAL PROTOTYPE */
void BridgePort1_TxdLedSetState(bool logic);
void BridgePort1_RxdLedSetState(bool logic);
void BridgePort1_DtrPinSetState(bool logic);
void BridgePort1_Enable(uint32_t dwDTERate);
void BridgePort1_Disable(void);
bool BridgePort1_IsTxReady(void);
bool BridgePort1_IsTxDone(void);
bool BridgePort1_IsRxReady(void);
void BridgePort1_WriteByte(uint8_t b);
uint8_t BridgePort1_ReadByte(void);
/* ************************************************************************** */
public void VCP1_Init(uint8_t portIdx);
public bool VCP1_IsTxReady(void);
public bool VCP1_IsTxDone(void);
public bool VCP1_IsRxReady(void);
public void VCP1_WriteByte(uint8_t b);
public uint8_t VCP1_ReadByte(void);

#endif