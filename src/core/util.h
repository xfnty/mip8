#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct err_t {
    bool succeded;
    const char *description;
    const char *source;
    int line;
    const char *func;
} err_t;

#define err_success() ((err_t){ .succeded = true })
#define err_format(fmt, ...) _err_format(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define err_format_errno(fmt, ...) _err_format_errno(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
err_t _err_format(const char *source, int line, const char *func, const char *fmt, ...);
err_t _err_format_errno(const char *source, int line, const char *func, const char *fmt, ...);

struct slist_node_t {
    struct slist_node_t* next;
};

#define slist_foreach(head, it_type, it_name) for (it_type *it_name = (head); it_name; it_name = (it_type*)(((struct slist_node_t*)it_name)->next))
void slist_add(struct slist_node_t **head, struct slist_node_t *new_node);

#define arr_t(T) struct { u64 size, capacity; T *data; }
#define arr_resize(a, s) do { (a).data = realloc((a).data, sizeof((a).data[0]) * (s)); memset((a).data + (a).size, 0, (s) - (a).size); (a).size = (a).capacity = (s); }while(0)
#define arr_assign(a, d, s) do { (a).data = (d); (a).size = (a).capacity = (s); }while(0)
#define arr_clear(a) do { (a).size = 0; } while(0)
#define arr_free(a) do { assert((a).data); free((a).data); (a).size = (a).capacity = 0; (a).data = NULL; } while(0)
#define arr_push(a, x) do {                                                     \
        if ((a).size == (a).capacity) {                                         \
            (a).capacity = ((a).capacity) ? ((a).capacity << 2) : (1);          \
            (a).data = realloc((a).data, sizeof((a).data[0]) * (a).capacity);   \
        }                                                                       \
        (a).data[(a).size++] = (x);                                             \
    } while(0)
#define arr_pop(a) ((a).data[--(a).size])
#define arr_top(a) ((a).data[(a).size - 1])

u32 hash(const u8 *buf, u64 len);

typedef arr_t(u8) buffer_t;
err_t read_file(const char *path, buffer_t *out_buffer);
err_t write_file(const char *path, const buffer_t *buffer);

#define LENGTH(array) (sizeof(array)/sizeof(array[0]))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define MIN(a, b) ((a < b) ? (a) : (b))

#define LOG(fmt, ...)           printf("\e[0m" fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_NOTICE(fmt, ...)    printf("\e[1;97m" fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_SUCCESS(fmt, ...)   printf("\e[1;92mok\e[1;97m: " fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     printf("\e[1;91merror\e[1;97m: " fmt "\e[0m\n\tAt %s:%d:%s()\e[0m\n", ##__VA_ARGS__, __FILE__, __LINE__, __func__)

#define UNREACHABLE()                                            do { LOG_ERROR("reached unreachable section"); abort(); } while(0)
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
#define CHECK_ERR_LOG_RETURN_VALUE(err, val)                     do { err_t e = (err); if (!e.succeded) { LOG("\e[1;91merror\e[1;97m: %s\e[0m\n\tHappened in %s:%d:%s()\n\tCaught in %s:%d:%s()", e.description, e.source, e.line, e.func, __FILE__, __LINE__, __func__); return (val);} } while(0)
#define CHECK_ERR_PROPAGATE(err)                                 do { err_t e = (err); if (!e.succeded) return e; } while(0)

#endif