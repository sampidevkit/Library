#include "intel_hex.h"

#define __db(...) printf(__VA_ARGS__)

private __PACKED_STRUCT
{
    uint8_t i;
    uint8_t j;
    uint8_t val;
}
Hex2IntCxt;

private __PACKED_STRUCT{
    hex_t Hex;
    uint32_t addr;
    uint32_t val;
    uint8_t i;
    uint8_t cks;
    uint8_t phase;
    uint8_t numofdigit;}
Decode;

private uint8_t IHEX_Hex2Int(int8_t c, uint8_t NumOfDigit, uint32_t *pVal, uint8_t *pCks) // <editor-fold defaultstate="collapsed" desc="Check hex">
{
    if(Hex2IntCxt.i==0)
        Hex2IntCxt.val=0;

    if(Hex2IntCxt.j==0)
        *pVal=0;

    Hex2IntCxt.val<<=4;

    if((c>='0')&&(c<='9'))
        Hex2IntCxt.val|=(uint8_t) (c-'0');
    else if((c>='A')&&(c<='F'))
        Hex2IntCxt.val|=(uint8_t) (c-'A'+10);
    else if((c>='a')&&(c<='f'))
        Hex2IntCxt.val|=(uint8_t) (c-'a'+10);
    else
    {
        Hex2IntCxt.i=0;
        Hex2IntCxt.j=0;
        Hex2IntCxt.val=0;
        *pCks=0;
        return IHEX_ERROR;
    }

    Hex2IntCxt.i++;

    if(Hex2IntCxt.i>=2)
    {
        Hex2IntCxt.i=0;
        Hex2IntCxt.j+=2;
        *pCks=*pCks+Hex2IntCxt.val;
        *pVal=*pVal<<8;
        *pVal|=Hex2IntCxt.val;
        Hex2IntCxt.val=0;

        if(Hex2IntCxt.j>=NumOfDigit)
        {
            Hex2IntCxt.i=0;
            Hex2IntCxt.j=0;
            Hex2IntCxt.val=0;
            return IHEX_DONE;
        }
    }

    return IHEX_BUSY;
} // </editor-fold>

public bool IHEX_IsHexData(uint8_t c) // <editor-fold defaultstate="collapsed" desc="Check hex data">
{
    if((c>='0')&&(c<='9'))
        return 1;

    if((c>='A')&&(c<='F'))
        return 1;

    if((c>='a')&&(c<='f'))
        return 1;

    if(c=='\r')
        return 1;

    if(c=='\n')
        return 1;

    if(c==':')
        return 1;

    return 0;
} // </editor-fold>

public void IHEX_Init(void) // <editor-fold defaultstate="collapsed" desc="Intel hex process initialize">
{
    memset(&Hex2IntCxt, 0, sizeof (Hex2IntCxt));
    memset(&Decode, 0, sizeof (Decode)); // Set all members to 0
    Decode.numofdigit=2;
} // </editor-fold>

