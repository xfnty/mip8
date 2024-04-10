#include <util.h>

#include "test.h"


int main(int argc, char const *argv[]) {
    u64 size = 0;
    u8* text = NULL;
    err_t err = load_file("assets/lorem.txt", &text, &size);
    TEST_ASSERT_LOG(err.succeded, "load_file(): %s", err.description);
    TEST_ASSERT_STREQ(text, "Lorem ipsum");
    TEST_SUCCEED();
}
