#include <string.h>
#include <stdio.h>
#include "sha1.h"

#define ROUNDTAIL(a,b,e,f,i,k)  \
        e += (a << 5 | a >> 27) + f + k + schedule[i & 0xF];  \
        b = b << 30 | b >> 2;

#define SCHEDULE(i)  \
        temp = schedule[(i-3)&0xF] ^ schedule[(i-8)&0xF] ^ schedule[(i-14)&0xF] ^ schedule[(i-16)&0xF];  \
        schedule[i & 0xF] = temp << 1 | temp >> 31;

#define ROUND0a(a,b,c,d,e,i)  \
        schedule[i] = (block[i] << 24) | ((block[i] & 0xFF00) << 8) | ((block[i] >> 8) & 0xFF00) | (block[i] >> 24);  \
        ROUNDTAIL(a, b, e, ((b & c) | (~b & d)), i, 0x5A827999)

#define ROUND0b(a,b,c,d,e,i)  \
        SCHEDULE(i)  \
        ROUNDTAIL(a, b, e, ((b & c) | (~b & d)), i, 0x5A827999)

#define ROUND1(a,b,c,d,e,i)  \
        SCHEDULE(i)  \
        ROUNDTAIL(a, b, e, (b ^ c ^ d), i, 0x6ED9EBA1)

#define ROUND2(a,b,c,d,e,i)  \
        SCHEDULE(i)  \
        ROUNDTAIL(a, b, e, ((b & c) ^ (b & d) ^ (c & d)), i, 0x8F1BBCDC)

#define ROUND3(a,b,c,d,e,i)  \
        SCHEDULE(i)  \
        ROUNDTAIL(a, b, e, (b ^ c ^ d), i, 0xCA62C1D6)

void sha1_compress(uint32_t *state, uint32_t *block)
{
    uint32_t schedule[16];
    uint32_t temp;

    uint32_t a=state[0];
    uint32_t b=state[1];
    uint32_t c=state[2];
    uint32_t d=state[3];
    uint32_t e=state[4];

    ROUND0a(a, b, c, d, e, 0)
    ROUND0a(e, a, b, c, d, 1)
    ROUND0a(d, e, a, b, c, 2)
    ROUND0a(c, d, e, a, b, 3)
    ROUND0a(b, c, d, e, a, 4)
    ROUND0a(a, b, c, d, e, 5)
    ROUND0a(e, a, b, c, d, 6)
    ROUND0a(d, e, a, b, c, 7)
    ROUND0a(c, d, e, a, b, 8)
    ROUND0a(b, c, d, e, a, 9)
    ROUND0a(a, b, c, d, e, 10)
    ROUND0a(e, a, b, c, d, 11)
    ROUND0a(d, e, a, b, c, 12)
    ROUND0a(c, d, e, a, b, 13)
    ROUND0a(b, c, d, e, a, 14)
    ROUND0a(a, b, c, d, e, 15)

    ROUND0b(e, a, b, c, d, 16)
    ROUND0b(d, e, a, b, c, 17)
    ROUND0b(c, d, e, a, b, 18)
    ROUND0b(b, c, d, e, a, 19)

    ROUND1(a, b, c, d, e, 20)
    ROUND1(e, a, b, c, d, 21)
    ROUND1(d, e, a, b, c, 22)
    ROUND1(c, d, e, a, b, 23)
    ROUND1(b, c, d, e, a, 24)
    ROUND1(a, b, c, d, e, 25)
    ROUND1(e, a, b, c, d, 26)
    ROUND1(d, e, a, b, c, 27)
    ROUND1(c, d, e, a, b, 28)
    ROUND1(b, c, d, e, a, 29)
    ROUND1(a, b, c, d, e, 30)
    ROUND1(e, a, b, c, d, 31)
    ROUND1(d, e, a, b, c, 32)
    ROUND1(c, d, e, a, b, 33)
    ROUND1(b, c, d, e, a, 34)
    ROUND1(a, b, c, d, e, 35)
    ROUND1(e, a, b, c, d, 36)
    ROUND1(d, e, a, b, c, 37)
    ROUND1(c, d, e, a, b, 38)
    ROUND1(b, c, d, e, a, 39)

    ROUND2(a, b, c, d, e, 40)
    ROUND2(e, a, b, c, d, 41)
    ROUND2(d, e, a, b, c, 42)
    ROUND2(c, d, e, a, b, 43)
    ROUND2(b, c, d, e, a, 44)
    ROUND2(a, b, c, d, e, 45)
    ROUND2(e, a, b, c, d, 46)
    ROUND2(d, e, a, b, c, 47)
    ROUND2(c, d, e, a, b, 48)
    ROUND2(b, c, d, e, a, 49)
    ROUND2(a, b, c, d, e, 50)
    ROUND2(e, a, b, c, d, 51)
    ROUND2(d, e, a, b, c, 52)
    ROUND2(c, d, e, a, b, 53)
    ROUND2(b, c, d, e, a, 54)
    ROUND2(a, b, c, d, e, 55)
    ROUND2(e, a, b, c, d, 56)
    ROUND2(d, e, a, b, c, 57)
    ROUND2(c, d, e, a, b, 58)
    ROUND2(b, c, d, e, a, 59)

    ROUND3(a, b, c, d, e, 60)
    ROUND3(e, a, b, c, d, 61)
    ROUND3(d, e, a, b, c, 62)
    ROUND3(c, d, e, a, b, 63)
    ROUND3(b, c, d, e, a, 64)
    ROUND3(a, b, c, d, e, 65)
    ROUND3(e, a, b, c, d, 66)
    ROUND3(d, e, a, b, c, 67)
    ROUND3(c, d, e, a, b, 68)
    ROUND3(b, c, d, e, a, 69)
    ROUND3(a, b, c, d, e, 70)
    ROUND3(e, a, b, c, d, 71)
    ROUND3(d, e, a, b, c, 72)
    ROUND3(c, d, e, a, b, 73)
    ROUND3(b, c, d, e, a, 74)
    ROUND3(a, b, c, d, e, 75)
    ROUND3(e, a, b, c, d, 76)
    ROUND3(d, e, a, b, c, 77)
    ROUND3(c, d, e, a, b, 78)
    ROUND3(b, c, d, e, a, 79)

    state[0]+=a;
    state[1]+=b;
    state[2]+=c;
    state[3]+=d;
    state[4]+=e;
}