public uint8_t IHEX_Decode(int8_t c) // <editor-fold defaultstate="collapsed" desc="Intel hex parsing">
{
    uint32_t tmp;
    uint8_t this_task_rslt;

    this_task_rslt=IHEX_Hex2Int(c, Decode.numofdigit, &Decode.val, &Decode.cks);

    if(this_task_rslt==IHEX_ERROR)
    {
        if(Decode.phase!=0)
        {
            IHEX_ErrorLogWrite(__LINE__);
            goto EXIT;
        }
    }
    else if(this_task_rslt==IHEX_BUSY)
    {
        return IHEX_BUSY;
    }
    else
        this_task_rslt=IHEX_ERROR;

    switch(Decode.phase)
    {
        case 0: // <editor-fold defaultstate="collapsed" desc="Get start code">
            if(c==':')
            {
                Decode.numofdigit=2;
                Decode.Hex.ByteCount=0;
                Decode.phase=1;
            } // </editor-fold>
            break;

        case 1: // <editor-fold defaultstate="collapsed" desc="Get byte count">
            Decode.Hex.ByteCount=(uint8_t) Decode.val;
            Decode.numofdigit=4;
            Decode.phase=2; // </editor-fold>
            break;

        case 2: // <editor-fold defaultstate="collapsed" desc="Get address">
            Decode.addr=Decode.val;
            Decode.numofdigit=2;
            Decode.phase=3;
            Decode.Hex.RecordType=0; // </editor-fold>
            break;

        case 3: // <editor-fold defaultstate="collapsed" desc="Get record type">
            Decode.Hex.RecordType=(uint8_t) Decode.val;

            switch(Decode.Hex.RecordType)
            {
                case IHEX_RECTYPE_DAT:
                    Decode.i=0;
                    Decode.phase=4;
                    Decode.numofdigit=2;
                    Decode.Hex.Address.Value=Decode.addr;
                    memset(Decode.Hex.Data, 0xFF, IHEX_DATA_SIZE); // reset buffer to blank
                    break;

                case IHEX_RECTYPE_EOF:
                    Decode.phase=7;
                    Decode.numofdigit=0;
                    break;

                case IHEX_RECTYPE_ESA:
                    Decode.addr=0;
                    Decode.phase=5;
                    Decode.numofdigit=4;
                    break;

                case IHEX_RECTYPE_ELA:
                    Decode.addr=0;
                    Decode.phase=6;
                    Decode.numofdigit=4;
                    break;

                case IHEX_RECTYPE_SSA:
                case IHEX_RECTYPE_SLA:
                    Decode.phase=8;
                    Decode.numofdigit=8;
                    break;

                default:
                    IHEX_ErrorLogWrite(__LINE__);
                    goto EXIT;
            } // </editor-fold>
            break;

        case 4: // <editor-fold defaultstate="collapsed" desc="Get data">
            Decode.Hex.Data[Decode.i]=(uint8_t) Decode.val;

            if(++Decode.i==Decode.Hex.ByteCount)
            {
                Decode.phase=7;
                Decode.Hex.Checksum=0;
            } // </editor-fold>
            break;

        case 5: // <editor-fold defaultstate="collapsed" desc="Get extended segment address">
            Decode.addr=Decode.val;
            Decode.phase=7;
            Decode.numofdigit=2;
            Decode.Hex.Address.ExtSeg=(Decode.addr<<8);
            Decode.Hex.Address.ExtLin=0; // </editor-fold>
            break;

        case 6: // <editor-fold defaultstate="collapsed" desc="Get extended linear address">
            Decode.addr=Decode.val;
            Decode.phase=7;
            Decode.numofdigit=2;
            Decode.Hex.Address.ExtLin=(Decode.addr<<16);
            Decode.Hex.Address.ExtSeg=0; // </editor-fold>
            break;

        case 7: // <editor-fold defaultstate="collapsed" desc="Get checksum">
            Decode.Hex.Checksum=(uint8_t) Decode.val;
            Decode.phase=0;
            Decode.numofdigit=2;

            if((Decode.cks&0xFF)==0x00)// checksum matched
            {
                if(Decode.Hex.RecordType==IHEX_RECTYPE_DAT)
                {
                    tmp=Decode.Hex.Address.Value+Decode.Hex.Address.ExtLin+Decode.Hex.Address.ExtSeg;

                    if(IHEX_NVM_Write(tmp, Decode.Hex.Data, Decode.Hex.ByteCount)==IHEX_ERROR)
                    {
                        IHEX_ErrorLogWrite(__LINE__);
                        goto EXIT;
                    }
                }
                else if(Decode.Hex.RecordType==IHEX_RECTYPE_EOF)
                {
                    this_task_rslt=IHEX_DONE;
                    goto EXIT;
                }
            }
            else
            {
                IHEX_ErrorLogWrite(__LINE__);
                goto EXIT;
            } // </editor-fold>
            break;

        case 8: // <editor-fold defaultstate="collapsed" desc="Exception">
            Decode.phase=7;
            Decode.numofdigit=2; // </editor-fold>
            break;

        default:
            IHEX_ErrorLogWrite(__LINE__);
            goto EXIT;
    }

    return IHEX_BUSY;

EXIT:
    IHEX_Init();

    if(this_task_rslt==IHEX_DONE)
    {
        if(IHEX_NVM_Write(0, NULL, 0)==IHEX_ERROR) // Write the last data and/or reset all states in IHEX_NVM_Write
        {
            IHEX_ErrorLogWrite(__LINE__);
            this_task_rslt=IHEX_ERROR;
        }
    }

    return this_task_rslt;
} // </editor-fold>
