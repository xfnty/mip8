#include "util.h"


void slist_add(struct slist_node_t **head, struct slist_node_t *new_node) {
    new_node->next = *head;
    *head = new_node;
}

u8* load_file(const char *path, u64 *size) {
    *size = 0;

    FILE* file = fopen(path, "rb");
    CHECK_RETURN_VALUE(file, NULL);

    ASSERT(fseek(file, 0, SEEK_END) == 0);
    i64 file_size = ftell(file);
    rewind(file);
    CHECK_RETURN_VALUE(file_size >= 0, NULL);

    *size = file_size + 1;
    u8* data = malloc(file_size + 1);
    data[*size - 1] = '\0';
    file_size = fread(data, 1, file_size, file);
    CHECK_RETURN_VALUE(*size - 1 == file_size, NULL);

    return data;
}
