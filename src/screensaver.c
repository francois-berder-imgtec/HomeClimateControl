#include <stddef.h>
#include <letmecreate/letmecreate.h>
#include "screensaver.h"
#include "common.h"
#include "state.h"


static void screensaver_touch_event_callback(void)
{
    state_machine_set_state(MAIN_MENU);
}

void screensaver_init(void)
{
    eve_click_attach_touch_event_callback(screensaver_touch_event_callback);
    eve_click_set_backlight_intensity(25);
    eve_click_disable_buffering();

    eve_click_screensaver();
    eve_click_clear(255, 255, 255);
    eve_click_draw(FT800_BITMAP_SOURCE, 0);
    eve_click_draw(FT800_BITMAP_LAYOUT, FT800_RGB565, CREATOR_IMAGE_WIDTH*2, CREATOR_IMAGE_HEIGHT);
    eve_click_draw(FT800_BITMAP_SIZE, FT800_NEAREST, FT800_BORDER, FT800_BORDER, CREATOR_IMAGE_WIDTH, CREATOR_IMAGE_HEIGHT);
    eve_click_draw(FT800_BEGIN, FT800_BITMAPS);
    eve_click_draw(FT800_MACRO, 0);
    eve_click_display();
}

void screensaver_release(void)
{
    eve_click_stop();
    eve_click_enable_buffering();
    eve_click_set_backlight_intensity(128);
    eve_click_attach_touch_event_callback(NULL);
}
