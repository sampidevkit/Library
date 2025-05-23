#ifndef MCC_USB_DEVICE_VCP_H
#define	MCC_USB_DEVICE_VCP_H

#include "usb/usb_device.h"
#include "usb/usb_device_cdc.h"
#include "system/gpio.h"
#include "system/system_tick.h"
#include "system/task_manager.h"
#include "project_cfg.h"

typedef struct {
    void (*TxdLedSetState)(bool logic);
    void (*RxdLedSetState)(bool logic);
    bool (*IsTxReady)(void);
    bool (*IsTxDone)(void);
    bool (*IsRxReady)(void);
    void (*WriteByte)(uint8_t b);
    uint8_t(*ReadByte)(void);
} port_fnc_t;

typedef struct {
    uint8_t data[CDC_DATA_IN_EP_SIZE];
    uint8_t len;
    uint8_t next;
    tick_timer_t tkData;
} port_txbuf_t;

typedef struct {
    const uint16_t size;
    uint16_t head;
    uint16_t tail;
    uint8_t *data;
} port_rxbuf_t;

typedef union {
    uint8_t val;

    struct {
        unsigned opened : 1; // port status
        unsigned mode : 1; // port mode: 0-vcp, 1-uart bridge
        unsigned index : 3; // port index
        unsigned rfu : 3; // unused
    };
} port_ctrl_t;

typedef struct {
    port_fnc_t bridgePort; // bridge port functions
    port_txbuf_t txBuf; // TX buffer
    port_rxbuf_t *rxBuf; // RX buffer
    tick_timer_t tkTxLed; // TX led timer
    tick_timer_t tkRxLed; // RX led timer
    port_ctrl_t Ctrl; // port control
} port_cxt_t;

extern port_cxt_t VcpCxt[NUM_OF_CDC_PORTS];

public void VCP_Init(void);
public bool VCP0_IsTxReady(void);
public bool VCP0_IsTxDone(void);
public bool VCP0_IsRxReady(void);
public void VCP0_WriteByte(uint8_t b);
public uint8_t VCP0_ReadByte(void);
    
#endif
