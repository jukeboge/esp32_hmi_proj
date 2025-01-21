
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lv_examples.h"
#include "ui.h"
#include "unigbk.h"
static const char* TAG = "font_parse";

enum {
    SHX_TYPE_INVALID = 0,
    SHX_TYPE_BIGFONT = 1,
    SHX_TYPE_SHAPES = 2,
    SHX_TYPE_UNIFONT = 3,
}ShxType;

typedef struct {
    uint16_t character; // 字符
    uint16_t shapeType; // 形状类型
    uint16_t numPoints; // 点数
    uint16_t* points;   // 点的数组
} ShxCharacter;

#pragma pack(1)
typedef struct {
    uint16_t index_sum; //
    uint16_t escape_num; //
} bigfont_info;
#pragma pack()

#pragma pack(1)
typedef struct {
    uint16_t index_byte; //
    bigfont_info info;
}  bigfont_header;
#pragma pack()

#pragma pack(1)

typedef struct
{
    uint16_t code;   // 2个字节的编码
    uint16_t length; // 2个字节的长度
    uint32_t offset; // 4个字节的绝对地址偏移
} IndexItem;

#pragma pack()

typedef struct
{
    uint16_t width;   // 字宽
    uint16_t height; // 字高
    uint16_t dir; // 方向
}FontBase;

#pragma pack(1)
typedef struct
{
    uint8_t pen;  // bit7，1：落笔，0：抬笔
    uint8_t x[3]; // 
    uint8_t y[3]; // 
    uint8_t z[3]; //
}SendPoint_t;
#pragma pack()

//文件版本结束：0x0d,0x0a,0x1a
static int read_header_from_shx(FILE* file, char* header, int offset)
{
    int header_size = 256;
    char* data = (char*)malloc(header_size + 1);
    int index = 0;
    int ending_position = offset;

    fseek(file, offset, SEEK_SET);

    char current_char, prev_char = '\0';
    while (index < header_size && fread(&current_char, sizeof(char), 1, file) == 1) {
        ending_position++;
        if (prev_char == 0x0d && current_char == 0x0a && fread(&current_char, sizeof(char), 1, file) == 1) {
            if (current_char == 0x1a) {
                ending_position++;
                break;
            }
            fseek(file, -1, SEEK_CUR);
        }

        data[index++] = current_char;
        prev_char = current_char;
    }

    data[index] = '\0';
    strncpy(header, data, header_size);

    free(data);

    return ending_position;
}


static int find_keyword_in_data(char* data)
{
    char* keywords[] = { "bigfont", "shapes", "unifont" };
    int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < num_keywords; i++) {
        if (strstr(data, keywords[i]) != NULL) {
            switch (i) {
            case 0:
                return SHX_TYPE_BIGFONT; // 返回1表示找到了"bigfont"关键词
            case 1:
                return SHX_TYPE_SHAPES; // 返回2表示找到了"shapes"关键词
            case 2:
                return SHX_TYPE_UNIFONT; // 返回3表示找到了"unifont"关键词
            }
        }
    }

    return SHX_TYPE_INVALID; // 返回0表示未找到任何关键词
}

//从file 的offset 位置读取数据到font_info，返回新的offset
static int read_bigfont_info(FILE* file, int offset, bigfont_header* font_info)
{
    fseek(file, offset, SEEK_SET); // 将文件指针移动到指定的偏移量处
    fread(font_info, sizeof(bigfont_header), 1, file); // 从文件中读取数据到font_info结构体变量
    return offset + sizeof(bigfont_header); // 返回新的偏移量
}

