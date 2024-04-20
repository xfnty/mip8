#include <asm/asm.h>
#include <core/util.h>

#include "test.h"


int main(int argc, char const *argv[]) {
    {
        const char *text = "nop";
        buffer_t src = {0};
        arr_copy(src, text, strlen(text));
        buffer_t out = {0};
        err_t err = asm_compile(&src, &out);
        TEST_ASSERT_LOG(err.succeded, "%s", err.description);
        TEST_ASSERT_EQ(out.size, 1);
        TEST_ASSERT_EQ(out.data[0], 0x00);
    }

    {
        const char *text = "label: psp $01 jmp .label ; comment";
        buffer_t src = {0};
        arr_copy(src, text, strlen(text));
        buffer_t out = {0};
        err_t err = asm_compile(&src, &out);
        TEST_ASSERT_LOG(err.succeded, "%s", err.description);
        TEST_ASSERT_EQ(out.size, 4);
    }

    TEST_SUCCEED();
}
