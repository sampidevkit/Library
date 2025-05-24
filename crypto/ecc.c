#include "ecc.h"

typedef unsigned int uint;

#define CONCAT1(a, b) a ## b
#define CONCAT(a, b) CONCAT1(a, b)

#define Curve_P_8 {  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

#define Curve_P_16 {  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF }

#define Curve_P_24 {  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF }

#define Curve_P_32 {  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF }

#define Curve_P_48 {  0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF }

#define Curve_B_16 {  0xD3, 0x5E, 0xEE, 0x2C, 0x3C, 0x99, 0x24, 0xD8, 0x3D, \
    0xF4, 0x79, 0x10, 0xC1, 0x79, 0x75, 0xE8 }

#define Curve_B_24 {  0xB1, 0xB9, 0x46, 0xC1, 0xEC, 0xDE, 0xB8, 0xFE, 0x49, \
    0x30, 0x24, 0x72, 0xAB, 0xE9, 0xA7, 0x0F, 0xE7, 0x80, 0x9C, 0xE5, 0x19, \
    0x05, 0x21, 0x64 }

#define Curve_B_32 {  0x4B, 0x60, 0xD2, 0x27, 0x3E, 0x3C, 0xCE, 0x3B, 0xF6, \
    0xB0, 0x53, 0xCC, 0xB0, 0x06, 0x1D, 0x65, 0xBC, 0x86, 0x98, 0x76, 0x55, \
    0xBD, 0xEB, 0xB3, 0xE7, 0x93, 0x3A, 0xAA, 0xD8, 0x35, 0xC6, 0x5A }

#define Curve_B_48 {  0xEF, 0x2A, 0xEC, 0xD3, 0xED, 0xC8, 0x85, 0x2A, 0x9D, \
    0xD1, 0x2E, 0x8A, 0x8D, 0x39, 0x56, 0xC6, 0x5A, 0x87, 0x13, 0x50, 0x8F, \
    0x08, 0x14, 0x03, 0x12, 0x41, 0x81, 0xFE, 0x6E, 0x9C, 0x1D, 0x18, 0x19, \
    0x2D, 0xF8, 0xE3, 0x6B, 0x05, 0x8E, 0x98, 0xE4, 0xE7, 0x3E, 0xE2, 0xA7, \
    0x2F, 0x31, 0xB3 }

#define Curve_G_16 { \
    {0x86, 0x5B, 0x2C, 0xA5, 0x7C, 0x60, 0x28, 0x0C, 0x2D, 0x9B, 0x89, 0x8B, \
        0x52, 0xF7, 0x1F, 0x16}, \
    {0x83, 0x7A, 0xED, 0xDD, 0x92, 0xA2, 0x2D, 0xC0, 0x13, 0xEB, 0xAF, 0x5B, \
        0x39, 0xC8, 0x5A, 0xCF}}

#define Curve_G_24 { \
    {0x12, 0x10, 0xFF, 0x82, 0xFD, 0x0A, 0xFF, 0xF4, 0x00, 0x88, 0xA1, 0x43, \
        0xEB, 0x20, 0xBF, 0x7C, 0xF6, 0x90, 0x30, 0xB0, 0x0E, 0xA8, 0x8D, \
        0x18}, \
    {0x11, 0x48, 0x79, 0x1E, 0xA1, 0x77, 0xF9, 0x73, 0xD5, 0xCD, 0x24, 0x6B, \
        0xED, 0x11, 0x10, 0x63, 0x78, 0xDA, 0xC8, 0xFF, 0x95, 0x2B, 0x19, \
        0x07}}

#define Curve_G_32 { \
    {0x96, 0xC2, 0x98, 0xD8, 0x45, 0x39, 0xA1, 0xF4, 0xA0, 0x33, 0xEB, 0x2D, \
        0x81, 0x7D, 0x03, 0x77, 0xF2, 0x40, 0xA4, 0x63, 0xE5, 0xE6, 0xBC, \
        0xF8, 0x47, 0x42, 0x2C, 0xE1, 0xF2, 0xD1, 0x17, 0x6B}, \
    {0xF5, 0x51, 0xBF, 0x37, 0x68, 0x40, 0xB6, 0xCB, 0xCE, 0x5E, 0x31, 0x6B, \
        0x57, 0x33, 0xCE, 0x2B, 0x16, 0x9E, 0x0F, 0x7C, 0x4A, 0xEB, 0xE7, \
        0x8E, 0x9B, 0x7F, 0x1A, 0xFE, 0xE2, 0x42, 0xE3, 0x4F}}

#define Curve_G_48 { \
    {0xB7, 0x0A, 0x76, 0x72, 0x38, 0x5E, 0x54, 0x3A, 0x6C, 0x29, 0x55, 0xBF, \
        0x5D, 0xF2, 0x02, 0x55, 0x38, 0x2A, 0x54, 0x82, 0xE0, 0x41, 0xF7, \
        0x59, 0x98, 0x9B, 0xA7, 0x8B, 0x62, 0x3B, 0x1D, 0x6E, 0x74, 0xAD, \
        0x20, 0xF3, 0x1E, 0xC7, 0xB1, 0x8E, 0x37, 0x05, 0x8B, 0xBE, 0x22, \
        0xCA, 0x87, 0xAA}, \
    {0x5F, 0x0E, 0xEA, 0x90, 0x7C, 0x1D, 0x43, 0x7A, 0x9D, 0x81, 0x7E, 0x1D, \
        0xCE, 0xB1, 0x60, 0x0A, 0xC0, 0xB8, 0xF0, 0xB5, 0x13, 0x31, 0xDA, \
        0xE9, 0x7C, 0x14, 0x9A, 0x28, 0xBD, 0x1D, 0xF4, 0xF8, 0x29, 0xDC, \
        0x92, 0x92, 0xBF, 0x98, 0x9E, 0x5D, 0x6F, 0x2C, 0x26, 0x96, 0x4A, \
        0xDE, 0x17, 0x36}}

