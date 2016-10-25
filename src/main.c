#include <linux/spi/spidev.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <letmecreate/letmecreate.h>

static volatile bool demo_running = true;

/* Initialise SPI and I2C.
 * Initialise EVE click and thermo3 click.
 */
static int init_demo()
{
    if (spi_init() < 0
    ||  spi_set_mode(MIKROBUS_1, SPI_MODE_0) < 0
    ||  eve_click_enable(MIKROBUS_1) < 0)
        return -1;

    if (i2c_init() < 0)
        return -1;

    i2c_select_bus(MIKROBUS_2);
    return thermo3_click_enable(0);
}

static void release_demo()
{
    thermo3_click_disable();
    i2c_release();
    eve_click_disable(MIKROBUS_1);
    spi_release();
}

static void exit_demo(int __attribute__ ((unused))signo)
{
    demo_running = false;
}

static void refresh_screen(float temperature)
{
    char str[256];
    snprintf(str, sizeof(str), "%4.1f degrees celsius", temperature);

    eve_click_clear(0, 0, 0);
    eve_click_draw(FT800_TEXT,
                   240,                 /* x */
                   100,                 /* y */
                   28,                  /* font */
                   FT800_OPT_CENTER,    /* options */
                   "Current room temperature:");
    eve_click_draw(FT800_TEXT,
                   240,                 /* x */
                   136,                 /* y */
                   28,                  /* font */
                   FT800_OPT_CENTER,    /* options */
                   str);
    eve_click_display();
}

int main(void)
{
    /* Set signal handler to exit program when Ctrl+c is pressed */
    struct sigaction action = {
        .sa_handler = exit_demo,
        .sa_flags = 0
    };
    sigemptyset(&action.sa_mask);
    sigaction (SIGINT, &action, NULL);


    if (init_demo() < 0) {
        fprintf(stderr, "Error during initialisation.\nExiting.\n");
        release_demo();
        return -1;
    }

    printf("Home Climate Control demo started");

    while (demo_running) {
        float temperature = 0.f;
        if (thermo3_click_get_temperature(&temperature) < 0) {
            fprintf(stderr, "Failed to get temperature from thermo3.\n");
            continue;
        }
        refresh_screen(temperature);
        sleep(1);
    }

    release_demo();

    return 0;
}