static int parse_font(uint16_t pinyin, char* font, uint32_t len, FontBase* base,uint8_t **out,uint32_t *outlen)
{
#if !CU_COMPILE_DEBUG
    uint8_t pendown = 0;
    uint32_t didx = 0;
    char* data = font;
    FontBase* pbase = base;
    int8_t dx = 0;
    int8_t dy = 0;
    char vfactx = 1;
    char vfacty = 1;
    lv_point_t curpt = { 100, 150 };
    lv_point_t endpt;
    lv_point_t m_curpt = { 0, 0 };
    lv_point_t m_endpt = { 0, 0 };
    char stack[4][2]; // 栈缓冲区
    uint8_t stack_point = 0;
    int npt = 0;
    short cmdcode = 0, last_cmd = 0;
    short vertonly = 0, gotdxdy = 0, circ, genpc, arcmode;
    short forcependown = 1;
    short repmode = 0; /* 0, 9, or 13 (the repeating commands) */
    short fi1;
    short skip = 0;
    uint8_t xDelta = 0, yDelta = 0;
    double bulge = 0;
    uint32_t pindex = 0;
    uint32_t tempx = 0;
    uint32_t tempy = 0;
    uint32_t tempz = 0;
    #if 0
    SendPoint_t* SendPoint = (SendPoint_t*)malloc(sizeof(SendPoint_t) * 1600);
    if (!SendPoint) {
        ESP_LOGE(TAG, "parse_font SendPoint is NULL!");
    }
    else{
        memset(SendPoint, 0, sizeof(SendPoint_t) * 1600);
    }
    #endif
    // send_index = 0;
    // pen_init(curpt);
    // SendPoint[pindex].pen = 0x80;
    for (didx = 0; didx < len; didx++) {
        vertonly = gotdxdy = circ = genpc = 0; arcmode = -1;
        if (repmode) {
            if (++didx < len) {
                if (!data[didx - 1] && !data[didx])
                    repmode = 0;
                if (repmode == 9) {
                    if (!skip) {
                        dx = data[didx - 1];
                        dy = data[didx];
                        gotdxdy = 1;
                    }
                }
                else if (repmode == 13) {
                    if (++didx < len && !skip) {
                        arcmode = 1;
                        dx = data[didx - 2];
                        dy = data[didx - 1];
                        fi1 = data[didx];
                        if (fi1 > 127)
                            fi1 = 127;
                        else if (fi1 < -127)
                            fi1 = -127;
                        bulge = fi1 / 127.0;
                    }
                }
            }
        }
        else {
            cmdcode = data[didx];//(forcependown) ? 1 : data[didx];
            switch (cmdcode) {
            case 0: /* 结束 */
                didx = len;
                break;
            case 1: /* 落笔 */
                /* If we're doing a forced pendown, decrement */
                /* didx so that we don't eat a def byte: */
                if (forcependown) {
                    // if(didx) didx--;
                    forcependown = 0;
                }
                ESP_LOGI(TAG," pen down [%lu]", didx);
                pendown = 1; dx = dy = 0; gotdxdy = 1;
                // pendown = 1;
                npt = 1;
                // SendPoint[pindex].pen = 0x80;
                break;
            case 2: /* 抬笔 */
                ESP_LOGI(TAG," pen up [%lu]", didx);
                pendown = 0;
                npt = 0;
                // SendPoint[pindex].pen = 0x80;
                if (data[didx + 1] == 0)//结束
                {
                    pindex++;
                }
                break;
            case 3: /* 字型缩小 */
                if (++didx >= len)
                    break;
                if (skip)
                    break;
                if (!data[didx])
                    break;
                vfactx /= ((unsigned char)data[didx]);
                vfacty /= ((unsigned char)data[didx]);
                break;
            case 4: /* 字型放大 */
                if (++didx >= len)
                    break;
                if (skip)
                    break;
                if (!data[didx])
                    break;
                vfactx *= ((unsigned char)data[didx]);
                vfacty *= ((unsigned char)data[didx]);
                break;
            case 5: /* 压栈 */
                if (vertonly) {
                    vertonly = false;
                    break;
                }
                if (stack_point == 3)
                    break;
                stack[stack_point][0] = dx;
                stack[stack_point][1] = dy;
                stack_point++;
                break;
            case 6: /* 出栈 */
                if (vertonly) {
                    vertonly = false;
                    break;
                }
                if (stack_point == 0)
                    break;
                stack_point--;
                dx = stack[stack_point][0];
                dy = stack[stack_point][1];
                gotdxdy = 1;
                /* Set css->forcependown if it's currently down: 如果是下笔状态，给予向前动力*/
                forcependown = (pendown != 0);
                /* Do a penup command: 起笔*/
                pendown = 0;
                break;
            case 7: /* Subshape 嵌套子对象 */
                break;
            case 8: /* 走一个相对位置，后面跟两个相对坐标 */
                if ((didx += 2) >= len)
                    break;
                dx = data[didx - 1];
                dy = data[didx];
                gotdxdy = 1;
                if (last_cmd == 1)
                    npt = 1;
                break;
            case 9:
            case 13: /* Repeat until (0,0) 重复 */
                repmode = data[didx];
                break;
            case 10: /* 八分圆弧 */
                // if ((didx += 2) >= len)
                //     break;
                // radius = data[didx - 1] * vfactx;
                // yDelta = data[didx];

                // if (vertonly)
                // {
                //     vertonly = false;
                //     break;
                // }
                break;
            case 11: /* 不规则圆弧*/
                break;
            case 12: /* Arc by bulge (next 3 bytes) 3字节圆弧, 指定凸度圆弧*/
                break;
            case 14: /* Process next command only for vertical text 垂直字体*/
                vertonly = 1;
                break;
            case 15: /* Not used 保留*/
                break;
            default:
                if (skip)
                    break;
                unsigned char vlen = (unsigned char)data[didx];
                char vdir = vlen & '\x0F';
                if (!(vlen >>= 4))
                    break;
                switch (vdir) {
                case '\x00':
                    dx = 1.0;
                    dy = 0.0;
                    break;
                case '\x01':
                    dx = 1.0;
                    dy = 0.5;
                    break;
                case '\x02':
                    dx = 1.0;
                    dy = 1.0;
                    break;
                case '\x03':
                    dx = 0.5;
                    dy = 1.0;
                    break;
                case '\x04':
                    dx = 0.0;
                    dy = 1.0;
                    break;
                case '\x05':
                    dx = -0.5;
                    dy = 1.0;
                    break;
                case '\x06':
                    dx = -1.0;
                    dy = 1.0;
                    break;
                case '\x07':
                    dx = -1.0;
                    dy = 0.5;
                    break;
                case '\x08':
                    dx = -1.0;
                    dy = 0.0;
                    break;
                case '\x09':
                    dx = -1.0;
                    dy = -0.5;
                    break;
                case '\x0A':
                    dx = -1.0;
                    dy = -1.0;
                    break;
                case '\x0B':
                    dx = -0.5;
                    dy = -1.0;
                    break;
                case '\x0C':
                    dx = 0.0;
                    dy = -1.0;
                    break;
                case '\x0D':
                    dx = 0.5;
                    dy = -1.0;
                    break;
                case '\x0E':
                    dx = 1.0;
                    dy = -1.0;
                    break;
                case '\x0F':
                    dx = 1.0;
                    dy = -0.5;
                    break;
                }
                dx *= vlen;
                dy *= vlen;
                gotdxdy = 1;
                break;
            }
        }
        last_cmd = cmdcode;
        if (gotdxdy) //|| arcmode > -1) /* Process vector or arc cmd 开始处理 矢量或圆弧*/
        {
            endpt.x = curpt.x + dx;//* 绘制坐标 x;
            endpt.y = curpt.y - dy;//* 绘制坐标 y;
            #if 0
            m_endpt.x = m_curpt.x + dx;//电机坐标 x
            m_endpt.y = m_curpt.y + dy;//电机坐标 y
            #endif
            if (pendown) {
                // printf("(%d,%d),(%d,%d)\n", endpt.x, endpt.y, dx, dy);
                if (npt == 1)
                    move_to(endpt);// 
                else if (npt > 1)
                    pen_draw_line(endpt);//
                #if 0
                if (npt == 2) {
                    SendPoint[pindex].pen = 0;
                }
                #endif
                npt++;
                // draw_line(endpt.x, endpt.y);
            }
            printf("(%ld,%ld),(%d,%d)\n", m_endpt.x, m_endpt.y, dx, dy);
            curpt = endpt;
            m_curpt = m_endpt;
            #if 0
            float x_count = get_x_count();
            float y_count = get_y_count();
            
            SendPoint[pindex].x[0] = ((uint32_t)(m_endpt.x * x_count)) & 0xff;
            SendPoint[pindex].x[1] = (((uint32_t)(m_endpt.x * x_count)) >> 8) & 0xff;
            SendPoint[pindex].x[2] = (((uint32_t)(m_endpt.x * x_count)) >> 16) & 0xff;
            tempx = (SendPoint[pindex].x[0] & 0xff) | ((SendPoint[pindex].x[1] << 8) & 0xff00) | ((SendPoint[pindex].x[2] << 16) & 0xff0000);
            SendPoint[pindex].y[0] = ((uint32_t)(m_endpt.y * y_count)) & 0xff;;
            SendPoint[pindex].y[1] = (((uint32_t)(m_endpt.y * y_count)) >> 8) & 0xff;
            SendPoint[pindex].y[2] = (((uint32_t)(m_endpt.y * y_count)) >> 16) & 0xff;
            tempy = (SendPoint[pindex].y[0] & 0xff) | ((SendPoint[pindex].y[1] << 8) & 0xff00) | ((SendPoint[pindex].y[2] << 16) & 0xff0000);
            SendPoint[pindex].z[0] = 0;
            SendPoint[pindex].z[1] = 0;
            SendPoint[pindex].z[2] = 0;
            #endif
            tempz = 0;

            // printf("pen=%d,x=%lu,y=%lu,z=%lu\n", SendPoint[pindex].pen,
            //     tempx, tempy, tempz);
            // printf("xl:%d,xh:%d\n", test1, test2);
            // printf("yl:%d,yh:%d\n", test1, test2);
            pindex++;
            if (pindex >= 1600) {
                pindex = 0;
                ESP_LOGE(TAG, "pindex out rang!");
            }
        }
    }
    #if 0
    if (pindex) {
        *out = malloc(pindex * sizeof(SendPoint_t));
        *outlen = pindex * sizeof(SendPoint_t);
        memcpy(*out, SendPoint, *outlen);
        ESP_LOGI(TAG, "parse_font out=%p,out_len=%lu", *out, *outlen);
        for (int i = 0; i < pindex; i++) {
            printf("[%d],pen=%d\n", i, SendPoint[i].pen);
        }
    }
    free(SendPoint);
    #endif
    // pen_draw_finsh();
#endif
    return 0;
}

