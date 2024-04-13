#ifndef ASM_H_
#define ASM_H_

#include "util.h"


err_t asm_compile(const char *text, u8 **out_buffer, u64 *out_buffer_size);
err_t asm_decompile(const u8 *binary, u64 binary_size, u8 **out_buffer, u64 *out_buffer_size);


#endif
