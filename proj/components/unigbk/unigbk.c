
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lv_examples.h"

static const char *TAG = "unigbk";


static char UK_FLAG = 0;
static const char* UNIGBK_PATH = "/sdcard/utf16.bin";

FILE* bin_fa = NULL;
#define PARSE_ERROR 0
/*
Unicode符号范围     |        UTF-8编码方式
(十六进制)        |              （二进制）
----------------------+---------------------------------------------
0000 0000-0000 007F | 0xxxxxxx
0000 0080-0000 07FF | 110xxxxx 10xxxxxx
0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
/**
 * @brief utf8编码转unicode字符集(usc4)，最大支持4字节utf8编码，(4字节以上在中日韩文中为生僻字)
 * @param *utf8 utf8变长编码字节集1~4个字节
 * @param *unicode utf8编码转unicode字符集结果，最大4个字节，返回的字节序与utf8编码序一致
 * @return length 0：utf8解码异常，others：本次utf8编码长度
 */
uint8_t UTF8ToUnicode(uint8_t* utf8, uint32_t* unicode) {
    const uint8_t lut_size = 3;
    const uint8_t length_lut[] = { 2, 3, 4 };
    const uint8_t range_lut[] = { 0xE0, 0xF0, 0xF8 };
    const uint8_t mask_lut[] = { 0x1F, 0x0F, 0x07 };

    uint8_t length = 0;
    uint8_t b = *(utf8 + 0);
    uint32_t i = 0;

    if (utf8 == NULL) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    // utf8编码兼容ASCII编码,使用0xxxxxx 表示00~7F
    if (b < 0x80) {
        *unicode = b;
        return 1;
    }
    // utf8不兼容ISO8859-1 ASCII拓展字符集
    // 同时最大支持编码6个字节即1111110X
    if (b < 0xC0 || b > 0xFD) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    for (i = 0; i < lut_size; i++) {
        if (b < range_lut[i]) {
            *unicode = b & mask_lut[i];
            length = length_lut[i];
            break;
        }
    }
    // 超过四字节的utf8编码不进行解析
    if (length == 0) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    // 取后续字节数据
    for (i = 1; i < length; i++) {
        b = *(utf8 + i);
        // 多字节utf8编码后续字节范围10xxxxxx‬~‭10111111‬
        if (b < 0x80 || b > 0xBF) {
            break;
        }
        *unicode <<= 6;
        // ‭00111111‬
        *unicode |= (b & 0x3F);
    }
    // 长度校验
    return (i < length) ? PARSE_ERROR : length;
}

/**
 * @brief 4字节unicode(usc4)字符集转utf16编码
 * @param unicode unicode字符值
 * @param *utf16 utf16编码结果
 * @return utf16长度，(2字节)单位
 */
uint8_t UnicodeToUTF16(uint32_t unicode, uint16_t* utf16)
{
    // Unicode范围 U+000~U+FFFF
    // utf16编码方式：2 Byte存储，编码后等于Unicode值
    if (unicode <= 0xFFFF) {
        if (utf16 != NULL) {
            *utf16 = (unicode & 0xFFFF);
        }
        return 1;
    }
    else if (unicode <= 0xEFFFF) {
        if (utf16 != NULL) {
            // 高10位
            *(utf16 + 0) = 0xD800 + (unicode >> 10) - 0x40;
            // 低10位
            *(utf16 + 1) = 0xDC00 + (unicode & 0x03FF);
        }
        return 2;
    }

    return 0;
}



int unigbk_open(void)
{
    lv_fs_res_t res;
    if (!UK_FLAG) {
        bin_fa = fopen(UNIGBK_PATH, "rb");
        if (bin_fa == NULL) {
            ESP_LOGE(TAG, "Failed to open %s .", UNIGBK_PATH);
            UK_FLAG = 0;
            return 0; // 返回错误
        }
        UK_FLAG = 1;
    }
    return 27852;
}

int unigbk_read(int off, void* buff, int size)
{
    uint32_t rn;
    fseek(bin_fa, off, SEEK_SET);        // 将文件指针移动到指定的偏移量处
    return fread(buff, size, 1, bin_fa); // 从文件中读取数据到font_info结构体变量
}

static void unigbk_close(void)
{
    fclose(bin_fa);
    bin_fa = NULL;
    UK_FLAG = 0;
}
static int binarySearch(int target, uint8_t* out)
{
    int left = 0;
    int right = 0;//size - 1;
    int arr;
    int size = unigbk_open() / 4;
    if (size != 6963)
    {
        // unigbk_close();
        return -1;
    }
    
    right = size - 1;
    int mid = 0;
    printf("filelen = %d\n", size);
    while (left <= right) {
        mid = left + (right - left) / 2;
        if (unigbk_read(mid * 4, &arr, 4) <=0 )
        {
            printf("read fail:%s\n", UNIGBK_PATH);
            return -1;
        }
        // 检查目标值是否在中间位置
        if ((arr & 0x0000ffff) == (target & 0x0000ffff)) {
            printf("find target:%x\n", arr);
            out[0] = (arr >> 24) & 0xff;
            out[1] = (arr >> 16) & 0xff;
            return mid;
        }

        // 目标值比中间位置小，在左半部分继续查找
        if ((arr & 0x0000ffff) > (target & 0x0000ffff)) {
            right = mid - 1;
        }
        // 目标值比中间位置大，在右半部分继续查找
        else {
            left = mid + 1;
        }
    }
    printf("Not find target:%d\n", mid);
    // 目标值不存在
    return -1;
}

int utf8togbk(uint8_t* in, uint8_t* out)
{
    // 严 utf8 E4 B8 A5
    // 木 E6 9C A8
    uint32_t buffer;
    uint8_t utf8[8];//= "\xE6\x9C\xA8"; //"\xE4\xB8\xA5";
    uint16_t utf16[2] = { 0 };
    memcpy(utf8, in, 3);
    uint8_t len = UTF8ToUnicode(utf8, &buffer);

    printf("len:%d\n", len);
    printf("buffer:%x\n", (unsigned int)buffer);

    len = UnicodeToUTF16(buffer, utf16);

    printf("len:%d\n", len);
    printf("utf16[0]:%x\n", utf16[0]);
    printf("utf16[1]:%x\n", utf16[1]);
    uint8_t gbkout[8] = { 0 };
    // unigbk_open();
    // unigbk_read(0,gbkout,4);
    if(binarySearch(buffer, gbkout) < 0)
    {
        printf("binarySearch:err\n");
    }
    memcpy(out, gbkout, 3);
    // uni2gbk(utf16,gbkout);
    printf("gbkout[0]:%x\n", gbkout[0]);
    printf("gbkout[1]:%x\n", gbkout[1]);
    printf("gbkout[2]:%x\n", gbkout[2]);
    printf("gbkout[3]:%x\n", gbkout[3]);
    unigbk_close();
    return 0;
}

