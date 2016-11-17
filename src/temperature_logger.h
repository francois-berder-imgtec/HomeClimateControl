#ifndef TEMPERATURE_LOGGER_H
#define TEMPERATURE_LOGGER_H

int temperature_logger_start(void);
int temperature_logger_get_current_temperature(float *temperature);
int temperature_logger_stop(void);

#endif