bool sha1_hash(uint8_t *message, uint32_t len, uint32_t *hash)
{
    uint32_t i, rem;
    uint32_t block[16];
    uint8_t *byteBlock;

    if(len>0x1FFFFFFF) // Max size is 512Gbit = 64Mbyte
    {
        // printf("\r\nLen=%ld is over flow", len);
        return 0;
    }

    hash[0]=0x67452301;
    hash[1]=0xEFCDAB89;
    hash[2]=0x98BADCFE;
    hash[3]=0x10325476;
    hash[4]=0xC3D2E1F0;

    for(i=0; (i+64)<=len; i+=64)
        sha1_compress(hash, (uint32_t*) (message+i));

    byteBlock=(uint8_t*) block;
    rem=len-i;
    memcpy(byteBlock, message+i, rem);
    byteBlock[rem]=0x80;
    rem++;

    if((64-rem)>=8)
        memset(byteBlock+rem, 0, 56-rem);
    else
    {
        memset(byteBlock+rem, 0, 64-rem);
        sha1_compress(hash, block);
        memset(block, 0, 56);
    }

#define long_t uint32_t

    long_t longLen=((long_t) len)<<3;

    // printf("\r\nlen=%ld", len);
    // printf(", longLen=%ld\r\n", longLen);

    for(i=0; i<8; i++)
    {
        uint8_t b, l;

        b=i*8;

        if(b<32)
            l=(uint8_t) (longLen>>b);
        else
            l=0;

        // printf("\r\nl=%d", l);
        byteBlock[64-1-i]=l;
    }

    // printf("\r\n");
    sha1_compress(hash, block);

    return 1;
#undef long_t
}

int self_check(void)
{
    uint32_t hash[5];

    if(!sha1_hash((uint8_t*) "", 0, hash))
        return 0;

    if(hash[0]!=0xDA39A3EE||hash[1]!=0x5E6B4B0D||hash[2]!=0x3255BFEF||hash[3]!=0x95601890||hash[4]!=0xAFD80709)
        return 0;

    if(!sha1_hash((uint8_t*) "a", 1, hash))
        return 0;

    if(hash[0]!=0x86F7E437||hash[1]!=0xFAA5A7FC||hash[2]!=0xE15D1DDC||hash[3]!=0xB9EAEAEA||hash[4]!=0x377667B8)
        return 0;

    if(!sha1_hash((uint8_t*) "abc", 3, hash))
        return 0;

    if(hash[0]!=0xA9993E36||hash[1]!=0x4706816A||hash[2]!=0xBA3E2571||hash[3]!=0x7850C26C||hash[4]!=0x9CD0D89D)
        return 0;

    if(!sha1_hash((uint8_t*) "message digest", 14, hash))
        return 0;

    if(hash[0]!=0xC12252CE||hash[1]!=0xDA8BE899||hash[2]!=0x4D5FA029||hash[3]!=0x0A47231C||hash[4]!=0x1D16AAE3)
        return 0;

    if(!sha1_hash((uint8_t*) "abcdefghijklmnopqrstuvwxyz", 26, hash))
        return 0;

    if(hash[0]!=0x32D10C7B||hash[1]!=0x8CF96570||hash[2]!=0xCA04CE37||hash[3]!=0xF2A19D84||hash[4]!=0x240D3A89)
        return 0;

    if(!sha1_hash((uint8_t*) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56, hash))
        return 0;

    if(hash[0]!=0x84983E44||hash[1]!=0x1C3BD26E||hash[2]!=0xBAAE4AA1||hash[3]!=0xF95129E5||hash[4]!=0xE54670F1)
        return 0;

    return 1;
}