#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include "asm/asm.h"
#include "core/cpu.h"
#include "core/util.h"


int main(int argc, char const *argv[]) {
    if (argc < 3) {
        LOG("Usage: mip8asm <src> <out>");
        return 0;
    }

    buffer_t source_text = {0};
    CHECK_ERR_LOG_RETURN_VALUE(read_file(argv[1], &source_text), 1);
    arr_push(source_text, '\0');

    buffer_t out_buffer = {0};
    CHECK_ERR_LOG_RETURN_VALUE(asm_compile(&source_text, &out_buffer), 1);

    CHECK_ERR_LOG_RETURN_VALUE(write_file(argv[2], &out_buffer), 1);

    return 0;
}