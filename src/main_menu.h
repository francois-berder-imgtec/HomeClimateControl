#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <stdint.h>

void main_menu_init(void);
void main_menu_refresh_screen(void);
void main_menu_event_handler(uint16_t x, uint16_t y);
void main_menu_release(void);

#endif
