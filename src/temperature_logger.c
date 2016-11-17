#include <letmecreate/letmecreate.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "temperature_logger.h"

static pthread_t thread;
static volatile bool running = false;

static void* run(void *arg)
{
    running = true;
    while (running) {
        /* TODO: Record temperature in XML file */
        sleep(5);
    }

    return NULL;
}

int temperature_logger_start(void)
{
    if (running)
        return 0;

    if (pthread_create(&thread, NULL, run, NULL) != 0)
        return -1;

    return 0;
}

int temperature_logger_get_current_temperature(float *temperature)
{
    return thermo3_click_get_temperature(temperature);
}

int temperature_logger_stop(void)
{
    if (!running)
        return 0;

    running = false;
    if (pthread_kill(thread, SIGINT) != 0
    ||  pthread_join(thread, NULL) != 0) {
        running = true;
        return -1;
    }

    return 0;
}
