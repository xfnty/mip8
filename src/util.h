#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

#define LENGTH(array) (sizeof(array)/sizeof(array[0]))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define MIN(a, b) ((a < b) ? (a) : (b))

#define LOG(fmt, ...)           printf("\e[0m" fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_NOTICE(fmt, ...)    printf("\e[1;97m" fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_SUCCESS(fmt, ...)   printf("\e[1;92mok\e[1;97m: " fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     printf("\e[1;91merror\e[1;97m: " fmt "\e[0m (At %s:%d:%s())\e[0m\n", ##__VA_ARGS__, __FILE__, __LINE__, __func__)

#define ASSERT(cond)                                             do { if (!(cond)) { LOG_ERROR("assertion \"%s\" failed", #cond); abort(); } } while(0)
#define __ASSERT_LOG_BOILERPLATE(expr, log_macro, fmt, end, ...) do { if (!(expr)) { log_macro(fmt, ##__VA_ARGS__); end; } } while(0)
#define ASSERT_LOG(cond, fmt, ...)                               __ASSERT_LOG_BOILERPLATE(cond, LOG_ERROR, fmt, abort(), ##__VA_ARGS__)
#define ASSERT_LOG_RETURN(cond, fmt, ...)                        __ASSERT_LOG_BOILERPLATE(cond, LOG_ERROR, fmt, return, ##__VA_ARGS__)
#define ASSERT_LOG_RETURN_VALUE(cond, value, fmt, ...)           __ASSERT_LOG_BOILERPLATE(cond, LOG_ERROR, fmt, return value, ##__VA_ARGS__)
#define CHECK_LOG(cond, fmt, ...)                                __ASSERT_LOG_BOILERPLATE(cond, LOG, fmt, ;, ##__VA_ARGS__)
#define CHECK_LOG_RETURN(cond, fmt, ...)                         __ASSERT_LOG_BOILERPLATE(cond, LOG, fmt, return, ##__VA_ARGS__)
#define CHECK_LOG_RETURN_VALUE(cond, value, fmt, ...)            __ASSERT_LOG_BOILERPLATE(cond, LOG, fmt, return value, ##__VA_ARGS__)
#define CHECK_RETURN(cond)                                       do { if (!(cond)) return; } while(0)
#define CHECK_RETURN_VALUE(cond, value)                          do { if (!(cond)) return (value); } while(0)
#define CHECK_PROPAGATE(cond)                                    do { bool _res = (cond); if (!_res) return _res; } while(0)
#define CHECK_GOTO(cond, label)                                  do { if (!(cond)) goto label; } while(0)
#define CHECK_SET_RESULT_GOTO(cond, var, value, label)           do { if (!(cond)) { (var) = (value); goto label; } } while(0)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

struct slist_node_t {
    struct slist_node_t* next;
};

#define slist_foreach(head, it_type, it_name) for (it_type* it_name = (head); it_name; it_name = (it_type*)(((struct slist_node_t*)it_name)->next))
extern void slist_add(struct slist_node_t **head, struct slist_node_t *new_node);

extern u8* load_file(const char *path, u64 *size);


#endif