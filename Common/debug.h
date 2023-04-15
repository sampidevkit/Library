#ifndef DEBUG_H
#define DEBUG_H

#include "LibDef.h"
#include "debug_cfg.h"

#define HEX_HEADER              0x80

void debug(const char *str);
/* debug_hex(b, dgcount): display b as hex format 0xABC... or ABC...
 * b: value to be displayed
 * dgcount: number of digit
 * dgcount option (dgcount|0x80): display header 0x
 */
void debug_hex(uint32_t b, uint8_t dgcount);
void debug_hexs(uint8_t *pD, int len);
void debug_i32(int32_t b);
void debug_u32(uint32_t b);
void debug_data(uint8_t *pD, int len);
void debug_time(tm_t *time);
void debug_date(tm_t *time);

#define __dbc(c)                debug_port_write(c)
#define __dbs(str)              debug(str)
#define __dbi(x)                debug_i32(x)
#define __dbu(x)                debug_u32(x)
#define __dbh(x, dg)            debug_hex(x, 0x80|dg)
#define __dbh2(x)               debug_hex(x, 2)
#define __dbhs(str, len)        debug_hexs(str, len)
#define __dbdata(str, len)      debug_data(str, len)

#define __dbsi(str, x)          do{debug(str); debug_i32(x);}while(0)
#define __dbsu(str, x)          do{debug(str); debug_u32(x);}while(0)
#define __dbsh(str, x)          do{debug(str); debug_hex(x, 0x88);}while(0)
#define __dbsc(str, c)          do{debug(str); debug_port_write(c);}while(0)
#define __dbss(str1, str2)      do{debug(str1); debug(str2);}while(0)
#define __dbstime(str, tcxt)    do{debug(str); debug_time(&tcxt);}while(0)
#define __dbsdate(str, tcxt)    do{debug(str); debug_date(&tcxt);}while(0)
#define __dbsdata(str, pd, len) do{debug(str); debug_data(pd, len);}while(0)

#endif