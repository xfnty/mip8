#ifndef ARGS_H_
#define ARGS_H_

#include <stddef.h>
#include <stdbool.h>

#include "util.h"


typedef enum {
    ARGS_FLAG,
    ARGS_UINT,
    ARGS_STR,
} arg_type_t;

typedef struct {
    const char* name;
    char short_name;
    const char* description;
    arg_type_t type;
    bool is_required;
    union {
        bool flag;
        unsigned int uint;
        const char* str;
    } default_value;
    int field_offset;
} args_rule_t;

err_t args_parse(const args_rule_t* scheme, void* config, int argc, const char** argv);
void args_free(void* config, const args_rule_t* scheme);
void args_print_help(const args_rule_t* scheme, int argc, const char** argv);


#endif