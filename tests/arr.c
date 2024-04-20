#include <core/util.h>

#include "test.h"


int main(int argc, char const *argv[]) {
    arr_t(int) a = {0};

    arr_push(a, 123);
    TEST_ASSERT_EQ(a.size, 1);
    TEST_ASSERT_EQ(a.capacity, 1);
    TEST_ASSERT_EQ(a.data[0], 123);
    
    arr_push(a, 456);
    TEST_ASSERT_EQ(a.size, 2);
    TEST_ASSERT_EQ(a.data[1], 456);
    
    int x = arr_pop(a);
    TEST_ASSERT_EQ(x, 456);
    TEST_ASSERT_EQ(a.size, 1);
    TEST_ASSERT_EQ(a.data[0], 123);

    arr_free(a);
    TEST_ASSERT_EQ(a.size, 0);
    TEST_ASSERT_EQ(a.data, NULL);

    TEST_SUCCEED();
}
