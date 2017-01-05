#include <linux/spi/spidev.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <letmecreate/letmecreate.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "common.h"
#include "gui.h"
#include "state.h"
#include "temperature_logger.h"

#define TOUCH_SCREEN_CALIBRATION_FILE_PATH              "data/calibration.xml"


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

static int load_touch_screen_calibration_matrix(void)
{
    uint8_t i = 0;
    uint8_t matrix_elements[6] = { 0, 0, 0, 0, 0, 0 };
    uint32_t calibration_matrix[6];
    xmlNode *root_node = NULL, *cur_node = NULL;
    xmlDocPtr doc = xmlReadFile(HOME_CLIMATE_CONTROLLER_ROOT_DIR TOUCH_SCREEN_CALIBRATION_FILE_PATH, NULL, 0);

    if (doc == NULL)
        return -1;

    root_node = xmlDocGetRootElement(doc);
    if (xmlStrcmp(root_node->name, BAD_CAST "calibration_matrix") != 0) {
        xmlFreeDoc(doc);
        return -1;
    }

    for (cur_node = root_node->children; cur_node; cur_node = cur_node->next) {
        xmlChar *content = NULL;
        uint8_t index = 0;
        if (cur_node->type != XML_ELEMENT_NODE)
            continue;

        if (xmlStrcmp(cur_node->name, BAD_CAST "a") == 0
        ||  xmlStrcmp(cur_node->name, BAD_CAST "b") == 0
        ||  xmlStrcmp(cur_node->name, BAD_CAST "c") == 0
        ||  xmlStrcmp(cur_node->name, BAD_CAST "d") == 0
        ||  xmlStrcmp(cur_node->name, BAD_CAST "e") == 0
        ||  xmlStrcmp(cur_node->name, BAD_CAST "f") == 0) {
            index = cur_node->name[0] -'a';
            content = xmlNodeGetContent(cur_node);
        }

        if (content == NULL || index >= 6)
            continue;

        calibration_matrix[index] = strtoul((char*)content, NULL, 10);
        ++matrix_elements[index];
    }

    xmlFreeDoc(doc);

    for (i = 0; i < 6; ++i)
        if (matrix_elements[i] != 1)
            return -1;

    eve_click_set_calibration_matrix(calibration_matrix[0], calibration_matrix[1], calibration_matrix[2],
                                     calibration_matrix[3], calibration_matrix[4], calibration_matrix[5]);

    return 0;
}

static void perform_touch_screen_calibration(void)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;
    uint32_t a, b, c, d, e, f;
    char str[16];

    eve_click_calibrate();
    eve_click_get_calibration_matrix(&a, &b, &c, &d, &e, &f);

    /* Save to XML file */
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "calibration_matrix");
    xmlDocSetRootElement(doc, root_node);
    snprintf(str, 16, "%u", a);
    xmlNewChild(root_node, NULL, BAD_CAST "a", BAD_CAST str);
    snprintf(str, 16, "%u", b);
    xmlNewChild(root_node, NULL, BAD_CAST "b", BAD_CAST str);
    snprintf(str, 16, "%u", c);
    xmlNewChild(root_node, NULL, BAD_CAST "c", BAD_CAST str);
    snprintf(str, 16, "%u", d);
    xmlNewChild(root_node, NULL, BAD_CAST "d", BAD_CAST str);
    snprintf(str, 16, "%u", e);
    xmlNewChild(root_node, NULL, BAD_CAST "e", BAD_CAST str);
    snprintf(str, 16, "%u", f);
    xmlNewChild(root_node, NULL, BAD_CAST "f", BAD_CAST str);
    xmlSaveFormatFileEnc(HOME_CLIMATE_CONTROLLER_ROOT_DIR TOUCH_SCREEN_CALIBRATION_FILE_PATH, doc, "UTF-8", 1);
    printf("Saved calibration data to %s\n", HOME_CLIMATE_CONTROLLER_ROOT_DIR TOUCH_SCREEN_CALIBRATION_FILE_PATH);
    xmlFreeDoc(doc);
}

int main(void)
{
    if (init_peripherals() < 0) {
        fprintf(stderr, "Error during initialisation.\nExiting.\n");
        release_peripherals();
        return -1;
    }

    LIBXML_TEST_VERSION
    if (load_touch_screen_calibration_matrix() < 0) {
        printf("Failed to read calibration file.\n");
        printf("Performing touch screen calibration.\n");
        perform_touch_screen_calibration();
    }
    xmlCleanupParser();

    temperature_logger_start();
    state_machine_init(MAIN_MENU);

    printf("Home Climate Control demo started\n");

    state_machine_run();

    state_machine_release();
    temperature_logger_stop();
    release_peripherals();

    return 0;
}
