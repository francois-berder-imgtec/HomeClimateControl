#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <letmecreate/letmecreate.h>
#include "gui.h"

struct button {
    int id;
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    char *str;
    bool pressed;
    struct button *next;
};
static struct button *buttons = NULL;
static pthread_mutex_t mutex;
static int current_id = 0;
static void(*user_callback)(int) = NULL;

static void gui_callback(uint16_t x, uint16_t y)
{
    struct button *cur = NULL;

    printf("%u %u\n", x, y);

    pthread_mutex_lock(&mutex);
    cur = buttons;
    while (cur) {
        if (!(x < cur->x || x > cur->x+cur->w || y < cur->y || y > cur->y + cur->h)) {
            cur->pressed = true;
            if (user_callback != NULL)
                user_callback(cur->id);
        } else {
            cur->pressed = false;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&mutex);
}

void gui_init(void)
{
    current_id = 0;
    pthread_mutex_init(&mutex, NULL);
    eve_click_attach_touch_callback(gui_callback);
}

int gui_add_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, char *str)
{
    struct button *button = malloc(sizeof(struct button));
    if (button == NULL)
        return -1;

    if (str == NULL)
        return -1;

    button->id = current_id++;
    button->x = x;
    button->y = y;
    button->w = w;
    button->h = h;
    button->str = str;
    button->pressed = false;
    button->next = NULL;

    struct button *last = buttons;
    if (last == NULL) {
        pthread_mutex_lock(&mutex);
        buttons = button;
        pthread_mutex_unlock(&mutex);
    }
    else {
        while (last->next)
            last = last->next;

        pthread_mutex_lock(&mutex);
        last->next = button;
        pthread_mutex_unlock(&mutex);
    }

    return button->id;
}

int gui_remove_button(int id)
{
    struct button *prev = NULL;
    struct button *cur = buttons;
    while (cur) {
        if (cur->id == id) {
            pthread_mutex_lock(&mutex);
            if (prev)
                prev->next = cur->next;
            else
                buttons = NULL;
            free(cur);
            pthread_mutex_unlock(&mutex);
            return id;
        }

        prev = cur;
        cur = cur->next;
    }

    return -1;
}

void gui_draw(void)
{
    struct button *cur = buttons;
    while (cur) {
        eve_click_draw(FT800_BUTTON,
                       cur->x,
                       cur->y,
                       cur->w,
                       cur->h,
                       26,
                       cur->pressed ? FT800_OPT_FLAT : 0,
                       cur->str);
        cur = cur->next;
    }
}

void gui_release(void)
{
    struct button *cur = NULL;
    eve_click_attach_touch_callback(NULL);
    pthread_mutex_lock(&mutex);
    cur = buttons;
    while (cur) {
        struct button *tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    buttons = NULL;
    pthread_mutex_unlock(&mutex);

    pthread_mutex_destroy(&mutex);
}

void gui_set_user_callback(void(*uc)(int))
{
    pthread_mutex_lock(&mutex);
    user_callback = uc;
    pthread_mutex_unlock(&mutex);
}
