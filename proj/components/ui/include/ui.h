/**
 * @file ui.h
 *
 */

#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

void ui_test(void);
void move_to(lv_point_t p);
int pen_draw_line(lv_point_t point);
float get_x_count(void);
float get_y_count(void);
void pen_draw_finsh(void);
void ui_init(void);
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_H*/
