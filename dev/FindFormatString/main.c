#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../misc/util.h"

typedef struct
{
    size_t Len;
    uint16_t Idx;
    uint8_t C0;
    uint8_t C1;
} findfs_cxt_t;

bool FindFormatString(char c, findfs_cxt_t *pCxt, const char *pFormat)
{
    if(pCxt->Len==0)
    {
        pCxt->Idx=0;
        pCxt->Len=slen(pFormat);

        if(pCxt->Len==0)
            return 0;

        pCxt->C0=pFormat[pCxt->Idx++];

        if(pCxt->C0=='*')
            pCxt->C1=pFormat[pCxt->Idx++];
    }

    if(pCxt->C0=='*')
    {
        if(c==pCxt->C1)
        {
            pCxt->C0=pFormat[pCxt->Idx++];

            if(pCxt->C0=='*')
                pCxt->C1=pFormat[pCxt->Idx++];
        }
    }
    else if(c==pCxt->C0)
    {
        pCxt->C0=pFormat[pCxt->Idx++];

        if(pCxt->C0=='*')
            pCxt->C1=pFormat[pCxt->Idx++];
    }
    else
    {
        pCxt->Len=0;
        return 0;
    }

    if(pCxt->Idx==(pCxt->Len-1))
    {
        pCxt->Len=0;
        return 1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    char input_data[]="Visit my page at https://github.com/sampidevkit to download the source code\r\n";
    size_t i, len=slen(input_data);
    findfs_cxt_t FindCxt;

    for(i=0, FindCxt.Len=0; i<len; i++)
    {
        if(FindFormatString(input_data[i], &FindCxt, "https://*.*t"))
            printf("\r\nFounded your string");
    }

    return (EXIT_SUCCESS);
}

