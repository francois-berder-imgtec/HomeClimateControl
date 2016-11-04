#include <linux/spi/spidev.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <letmecreate/letmecreate.h>
#include "gui.h"
#include "state.h"

/* Initialise SPI and I2C.
 * Initialise EVE click and thermo3 click.
 */
static int init_peripherals(void)
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

static void release_peripherals(void)
{
    thermo3_click_disable();
    i2c_release();
    eve_click_disable(MIKROBUS_1);
    spi_release();
}

int main(void)
{
    if (init_peripherals() < 0) {
        fprintf(stderr, "Error during initialisation.\nExiting.\n");
        release_peripherals();
        return -1;
    }

    eve_click_calibrate();

    gui_init();

    state_machine_init(MAIN_MENU);

    printf("Home Climate Control demo started");

    state_machine_run();

    state_machine_release();
    gui_release();
    release_peripherals();

    return 0;
}