#define Curve_N_16 {  0x15, 0xA1, 0x38, 0x90, 0x1B, 0x0D, 0xA3, 0x75, 0x00, \
    0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF }

#define Curve_N_24 {  0x31, 0x28, 0xD2, 0xB4, 0xB1, 0xC9, 0x6B, 0x14, 0x36, \
    0xF8, 0xDE, 0x99, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF}

#define Curve_N_32 {  0x51, 0x25, 0x63, 0xFC, 0xC2, 0xCA, 0xB9, 0xF3, 0x84, \
    0x9E, 0x17, 0xA7, 0xAD, 0xFA, 0xE6, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF }

#define Curve_N_48 {  0x73, 0x29, 0xC5, 0xCC, 0x6A, 0x19, 0xEC, 0xEC, 0x7A, \
    0xA7, 0xB0, 0x48, 0xB2, 0x0D, 0x1A, 0x58, 0xDF, 0x2D, 0x37, 0xF4, 0x81, \
    0x4D, 0x63, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF}

#ifndef NOMINMAX
#ifndef max
#define max(a,b)                            ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b)                            ((a)<(b)?(a):(b))
#endif
#endif

#define EVEN(vli)                           (!(vli[0] & 1))
#define vli_clear(p_vli)                    memset((void *)(p_vli), 0x00, NUM_ECC_DIGITS)
#define vli_set(p_dest, p_src)              memcpy((void*)(p_dest), (void *)(p_src), NUM_ECC_DIGITS)
#define vli_square(result, left, size)      vli_mult((result), (left), (left), (size))
#define vli_modSquare_fast(result, left)    vli_modMult_fast((result), (left), (left))

const uint8_t curve_p[NUM_ECC_DIGITS]=CONCAT(Curve_P_, ECC_CURVE);
const uint8_t curve_b[NUM_ECC_DIGITS]=CONCAT(Curve_B_, ECC_CURVE);
const ecc_double_point_t curve_G=CONCAT(Curve_G_, ECC_CURVE);
const uint8_t curve_n[NUM_ECC_DIGITS]=CONCAT(Curve_N_, ECC_CURVE);

#ifndef ecc_rng_function

void ecc_rng_function(uint8_t *pD, size_t size) // <editor-fold defaultstate="collapsed" desc="ecc_rng_function">
{
#ifdef __WIN32

    HCRYPTPROV l_prov;

    CryptAcquireContext(&l_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptGenRandom(l_prov, size, (BYTE *) pD);
    CryptReleaseContext(l_prov, 0);
#else
    static bool __init=0;
    size_t i;

    if(__init==0)
    {
        srand((unsigned int) NULL);
        __init=1;
    }

    for(i=0; i<size; i++)
        pD[i]=(uint8_t) rand();
#endif
} // </editor-fold>
#endif

int vli_isZero(const uint8_t *p_vli) // <editor-fold defaultstate="collapsed" desc="vli_isZero">
{
    uint i;

    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        if(p_vli[i])
            return 0;
    }

    return 1;
} // </editor-fold>

uint8_t vli_testBit(const uint8_t *p_vli, uint p_bit) // <editor-fold defaultstate="collapsed" desc="vli_testBit">
{
    return (p_vli[p_bit>>3] & (1<<(p_bit&7)));
} // </editor-fold>

uint vli_numDigits(const uint8_t *p_vli) // <editor-fold defaultstate="collapsed" desc="vli_numDigits">
{
    int i;

    for(i=(NUM_ECC_DIGITS-1); (i>=0)&&(p_vli[i]==0); --i);

    return (uint) (i+1);
} // </editor-fold>

uint vli_numBits(const uint8_t *p_vli) // <editor-fold defaultstate="collapsed" desc="vli_numBits">
{
    uint i;
    uint8_t l_digit;
    uint l_numDigits=vli_numDigits(p_vli);

    if(l_numDigits==0)
        return 0;

    l_digit=p_vli[l_numDigits-1];

    for(i=0; l_digit; ++i)
        l_digit>>=1;

    return (((l_numDigits-1)<<3)+i);
} // </editor-fold>

int vli_cmp(const uint8_t *p_left, const uint8_t *p_right) // <editor-fold defaultstate="collapsed" desc="vli_cmp">
{
    int8_t i;

    for(i=(NUM_ECC_DIGITS-1); i>=0; --i)
    {
        if(p_left[i]>p_right[i])
            return 1;
        else if(p_left[i]<p_right[i])
            return -1;
    }

    return 0;
} // </editor-fold>

uint8_t vli_lshift(uint8_t *p_result, const uint8_t *p_in, uint p_shift) // <editor-fold defaultstate="collapsed" desc="vli_lshift">
{
    uint i;
    uint8_t l_carry=0;

    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        uint8_t l_temp=p_in[i];

        p_result[i]=((uint8_t) (l_temp<<p_shift))|l_carry;
        l_carry=l_temp>>(8-p_shift);
    }

    return l_carry;
} // </editor-fold>

void vli_rshift1(uint8_t *p_vli) // <editor-fold defaultstate="collapsed" desc="vli_rshift1">
{
    uint8_t *l_end=p_vli;
    uint8_t l_carry=0;

    p_vli+=NUM_ECC_DIGITS;

    while(p_vli-- > l_end)
    {
        uint8_t l_temp= *p_vli;

        *p_vli=(l_temp>>1)|l_carry;
        l_carry=(uint8_t) (l_temp<<7);
    }
} // </editor-fold>

uint8_t vli_add(uint8_t *p_result, const uint8_t *p_left,
                const uint8_t *p_right) // <editor-fold defaultstate="collapsed" desc="vli_add">
{
    uint i;
    uint8_t l_carry=0;

    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        uint8_t l_sum=p_left[i]+p_right[i]+l_carry;

        if(l_sum!=p_left[i])
            l_carry=(l_sum<p_left[i]);

        p_result[i]=l_sum;
    }

    return l_carry;
} // </editor-fold>

