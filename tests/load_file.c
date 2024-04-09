#include <util.h>

#include "test.h"


int main(int argc, char const *argv[]) {
    u64 size = 0;
    u8* f = load_file("assets/lorem.txt", &size);
    ASSERT_NEQ(f, NULL);
    ASSERT_STREQ(f, "Lorem ipsum");
    SUCCEED();
}
