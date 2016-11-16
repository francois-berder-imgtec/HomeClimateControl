#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include "state.h"
#include "main_menu.h"
#include "screensaver.h"


static bool running = true;
static struct sigaction action;

static unsigned int current_state, new_state;
static struct state states[STATE_CNT] = {
    [MAIN_MENU] = {
        .init = main_menu_init,
        .refresh_screen = main_menu_refresh_screen,
        .release = main_menu_release
    },
    [SCREENSAVER] = {
        .init = screensaver_init,
        .refresh_screen = NULL,
        .release = screensaver_release
    }
};


static void sleep_ms(unsigned int ms)
{
    struct timespec req, rem;

    req.tv_sec = ms / 1000;
    ms -= req.tv_sec * 1000;
    req.tv_nsec = ms * 1000000;

    while (nanosleep(&req, &rem))
        req = rem;
}

static void exit_state_machine(int __attribute__ ((unused))signo)
{
    running = false;
}

void state_machine_init(unsigned int state)
{
    /* Set signal handler to stop state machine when Ctrl+c is pressed */
    action.sa_handler = exit_state_machine;
    action.sa_flags = 0;

    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);

    new_state = state;
    current_state = state;
    running = true;

    if (states[current_state].init)
        states[current_state].init();
}

void state_machine_set_state(unsigned int state)
{
    new_state = state;
}

static void change_state(void)
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
}

void state_machine_run(void)
{
    while (running) {
        if (states[current_state].refresh_screen != NULL)
            states[current_state].refresh_screen();
        sleep_ms(16);

        if (new_state != current_state)
            change_state();
    }
}

void state_machine_release(void)
{
    if (states[current_state].release)
        states[current_state].release();

    /* Remove signal handler */
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, NULL, NULL);
}

void state_machine_exit(void)
{
    running = false;
}
