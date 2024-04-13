#include "args.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>


static bool str_starts_with(const char* str, const char* prefix);
static bool str_eql(const char* str1, const char* str2);
static const char* str_skip_str(const char* str, const char* prefix);
static err_t str_to_uint(const char* str, unsigned int* uint);
static bool arg_matches_rule(const char* arg, const args_rule_t* rule);
static err_t arg_try_parse(const args_rule_t* rule, const char* value, void* config);
static bool is_rule_empty(const args_rule_t* rule);

err_t args_parse(const args_rule_t* scheme, void* config, int argc, const char** argv) {
    assert(config != NULL);
    assert(argc >= 1);

    const args_rule_t* rule = &scheme[0];
    while (!is_rule_empty(rule)) {
        bool was_set = false;

        if (rule->type == ARGS_FLAG && (rule->is_required || rule->default_value.flag == true))
            return err_format("invalid argument rule given");

        int i = 1;
        for (; i < argc; i++) {
            if (arg_matches_rule(argv[i], rule)) {
                const char* value = NULL;
                if (rule->type != ARGS_FLAG) {
                    if (i >= argc - 1)
                        return err_format("required field is missing");
                    value = argv[++i];
                }
                CHECK_ERR_PROPAGATE(arg_try_parse(rule, value, config));
                was_set = true;
            }
        }

        if (!was_set) {
            if (!rule->is_required) {
                switch (rule->type) {
                    case ARGS_FLAG: {
                        *((bool*)(config + rule->field_offset)) = rule->default_value.flag;
                    } break;
                    
                    case ARGS_UINT: {
                        *((unsigned int*)(config + rule->field_offset)) = rule->default_value.uint;
                    } break;

                    case ARGS_STR: {
                        assert(rule->default_value.str != NULL);
                        *((char**)(config + rule->field_offset)) = strdup(rule->default_value.str);
                    } break;

                    default: UNREACHABLE();
                }
            } else {
                return err_format("required field is missing");
            }
        }

        rule++;
    }

    return err_success();
}

void args_free(void* config, const args_rule_t* scheme) {
    assert(config != NULL);
    assert(scheme != NULL);

    const args_rule_t* rule = &scheme[0];
    while (!is_rule_empty(rule) && rule->type == ARGS_STR)
        free(*(void**)(config + rule->field_offset));
}

static void print_rule_format(const args_rule_t* rule) {
    if (!rule->is_required || rule->type == ARGS_FLAG)
        printf("[");

    if (rule->short_name != '\0')
        printf("-%c", rule->short_name);
    else
        printf("--%s", rule->name);

    if (rule->type == ARGS_STR)
        printf(" <str>");
    if (rule->type == ARGS_UINT)
        printf(" <uint>");

    if (!rule->is_required || rule->type == ARGS_FLAG)
        printf("]");
}

void args_print_help(const args_rule_t* scheme, int argc, const char** argv) {
    assert(scheme != NULL);
    assert(argc >= 1);
    assert(argv != NULL && argv[0] != NULL);
    
    const char* program_name = strrchr(argv[0], '/');
    program_name = (program_name == NULL) ? (strrchr(argv[0], '\\')) : (NULL);
    program_name = (program_name != NULL && program_name[0] != '\0')
        ? (program_name + 1)
        : (argv[0]);

    printf("Usage: %s", program_name);
    for (const args_rule_t* rule = &scheme[0]; !is_rule_empty(rule); rule++) {
        if (rule->is_required)
            continue;
        printf(" ");
        print_rule_format(rule);
    }
    for (const args_rule_t* rule = &scheme[0]; !is_rule_empty(rule); rule++) {
        if (!rule->is_required)
            continue;
        printf(" ");
        print_rule_format(rule);
    }
    printf("\n");

    if (is_rule_empty(&scheme[0]))
        return;

    int longest_rule_name = 0;
    for (const args_rule_t* rule = &scheme[0]; !is_rule_empty(rule); rule++) {
        int len = (rule->name != NULL) ? (2 + strlen(rule->name)) : (2);
        if (rule->name != NULL && rule->short_name != '\0')
            len += 2 + 2; // ", -x"
        longest_rule_name = fmaxf(longest_rule_name, len);
    }

    printf("Options:\n");
    for (const args_rule_t* rule = &scheme[0]; !is_rule_empty(rule); rule++) {
        printf("  ");

        int len = 0;

        if (rule->name != NULL && rule->short_name != '\0') {
            printf("--%s, -%c", rule->name, rule->short_name);
            len += 2 + strlen(rule->name) + 4;
        } else if (rule->name != NULL) {
            printf("--%s", rule->name);
            len += 2 + strlen(rule->name);
        } else {
            printf("-%c", rule->short_name);
            len += 2;
        }

        for (int i = len; i < longest_rule_name + 2; i++)
            printf(" ");

        if (rule->description != NULL)
            printf("%s ", rule->description);

        if (!rule->is_required && rule->type != ARGS_FLAG) {
            if (rule->type == ARGS_STR)
                printf("\"%s\"", rule->default_value.str);
            else if (rule->type == ARGS_UINT)
                printf("%d", (int)rule->default_value.uint);
            printf(" by default.");
        }

        printf("\n");
    }
}

bool str_starts_with(const char* str, const char* prefix) {
    return str_skip_str(str, prefix) != NULL;
}

bool str_eql(const char* str1, const char* str2) {
    assert(str1 != NULL);
    assert(str2 != NULL);
    while (*str1 || *str2) {
        if (*str1 != *str2)
            return false;
        str1++;
        str2++;
    }
    return true;
}

const char* str_skip_str(const char* str, const char* prefix) {
    assert(str != NULL);
    assert(prefix != NULL);
    while (*prefix) {
        if (*str != *prefix)
            return NULL;
        str++;
        prefix++;
    }
    return str;
}

bool arg_matches_rule(const char* arg, const args_rule_t* rule) {
    assert(arg != NULL);
    assert(rule != NULL);
    return (rule->name != NULL && str_starts_with(arg, "--") && str_eql(rule->name, str_skip_str(arg, "--")))
        || (rule->short_name != '\0' && str_starts_with(arg, "-") && rule->short_name == str_skip_str(arg, "-")[0]);
}

err_t arg_try_parse(const args_rule_t* rule, const char* value, void* config) {
    assert(rule != NULL);
    assert(config != NULL);

    switch (rule->type) {
        case ARGS_FLAG: {
            *((bool*)(config + rule->field_offset)) = true;
        } break;

        case ARGS_UINT: {
            assert(value != NULL);
            unsigned int uint = 0;
            CHECK_ERR_PROPAGATE(str_to_uint(value, &uint));
            *((unsigned int*)(config + rule->field_offset)) = uint;
        } break;

        case ARGS_STR: {
            assert(value != NULL);
            *((char**)(config + rule->field_offset)) = strdup(value);
        } break;

        default: UNREACHABLE();
    };

    return err_success();
}

err_t str_to_uint(const char* str, unsigned int* uint) {
    assert(str != NULL);
    assert(uint != NULL);
    *uint = 0;
    for (int i = strlen(str); *str; i--) {
        if (*str < '0' || *str > '9')
            return err_format("failed to parse unsigned integer argument");
        unsigned int n = (unsigned int)(*str - '0');
        for (int j = 1; j < i; j++)
            n *= 10;
        *uint += n;
        str++;
    }
    return err_success();
}

bool is_rule_empty(const args_rule_t* rule) {
    return rule->name == NULL && rule->short_name == '\0';
}