#include "config.h"

#include <assert.h>


err_t config_parse_args(struct config_t *config, int argc, const char **argv) {
    assert(config);
    assert(argv);

    *config = (struct config_t){0};

    if (argc == 2) {
        config->source_filepath = argv[1];
        return err_success();
    } else if (argc == 3) {
        config->source_filepath = argv[1];
        config->output_filepath = argv[2];
        return err_success();
    }

    return err_format("invalid command-line arguments");
}

void config_print_help() {
    printf(
        "Usage: mip8 [-i] <src> [<out>]\n"
        "Options:\n"
        "  <src> - MIP8 assembly source file to compile or execute.\n"
        "  <out> - output binary file name to which the machine code will be saved.\n"
    );
}
