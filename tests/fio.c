#include <core/util.h>

#include "test.h"


int main(int argc, char const *argv[]) {
    buffer_t buffer = {0};
    err_t err = read_file("assets/lorem.txt", &buffer);
    TEST_ASSERT_LOG(err.succeded, "read_file(): %s", err.description);
    arr_push(buffer, '\0');
    TEST_ASSERT_STREQ(buffer.data, "Lorem ipsum");
    TEST_SUCCEED();
}
