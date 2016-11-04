#ifndef GUI_H
#define GUI_H

void gui_init(void);
int gui_add_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, char *str);
int gui_remove_button(int id);
void gui_draw(void);
void gui_release(void);
void gui_set_user_callback(void(*user_callback)(int));

#endif
