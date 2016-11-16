#include <stdio.h>
#include <stdlib.h>
#include <letmecreate/letmecreate.h>
#include "common.h"


int load_image_from_file(char *filepath, char **buffer, uint32_t *buffer_length)
{
    FILE *file = NULL;

    if (filepath == NULL || buffer == NULL || buffer_length == NULL)
        return -1;

    file = fopen(filepath, "rb");
    if (file == NULL)
        return -1;

    fseek(file, 0, SEEK_END);
    *buffer_length = ftell(file);
    rewind(file);

    *buffer = malloc(*buffer_length);
    if (*buffer == NULL) {
        fclose(file);
        return -1;
    }

    if (fread(*buffer, 1, *buffer_length, file) != *buffer_length) {
        fclose(file);
        return -1;
    }
    fclose(file);

    return 0;
}

void load_creator_logo(void)
{
    char *buffer = NULL;
    uint32_t buffer_length = 0;

    if (load_image_from_file("data/creator_logo.jpg", &buffer, &buffer_length) < 0) {
        fprintf(stderr, "Failed to load creator logo.\n");
        return;
    }

    eve_click_load_image(0,
                         FT800_OPT_RGB565,
                         buffer,
                         buffer_length);

    free(buffer);
}
