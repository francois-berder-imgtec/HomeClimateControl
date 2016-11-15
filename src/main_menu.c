#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <letmecreate/letmecreate.h>
#include "gui.h"
#include "main_menu.h"
#include "state.h"

#define CREATOR_IMAGE_WIDTH             (100)
#define CREATOR_IMAGE_HEIGHT            (34)

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
        state_machine_exit();
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

static int load_image_from_file(char *filepath, char **buffer, uint32_t *buffer_length)
{
    FILE *file = fopen(filepath, "rb");
    if (file == NULL)
        return -1;

    fseek(file, 0, SEEK_END);
    *buffer_length = ftell(file);
    rewind(file);

    *buffer = malloc(*buffer_length);
    if (buffer == NULL) {
        fclose(file);
        return -1;
    }

    if (fread(*buffer, 1, *buffer_length, file) != *buffer_length) {
        fclose(file);
        return -1;
    }
    fclose(file);

    return 0;
}

static void load_creator_logo(void)
{
    char *buffer = NULL;
    uint32_t buffer_length = 0;

    if (load_image_from_file("data/creator_logo.jpg", &buffer, &buffer_length) < 0) {
        fprintf(stderr, "Failed to load creator logo.\n");
        return;
    }

    eve_click_load_image(0,
                         FT800_OPT_RGB565,
                         buffer,
                         buffer_length);

    free(buffer);
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

    bid = gui_add_button(220,235,50,30,"Exit");
    gui_set_user_callback(main_menu_gui_handler);
}

void main_menu_refresh_screen(void)
{
    float temperature = 0.f;
    char str[255];

    if (thermo3_click_get_temperature(&temperature) < 0)
        snprintf(str, sizeof(str)-1, "Could not get temperature from sensor");
    else
        snprintf(str, sizeof(str)-1, "Current temperature: %5.2f degrees celsius", temperature);

    eve_click_clear(0, 0, 0);
    eve_click_draw(FT800_TEXT,
                   240,                 /* x */
                   100,                 /* y */
                   28,                  /* font */
                   FT800_OPT_CENTER,    /* options */
                   str);

    draw_time();
    draw_logo();

    gui_draw();
    eve_click_display();
}

void main_menu_release(void)
{
    if (bid >= 0)
        gui_remove_button(bid);

    gui_set_user_callback(NULL);
}
