#include <string.h>

#include "cpu.h"
#include "bus.h"
#include "util.h"
#include "args.h"


struct config_t {
    const char *input_file;
    bool compile;
    bool print_disassembly;
    bool interactive;
};

static const args_rule_t s_config_scheme[] = {
    {
        .short_name = 'i',
        .description = "Input binary file to execute or assembly source file to compile.",
        .type = ARGS_STR,
        .is_required = true,
        .field_offset = offsetof(struct config_t, input_file)
    },
    {
        .short_name = 'c',
        .description = "Compile into binary program ROM file.",
        .type = ARGS_FLAG,
        .field_offset = offsetof(struct config_t, compile)
    },
    {
        .short_name = 'p',
        .description = "Print assembly instructions.",
        .type = ARGS_FLAG,
        .field_offset = offsetof(struct config_t, print_disassembly)
    },
    {
        .short_name = 'e',
        .description = "Execute instructions one by one.",
        .type = ARGS_FLAG,
        .field_offset = offsetof(struct config_t, interactive)
    },
    {0}
};

static u8 ram_buffer[0x100];

err_t ram_read(struct bus_device_slist_node_t *dev, u8 addr, u8* out) {
    assert(ram_buffer != NULL);
    *out = ram_buffer[addr];
    LOG("RAM[0x%X] >> 0x%X", addr, *out);
    return err_success();
}

err_t ram_write(struct bus_device_slist_node_t *dev, u8 addr, u8 value) {
    assert(ram_buffer != NULL);
    ram_buffer[addr] = value;
    LOG("RAM[0x%X] << 0x%X (%d)", addr, value, value);
    return err_success();
}

int main(int argc, const char** argv) {
    if (argc <= 1) {
        args_print_help(s_config_scheme, argc, argv);
        return 0;
    }

    struct config_t config = {0};
    CHECK_ERR_LOG_RETURN_VALUE(args_parse(s_config_scheme, &config, argc, argv), 1);

    if (config.compile) {
        // run assembler
        LOG("compiling");
        return 0;
    } else if (config.print_disassembly) {
        // tell assembler to print source code
        LOG("*assembly*");
        return 0;
    }


    struct bus_device_slist_node_t ram_dev = {
        .name = "Test RAM",
        .addr_start = 0x00,
        .addr_end = 0xFF,
        .read = ram_read,
        .write = ram_write,
    };
    struct cpu_t cpu = {0};
    CHECK_ERR_LOG_RETURN_VALUE(cpu_load_program_memory_from_file(&cpu, config.input_file), 1);

    err_t err = err_success();
    if (config.interactive) {
        LOG("Running in interactive mode. Press <enter> to execute next instruction or <q> to exit.");
        while (getchar() != 'q' && err.succeded) {
            struct cpu_opcode_t op = g_cpu_opcode_table[cpu.program_memory[cpu.registers.IP]];

            printf("0x%X: %s", cpu.registers.IP, op.mnemonic);
            for (int i = 1; i < op.length; i++)
                printf(" 0x%X", cpu.program_memory[cpu.registers.IP + 1]);
            printf("\n");

            err = cpu_exec_next(&cpu, &ram_dev);
            cpu_dump_state(&cpu);
            LOG();
        }
    } else {
        err = cpu_exec_all(&cpu, &ram_dev);
    }

    if (!err.succeded && cpu.program_memory[cpu.registers.IP] != OP_HALT)
            LOG_ERROR("execution failed: %s", err.description);

    return 0;
}
