#include "crc.h"

public uint32_t crc32(const uint8_t *pData, uint32_t len) // <editor-fold defaultstate="collapsed" desc="CRC32 Calculator">
{
   uint32_t i, byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   
   while (i < len) 
   {
	  int8_t j;
	   
      byte = pData[i]; // Get next byte.
      crc = crc ^ byte;
	  
      for (j = 7; j >= 0; j--) // Do eight times.
	  {    
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
	  
      i = i + 1;
   }
   
   return ~crc;
} // </editor-fold>

public uint16_t crc16(const uint8_t *pData, uint16_t len) // <editor-fold defaultstate="collapsed" desc="CRC16 Calculator">
{
    uint8_t i;
    uint16_t data;
    uint16_t crc=0xFFFF;

    if(len==0)
        return 0x0000;

    do
    {
        for(i=0, data=0xFF&*pData++; i<8; i++, data>>=1)
        {
            if((crc&1)^(data&1))
                crc=(crc>>1)^0x8408;
            else
                crc>>=1;
        }
    }
    while(--len);

    crc=~crc;
    data=crc;
    crc=(crc<<8)|(data>>8&0xFF);

    return crc;
} // </editor-fold>

public uint8_t checksum(uint8_t *pData, uint8_t len) // <editor-fold defaultstate="collapsed" desc="Calculate 8-bit sumary">
{
    uint8_t sum=0;

    while(len>0)
    {
        sum+=*pData++;
        len--;
    }

    sum=0xFF-sum;
	
	return (sum+1);
} // </editor-fold>