uint8_t vli_sub(uint8_t *p_result, const uint8_t *p_left,
                const uint8_t *p_right) // <editor-fold defaultstate="collapsed" desc="vli_sub">
{
    uint i;
    uint8_t l_borrow=0;

    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        uint8_t l_diff=p_left[i]-p_right[i]-l_borrow;

        if(l_diff!=p_left[i])
            l_borrow=(l_diff>p_left[i]);

        p_result[i]=l_diff;
    }

    return l_borrow;
} // </editor-fold>

void vli_mult(uint8_t *p_result, const uint8_t *p_left,
              const uint8_t *p_right) // <editor-fold defaultstate="collapsed" desc="vli_mult">
{
    uint i, k;
    uint16_t r01=0;
    uint8_t r2=0;

    for(k=0; k<((NUM_ECC_DIGITS<<1)-1); ++k)
    {
        uint l_min=(k<NUM_ECC_DIGITS?0:(k+1)-NUM_ECC_DIGITS);

        for(i=l_min; i<=k&&i<NUM_ECC_DIGITS; ++i)
        {
            uint16_t l_product=(uint16_t) p_left[i]*p_right[k-i];

            r01+=l_product;
            r2+=(r01<l_product);
        }

        p_result[k]=(uint8_t) r01;
        r01=(r01>>8)|(((uint16_t) r2)<<8);
        r2=0;
    }

    p_result[(NUM_ECC_DIGITS<<1)-1]=(uint8_t) r01;
} // </editor-fold>

void vli_modAdd(uint8_t *p_result, const uint8_t *p_left,
                const uint8_t *p_right, const uint8_t *p_mod) // <editor-fold defaultstate="collapsed" desc="vli_modAdd">
{
    uint8_t l_carry=vli_add(p_result, p_left, p_right);

    if(l_carry||vli_cmp(p_result, p_mod)>=0)
        vli_sub(p_result, p_result, p_mod);
} // </editor-fold>

void vli_modSub(uint8_t *p_result, const uint8_t *p_left,
                const uint8_t *p_right, const uint8_t *p_mod) // <editor-fold defaultstate="collapsed" desc="vli_modSub">
{
    uint8_t l_borrow=vli_sub(p_result, p_left, p_right);

    if(l_borrow)
        vli_add(p_result, p_result, p_mod);
} // </editor-fold>

#if ECC_CURVE == secp384r1

void omega_mult(uint8_t *p_result, uint8_t *p_right) // <editor-fold defaultstate="collapsed" desc="omega_mult">
{
    /* Multiply by (2^128 + 2^96 - 2^32 + 1). */
    vli_set(p_result, p_right); /* 1 */
    p_result[16+NUM_ECC_DIGITS]=vli_add(p_result+12, p_result+12, p_right); /* 2^96 + 1 */
    p_result[17+NUM_ECC_DIGITS]=0;
    p_result[18+NUM_ECC_DIGITS]=0;
    p_result[19+NUM_ECC_DIGITS]=0;
    p_result[20+NUM_ECC_DIGITS]=vli_add(p_result+16, p_result+16, p_right); /* 2^128 + 2^96 + 1 */
    p_result[21+NUM_ECC_DIGITS]=0;
    p_result[22+NUM_ECC_DIGITS]=0;
    p_result[23+NUM_ECC_DIGITS]=0;

    if(vli_sub(p_result+4, p_result+4, p_right)) /* 2^128 + 2^96 - 2^32 + 1 */
    { /* Propagate borrow if necessary. */
        uint i;

        for(i=4+NUM_ECC_DIGITS;; ++i)
        {
            --p_result[i];

            if(p_result[i]!=0xff)
                break;
        }
    }
} // </editor-fold>
#endif

