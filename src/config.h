#ifndef CONFIG_H_
#define CONFIG_H_

#include "core/util.h"


struct config_t {
    const char *source_filepath;
    const char *output_filepath;
};

err_t config_parse_args(struct config_t *config, int argc, const char **argv);
void config_print_help();

#endif