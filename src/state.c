#include <stdio.h>
#include "state.h"
#include "main_menu.h"

static unsigned int current_state;

static struct state states[STATE_CNT] = {
    [MAIN_MENU] = {
        .init = main_menu_init,
        .refresh_screen = main_menu_refresh_screen,
        .release = main_menu_release,
    }
};

void state_machine_init(unsigned int state)
{
    current_state = state;

    if (states[current_state].init)
        states[current_state].init();
}

void change_state(unsigned int new_state)
{
    if (new_state == current_state)
        return;

    if (new_state >= STATE_CNT) {
        fprintf(stderr, "Tried to change to an unknown state.\n");
        return;
    }

    if (states[current_state].release)
        states[current_state].release();

    current_state = new_state;

    if (states[current_state].init)
        states[current_state].init();

    state_machine_update();
}

void state_machine_update(void)
{
    if (states[current_state].refresh_screen != NULL)
        states[current_state].refresh_screen();
}

void state_machine_release(void)
{
    if (states[current_state].release)
        states[current_state].release();
}
