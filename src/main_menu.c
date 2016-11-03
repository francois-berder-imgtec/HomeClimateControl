#include <letmecreate/letmecreate.h>
#include "main_menu.h"


void main_menu_init(void)
{

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
    eve_click_display();
}

void main_menu_event_handler(uint16_t x, uint16_t y)
{

}

void main_menu_release(void)
{

}