static int bigfont_process(FILE* file, uint16_t pinyin, int offset,uint8_t **out,uint32_t *len)
{
    bigfont_header font_info;
    int i;
    IndexItem item;
    offset = read_bigfont_info(file, offset, &font_info); // 获取新的偏移量

    fseek(file, offset, SEEK_SET); // 将文件指针移动到偏移量处

    for (i = 0; i < font_info.info.escape_num; i++) {
        unsigned short start_code, end_code;
        fread(&start_code, sizeof(start_code), 1, file); // 读取2字节的start_code
        fread(&end_code, sizeof(end_code), 1, file);     // 读取2字节的end_code
        // 处理读取的start_code和end_code数据
        offset += 4; // 更新偏移量
    }
    printf("offset:%d\n", offset);
    fseek(file, offset, SEEK_SET);
    for (int i = 0; i < font_info.info.index_sum;) {
        fread(&item, sizeof(item), 1, file);
        // 判断 IndexItem 是否全为0
        if (item.code == 0 && item.length == 0 && item.offset == 0) {
            continue; // 忽略该次读取
        }
        i++; // 当非全为0时才增加i的值

        // 在这里可以使用读取到的 IndexItem 做其他操作
        if (pinyin == item.code) {
            ESP_LOGI(TAG, "get font code:%d", i);
            break;
        }
        if (0 == item.code)//字体信息
        {
            ESP_LOGI(TAG, "get font info:%d", i);
        }
    }
    if (i >= font_info.info.index_sum) {
        ESP_LOGE(TAG, "Not find item .");
        return -1;
    }
    char* pfont = malloc(512);
    if (!pfont) {
        ESP_LOGE(TAG, "pfont malloc failed .");
        return -1;
    }
    memset(pfont, 0, 512);
    FontBase tbase;
    fseek(file, item.offset + 2, SEEK_SET);
    fread(pfont, item.length - 2, 1, file);
    ESP_LOGI(TAG, "item.offset:%d,item.length:%d", (int)item.offset, (int)item.length);
    for (i = 0; i < (item.length - 2); i++) {
        if (i % 16 == 0)printf("\n");
        printf("[%d],", pfont[i]);
    }
    printf("\n");
    // if (parse_font(pinyin, testshp, sizeof(testshp), &tbase))
    if (parse_font(pinyin, (char*)pfont, item.length - 2, &tbase,out,len)) {
        ESP_LOGE(TAG, "parse_font failed! .");
        return -1;
    }

    return 0;
}

