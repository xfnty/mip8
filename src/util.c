#include "util.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>


void slist_add(struct slist_node_t **head, struct slist_node_t *new_node) {
    assert(head != NULL);
    assert(*head != NULL);
    assert(new_node != NULL);
    new_node->next = *head;
    *head = new_node;
}

err_t load_file(const char *path, u8 **data, u64 *size) {
    assert(path != NULL);
    assert(data != NULL);
    assert(size != NULL);

    *size = 0;

    FILE* file = fopen(path, "rb");
    CHECK_RETURN_VALUE(file, err_format_errno("fopen()"));

    ASSERT(fseek(file, 0, SEEK_END) == 0);
    i64 actual_file_size = ftell(file);
    rewind(file);
    CHECK_RETURN_VALUE(actual_file_size >= 0, err_format_errno("could not get file size"));
    *size = actual_file_size + 1;  // account for '\0' at the end
    *data = malloc(*size);

    u64 bytes_read = fread(*data, 1, actual_file_size, file);
    CHECK_RETURN_VALUE(ferror(file) == 0, err_format_errno("fread()"));
    CHECK_RETURN_VALUE(bytes_read == actual_file_size, err_format_errno("could not read the file completely"));
    (*data)[bytes_read] = '\0';

    return err_success();
}

err_t err_format(const char *fmt, ...) {
    static char description_buffer[512] = {'\0'};

    va_list args;
    va_start(args, fmt);
    vsnprintf(description_buffer, sizeof(description_buffer), fmt, args);
    va_end(args);

    return (err_t){ .succeded = false, .description = description_buffer };
}

err_t err_format_errno(const char *prefix) {
    return err_format("%s: %s [%d]", prefix, strerror(errno), errno);
}
