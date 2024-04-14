#include <string.h>

#include "core/cpu.h"
#include "core/bus.h"
#include "core/util.h"


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
    if (argc < 2) {
        LOG("Usage: mip8vm.exe <rom>");
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
    CHECK_ERR_LOG_RETURN_VALUE(cpu_load_program_memory_from_file(&cpu, argv[1]), 1);

    err_t err = err_success();

    // LOG("Running in interactive mode. Press <enter> to execute next instruction or <q> to exit.");
    // while (getchar() != 'q' && err.succeded) {
    //     struct cpu_opcode_t op = g_cpu_opcode_table[cpu.program_memory[cpu.registers.IP]];

    //     printf("0x%X: %s", cpu.registers.IP, op.mnemonic);
    //     for (int i = 1; i < op.length; i++)
    //         printf(" 0x%X", cpu.program_memory[cpu.registers.IP + 1]);
    //     printf("\n");

    //     err = cpu_exec_next(&cpu, &ram_dev);
    //     cpu_dump_state(&cpu);
    //     LOG();
    // }

    err = cpu_exec_all(&cpu, &ram_dev);

    if (!err.succeded)
        LOG_ERROR("execution failed: %s", err.description);

    return 0;
}
