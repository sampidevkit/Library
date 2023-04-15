#include "debug.h"

static int8_t _i;
static int8_t _idx;
static uint8_t _digit[10];

void debug(const char *str)
{
    while(*str!=0x00)
        debug_port_write(*str++);
}

void debug_hex(uint32_t b, uint8_t dgcount)
{
    for(_i=7; _i>=0; _i--)
    {
        _digit[_i]=(uint8_t) (b&0x0F);

        if(_digit[_i]>9)
        {
            _digit[_i]-=10;
            _digit[_i]+='A';
        }
        else
            _digit[_i]+='0';

        b>>=4;
    }

    if(dgcount&0x80)
    {
        debug_port_write('0');
        debug_port_write('x');
    }

    dgcount&=0x7F;

    for(_i=0, _idx=8; _i<8; _i++)
    {
        if((_digit[_i]=='0')&&(_idx>dgcount))
        {
            _idx--;
            continue;
        }

        _idx=0;
        debug_port_write(_digit[_i]);
    }
}

void debug_hexs(uint8_t *pD, int len)
{
    while(len>0)
    {
        debug_hex(*pD++, 2);
        len--;
    }
}

void debug_u32(uint32_t b)
{
    for(_i=9; _i>=0; _i--)
    {
        _digit[_i]=((uint8_t) (b%10))+'0';
        b/=10;
    }

    for(_i=0, _idx=1; _i<10; _i++)
    {
        if(_digit[_i]!='0')
            _idx=0;

        if(_idx==0)
            debug_port_write(_digit[_i]);
    }

    if(_idx==1)
        debug_port_write('0');
}

void debug_i32(int32_t b)
{
    if(b<0)
    {
        debug_port_write('-');
        b*=(-1);
    }
    
    debug_u32((uint32_t)b);
}

void debug_data(uint8_t *pD, int len)
{
    while(len>0)
    {
        if((*pD>=' ')&&(*pD<='~'))
            debug_port_write(*pD);
        else
        {
            debug_port_write('<');
            debug_hex(*pD, 2);
            debug_port_write('>');
        }

        pD++;
        len--;
    }
}

void debug_time(tm_t *time)
{
    debug_u32(time->tm_hour);
    debug_port_write(':');
    debug_u32(time->tm_min);
    debug_port_write(':');
    debug_u32(time->tm_sec);
}

void debug_date(tm_t *time)
{
    debug_u32(time->tm_mday);
    debug_port_write('/');
    debug_u32(time->tm_mon);
    debug_port_write('/');
    debug_u32(time->tm_year);
}
