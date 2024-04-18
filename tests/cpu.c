#include <core/cpu.h>
#include <core/bus.h>
#include <core/util.h>

#include "test.h"


static u8* ram_buffer = NULL;

err_t ram_read(struct bus_device_slist_node_t *dev, u8 addr, u8* out) {
    assert(ram_buffer != NULL);
    *out = ram_buffer[addr];
    return err_success();
}

err_t ram_write(struct bus_device_slist_node_t *dev, u8 addr, u8 value) {
    assert(ram_buffer != NULL);
    ram_buffer[addr] = value;
    return err_success();
}

int main(int argc, char const *argv[]) {
    TEST_IGNORE();
    
    struct bus_device_slist_node_t ram_dev = {
        .name = "Test RAM",
        .addr_start = 0x00,
        .addr_end = 0xFF,
        .read = ram_read,
        .write = ram_write,
    };

    u8 ram[0x100] = {0};
    ram_buffer = ram;
    struct cpu_t cpu = {0};
    TEST_ASSERT_EQ(cpu_load_program_memory_from_file(&cpu, "assets\\two-plus-two.bin").succeded, true);
    err_t err = cpu_exec_all(&cpu, &ram_dev);
    TEST_ASSERT_EQ(cpu.registers.DSS, 1);
    TEST_ASSERT_EQ(cpu.registers.DS[0], 4);
    TEST_ASSERT_EQ(err.succeded, false);
    TEST_ASSERT_STREQ(err.description, "halted");

    TEST_SUCCEED();
}
