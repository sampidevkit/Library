#ifndef MCC_USB_DEVICE_VCP_H
#define	MCC_USB_DEVICE_VCP_H

#include "usb/usb_device_cdc.h"
#include "system/gpio.h"
#include "system/system_tick.h"
#include "system/task_manager.h"
#include "project_cfg.h"

typedef struct {
    uint8_t data[CDC_DATA_OUT_EP_SIZE];
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

typedef struct {
    port_txbuf_t txBuf; // TX buffer
    tick_timer_t tkTxLed; // TX led timer
    tick_timer_t tkRxLed; // RX led timer
    uint8_t index; // Port index
    bool opened; // Port status
} port_cxt_t;

extern port_cxt_t VcpCxt[NUM_OF_CDC_PORTS];

public void VCP_Init(void);

#endif
