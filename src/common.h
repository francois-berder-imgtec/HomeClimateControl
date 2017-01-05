#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define HOME_CLIMATE_CONTROLLER_ROOT_DIR    "/usr/bin/HomeClimateControl/"

#define CREATOR_IMAGE_WIDTH             (100)
#define CREATOR_IMAGE_HEIGHT            (34)

int load_image_from_file(char *filepath, char **buffer, uint32_t *buffer_length);
void load_creator_logo(void);

#endif
