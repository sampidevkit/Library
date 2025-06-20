#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// --- PHẦN MÃ GỐC (chỉ giữ lại slen) ---

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

/**
 * @brief Hàm tìm kiếm đã được tối ưu.
 *
 * @param c Ký tự đầu vào hiện tại.
 * @param pState Con trỏ đến trạng thái tìm kiếm (thay đổi).
 * @param pConfig Con trỏ đến cấu hình tìm kiếm (không đổi).
 * @return true nếu tìm thấy toàn bộ chuỗi, false trong các trường hợp khác.
 */
bool FindFormatString_Optimized(char c, find_state_t *pState, const find_config_t *pConfig)
{
    // Xử lý ký tự đại diện '*'
    // Vòng lặp này chỉ chạy khi con trỏ mẫu đang ở vị trí của '*'
    while(pState->Idx<pConfig->Len&&pConfig->Pattern[pState->Idx]=='*')
    {
        pState->Skip^=1; // Đảo trạng thái Skip
        pState->Idx++;
    }

    // Nếu đã duyệt hết mẫu sau khi xử lý '*', nghĩa là mẫu kết thúc bằng '*'
    if(pState->Idx==pConfig->Len)
    {
        return true;
    }

    bool match=false;
    if(pState->Skip==1) // Chế độ bỏ qua
    {
        if(c==pConfig->Pattern[pState->Idx])
        {
            pState->Skip=0; // Tắt chế độ bỏ qua khi tìm thấy ký tự khớp
            pState->Idx++;
            match=true;
        }
    }
    else // Chế độ so khớp bình thường
    {
        if(c==pConfig->Pattern[pState->Idx])
        {
            pState->Idx++;
            match=true;
        }
    }

    if(!match)
    {
        // Nếu không khớp, reset trạng thái để bắt đầu tìm lại từ đầu mẫu
        pState->Idx=0;
        pState->Skip=0;

        // QUAN TRỌNG: Kiểm tra lại ký tự hiện tại với ký tự đầu tiên của mẫu
        // Điều này xử lý trường hợp như tìm "aba" trong "ababa"
        if(c==pConfig->Pattern[pState->Idx])
        {
            pState->Idx++;
        }
    }

    // Kiểm tra xem đã khớp toàn bộ mẫu chưa
    if(pState->Idx==pConfig->Len)
    {
        // Khớp hoàn toàn, reset để có thể tìm lần nữa (nếu muốn)
        pState->Idx=0;
        pState->Skip=0;
        return true;
    }

    return false;
}

/**
 * @brief Hàm trợ giúp để thực hiện tìm kiếm và in kết quả
 */
void TestAndPrintResult(const char* input_data, const char* pattern)
{
    printf("\nSearching for: \"%s\"", pattern);

    // 2. Cấu hình chỉ được thiết lập một lần
    find_config_t config={.Pattern=pattern, .Len=slen(pattern)};
    if(config.Len==0)
    {
        printf("\n -> Pattern is empty, cannot search.");
        return;
    }

    // Trạng thái tìm kiếm được khởi tạo
    find_state_t state={.Idx=0, .Skip=0};

    bool found=false;
    size_t input_len=slen(input_data);

    for(size_t i=0; i<input_len; i++)
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
    char input_data[]="Visit my page at https://github.com/sampidevkit to download the source code\r\n";

    // 3. Hàm main giờ đây gọn gàng và dễ đọc hơn rất nhiều
    TestAndPrintResult(input_data, "sampidevkit");
    TestAndPrintResult(input_data, "\r");
    TestAndPrintResult(input_data, "https://*.*/*");
    TestAndPrintResult(input_data, "https://*.*/ "); // Sẽ thất bại
    TestAndPrintResult(input_data, "https://*.*/*to");
    TestAndPrintResult(input_data, "https://*.*/* to");

    printf("\n");
    return (EXIT_SUCCESS);
}