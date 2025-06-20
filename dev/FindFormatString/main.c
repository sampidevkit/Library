#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    size_t Len;
    uint16_t Idx;
    uint8_t Skip;
} findfs_cxt_t;

size_t slen(const char *pStr)
{
    size_t len=0;

    while((*pStr!=0)&&(*pStr!=255))
    {
        len++;
        pStr++;
    }

    return len;
}

bool FindFormatString(char c, findfs_cxt_t *pCxt, const char *pFormat)
{
    if(pCxt->Len==0)
    {
        pCxt->Idx=0;
        pCxt->Skip=0;
        pCxt->Len=slen(pFormat);

        if(pCxt->Len==0)
        {
            return 0;
        }
    }

    while(pFormat[pCxt->Idx]=='*')
    {
        pCxt->Skip^=1;
        pCxt->Idx++;
    }

    if(pCxt->Skip==1)
    {
        if(c==pFormat[pCxt->Idx])
        {
            pCxt->Skip=0;
            pCxt->Idx++;
        }
    }
    else if(c==pFormat[pCxt->Idx])
    {
        pCxt->Idx++;
    }
    else
    {
        pCxt->Len=0;
        return 0;
    }

    if(pCxt->Idx==pCxt->Len)
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
    bool found;

    for(i=0, found=0, FindCxt.Len=0; i<len; i++)
    {
        if(FindFormatString(input_data[i], &FindCxt, "sampidevkit"))
        {
            printf("\r\nFounded your string");
            found=1;
            break;
        }
    }

    if(found==0)
        printf("\r\nYour string not found");

    for(i=0, found=0, FindCxt.Len=0; i<len; i++)
    {
        if(FindFormatString(input_data[i], &FindCxt, "\r"))
        {
            printf("\r\nFounded your string");
            found=1;
            break;
        }
    }

    if(found==0)
        printf("\r\nYour string not found");

    for(i=0, found=0, FindCxt.Len=0; i<len; i++)
    {
        if(FindFormatString(input_data[i], &FindCxt, "https://*.*/*"))
        {
            printf("\r\nFounded your string");
            found=1;
            break;
        }
    }

    if(found==0)
        printf("\r\nYour string not found");

    for(i=0, found=0, FindCxt.Len=0; i<len; i++)
    {
        if(FindFormatString(input_data[i], &FindCxt, "https://*.*/ "))
        {
            printf("\r\nFounded your string");
            found=1;
            break;
        }
    }

    if(found==0)
        printf("\r\nYour string not found");

    for(i=0, found=0, FindCxt.Len=0; i<len; i++)
    {
        if(FindFormatString(input_data[i], &FindCxt, "https://*.*/*to"))
        {
            printf("\r\nFounded your string");
            found=1;
            break;
        }
    }

    if(found==0)
        printf("\r\nYour string not found");

    for(i=0, found=0, FindCxt.Len=0; i<len; i++)
    {
        if(FindFormatString(input_data[i], &FindCxt, "https://*.*/* to"))
        {
            printf("\r\nFounded your string");
            found=1;
            break;
        }
    }

    if(found==0)
        printf("\r\nYour string not found");

    return (EXIT_SUCCESS);
}

