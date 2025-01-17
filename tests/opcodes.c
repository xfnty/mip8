#include <core/cpu.h>
#include <core/bus.h>
#include <core/util.h>

#include "test.h"


static u8* ram_buffer = NULL;

static err_t ram_read(struct bus_device_slist_node_t *dev, u8 addr, u8* out) {
    assert(ram_buffer != NULL);
    *out = ram_buffer[addr];
    return err_success();
}

static err_t ram_write(struct bus_device_slist_node_t *dev, u8 addr, u8 value) {
    assert(ram_buffer != NULL);
    ram_buffer[addr] = value;
    return err_success();
}

int main(int argc, char const *argv[]) {
    struct bus_device_slist_node_t ram_dev = {
        .name = "Test RAM",
        .addr_start = 0x00,
        .addr_end = 0xFF,
        .read = ram_read,
        .write = ram_write,
    };

    // FIXME: TEST_CASE(name)'s maybe?
    {  // OP_NOP
        u8 ram[0x100] = {0};
        ram_buffer = ram;
        struct cpu_t cpu = {0};
        arr_assign(cpu.program_memory, (u8*)((u8[]){ OP_NOP }), 1);
        err_t err = cpu_exec_next(&cpu, &ram_dev);
        TEST_ASSERT_EQ(err.succeded, true);
        TEST_ASSERT_EQ(cpu.registers.IP, 1);
    }

    {  // OP_HALT
        u8 ram[0x100] = {0};
        ram_buffer = ram;
        struct cpu_t cpu = {0};
        arr_assign(cpu.program_memory, (u8*)((u8[]){ OP_HALT }), 1);
        err_t err = cpu_exec_next(&cpu, &ram_dev);
        TEST_ASSERT_EQ(err.succeded, true);
        TEST_ASSERT_EQ(cpu.registers.IP, 0);
    }

    {  // OP_AND
        u8 ram[0x100] = {0};
        ram_buffer = ram;
        struct cpu_t cpu = {0};
        arr_assign(cpu.program_memory, (u8*)((u8[]){ OP_PUSHP, 0x3, OP_PUSHP, 0x6, OP_AND, OP_HALT }), 6);
        err_t err = cpu_exec_all(&cpu, &ram_dev);
        TEST_ASSERT_EQ(err.succeded, true);
        TEST_ASSERT_EQ(cpu.registers.DSS, 1);
        TEST_ASSERT_EQ(cpu.registers.DS[0], 0x2);
    }

    {  // OP_OR
        u8 ram[0x100] = {0};
        ram_buffer = ram;
        struct cpu_t cpu = {0};
        arr_assign(cpu.program_memory, (u8*)((u8[]){ OP_PUSHP, 0x1, OP_PUSHP, 0x2, OP_OR, OP_HALT }), 6);
        err_t err = cpu_exec_all(&cpu, &ram_dev);
        TEST_ASSERT_EQ(err.succeded, true);
        TEST_ASSERT_EQ(cpu.registers.DSS, 1);
        TEST_ASSERT_EQ(cpu.registers.DS[0], 0x3);
    }

    {  // OP_XOR
        u8 ram[0x100] = {0};
        ram_buffer = ram;
        struct cpu_t cpu = {0};
        arr_assign(cpu.program_memory, (u8*)((u8[]){ OP_PUSHP, 0x1, OP_PUSHP, 0x1, OP_XOR, OP_HALT }), 6);
        err_t err = cpu_exec_all(&cpu, &ram_dev);
        TEST_ASSERT_EQ(err.succeded, true);
        TEST_ASSERT_EQ(cpu.registers.DSS, 1);
        TEST_ASSERT_EQ(cpu.registers.DS[0], 0x00);
    }

    {  // OP_ADD
        u8 ram[0x100] = {0};
        ram_buffer = ram;
        struct cpu_t cpu = {0};
        arr_assign(cpu.program_memory, (u8*)((u8[]){ OP_PUSHP, 0x3, OP_PUSHP, 0x1, OP_ADD, OP_HALT }), 6);
        err_t err = cpu_exec_all(&cpu, &ram_dev);
        TEST_ASSERT_EQ(err.succeded, true);
        TEST_ASSERT_EQ(cpu.registers.DSS, 1);
        TEST_ASSERT_EQ(cpu.registers.DS[0], 0x4);
    }

    {  // OP_PUSH
        u8 ram[0x100] = { 0xAA };
        ram_buffer = ram;
        struct cpu_t cpu = {0};
        arr_assign(cpu.program_memory, (u8*)((u8[]){ OP_PUSH, OP_HALT }), 2);
        err_t err = cpu_exec_all(&cpu, &ram_dev);
        TEST_ASSERT_EQ(err.succeded, true);
        TEST_ASSERT_EQ(cpu.registers.DSS, 1);
        TEST_ASSERT_EQ(cpu.registers.DS[0], 0xAA);
    }

    // TODO: and like 20 more, yeah
    
    TEST_SUCCEED();
}
