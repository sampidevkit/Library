#ifndef UTIL_H
#define UTIL_H

#include "Common/LibDef.h"

typedef struct {
    uint8_t idx;
    const uint8_t *pStr;
} strcmp_t;

#define mem_init(x, v)        memset((void *)x, v, membersof(x))
#define StrCmp_Init(cxt, src) do{cxt.idx=0; cxt.pStr=(uint8_t*)src;}while(0)

public int8_t is_number(uint8_t c);

public void mymemset(uint8_t *pD, uint8_t init, uint16_t len);
public void mymemcpy(uint8_t *pDes, const uint8_t *pSrc, uint16_t size);
public const char *mystrchr(const char *pStr, char c);
public void mystrcpy(char *pDes, const char *pSrc);
public void u32str(uint32_t b, char *pDes);
public void i32str(int32_t b, char *pDes);
public uint32_t make32(const uint8_t *p); // big endian
public void unmake32(uint32_t Value, uint8_t *pArr); // big endian
public uint16_t make16(const uint8_t *p); // big endian
public void unmake16(uint16_t Value, uint8_t *pArr); // big endian
/* ******************************************************** STRING PROCESSING */
public void random8(uint8_t *des, int len, uint8_t min, uint8_t max);
public void random8s(int8_t *des, int len, int8_t min, int8_t max);
public size_t slen(const char *pStr);
public uint8_t UpperCase(uint8_t Data);
public uint8_t LowerCase(uint8_t Data);
public void str_uppercase(uint8_t *pData);
public void str_lowercase(uint8_t *pData);
public bool FindString(uint8_t c, size_t *pIdx, const char *pStrSample);
public uint16_t Copy_Str2Break(const uint8_t *pDatain, uint8_t break_byte, uint8_t *pDataout);
public uint16_t str_len2break(const uint8_t *pData, uint8_t break_byte);
public bool str_cmp(const char *pDatain, const char *pSample);
public bool str_cmp_without_case(const uint8_t *pDatain, int Len, const uint8_t *pSample);
public int str_remove(char *p, char c);
public void str_sremove(char *str, char c, uint8_t amount);
public uint16_t str_nremove(char *p, const char *c);
public uint8_t str_is_number(char c);
public uint16_t str_count(const char *p, char c);
public int str_1st_index(const char *p, char c);
public int str_n_index(const char *p, char c, int count);
public const char *str_1st_contain(const char *str, const char *sub);
public int str_1st_contain_idx(const char *sub, const char *str);
public const char *str_last_contain(const char *sub, const char *str);
public int str_last_contain_idx(const char *sub, const char *str);
public const char *str_n_contain(const char *sub, const char *str, int count);
public int str_sub(char *des, const char *src, char c1, int count1, int offset1, char c2, int count2, int offset2);
public int str_sub_between_2sub(char *des, const char *src, const char *sub1, const char *sub2);
public char *str_first(char *p, char c);
public char *str_last(char *p, char c);
public int32_t IntParse(const char *c); // NULL input is 0
public uint32_t UIntParse(const char *c); // NULL input is 0
public uint32_t HexParse(const char *c); // NULL input is 0
public float FloatParse(const char *c);
public bool is_hex_data(uint8_t b);
public bool is_printable(uint8_t c);
public bool is_visible(char c);
public int8_t StrCmp(strcmp_t *pCxt, uint8_t c);
public int32_t Float2Int(float f);
public bool findSString(char *pDatain, const char *pSample);
public bool remove_1st_Substring(char *str, const char *sub);
public void delSString(char *pDatain, const char *pSample);

#define str_sub_between(des, src, c1, count1, c2, count2) str_sub((char *)(des), (const char *)(src), c1, count1, 1, c2, count2, -1)

/* ************************************************** CRYPTOGRAPHY PROCESSING */
public uint16_t crc_ccitt(uint16_t crc, const uint8_t *buf_p, size_t size);
public uint16_t Crc16_Calc(const uint8_t *pData, uint16_t len);
public uint16_t Mask(uint8_t *pData, uint16_t len);
public uint16_t UnMask(uint8_t *pData, uint16_t len);
public uint8_t CalcSum8(void *pData, uint16_t len);
/* ****************************************************** DATA TYPE CONVERTER */
public uint32_t Swap_nBits(uint32_t Value, uint8_t nBit);
public uint16_t Swap_2Bytes(uint16_t Value);
public uint32_t swap_endian(uint32_t num);
public void swap_endian_array(uint32_t *pNum, int len);
public uint32_t set_bigendian32(uint32_t num);
public uint16_t set_bigendian16(uint16_t num);
public void Convert16to2x8(uint16_t Value, uint8_t *pArr);
public void Convert32to4x8(uint32_t Value, uint8_t *pArr);
public uint16_t Convert2x8to16(const uint8_t *pArr);
public uint32_t Convert4x8to32(const uint8_t *pArr);
public int32_t ConvertStr2Integer(const uint8_t *pArr);
public int8_t chr2int(uint8_t c);
public uint32_t StrHex2Int(uint8_t *p);
public int BinSearch(const uint8_t *pSource, int Size);
public void PrintHex(const uint8_t *pSource, int Size, int NumOfCol);
public uint8_t bits_reverse_8(uint8_t value);
public uint32_t bits_reverse_16(uint32_t value);
public uint32_t bits_reverse_32(uint32_t value);
public uint32_t bits_mask_32(int width);
public uint32_t bits_insert_32(uint32_t dst, int position, int size, uint32_t src);
public char Bcd2AHex(uint8_t bcd);
public uint8_t AHex2Bcd(char ahex);
public int Array2AHex(char *des, const uint8_t *src, int len);
public int AHex2Array(uint8_t *des, const char *src, int len);
///* **************************************************************** DATE TIME */
extern const uint8_t DayOfMonth[12];
public bool CheckLeapYear(uint16_t Year);
public uint8_t Calc_DayOfWeek(uint32_t Day, uint32_t Month, uint32_t Year);
public uint8_t Dec2BCD(uint8_t hexvalue);
public uint8_t BCD2Dec(uint8_t bcdvalue);

public uint16_t iir(uint32_t *prev, uint16_t current, uint8_t hardness);

public bool getBit(const uint8_t *arr, uint16_t idx);
public void setBit(uint8_t *arr, uint16_t idx, bool value);
public uint32_t count_divisible(uint32_t Ts0, uint32_t Ts1, uint32_t k);
public int32_t value_boundary(int32_t current, int32_t min, int32_t max);
public uint32_t address_boundary(uint32_t current, uint32_t min, uint32_t max, uint8_t align);
public int16_t make_int15(int16_t a);
    
#endif
    