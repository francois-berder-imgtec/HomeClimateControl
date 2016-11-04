#include <letmecreate/letmecreate.h>
#include "gui.h"
#include "main_menu.h"

static int bid = -1;

void main_menu_init(void)
{
    bid = gui_add_button(100,100,50,30,"Exit");
}

void main_menu_refresh_screen(void)
{
    eve_click_clear(0, 0, 0);
    eve_click_draw(FT800_TEXT,
                   240,                 /* x */
                   100,                 /* y */
                   28,                  /* font */
                   FT800_OPT_CENTER,    /* options */
                   "Current room temperature:");

    gui_draw();
    eve_click_display();
}

void main_menu_release(void)
{
    if (bid >= 0)
        gui_remove_button(bid);
}
