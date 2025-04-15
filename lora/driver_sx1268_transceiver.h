#ifndef DRIVER_SX1268_TRANSCEIVER_H
#define DRIVER_SX1268_TRANSCEIVER_H

#include "sx1268.h"

/**
 * @brief  sx1268 interrupt test irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t sx1268_interrupt_test_irq_handler(void);

/**
 * @brief  send test
 * @return status code
 *         - 0 success
 *         - 1 test failed
 * @note   none
 */
uint8_t sx1268_send_test(void);

/**
 * @brief     receive test
 * @param[in] s timeout
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t sx1268_receive_test(uint32_t s);

#endif