void vli_mmod_fast(uint8_t *p_result, uint8_t *p_product) // <editor-fold defaultstate="collapsed" desc="vli_mmod_fast">
{
    int l_carry;

#if ECC_CURVE != secp384r1
    uint8_t l_tmp[NUM_ECC_DIGITS];
#endif

    vli_set(p_result, p_product);

#if ECC_CURVE == secp128r1
    l_tmp[0]=p_product[16];
    l_tmp[1]=p_product[17];
    l_tmp[2]=p_product[18];
    l_tmp[3]=p_product[19];
    l_tmp[4]=p_product[20];
    l_tmp[5]=p_product[21];
    l_tmp[6]=p_product[22];
    l_tmp[7]=p_product[23];
    l_tmp[8]=p_product[24];
    l_tmp[9]=p_product[25];
    l_tmp[10]=p_product[26];
    l_tmp[11]=p_product[27];
    l_tmp[12]=(p_product[28] & 1)|(uint8_t) (p_product[16]<<1);
    l_tmp[13]=(p_product[16]>>7)|(uint8_t) (p_product[17]<<1);
    l_tmp[14]=(p_product[17]>>7)|(uint8_t) (p_product[18]<<1);
    l_tmp[15]=(p_product[18]>>7)|(uint8_t) (p_product[19]<<1);
    l_carry=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=(p_product[19]>>7)|(uint8_t) (p_product[20]<<1);
    l_tmp[1]=(p_product[20]>>7)|(uint8_t) (p_product[21]<<1);
    l_tmp[2]=(p_product[21]>>7)|(uint8_t) (p_product[22]<<1);
    l_tmp[3]=(p_product[22]>>7)|(uint8_t) (p_product[23]<<1);
    l_tmp[4]=(p_product[23]>>7)|(uint8_t) (p_product[24]<<1);
    l_tmp[5]=(p_product[24]>>7)|(uint8_t) (p_product[25]<<1);
    l_tmp[6]=(p_product[25]>>7)|(uint8_t) (p_product[26]<<1);
    l_tmp[7]=(p_product[26]>>7)|(uint8_t) (p_product[27]<<1);
    l_tmp[8]=(p_product[27]>>7)|(uint8_t) (p_product[28]<<1);
    l_tmp[9]=(p_product[28]>>7)|(uint8_t) (p_product[29]<<1);
    l_tmp[10]=(p_product[29]>>7)|(uint8_t) (p_product[30]<<1);
    l_tmp[11]=(p_product[30]>>7)|(uint8_t) (p_product[31]<<1);
    l_tmp[12]=(p_product[31]>>7)|((p_product[19] & 0x80)>>6)|(uint8_t) (p_product[20]<<2);
    l_tmp[13]=(p_product[20]>>6)|(uint8_t) (p_product[21]<<2);
    l_tmp[14]=(p_product[21]>>6)|(uint8_t) (p_product[22]<<2);
    l_tmp[15]=(p_product[22]>>6)|(uint8_t) (p_product[23]<<2);
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=(p_product[23]>>6)|(uint8_t) (p_product[24]<<2);
    l_tmp[1]=(p_product[24]>>6)|(uint8_t) (p_product[25]<<2);
    l_tmp[2]=(p_product[25]>>6)|(uint8_t) (p_product[26]<<2);
    l_tmp[3]=(p_product[26]>>6)|(uint8_t) (p_product[27]<<2);
    l_tmp[4]=(p_product[27]>>6)|(uint8_t) (p_product[28]<<2);
    l_tmp[5]=(p_product[28]>>6)|(uint8_t) (p_product[29]<<2);
    l_tmp[6]=(p_product[29]>>6)|(uint8_t) (p_product[30]<<2);
    l_tmp[7]=(p_product[30]>>6)|(uint8_t) (p_product[31]<<2);
    l_tmp[8]=(p_product[31]>>6);
    l_tmp[9]=0;
    l_tmp[10]=0;
    l_tmp[11]=0;
    l_tmp[12]=((p_product[23] & 0xC0)>>5)|(uint8_t) (p_product[24]<<3);
    l_tmp[13]=(p_product[24]>>5)|(uint8_t) (p_product[25]<<3);
    l_tmp[14]=(p_product[25]>>5)|(uint8_t) (p_product[26]<<3);
    l_tmp[15]=(p_product[26]>>5)|(uint8_t) (p_product[27]<<3);
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=(p_product[27]>>5)|(uint8_t) (p_product[28]<<3);
    l_tmp[1]=(p_product[28]>>5)|(uint8_t) (p_product[29]<<3);
    l_tmp[2]=(p_product[29]>>5)|(uint8_t) (p_product[30]<<3);
    l_tmp[3]=(p_product[30]>>5)|(uint8_t) (p_product[31]<<3);
    l_tmp[4]=(p_product[31]>>5);
    l_tmp[5]=0;
    l_tmp[6]=0;
    l_tmp[7]=0;
    l_tmp[8]=0;
    l_tmp[9]=0;
    l_tmp[10]=0;
    l_tmp[11]=0;
    l_tmp[12]=((p_product[27] & 0xE0)>>4)|(uint8_t) (p_product[28]<<4);
    l_tmp[13]=(p_product[28]>>4)|(uint8_t) (p_product[29]<<4);
    l_tmp[14]=(p_product[29]>>4)|(uint8_t) (p_product[30]<<4);
    l_tmp[15]=(p_product[30]>>4)|(uint8_t) (p_product[31]<<4);
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=(p_product[31]>>4);
    l_tmp[1]=0;
    l_tmp[2]=0;
    l_tmp[3]=0;
    l_tmp[4]=0;
    l_tmp[5]=0;
    l_tmp[6]=0;
    l_tmp[7]=0;
    l_tmp[8]=0;
    l_tmp[9]=0;
    l_tmp[10]=0;
    l_tmp[11]=0;
    l_tmp[12]=(p_product[28] & 0xFE);
    l_tmp[13]=p_product[29];
    l_tmp[14]=p_product[30];
    l_tmp[15]=p_product[31];
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=0;
    l_tmp[1]=0;
    l_tmp[2]=0;
    l_tmp[3]=0;
    l_tmp[4]=0;
    l_tmp[5]=0;
    l_tmp[6]=0;
    l_tmp[7]=0;
    l_tmp[8]=0;
    l_tmp[9]=0;
    l_tmp[10]=0;
    l_tmp[11]=0;
    l_tmp[12]=((p_product[31] & 0xF0)>>3);
    l_tmp[13]=0;
    l_tmp[14]=0;
    l_tmp[15]=0;
    l_carry+=vli_add(p_result, p_result, l_tmp);

    while(l_carry||vli_cmp(curve_p, p_result)!=1)
        l_carry-=vli_sub(p_result, p_result, curve_p);

#elif ECC_CURVE == secp192r1

    vli_set(l_tmp, &p_product[NUM_ECC_DIGITS]); // the top 192 bits of result
    l_carry=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=l_tmp[1]=l_tmp[2]=l_tmp[3]=0;
    l_tmp[4]=l_tmp[5]=l_tmp[6]=l_tmp[7]=0;

    l_tmp[8]=p_product[24];
    l_tmp[9]=p_product[25];
    l_tmp[10]=p_product[26];
    l_tmp[11]=p_product[27];
    l_tmp[12]=p_product[28];
    l_tmp[13]=p_product[29];
    l_tmp[14]=p_product[30];
    l_tmp[15]=p_product[31];
    l_tmp[16]=p_product[32];
    l_tmp[17]=p_product[33];
    l_tmp[18]=p_product[34];
    l_tmp[19]=p_product[35];
    l_tmp[20]=p_product[36];
    l_tmp[21]=p_product[37];
    l_tmp[22]=p_product[38];
    l_tmp[23]=p_product[39];
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=l_tmp[8]=p_product[40];
    l_tmp[1]=l_tmp[9]=p_product[41];
    l_tmp[2]=l_tmp[10]=p_product[42];
    l_tmp[3]=l_tmp[11]=p_product[43];
    l_tmp[4]=l_tmp[12]=p_product[44];
    l_tmp[5]=l_tmp[13]=p_product[45];
    l_tmp[6]=l_tmp[14]=p_product[46];
    l_tmp[7]=l_tmp[15]=p_product[47];
    l_tmp[16]=l_tmp[17]=l_tmp[18]=l_tmp[19]=0;
    l_tmp[20]=l_tmp[21]=l_tmp[22]=l_tmp[23]=0;
    l_carry+=vli_add(p_result, p_result, l_tmp);

    while(l_carry||vli_cmp(curve_p, p_result)!=1)
        l_carry-=vli_sub(p_result, p_result, curve_p);

#elif ECC_CURVE == secp256r1
    l_tmp[0]=l_tmp[1]=l_tmp[2]=l_tmp[3]=0;
    l_tmp[4]=l_tmp[5]=l_tmp[6]=l_tmp[7]=0;
    l_tmp[8]=l_tmp[9]=l_tmp[10]=l_tmp[11]=0;
    l_tmp[12]=p_product[44];
    l_tmp[13]=p_product[45];
    l_tmp[14]=p_product[46];
    l_tmp[15]=p_product[47];
    l_tmp[16]=p_product[48];
    l_tmp[17]=p_product[49];
    l_tmp[18]=p_product[50];
    l_tmp[19]=p_product[51];
    l_tmp[20]=p_product[52];
    l_tmp[21]=p_product[53];
    l_tmp[22]=p_product[54];
    l_tmp[23]=p_product[55];
    l_tmp[24]=p_product[56];
    l_tmp[25]=p_product[57];
    l_tmp[26]=p_product[58];
    l_tmp[27]=p_product[59];
    l_tmp[28]=p_product[60];
    l_tmp[29]=p_product[61];
    l_tmp[30]=p_product[62];
    l_tmp[31]=p_product[63];
    l_carry=vli_lshift(l_tmp, l_tmp, 1);
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[12]=p_product[48];
    l_tmp[13]=p_product[49];
    l_tmp[14]=p_product[50];
    l_tmp[15]=p_product[51];
    l_tmp[16]=p_product[52];
    l_tmp[17]=p_product[53];
    l_tmp[18]=p_product[54];
    l_tmp[19]=p_product[55];
    l_tmp[20]=p_product[56];
    l_tmp[21]=p_product[57];
    l_tmp[22]=p_product[58];
    l_tmp[23]=p_product[59];
    l_tmp[24]=p_product[60];
    l_tmp[25]=p_product[61];
    l_tmp[26]=p_product[62];
    l_tmp[27]=p_product[63];
    l_tmp[28]=0;
    l_tmp[29]=0;
    l_tmp[30]=0;
    l_tmp[31]=0;
    l_carry+=vli_lshift(l_tmp, l_tmp, 1);
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=p_product[32];
    l_tmp[1]=p_product[33];
    l_tmp[2]=p_product[34];
    l_tmp[3]=p_product[35];
    l_tmp[4]=p_product[36];
    l_tmp[5]=p_product[37];
    l_tmp[6]=p_product[38];
    l_tmp[7]=p_product[39];
    l_tmp[8]=p_product[40];
    l_tmp[9]=p_product[41];
    l_tmp[10]=p_product[42];
    l_tmp[11]=p_product[43];
    l_tmp[12]=l_tmp[13]=l_tmp[14]=l_tmp[15]=0;
    l_tmp[16]=l_tmp[17]=l_tmp[18]=l_tmp[19]=0;
    l_tmp[20]=l_tmp[21]=l_tmp[22]=l_tmp[23]=0;
    l_tmp[24]=p_product[56];
    l_tmp[25]=p_product[57];
    l_tmp[26]=p_product[58];
    l_tmp[27]=p_product[59];
    l_tmp[28]=p_product[60];
    l_tmp[29]=p_product[61];
    l_tmp[30]=p_product[62];
    l_tmp[31]=p_product[63];
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=p_product[36];
    l_tmp[1]=p_product[37];
    l_tmp[2]=p_product[38];
    l_tmp[3]=p_product[39];
    l_tmp[4]=p_product[40];
    l_tmp[5]=p_product[41];
    l_tmp[6]=p_product[42];
    l_tmp[7]=p_product[43];
    l_tmp[8]=p_product[44];
    l_tmp[9]=p_product[45];
    l_tmp[10]=p_product[46];
    l_tmp[11]=p_product[47];
    l_tmp[12]=p_product[52];
    l_tmp[13]=p_product[53];
    l_tmp[14]=p_product[54];
    l_tmp[15]=p_product[55];
    l_tmp[16]=p_product[56];
    l_tmp[17]=p_product[57];
    l_tmp[18]=p_product[58];
    l_tmp[19]=p_product[59];
    l_tmp[20]=p_product[60];
    l_tmp[21]=p_product[61];
    l_tmp[22]=p_product[62];
    l_tmp[23]=p_product[63];
    l_tmp[24]=p_product[52];
    l_tmp[25]=p_product[53];
    l_tmp[26]=p_product[54];
    l_tmp[27]=p_product[55];
    l_tmp[28]=p_product[32];
    l_tmp[29]=p_product[33];
    l_tmp[30]=p_product[34];
    l_tmp[31]=p_product[35];
    l_carry+=vli_add(p_result, p_result, l_tmp);

    l_tmp[0]=p_product[44];
    l_tmp[1]=p_product[45];
    l_tmp[2]=p_product[46];
    l_tmp[3]=p_product[47];
    l_tmp[4]=p_product[48];
    l_tmp[5]=p_product[49];
    l_tmp[6]=p_product[50];
    l_tmp[7]=p_product[51];
    l_tmp[8]=p_product[52];
    l_tmp[9]=p_product[53];
    l_tmp[10]=p_product[54];
    l_tmp[11]=p_product[55];
    l_tmp[12]=l_tmp[13]=l_tmp[14]=l_tmp[15]=0;
    l_tmp[16]=l_tmp[17]=l_tmp[18]=l_tmp[19]=0;
    l_tmp[20]=l_tmp[21]=l_tmp[22]=l_tmp[23]=0;
    l_tmp[24]=p_product[32];
    l_tmp[25]=p_product[33];
    l_tmp[26]=p_product[34];
    l_tmp[27]=p_product[35];
    l_tmp[28]=p_product[40];
    l_tmp[29]=p_product[41];
    l_tmp[30]=p_product[42];
    l_tmp[31]=p_product[43];
    l_carry-=vli_sub(p_result, p_result, l_tmp);

    l_tmp[0]=p_product[48];
    l_tmp[1]=p_product[49];
    l_tmp[2]=p_product[50];
    l_tmp[3]=p_product[51];
    l_tmp[4]=p_product[52];
    l_tmp[5]=p_product[53];
    l_tmp[6]=p_product[54];
    l_tmp[7]=p_product[55];
    l_tmp[8]=p_product[56];
    l_tmp[9]=p_product[57];
    l_tmp[10]=p_product[58];
    l_tmp[11]=p_product[59];
    l_tmp[12]=p_product[60];
    l_tmp[13]=p_product[61];
    l_tmp[14]=p_product[62];
    l_tmp[15]=p_product[63];
    l_tmp[16]=l_tmp[17]=l_tmp[18]=l_tmp[19]=0;
    l_tmp[20]=l_tmp[21]=l_tmp[22]=l_tmp[23]=0;
    l_tmp[24]=p_product[36];
    l_tmp[25]=p_product[37];
    l_tmp[26]=p_product[38];
    l_tmp[27]=p_product[39];
    l_tmp[28]=p_product[44];
    l_tmp[29]=p_product[45];
    l_tmp[30]=p_product[46];
    l_tmp[31]=p_product[47];
    l_carry-=vli_sub(p_result, p_result, l_tmp);

    l_tmp[0]=p_product[52];
    l_tmp[1]=p_product[53];
    l_tmp[2]=p_product[54];
    l_tmp[3]=p_product[55];
    l_tmp[4]=p_product[56];
    l_tmp[5]=p_product[57];
    l_tmp[6]=p_product[58];
    l_tmp[7]=p_product[59];
    l_tmp[8]=p_product[60];
    l_tmp[9]=p_product[61];
    l_tmp[10]=p_product[62];
    l_tmp[11]=p_product[63];
    l_tmp[12]=p_product[32];
    l_tmp[13]=p_product[33];
    l_tmp[14]=p_product[34];
    l_tmp[15]=p_product[35];
    l_tmp[16]=p_product[36];
    l_tmp[17]=p_product[37];
    l_tmp[18]=p_product[38];
    l_tmp[19]=p_product[39];
    l_tmp[20]=p_product[40];
    l_tmp[21]=p_product[41];
    l_tmp[22]=p_product[42];
    l_tmp[23]=p_product[43];
    l_tmp[24]=l_tmp[25]=l_tmp[26]=l_tmp[27]=0;
    l_tmp[28]=p_product[48];
    l_tmp[29]=p_product[49];
    l_tmp[30]=p_product[50];
    l_tmp[31]=p_product[51];
    l_carry-=vli_sub(p_result, p_result, l_tmp);

    l_tmp[0]=p_product[56];
    l_tmp[1]=p_product[57];
    l_tmp[2]=p_product[58];
    l_tmp[3]=p_product[59];
    l_tmp[4]=p_product[60];
    l_tmp[5]=p_product[61];
    l_tmp[6]=p_product[62];
    l_tmp[7]=p_product[63];
    l_tmp[8]=l_tmp[9]=l_tmp[10]=l_tmp[11]=0;
    l_tmp[12]=p_product[36];
    l_tmp[13]=p_product[37];
    l_tmp[14]=p_product[38];
    l_tmp[15]=p_product[39];
    l_tmp[16]=p_product[40];
    l_tmp[17]=p_product[41];
    l_tmp[18]=p_product[42];
    l_tmp[19]=p_product[43];
    l_tmp[20]=p_product[44];
    l_tmp[21]=p_product[45];
    l_tmp[22]=p_product[46];
    l_tmp[23]=p_product[47];
    l_tmp[24]=l_tmp[25]=l_tmp[26]=l_tmp[27]=0;
    l_tmp[28]=p_product[52];
    l_tmp[29]=p_product[53];
    l_tmp[30]=p_product[54];
    l_tmp[31]=p_product[55];
    l_carry-=vli_sub(p_result, p_result, l_tmp);

    if(l_carry<0)
    {
        do
            l_carry+=vli_add(p_result, p_result, curve_p);
        while(l_carry<0);
    }
    else
    {
        while(l_carry||vli_cmp(curve_p, p_result)!=1)
            l_carry-=vli_sub(p_result, p_result, curve_p);
    }

#elif ECC_CURVE == secp384r1
    int i;
    uint8_t l_tmp[2*NUM_ECC_DIGITS];

    while(!vli_isZero(p_product+NUM_ECC_DIGITS))
    {
        l_carry=0;

        vli_clear(l_tmp);
        vli_clear(l_tmp+NUM_ECC_DIGITS);
        omega_mult(l_tmp, p_product+NUM_ECC_DIGITS);
        vli_clear(p_product+NUM_ECC_DIGITS);

        for(i=0; i<NUM_ECC_DIGITS+20; ++i)
        {
            uint8_t l_sum=p_product[i]+l_tmp[i]+l_carry;

            if(l_sum!=p_product[i])
                l_carry=(l_sum<p_product[i]);

            p_product[i]=l_sum;
        }
    }

    while(vli_cmp(p_product, curve_p)>0)
        vli_sub(p_product, p_product, curve_p);

    vli_set(p_result, p_product);
#endif
} // </editor-fold>