void test_shx(uint8_t* in,uint8_t **out,uint32_t *len)
{
    uint16_t pinyin_test = 0;                     // 0xD2BB; //"一"
    const char* file_shx = "/sdcard/31.shx";//MOUNT_POINT "/31.shx"; // MOUNT_POINT "/test.shx";
    pinyin_test = *((uint16_t*)in);

    FILE* file = fopen(file_shx, "rb");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open %s .", file_shx);
        return; // 返回错误
    }
    int header_size = 256;
    char* header = (char*)malloc(header_size + 1);
    int index = read_header_from_shx(file, header, 0);
    int type = find_keyword_in_data(header);
    if (!type) {
        ESP_LOGE(TAG, "find_keyword_in_data fail!");
        return;
    }
    switch (type) {
    case SHX_TYPE_BIGFONT:
        /* code */
        ESP_LOGI(TAG, "SHX_TYPE_BIGFONT");
        bigfont_process(file, pinyin_test, index,out,len);
        break;
    case SHX_TYPE_SHAPES:
        /* code */
        ESP_LOGI(TAG, "SHX_TYPE_SHAPES");
        break;
    case SHX_TYPE_UNIFONT:
        /* code */
        ESP_LOGI(TAG, "SHX_TYPE_UNIFONT");
        break;
    default:
        ESP_LOGE(TAG, "Not support type fail!");
        break;
    }

    fclose(file);
}

void font_parse_test(void)
{
    char in[] = "木";
    uint8_t out[4];
    uint8_t* buff = NULL;
    uint32_t out_len = 0;
    printf("in[0]:%x\n", in[0]);
    printf("in[1]:%x\n", in[1]);
    printf("in[2]:%x\n", in[2]);
    utf8togbk((uint8_t*)in, out);
    // vTaskDelay(pdMS_TO_TICKS(200));
    test_shx(out, &buff,&out_len);
}