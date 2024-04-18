#include <string.h>

#include "vm/vm.h"
#include "core/cpu.h"
#include "core/bus.h"
#include "core/util.h"


int main(int argc, const char** argv) {
    if (argc <= 1 || argc > 3) {
        LOG("Usage: mip8vm [-i] <rom>");
        return 0;
    }

    err_t err = err_success();
    struct vm_t vm = {0};
    vm_init(&vm);

    bool interactive = false;
    const char* rom_filepath = argv[1];

    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        interactive = true;
        rom_filepath = argv[2];
    }

    CHECK_ERR_LOG_RETURN_VALUE(cpu_load_program_memory_from_file(&vm.cpu, rom_filepath), 1);

    if (interactive) {
        LOG("Running in interactive mode. Press <enter> to execute next instruction or <q> to exit.");
        while (getchar() != 'q' && err.succeded && !vm.cpu.registers.F.H) {
            struct cpu_opcode_t op = g_cpu_opcode_table[vm.cpu.program_memory.data[vm.cpu.registers.IP]];

            printf("0x%X: %s", vm.cpu.registers.IP, op.mnemonic);
            for (int i = 1; i < op.length; i++)
                printf(" 0x%X", vm.cpu.program_memory.data[vm.cpu.registers.IP + 1]);
            printf("\n");

            err = cpu_exec_next(&vm.cpu, vm.devices);
            cpu_dump_state(&vm.cpu);
            LOG();
        }
    } else {
        err = cpu_exec_all(&vm.cpu, vm.devices);
    }

    if (!err.succeded)
        LOG_ERROR("execution failed: %s", err.description);

    return 0;
}