void vli_modMult_fast(uint8_t *p_result, const uint8_t *p_left, const uint8_t *p_right) // <editor-fold defaultstate="collapsed" desc="vli_modMult_fast">
{
    uint8_t l_product[NUM_ECC_DIGITS<<1];

    vli_mult(l_product, p_left, p_right);
    vli_mmod_fast(p_result, l_product);
} // </editor-fold>

void vli_modInv(uint8_t *p_result, const uint8_t *p_input, const uint8_t *p_mod) // <editor-fold defaultstate="collapsed" desc="vli_modInv">
{
    int l_cmpResult;
    uint8_t l_carry;
    uint8_t a[NUM_ECC_DIGITS], b[NUM_ECC_DIGITS], u[NUM_ECC_DIGITS], v[NUM_ECC_DIGITS];

    vli_set(a, p_input);
    vli_set(b, p_mod);
    vli_clear(u);
    u[0]=1;
    vli_clear(v);

    while((l_cmpResult=vli_cmp(a, b))!=0)
    {
        l_carry=0;

        if(EVEN(a))
        {
            vli_rshift1(a);

            if(!EVEN(u))
                l_carry=vli_add(u, u, p_mod);

            vli_rshift1(u);

            if(l_carry)
                u[NUM_ECC_DIGITS-1]|=0x80;
        }
        else if(EVEN(b))
        {
            vli_rshift1(b);

            if(!EVEN(v))
                l_carry=vli_add(v, v, p_mod);

            vli_rshift1(v);

            if(l_carry)
                v[NUM_ECC_DIGITS-1]|=0x80;
        }
        else if(l_cmpResult>0)
        {
            vli_sub(a, a, b);
            vli_rshift1(a);

            if(vli_cmp(u, v)<0)
                vli_add(u, u, p_mod);

            vli_sub(u, u, v);

            if(!EVEN(u))
                l_carry=vli_add(u, u, p_mod);

            vli_rshift1(u);

            if(l_carry)
                u[NUM_ECC_DIGITS-1]|=0x80;
        }
        else
        {
            vli_sub(b, b, a);
            vli_rshift1(b);

            if(vli_cmp(v, u)<0)
                vli_add(v, v, p_mod);

            vli_sub(v, v, u);

            if(!EVEN(v))
                l_carry=vli_add(v, v, p_mod);

            vli_rshift1(v);

            if(l_carry)
                v[NUM_ECC_DIGITS-1]|=0x80;
        }
    }

    vli_set(p_result, u);
} // </editor-fold>

