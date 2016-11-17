#include <letmecreate/letmecreate.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "temperature_logger.h"

#define TEMPERATURE_RECORDS_FILE_PATH       "data/temperature.xml"
#define COUNTER_RESET_VALUE         (50)        /* record temperature every 5 seconds, so 50 * 100ms */


static pthread_t thread;
static volatile bool running = false;
static pthread_mutex_t thermo3_mutex;       /* Prevent main_menu and temperature_logger accessing sensor
                                             *  at the same time.
                                             */

static void sleep_100ms(void)
{
    struct timespec req, rem;
    req.tv_sec = 0;
    req.tv_nsec = 100 * 1000 * 1000;

    while (nanosleep(&req, &rem))
        req = rem;
}

static void* run(void *arg)
{
    unsigned int counter = 0;

    running = true;
    while (running) {
        /* Record temperature in XML file */
        if (counter == 0) {
            int ret = 0;
            float temperature = 0.f;
            char temperature_str[255], timestamp_str[255];
            xmlNodePtr root_node = NULL, record_node = NULL;
            xmlDocPtr doc = xmlReadFile(TEMPERATURE_RECORDS_FILE_PATH, NULL, 0);
            if (doc == NULL) {
                doc = xmlNewDoc(BAD_CAST "1.0");
                root_node = xmlNewNode(NULL, BAD_CAST "records");
                xmlDocSetRootElement(doc, root_node);
            } else
                root_node = xmlDocGetRootElement(doc);

            record_node = xmlNewChild(root_node, NULL, BAD_CAST "record", NULL);

            pthread_mutex_lock(&thermo3_mutex);
            ret = thermo3_click_get_temperature(&temperature);
            pthread_mutex_unlock(&thermo3_mutex);

            if (ret == 0) {
                sprintf(timestamp_str, "%u", (unsigned)time(NULL));
                sprintf(temperature_str, "%f", temperature);
                xmlNewChild(record_node, NULL, BAD_CAST "timestamp", BAD_CAST timestamp_str);
                xmlNewChild(record_node, NULL, BAD_CAST "temperature", BAD_CAST temperature_str);
                xmlSaveFormatFileEnc(TEMPERATURE_RECORDS_FILE_PATH, doc, "UTF-8", 1);
            }
            xmlFreeDoc(doc);

            counter = COUNTER_RESET_VALUE;
        }

        if (running)
            sleep_100ms();

        --counter;
    }

    return NULL;
}

int temperature_logger_start(void)
{
    if (running)
        return 0;

    if (pthread_mutex_init(&thermo3_mutex, NULL) != 0)
        return -1;

    if (pthread_create(&thread, NULL, run, NULL) != 0) {
        pthread_mutex_destroy(&thermo3_mutex);
        return -1;
    }

    return 0;
}

int temperature_logger_get_current_temperature(float *temperature)
{
    int ret = 0;

    if (temperature == NULL)
        return -1;

    pthread_mutex_lock(&thermo3_mutex);
    ret = thermo3_click_get_temperature(temperature);
    pthread_mutex_unlock(&thermo3_mutex);

    return ret;
}

int temperature_logger_stop(void)
{
    if (!running)
        return 0;

    running = false;
    if (pthread_join(thread, NULL) != 0) {
        running = true;
        return -1;
    }
    if (pthread_mutex_destroy(&thermo3_mutex) != 0)
        return -1;

    return 0;
}
