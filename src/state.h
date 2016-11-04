#ifndef STATE_H
#define STATE_H

#include <stdint.h>

struct state {
    void (*init)(void);
    void (*refresh_screen)(void);   /* Function called every 1/60th second */
    void (*event_handler)(uint16_t, uint16_t);
    void (*release)(void);
};
typedef struct state state;

enum STATE {
    MAIN_MENU,
    STATE_CNT
};

void state_machine_init(unsigned int state);
void change_state(unsigned int new_state);
void state_machine_run(void);
void state_machine_release(void);
void state_machine_exit(void);

#endif