int ecc_point_t_isZero(ecc_double_point_t *p_point) // <editor-fold defaultstate="collapsed" desc="ecc_point_t_isZero">
{
    return (vli_isZero(p_point->x)&&vli_isZero(p_point->y));
} // </editor-fold>

void ecc_point_t_double_jacobian(uint8_t *X1, uint8_t *Y1, uint8_t *Z1) // <editor-fold defaultstate="collapsed" desc="ecc_point_t_double_jacobian">
{
    uint8_t t4[NUM_ECC_DIGITS], t5[NUM_ECC_DIGITS];

    if(vli_isZero(Z1))
        return;

    vli_modSquare_fast(t4, Y1);
    vli_modMult_fast(t5, X1, t4);
    vli_modSquare_fast(t4, t4);
    vli_modMult_fast(Y1, Y1, Z1);
    vli_modSquare_fast(Z1, Z1);
    vli_modAdd(X1, X1, Z1, curve_p);
    vli_modAdd(Z1, Z1, Z1, curve_p);
    vli_modSub(Z1, X1, Z1, curve_p);
    vli_modMult_fast(X1, X1, Z1);
    vli_modAdd(Z1, X1, X1, curve_p);
    vli_modAdd(X1, X1, Z1, curve_p);

    if(vli_testBit(X1, 0))
    {
        uint8_t l_carry=vli_add(X1, X1, curve_p);

        vli_rshift1(X1);
        X1[NUM_ECC_DIGITS-1]|=l_carry<<7;
    }
    else
        vli_rshift1(X1);

    vli_modSquare_fast(Z1, X1);
    vli_modSub(Z1, Z1, t5, curve_p);
    vli_modSub(Z1, Z1, t5, curve_p);
    vli_modSub(t5, t5, Z1, curve_p);
    vli_modMult_fast(X1, X1, t5);
    vli_modSub(t4, X1, t4, curve_p);
    vli_set(X1, Z1);
    vli_set(Z1, Y1);
    vli_set(Y1, t4);
} // </editor-fold>

