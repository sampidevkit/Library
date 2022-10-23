#include "QWIIC.h"
#include "libcomp.h"

qwiic_stt_t qwiic_last_state=QWIIC_ERR;
static tick_t Tick={1, 0, 0};
static uint8_t DoNext=0;

void QWIIC_WriteNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz)
{
    if(qwiic_last_state==QWIIC_ERR)
    {
        DoNext=0;
        i2c_force_close();
    }

    switch(DoNext)
    {
        default:
        case 0:
            DoNext++;
            Tick=Tick_Get();
            qwiic_last_state=QWIIC_BUSY;

        case 1:
            if(i2c_open(SlvAddr)!=I2C_NOERR)
            {
                if(Tick_Dif_Ms(Tick_Get(), Tick)>100)
                {
                    DoNext=0;
                    qwiic_last_state=QWIIC_ERR;
                }

                break;
            }

            DoNext++;
            Tick=Tick_Get();
            i2c_setBuffer(pD, sz);
            i2c_setAddressNACKCallback(i2c_restartWrite, NULL); //NACK polling?

        case 2:
            if(i2c_masterOperation(false)!=I2C_NOERR)
            {
                if(Tick_Dif_Ms(Tick_Get(), Tick)>100)
                {
                    DoNext=0;
                    qwiic_last_state=QWIIC_ERR;
                }

                break;
            }

            DoNext++;
            Tick=Tick_Get();

        case 3:
            if(i2c_close()!=I2C_NOERR)
            {
                if(Tick_Dif_Ms(Tick_Get(), Tick)>100)
                {
                    DoNext=0;
                    qwiic_last_state=QWIIC_ERR;
                }

                break;
            }

            DoNext=0;
            qwiic_last_state=QWIIC_OK;
            break;
    }
}

void QWIIC_ReadNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz)
{
    if(qwiic_last_state!=QWIIC_OK)
        return;

    switch(DoNext)
    {
        default:
        case 0:
            DoNext++;
            Tick=Tick_Get();
            qwiic_last_state=QWIIC_BUSY;

        case 1:
            if(i2c_open(SlvAddr)!=I2C_NOERR)
            {
                if(Tick_Dif_Ms(Tick_Get(), Tick)>100)
                {
                    DoNext=0;
                    qwiic_last_state=QWIIC_ERR;
                }

                break;
            }

            DoNext++;
            Tick=Tick_Get();
            i2c_setBuffer(pD, sz);

        case 2:
            if(i2c_masterOperation(true)!=I2C_NOERR)
            {
                if(Tick_Dif_Ms(Tick_Get(), Tick)>100)
                {
                    DoNext=0;
                    qwiic_last_state=QWIIC_ERR;
                }

                break;
            }

            DoNext++;
            Tick=Tick_Get();

        case 3:
            if(i2c_close()!=I2C_NOERR)
            {
                if(Tick_Dif_Ms(Tick_Get(), Tick)>100)
                {
                    DoNext=0;
                    qwiic_last_state=QWIIC_ERR;
                }

                break;
            }

            DoNext=0;
            qwiic_last_state=QWIIC_OK;
            break;
    }
}