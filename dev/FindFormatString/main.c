#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_printable(uint8_t c)
{
    if((c>=' ')&&(c<='~'))
        return 1;

    return 0;
}

size_t slen(const char *pStr)
{
    size_t len=0;
    while((*pStr!=0) && (*pStr!=255))
    {
        len++;
        pStr++;
    }
    return len;
}

typedef struct
{
    const char *Pattern;
    size_t Len;
    uint16_t Idx;
    uint8_t Skip;
} find_cxt_t;

void FindStrf_Init(find_cxt_t *pFindCxt, const uint8_t *pSample)
{
    pFindCxt->Pattern=pSample;
    pFindCxt->Len=slen(pSample);
    pFindCxt->Idx=0;
    pFindCxt->Skip=false;
}

bool FindStrf(char c, find_cxt_t *pFindCxt)
{
    if(pFindCxt->Len==0)
        return false;

    while(pFindCxt->Pattern[pFindCxt->Idx]=='*')
    {
        pFindCxt->Skip=1;
        pFindCxt->Idx++;
    }

    if(pFindCxt->Skip==1)
    {
        if(c==pFindCxt->Pattern[pFindCxt->Idx])
        {
            pFindCxt->Idx++;
            pFindCxt->Skip++;
        }
    }
    else if(pFindCxt->Skip>1)
    {
        if(c!=pFindCxt->Pattern[pFindCxt->Idx])
        {
            pFindCxt->Idx-=pFindCxt->Skip;
            pFindCxt->Skip=1;
        }
        else
        {
            pFindCxt->Idx++;
            pFindCxt->Skip++;
        }
    }
    else if(c==pFindCxt->Pattern[pFindCxt->Idx])
    {
        pFindCxt->Idx++;
    }
    else
    {
        pFindCxt->Idx=0;
    }

EXIT:
    if(pFindCxt->Idx==pFindCxt->Len)
    {
        pFindCxt->Idx=0;
        pFindCxt->Skip=0;
        return 1;
    }

    return 0;
}

void TestAndPrintResult(const char* input_data, const char* pattern, bool resultCheck)
{
    size_t i;
    find_cxt_t FindCxt;

    FindStrf_Init(&FindCxt, pattern);

    if(FindCxt.Len==0)
    {
        printf("\n\n -> Pattern is empty, cannot search.");
        return;
    }

    printf("\n\nSearching for: \"");

    for(i=0; i<FindCxt.Len; i++)
    {
        if(is_printable((uint8_t) FindCxt.Pattern[i]))
            printf("%c", FindCxt.Pattern[i]);
        else
            printf("<%02X>", FindCxt.Pattern[i]);
    }

    printf("\", len=%d", FindCxt.Len);

    bool found=false;
    size_t input_len=slen(input_data);

    for(i=0; i<input_len; i++)
    {
        if(FindStrf(input_data[i], &FindCxt))
        {
            found=true;
            break;
        }
    }

    if(found)
        printf("\n -> Founded your string");
    else
        printf("\n -> Your string not found");

    if(resultCheck==found)
        printf("\n  |\n  |--> Correct");
    else
        printf("\n  |\n  |--> Incorrect");
}

int main(int argc, char** argv)
{
    char input_data[]="Visit my page at https://github.com/sampidevkit to download the source code\r\n";

    TestAndPrintResult(input_data, "sampidevkit", true);
    TestAndPrintResult(input_data, "\r", true);
    TestAndPrintResult(input_data, "https://*.*/*", true);
    TestAndPrintResult(input_data, "https://*.*/ ", false);
    TestAndPrintResult(input_data, "https://*.*/*to", true);
    TestAndPrintResult(input_data, "https://*.*/* to", true);
    TestAndPrintResult(input_data, "https://*.*/* t0", false);
    TestAndPrintResult("192.168.1.123:456", "*.*.*.*:457", false);
    printf("\n");
    return (EXIT_SUCCESS);
}