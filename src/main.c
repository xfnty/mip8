#include <string.h>

#include "config.h"
#include "vm/vm.h"
#include "asm/asm.h"
#include "core/cpu.h"
#include "core/bus.h"
#include "core/util.h"


static struct config_t s_config;

int main(int argc, const char** argv) {
    if (!config_parse_args(&s_config, argc, argv).succeded) {
        config_print_help();
        return 0;
    }

    buffer_t source = {0};
    CHECK_ERR_LOG_RETURN_VALUE(read_file(s_config.source_filepath, &source), 1);

    buffer_t machine_code = {0};
    CHECK_ERR_LOG_RETURN_VALUE(asm_compile(&source, &machine_code), 1);

    if (s_config.output_filepath) {
        CHECK_ERR_LOG_RETURN_VALUE(write_file(s_config.output_filepath, &machine_code), 1);
        return 0;
    }

    struct vm_t vm = {0};
    vm_init(&vm);

    arr_assign(vm.cpu.program_memory, machine_code.data, machine_code.size);
    CHECK_ERR_LOG_RETURN_VALUE(cpu_exec_all(&vm.cpu, vm.devices), 1);

    // if (interactive) {
    //     LOG("Running in interactive mode. Press <enter> to execute next instruction or <q> to exit.");
    //     while (getchar() != 'q' && err.succeded && !vm.cpu.registers.F.H) {
    //         struct cpu_opcode_t op = g_cpu_opcode_table[vm.cpu.program_memory.data[vm.cpu.registers.IP]];

    //         printf("0x%X: %s", vm.cpu.registers.IP, op.mnemonic);
    //         for (int i = 1; i < op.length; i++)
    //             printf(" 0x%X", vm.cpu.program_memory.data[vm.cpu.registers.IP + 1]);
    //         printf("\n");

    //         err = cpu_exec_next(&vm.cpu, vm.devices);
    //         cpu_dump_state(&vm.cpu);
    //         LOG();
    //     }
    // }
    return 0;
}