void apply_z(uint8_t *X1, uint8_t *Y1, uint8_t *Z) // <editor-fold defaultstate="collapsed" desc="apply_z">
{
    uint8_t t1[NUM_ECC_DIGITS];

    vli_modSquare_fast(t1, Z);
    vli_modMult_fast(X1, X1, t1);
    vli_modMult_fast(t1, t1, Z);
    vli_modMult_fast(Y1, Y1, t1);
} // </editor-fold>

void XYcZ_initial_double(uint8_t *X1, uint8_t *Y1, uint8_t *X2,
                         uint8_t *Y2, const uint8_t *p_initialZ) // <editor-fold defaultstate="collapsed" desc="XYcZ_initial_double">
{
    uint8_t z[NUM_ECC_DIGITS];

    vli_set(X2, X1);
    vli_set(Y2, Y1);
    vli_clear(z);
    z[0]=1;

    if(p_initialZ)
        vli_set(z, p_initialZ);

    apply_z(X1, Y1, z);
    ecc_point_t_double_jacobian(X1, Y1, z);
    apply_z(X2, Y2, z);
} // </editor-fold>

void XYcZ_add(uint8_t *X1, uint8_t *Y1, uint8_t *X2, uint8_t *Y2) // <editor-fold defaultstate="collapsed" desc="XYcZ_add">
{
    uint8_t t5[NUM_ECC_DIGITS];

    vli_modSub(t5, X2, X1, curve_p);
    vli_modSquare_fast(t5, t5);
    vli_modMult_fast(X1, X1, t5);
    vli_modMult_fast(X2, X2, t5);
    vli_modSub(Y2, Y2, Y1, curve_p);
    vli_modSquare_fast(t5, Y2);
    vli_modSub(t5, t5, X1, curve_p);
    vli_modSub(t5, t5, X2, curve_p);
    vli_modSub(X2, X2, X1, curve_p);
    vli_modMult_fast(Y1, Y1, X2);
    vli_modSub(X2, X1, t5, curve_p);
    vli_modMult_fast(Y2, Y2, X2);
    vli_modSub(Y2, Y2, Y1, curve_p);
    vli_set(X2, t5);
} // </editor-fold>

