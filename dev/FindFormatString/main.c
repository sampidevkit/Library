#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// --- PHẦN MÃ GỐC (chỉ giữ lại slen) ---

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

// --- PHẦN TỐI ƯU ---

// 1. Tách cấu hình (không đổi) và trạng thái (thay đổi liên tục)
// Cấu hình tìm kiếm: chứa thông tin tĩnh về mẫu cần tìm

typedef struct
{
    const char *Pattern;
    size_t Len;
} find_config_t;

// Trạng thái tìm kiếm: chứa các biến thay đổi trong quá trình tìm

typedef struct
{
    uint16_t Idx;
    uint8_t Skip;
} find_state_t;

bool FindFormatString(char c, find_state_t *pCxt, const find_config_t *pFormat)
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

/**
 * @brief Hàm trợ giúp để thực hiện tìm kiếm và in kết quả
 */
void TestAndPrintResult(const char* input_data, const char* pattern)
{
    size_t i;

    // 2. Cấu hình chỉ được thiết lập một lần
    find_config_t config={.Pattern=pattern, .Len=slen(pattern)};

    if(config.Len==0)
    {
        printf("\n -> Pattern is empty, cannot search.");
        return;
    }

    printf("\nSearching for: \"");

    for(i=0; i<config.Len; i++)
    {
        if(is_printable((uint8_t) config.Pattern[i]))
            printf("%c", config.Pattern[i]);
        else
            printf("<%02X>", config.Pattern[i]);
    }
    
    printf("\", len=%d", config.Len);

    // Trạng thái tìm kiếm được khởi tạo
    find_state_t state={.Idx=0, .Skip=0};
    bool found=false;
    size_t input_len=slen(input_data);

    for(i=0; i<input_len; i++)
    {
        if(FindFormatString_Optimized(input_data[i], &state, &config))
        {
            found=true;
            break;
        }
    }

    if(found)
        printf("\n -> Founded your string");
    else
        printf("\n -> Your string not found");
}

int main(int argc, char** argv)
{
    char input_data[]="Visit my page at * * https://github.com/sampidevkit to download the source code\r\n";

    // 3. Hàm main giờ đây gọn gàng và dễ đọc hơn rất nhiều
    TestAndPrintResult(input_data, "sampidevkit");
    TestAndPrintResult(input_data, "\r");
    TestAndPrintResult(input_data, "https://*.*/*");
    TestAndPrintResult(input_data, "https://*.*/ "); // Sẽ thất bại
    TestAndPrintResult(input_data, "https://*.*/*to");
    TestAndPrintResult(input_data, "https://*.*/* to");
    TestAndPrintResult(input_data, "page at **");

    printf("\n");
    return (EXIT_SUCCESS);
}