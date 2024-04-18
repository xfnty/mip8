#ifndef ASM_ASM_H_
#define ASM_ASM_H_

#include "core/util.h"

err_t asm_compile(const buffer_t *source_buffer, buffer_t *out_buffer);

#endif