#ifndef CRC_H
#define CRC_H

#include "common/libdef.h"

public uint32_t crc32(const uint8_t *pData, uint32_t len)
public uint16_t crc16(const uint8_t *pData, uint16_t len);
public uint8_t checksum(uint8_t *pData, uint8_t len);

#endif
