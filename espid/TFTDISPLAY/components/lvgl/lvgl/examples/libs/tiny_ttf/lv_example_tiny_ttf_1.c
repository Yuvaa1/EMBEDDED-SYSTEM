#include "../C:\Users\YUVA\Desktop\espid\TFTDISPLAY\components\lvgl\lvgl\lvgl.h\lv_examples.h"
#if LV_USE_TINY_TTF && LV_BUILD_EXAMPLES

#include "ubuntu_font.h"

/**
 * Load a font with Tiny_TTF
 */
void lv_example_tiny_ttf_1(void)
{
    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_font_t * font = lv_tiny_ttf_create_data(ubuntu_font, sizeof(ubuntu_font), 30);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\nI'm a font\ncreated\nwith Tiny TTF");
    lv_obj_center(label);
}
#endif
