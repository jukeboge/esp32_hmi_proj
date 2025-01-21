#include "lvgl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "unigbk.h"
#include "font_parse.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "lv_examples.h"
#if 1
#define CANVAS_WIDTH  470
#define CANVAS_HEIGHT  180


static void ta_event_cb(lv_event_t * e);

static lv_obj_t * kb;
static lv_style_t g_style_line;
static lv_coord_t g_last_x = -32768, g_last_y = -32768;
static float x_count = 0.00;//脉冲/点
static float y_count = 0.00;//脉冲/点
lv_layer_t layer;
lv_obj_t * canvas;
lv_draw_line_dsc_t dsc;
char input_string[32];
int g_new_input = 0;
int input_len = 0;
#define FONT_DRAW_EVENT BIT0
static EventGroupHandle_t draw_event_group = NULL;
static const char* TAG = "ui";

extern bool example_lvgl_lock(int timeout_ms);
extern void example_lvgl_unlock(void);
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
        xEventGroupSetBits(draw_event_group, FONT_DRAW_EVENT);
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}


void canvas_test(void)
{
    /*Create a buffer for the canvas*/
    LV_DRAW_BUF_DEFINE(draw_buf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_RGB565);

    /*Create a canvas and initialize its palette*/
    canvas = lv_canvas_create(lv_screen_active());
    lv_obj_move_background(canvas);
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER);
    lv_obj_center(canvas);
    lv_obj_align(canvas, LV_ALIGN_TOP_LEFT, 5, lv_pct(20)+10);

    // lv_canvas_init_layer(canvas, &layer);


    lv_draw_line_dsc_init(&dsc);
    dsc.color = lv_palette_main(LV_PALETTE_RED);
    dsc.width = 4;
    dsc.round_end = 1;
    dsc.round_start = 1;
    // dsc.p1.x = 15;
    // dsc.p1.y = 15;
    // dsc.p2.x = 35;
    // dsc.p2.y = 10;
    // lv_draw_line(&layer, &dsc);

    // lv_canvas_finish_layer(canvas, &layer);


}
#if 0
void test_shx(uint8_t *in)
{
    uint16_t pinyin_test = 50366;//52670;//50366;//0xD2BB; //"一"
    const char *file_shx = "D:\\test\\31.SHX";//"D:\\test\\chineset.shx";//MOUNT_POINT "/test.shx";
    lv_fs_file_t fa;
    lv_fs_res_t res;
    // pinyin_test = *((uint16_t *)in);
    LV_LOG_USER("pinyin_test:%x", pinyin_test);
    // FILE *file = fopen(file_shx, "rb");
    res = lv_fs_open(&fa, file_shx, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK)
    // if(file == NULL)
    {
        LV_LOG_USER("file open fail:%s!", file_shx);
        return;
    }


    LV_LOG_USER("file open ok! ");
#if 0
    int header_size = 256;
    char *header = (char *)malloc(header_size + 1);
    int index =  read_header_from_shx(&fa,header,0);
    int type = find_keyword_in_data(header);
    if(!type)
    {
        LV_LOG_USER("find_keyword_in_data fail!");
        return;
    }
    switch (type)
    {
    case SHX_TYPE_BIGFONT:
        /* code */
        LV_LOG_USER("SHX_TYPE_BIGFONT");
        break;
    case SHX_TYPE_SHAPES:
        /* code */
        LV_LOG_USER("SHX_TYPE_SHAPES");
        break;
    case SHX_TYPE_UNIFONT:
        /* code */
        LV_LOG_USER("SHX_TYPE_UNIFONT");
        break;
    default:
        LV_LOG_USER("Not support type fail!");
        break;
    }
#endif
    //fclose(file);
    lv_fs_close(&fa);
}
#endif
void ui_test(void)
{
    /*Create the one-line mode text area*/
    lv_obj_t * text_ta = lv_textarea_create(lv_screen_active());
    lv_textarea_set_one_line(text_ta, true);
    lv_obj_set_style_text_font(text_ta, &lv_font_simsun_16_cjk, 0);
    lv_textarea_set_password_mode(text_ta, false);
    lv_obj_set_width(text_ta, lv_pct(80));
    lv_obj_add_event_cb(text_ta, ta_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(text_ta, LV_ALIGN_TOP_LEFT, 5, 20);
    lv_obj_set_height(text_ta, lv_pct(20));

    /*Create a keyboard*/
    kb = lv_keyboard_create(lv_screen_active());
    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);

    lv_keyboard_set_popovers(kb,true);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_textarea_set_placeholder_text(text_ta, "Please input here...");


    lv_obj_t * pinyin_ime = lv_ime_pinyin_create(lv_screen_active());
    lv_obj_set_style_text_font(pinyin_ime, &lv_font_simsun_16_cjk, 0);
    lv_ime_pinyin_set_keyboard(pinyin_ime, kb);

    /*Get the cand_panel, and adjust its size and position*/
    lv_obj_t * cand_panel = lv_ime_pinyin_get_cand_panel(pinyin_ime);
    lv_obj_set_size(cand_panel, LV_PCT(100), LV_PCT(10));
    lv_obj_align_to(cand_panel, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);

    /*The keyboard will show Arabic characters if they are enabled */
#if LV_USE_ARABIC_PERSIAN_CHARS && LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    // lv_obj_set_style_text_font(kb, &lv_font_dejavu_16_persian_hebrew, 0);
    lv_obj_set_style_text_font(text_ta, &lv_font_dejavu_16_persian_hebrew, 0);
#endif

    lv_obj_t * btn1 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_TOP_RIGHT, -5, 20);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_width(btn1, lv_pct(15));
    lv_obj_set_height(btn1, lv_pct(20));

    canvas_test();
    // test_shx(NULL);
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED ) {
        /*Focus on the clicked text area*/
        if(kb != NULL)
        {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
            LV_LOG_USER("LV_EVENT_CLICKED");
        }

    }

    else if(code == LV_EVENT_READY) {
        LV_LOG_USER("Ready, current text: %s", lv_textarea_get_text(ta));
        // uint32_t inputlen = strlen(lv_textarea_get_text(ta));
        // char *in = lv_textarea_get_text(ta);
        // strncpy(input_string,in,(inputlen > 31)?(31):(inputlen));
        g_new_input = 1;

    }
    else if(code == LV_EVENT_DEFOCUSED) {
        uint32_t inputlen = strlen(lv_textarea_get_text(ta));
        char *in = lv_textarea_get_text(ta);
        strncpy(input_string,in,(inputlen > 31)?(31):(inputlen));
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

float get_x_count(void)
{
    return x_count;
}

float get_y_count(void)
{
    return y_count;
}

void move_to(lv_point_t p)
{
    g_last_x = p.x;
    g_last_y = p.y;
}

int pen_draw_line(lv_point_t point)
{
    lv_point_t points[2];
    
    /*Release or first use*/
    if ((g_last_x == -32768) || (g_last_y == -32768)) {
        g_last_x = point.x;
        g_last_y = point.y;
    }
    else {
        points[0].x = g_last_x;
        points[0].y = g_last_y;
        points[1].x = point.x;
        points[1].y = point.y;
        g_last_x = point.x;
        g_last_y = point.y;
        printf("draw:[%ld,%ld]->[%ld,%ld]\n", points[0].x, points[0].y, points[1].x, points[1].y);
        dsc.p1.x = points[0].x;
        dsc.p1.y = points[0].y;
        dsc.p2.x = points[1].x;
        dsc.p2.y = points[1].y;
        lv_canvas_init_layer(canvas, &layer);
        lv_draw_line(&layer, &dsc);
        lv_canvas_finish_layer(canvas, &layer);
        // lv_canvas_init_layer(canvas, &layer);
    }
    return 0;
}


void pen_draw_finsh(void)
{
    lv_canvas_finish_layer(canvas, &layer);
}


static void draw_task(void* arg)
{
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(draw_event_group,
            FONT_DRAW_EVENT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);
        if (bits & FONT_DRAW_EVENT) {
            ESP_LOGI(TAG, "font draw event");
            {
                uint8_t* buff = NULL;
                uint32_t out_len = 0;
                uint8_t out_gbk[8];
                memset(out_gbk, 0, sizeof(out_gbk));
                int i = 0;
                int length = strlen(input_string);
                utf8togbk((uint8_t*)input_string, out_gbk);
                if (example_lvgl_lock(-1)) {
                    lv_canvas_fill_bg(canvas, lv_color_hex(0xffffff), LV_OPA_COVER);
                    test_shx(out_gbk, &buff, &out_len);
                    // Release the mutex
                    example_lvgl_unlock();
                }
            }
        }
    }
}

void ui_init(void)
{
    #if 1
    draw_event_group = xEventGroupCreate();
    if (!draw_event_group) {
        ESP_LOGE(TAG, "Creating draw_event_group failed");
        return;
    }
    // writer_ui_setup(&writer_ui);
    ui_test();
    xTaskCreate(draw_task, TAG, 4096, NULL, 10, NULL);
    #endif
}

#endif
