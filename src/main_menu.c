#include <stddef.h>
#include <stdio.h>
#include <letmecreate/letmecreate.h>
#include "gui.h"
#include "main_menu.h"
#include "state.h"


static int bid = -1;

static void main_menu_gui_handler(int id)
{
    if (id == bid)
        state_machine_exit();
}

void main_menu_init(void)
{
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

    gui_draw();
    eve_click_display();
}

void main_menu_release(void)
{
    if (bid >= 0)
        gui_remove_button(bid);

    gui_set_user_callback(NULL);
}
