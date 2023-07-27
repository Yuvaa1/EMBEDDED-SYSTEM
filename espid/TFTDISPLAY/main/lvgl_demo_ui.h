
#ifndef MAIN_LVGL_DEMO_UI_H_
#define MAIN_LVGL_DEMO_UI_H_


/* INCLUDES ------------------------------------------------------------------*/
#include <math.h>

#include "esp_log.h"
#include <lvgl.h>
/* MACROS --------------------------------------------------------------------*/
#ifndef PI
#define PI  (3.14159f)
#endif


typedef struct
{
    lv_obj_t *scr;
    int count_val;
} my_timer_context_t;
/* VARIABLES -----------------------------------------------------------------*/

/* FUNCTIONS DECLARATION -----------------------------------------------------*/
void lvgl_demo_ui(lv_obj_t *scr);


#endif /* MAIN_LVGL_DEMO_UI_H_ */

