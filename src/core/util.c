#include "util.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>


void slist_add(struct slist_node_t **head, struct slist_node_t *new_node) {
    assert(head != NULL);
    assert(new_node != NULL);
    if (*head == NULL) {
        *head = new_node;
    } else {
        new_node->next = *head;
        *head = new_node;
    }
}

// FNV1A: https://create.stephan-brumme.com/fnv-hash/
u32 hash(const u8 *buf, u64 len) {
    u32 hash = 0x811C9DC5;
    while (len > 0)
        hash = (buf[--len] ^ hash) * 16777619;
    return hash;
}

err_t read_file(const char *path, buffer_t *out_buffer) {
    assert(path != NULL);
    assert(out_buffer != NULL);

    FILE* file = fopen(path, "rb");
    CHECK_RETURN_VALUE(file, err_format_errno("fopen()"));

    ASSERT(fseek(file, 0, SEEK_END) == 0);
    i64 actual_file_size = ftell(file);
    rewind(file);
    CHECK_RETURN_VALUE(actual_file_size >= 0, err_format_errno("could not get file size"));
    arr_resize(*out_buffer, actual_file_size);

    u64 bytes_read = fread(out_buffer->data, 1, out_buffer->size, file);
    CHECK_RETURN_VALUE(ferror(file) == 0, err_format_errno("fread()"));
    CHECK_RETURN_VALUE(bytes_read == actual_file_size, err_format_errno("could not read the file completely"));

    return err_success();
}

err_t write_file(const char *path, const buffer_t *buffer) {
    assert(path != NULL);
    assert(buffer != NULL);

    FILE* file = fopen(path, "wb");
    CHECK_RETURN_VALUE(file, err_format_errno("fopen()"));

    u64 bytes_written = fwrite(buffer->data, 1, buffer->size, file);
    CHECK_RETURN_VALUE(ferror(file) == 0, err_format_errno("fwrite()"));
    CHECK_RETURN_VALUE(bytes_written == buffer->size, err_format_errno("could not write the file completely"));

    return err_success();
}

bool streql(const char *s1, const char *s2) {
    assert(s1);
    assert(s2);
    return strcmp(s1, s2) == 0;
}

err_t _err_format(const char *source, int line, const char *func, const char *fmt, ...) {
    static char description_buffer[512] = {'\0'};

    va_list args;
    va_start(args, fmt);
    vsnprintf(description_buffer, sizeof(description_buffer), fmt, args);
    va_end(args);

    return (err_t){ .succeded = false, .description = description_buffer, .source = source, .line = line, .func = func };

}

err_t _err_format_errno(const char *source, int line, const char *func, const char *fmt, ...) {
    static char description_buffer[512] = {'\0'};

    va_list args;
    va_start(args, fmt);
    vsnprintf(description_buffer, sizeof(description_buffer), fmt, args);
    va_end(args);

    return _err_format(source, line, func, "%s: %s [%d]", description_buffer, strerror(errno), errno);
}

void _winvterm_enable_ansi_sequences() {
    static bool enabled = false;
    if (enabled)
        return;

    HANDLE handleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode( handleOut , &consoleMode);
    consoleMode |= 0x0004;
    consoleMode |= 0x0008;            
    SetConsoleMode( handleOut , consoleMode );
    enabled = true;
}