void XYcZ_addC(uint8_t *X1, uint8_t *Y1, uint8_t *X2, uint8_t *Y2) // <editor-fold defaultstate="collapsed" desc="XYcZ_addC">
{
    uint8_t t5[NUM_ECC_DIGITS], t6[NUM_ECC_DIGITS], t7[NUM_ECC_DIGITS];

    vli_modSub(t5, X2, X1, curve_p);
    vli_modSquare_fast(t5, t5);
    vli_modMult_fast(X1, X1, t5);
    vli_modMult_fast(X2, X2, t5);
    vli_modAdd(t5, Y2, Y1, curve_p);
    vli_modSub(Y2, Y2, Y1, curve_p);
    vli_modSub(t6, X2, X1, curve_p);
    vli_modMult_fast(Y1, Y1, t6);
    vli_modAdd(t6, X1, X2, curve_p);
    vli_modSquare_fast(X2, Y2);
    vli_modSub(X2, X2, t6, curve_p);
    vli_modSub(t7, X1, X2, curve_p);
    vli_modMult_fast(Y2, Y2, t7);
    vli_modSub(Y2, Y2, Y1, curve_p);
    vli_modSquare_fast(t7, t5);
    vli_modSub(t7, t7, t6, curve_p);
    vli_modSub(t6, t7, X1, curve_p);
    vli_modMult_fast(t6, t6, t5);
    vli_modSub(Y1, t6, Y1, curve_p);
    vli_set(X1, t7);
} // </editor-fold>

void ecc_point_t_mult(ecc_double_point_t *p_result,
                      const ecc_double_point_t *p_point,
                      const uint8_t *p_scalar,
                      const uint8_t *p_initialZ) // <editor-fold defaultstate="collapsed" desc="ecc_point_t_mult">
{
    uint i, nb;
    uint8_t Rx[2][NUM_ECC_DIGITS], Ry[2][NUM_ECC_DIGITS], z[NUM_ECC_DIGITS];

    vli_set(Rx[1], p_point->x);
    vli_set(Ry[1], p_point->y);
    XYcZ_initial_double(Rx[1], Ry[1], Rx[0], Ry[0], p_initialZ);

    for(i=vli_numBits(p_scalar)-2; i>0; --i)
    {
        nb= !vli_testBit(p_scalar, i);
        XYcZ_addC(Rx[1-nb], Ry[1-nb], Rx[nb], Ry[nb]);
        XYcZ_add(Rx[nb], Ry[nb], Rx[1-nb], Ry[1-nb]);
    }

    nb= !vli_testBit(p_scalar, 0);
    XYcZ_addC(Rx[1-nb], Ry[1-nb], Rx[nb], Ry[nb]);
    vli_modSub(z, Rx[1], Rx[0], curve_p);
    vli_modMult_fast(z, z, Ry[1-nb]);
    vli_modMult_fast(z, z, p_point->x);
    vli_modInv(z, z, curve_p);
    vli_modMult_fast(z, z, p_point->y);
    vli_modMult_fast(z, z, Rx[1-nb]);
    XYcZ_add(Rx[nb], Ry[nb], Rx[1-nb], Ry[1-nb]);
    apply_z(Rx[0], Ry[0], z);
    vli_set(p_result->x, Rx[0]);
    vli_set(p_result->y, Ry[0]);
} // </editor-fold>

int ecc_make_key(ecc_key_t *pCtx) // <editor-fold defaultstate="collapsed" desc="Generate ECC key">
{
    ecc_rng_function(pCtx->prv.key, NUM_ECC_DIGITS);

    if(vli_cmp(curve_n, pCtx->prv.key)!=1)
        vli_sub(pCtx->prv.key, pCtx->prv.key, curve_n);

    if(vli_isZero(pCtx->prv.key))
        return 0;

    ecc_point_t_mult(&pCtx->pub, &curve_G, pCtx->prv.key, NULL);

    return 1;
} // </editor-fold>

int ecdh_shared_secret(uint8_t *p_secret,
                       ecc_double_point_t *p_Key,
                       uint8_t *p_privateKey) // <editor-fold defaultstate="collapsed" desc="Generate ECC shared key">
{
    uint8_t p_random[NUM_ECC_DIGITS];
    ecc_double_point_t l_product;

    ecc_rng_function(p_random, NUM_ECC_DIGITS);
    ecc_point_t_mult(&l_product, p_Key, p_privateKey, p_random);

    if(ecc_point_t_isZero(&l_product))
        return 0;

    vli_set(p_secret, l_product.x);

    return 1;
} // </editor-fold>