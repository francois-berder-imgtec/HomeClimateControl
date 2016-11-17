#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <letmecreate/letmecreate.h>
#include "common.h"
#include "gui.h"
#include "main_menu.h"
#include "state.h"
#include "temperature_logger.h"

static char* month_str[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

static char *weekday_str[] = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

static int bid = -1;

static void main_menu_gui_handler(int id)
{
    if (id == bid)
        state_machine_set_state(SCREENSAVER);
}

static void draw_time(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char str[255];

    if (snprintf(str, sizeof(str)-1, "%s %d %s %d:%02d", weekday_str[tm.tm_wday], tm.tm_mday, month_str[tm.tm_mon], tm.tm_hour, tm.tm_min) < 0)
        return;

    eve_click_draw(FT800_TEXT,
                   471,
                   0,
                   20,
                   FT800_OPT_RIGHTX,
                   str);
}

static void draw_logo(void)
{
    eve_click_draw(FT800_BITMAP_SOURCE, 0);
    eve_click_draw(FT800_BITMAP_LAYOUT, FT800_RGB565, CREATOR_IMAGE_WIDTH*2, CREATOR_IMAGE_HEIGHT);
    eve_click_draw(FT800_BITMAP_SIZE, FT800_NEAREST, FT800_BORDER, FT800_BORDER, CREATOR_IMAGE_WIDTH, CREATOR_IMAGE_HEIGHT);
    eve_click_draw(FT800_BEGIN, FT800_BITMAPS);
    eve_click_draw(FT800_VERTEX2II, 0, 0, 0, 0);
    eve_click_draw(FT800_END);
}

void main_menu_init(void)
{
    load_creator_logo();
    gui_init();
    bid = gui_add_button(220,235,50,30,"Sleep");
    gui_set_user_callback(main_menu_gui_handler);
}

void main_menu_refresh_screen(void)
{
    float temperature = 0.f;
    char str[255];

    if (temperature_logger_get_current_temperature(&temperature) < 0)
        snprintf(str, sizeof(str)-1, "Could not get temperature from sensor");
    else
        snprintf(str, sizeof(str)-1, "Current temperature: %5.2f degrees celsius", temperature);

    eve_click_clear(255, 255, 255);
    eve_click_draw(FT800_COLOR_RGB, 0, 0, 0);

    eve_click_draw(FT800_TEXT,
                   240,                 /* x */
                   100,                 /* y */
                   28,                  /* font */
                   FT800_OPT_CENTER,    /* options */
                   str);
    draw_time();
    eve_click_draw(FT800_COLOR_RGB, 255, 255, 255);

    draw_logo();

    gui_draw();
    eve_click_display();
}

void main_menu_release(void)
{
    if (bid >= 0)
        gui_remove_button(bid);

    gui_release();
}
