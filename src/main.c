#include <string.h>

#include "cpu.h"
#include "bus.h"
#include "util.h"


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
    struct bus_device_slist_node_t ram_dev = {
        .name = "Test RAM",
        .addr_start = 0x00,
        .addr_end = 0xFF,
        .read = ram_read,
        .write = ram_write,
    };
    struct cpu_t cpu = {0};
    err_t err = err_success();

    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        CHECK_ERR_LOG_RETURN_VALUE(cpu_load_program_memory_from_file(&cpu, argv[2]), 1);

        LOG("type <enter> to execute next instruction");
        while (getchar() != 'q' && err.succeded) {
            struct cpu_opcode_t op = g_cpu_opcode_table[cpu.program_memory[cpu.registers.IP]];
            
            printf("0x%X: %s", cpu.registers.IP, op.mnemonic);
            for (int i = 1; i < op.length; i++)
                printf(" 0x%X", cpu.program_memory[cpu.registers.IP + 1]);
            printf("\n");

            err = cpu_exec_next(&cpu, &ram_dev);
            cpu_dump_state(&cpu);
        }
    } else if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        CHECK_ERR_LOG_RETURN_VALUE(cpu_load_program_memory_from_file(&cpu, argv[2]), 1);
        u8 program_end = 0xFF;
        for (; cpu.program_memory[program_end] == OP_NOP && program_end > 1; program_end--) {
        }
        while (cpu.registers.IP < program_end) {
            struct cpu_opcode_t op = g_cpu_opcode_table[cpu.program_memory[cpu.registers.IP]];
            
            printf("0x%X: %s", cpu.registers.IP, op.mnemonic);
            for (int i = 1; i < op.length; i++)
                printf(" 0x%X", cpu.program_memory[cpu.registers.IP + 1]);
            printf("\n");

            cpu.registers.IP += op.length;
        }
    } else if (argc == 2) {
        CHECK_ERR_LOG_RETURN_VALUE(cpu_load_program_memory_from_file(&cpu, argv[1]), 1);
        err = cpu_exec_all(&cpu, &ram_dev);
    } else {
        LOG("Usage: mip8 [-i] [-p] <rom>");
    }

    CHECK_ERR_LOG_RETURN_VALUE(err, 1);
    return 0;
}
