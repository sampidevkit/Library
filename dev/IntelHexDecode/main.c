#include "misc/intel_hex.h"

int main(int argc, char** argv)
{
    uint8_t rslt;
    FILE *pFs=fopen("app.hex", "r");

    if(pFs==NULL)
    {
        printf("\r\nLoad hex file failed");
        return (EXIT_FAILURE);
    }

    IHEX_Init();

    while(true)
    {
        int c=fgetc(pFs);

        if(c==EOF)
            break;

        rslt=IHEX_Decode((uint8_t) c);

        if(rslt==IHEX_ERROR)
        {
            printf("\r\nDecode fail");
            break;
        }

        if(rslt==IHEX_DONE)
        {
            printf("\r\nDecode done");
            break;
        }
    }

    fclose(pFs);
    return (EXIT_SUCCESS);
}

