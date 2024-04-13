#include "cpu.h"


err_t AND(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t OR(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t XOR(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t ADD(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t PUSH(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t PUSHI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t PUSHP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t PUSHA(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t POP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t POPA(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t POPI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t DUP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t OVER(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t DROP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t JUMP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t CMP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t JZR(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t JOV(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t CALL(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t RET(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t RETI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t SEI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t CLI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t NOP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t HALT(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);

extern const struct cpu_opcode_t g_cpu_opcode_table[0x100] = {
    [OP_NOP]   = { "NOP",   "Do nothing",                                            1, NOP },
    [OP_HALT]  = { "HALT",  "Stop the execution",                                    1, HALT },
    [OP_AND]   = { "AND",   "Logical AND",                                           1, AND },
    [OP_OR]    = { "OR",    "Logical OR",                                            1, OR },
    [OP_XOR]   = { "XOR",   "Logical XOR",                                           1, XOR },
    [OP_ADD]   = { "ADD",   "Add two values",                                        1, ADD },
    [OP_PUSH]  = { "PUSH",  "Push value in memory at A",                             1, PUSH },
    [OP_PUSHI] = { "PUSHI", "Push value in memory at A and increase A",              1, PUSHI },
    [OP_PUSHP] = { "PUSHP", "Push next byte in program memory",                      2, PUSHP },
    [OP_PUSHA] = { "PUSHA", "Push A to the stack",                                   1, PUSHA },
    [OP_POP]   = { "POP",   "Pop value from the stack to memory at A",               1, POP },
    [OP_POPA]  = { "POPA",  "Pop value into A",                                      1, POPA },
    [OP_POPI]  = { "POPI",  "Pop value from the stack to memory at A, add 1 to A",   1, POPI },
    [OP_DUP]   = { "DUP",   "Duplicate value on top of the stack",                   1, DUP },
    [OP_OVER]  = { "OVER",  "Duplicate second topmost value on the stack",           1, OVER },
    [OP_DROP]  = { "DROP",  "Drop value on top of the stack",                        1, DROP },
    [OP_JUMP]  = { "JUMP",  "Copy next byte in program memory into IP",              2, JUMP },
    [OP_CMP]   = { "CMP",   "Compare two values on the stack without removing them", 1, CMP },
    [OP_JZR]   = { "JZR",   "Jump if Zero flag is set",                              2, JZR },
    [OP_JOV]   = { "JOV",   "Jump if Overflow flag is set",                          2, JOV },
    [OP_CALL]  = { "CALL",  "Push IP to RS then copy next byte into IP",             2, CALL },
    [OP_RET]   = { "RET",   "Pop value from RS into IP then add 2 to IP",            1, RET },
    [OP_RETI]  = { "RETI",  "Return from an interrupt",                              1, RETI },
    [OP_SEI]   = { "SEI",   "Set Interrupts Enabled flag",                           1, SEI },
    [OP_CLI]   = { "CLI",   "Clear Interrupts Enabled flag",                         1, CLI },
};

err_t cpu_load_program_memory_from_file(struct cpu_t *cpu, const char *path) {
    assert(cpu != NULL);
    assert(path != NULL);

    u64 file_size;
    CHECK_ERR_PROPAGATE(load_file(path, &cpu->program_memory, &file_size));
    CHECK_RETURN_VALUE(file_size > 1, err_format("given rom file is empty"));
    CHECK_RETURN_VALUE(file_size <= 0x100, err_format("given rom file is too large"));
    cpu->program_memory = realloc(cpu->program_memory, 0x100);
    memset(cpu->program_memory + file_size, 0, 0x100 - file_size);
    CHECK_RETURN_VALUE(cpu->program_memory != NULL, err_format_errno("realloc() failed"));

    return err_success();
}

err_t cpu_exec_all(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    err_t err;
    while ((err = cpu_exec_next(cpu, bus)).succeded && !cpu->registers.F.H);
    return err;
}

err_t cpu_exec_next(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(!cpu->registers.F.H, err_format("attempted to execute next instruction on halted CPU"));
    u8 opcode = cpu->program_memory[cpu->registers.IP];
    CHECK_RETURN_VALUE(g_cpu_opcode_table[opcode].exec != NULL, err_format("unknown opcode 0x%X", opcode));
    CHECK_ERR_PROPAGATE(g_cpu_opcode_table[opcode].exec(cpu, bus));
    return err_success();
}

err_t cpu_interrupt(struct cpu_t *cpu, struct bus_device_slist_node_t *bus, u8 addr) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(!cpu->registers.F.H, err_format("attempted to execute next instruction on halted CPU"));
    CHECK_RETURN_VALUE(cpu->registers.F.I, err_success());
    cpu->registers.IRA = cpu->registers.IP;
    cpu->registers.IP = addr;
    return err_success();
}

void cpu_dump_state(const struct cpu_t *cpu) {
    LOG("----- CPU state -----");
    LOG("A=0x%X  IP=0x%X  Z=%d  O=%d  H=%d  I=%d", cpu->registers.A, cpu->registers.IP, cpu->registers.F.Z, cpu->registers.F.O, cpu->registers.F.H, cpu->registers.F.I);
    LOG("DSS=%X  RSS=%X", cpu->registers.DSS, cpu->registers.RSS);
    printf("DS=[");
    for (int i = 0; i < cpu->registers.DSS; i++)
        printf("0x%X%s", cpu->registers.DS[i], (cpu->registers.DSS > 0 && i < cpu->registers.DSS - 1) ? (", ") : "");
    printf("]\n");
    printf("RS=[");
    for (int i = 0; i < cpu->registers.RSS; i++)
        printf("0x%X%s", cpu->registers.RS[i], (cpu->registers.RSS > 0 && i < cpu->registers.RSS - 1) ? (", ") : "");
    printf("]\n");
    LOG("---------------------");
}

static err_t cpu_push(struct cpu_t *cpu, u8 value) {
    assert(cpu != NULL);
    CHECK_RETURN_VALUE(cpu->registers.DSS < 0x20, err_format("data stack overflow"));
    cpu->registers.DS[cpu->registers.DSS++] = value;
    return err_success();
}

static err_t cpu_pop(struct cpu_t *cpu, u8 *out) {
    assert(cpu != NULL);
    assert(out != NULL);
    CHECK_RETURN_VALUE(cpu->registers.DSS > 0, err_format("attempted to pop value from zero-size stack"));
    *out = cpu->registers.DS[--cpu->registers.DSS];
    return err_success();
}

err_t AND(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    u8 a, b;
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &a));
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &b));
    cpu_push(cpu, a & b);
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_AND].length;
    return err_success();
}

err_t OR(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    u8 a, b;
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &a));
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &b));
    cpu_push(cpu, a | b);
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_OR].length;
    return err_success();
}

err_t XOR(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    u8 a, b;
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &a));
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &b));
    cpu_push(cpu, a ^ b);
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_XOR].length;
    return err_success();
}

err_t ADD(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    u8 a, b;
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &a));
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &b));
    cpu_push(cpu, a + b);
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = (u16)a + b > 0xFF;
    cpu->registers.IP += g_cpu_opcode_table[OP_ADD].length;
    return err_success();
}

err_t PUSH(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    u8 a;
    CHECK_ERR_PROPAGATE(bus_read(bus, cpu->registers.A, &a));
    CHECK_ERR_PROPAGATE(cpu_push(cpu, a));
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_PUSH].length;
    return err_success();
}

err_t PUSHI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    CHECK_ERR_PROPAGATE(PUSH(cpu, bus));
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.A++;
    cpu->registers.IP += g_cpu_opcode_table[OP_PUSHI].length;
    return err_success();
}

err_t PUSHP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_ERR_PROPAGATE(cpu_push(cpu, cpu->program_memory[cpu->registers.IP + 1]));
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_PUSHP].length;
    return err_success();
}

err_t PUSHA(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_ERR_PROPAGATE(cpu_push(cpu, cpu->registers.A));
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_PUSHA].length;
    return err_success();
}

err_t POP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    u8 a;
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &a));
    CHECK_ERR_PROPAGATE(bus_write(bus, cpu->registers.A, a));
    cpu->registers.IP += g_cpu_opcode_table[OP_POP].length;
    return err_success();
}

err_t POPA(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_ERR_PROPAGATE(cpu_pop(cpu, &cpu->registers.A));
    cpu->registers.IP += g_cpu_opcode_table[OP_POPA].length;
    return err_success();
}

err_t POPI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    CHECK_ERR_PROPAGATE(POPA(cpu, bus));
    cpu->registers.A++;
    return err_success();
}

err_t DUP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(cpu->registers.DSS > 0, err_format("no values on the stack to duplicate"));
    CHECK_ERR_PROPAGATE(cpu_push(cpu, cpu->registers.DS[cpu->registers.DSS-1]));
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_DUP].length;
    return err_success();
}

err_t OVER(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(cpu->registers.DSS > 1, err_format("not enough values on the stack to execute OVER"));
    CHECK_ERR_PROPAGATE(cpu_push(cpu, cpu->registers.DS[cpu->registers.DSS-2]));
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == 0;
    cpu->registers.F.O = 0;
    cpu->registers.IP += g_cpu_opcode_table[OP_OVER].length;
    return err_success();
}

err_t DROP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(cpu->registers.DSS > 1, err_format("no values on the stack to DROP"));
    cpu->registers.DSS--;
    cpu->registers.IP += g_cpu_opcode_table[OP_DROP].length;
    return err_success();
}

err_t JUMP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    u8 a = cpu->program_memory[cpu->registers.IP + 1];
    cpu->registers.IP = a;
    return err_success();
}

err_t CMP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(cpu->registers.DSS > 1, err_format("not enough values on the stack to compare"));
    cpu->registers.F.Z = cpu->registers.DS[cpu->registers.DSS - 1] == cpu->registers.DS[cpu->registers.DSS - 2];
    cpu->registers.IP += g_cpu_opcode_table[OP_CMP].length;
    return err_success();
}

err_t JZR(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    if (!cpu->registers.F.Z)
        return JUMP(cpu, bus);
    cpu->registers.IP += g_cpu_opcode_table[OP_JZR].length;
    return err_success();
}

err_t JOV(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    if (cpu->registers.F.O)
        return JUMP(cpu, bus);
    cpu->registers.IP += g_cpu_opcode_table[OP_JOV].length;
    return err_success();
}

err_t CALL(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(cpu->registers.RSS < 0x8, err_format("return stack overflow"));
    cpu->registers.RS[cpu->registers.RSS++] = cpu->registers.IP;
    cpu->registers.IP = cpu->program_memory[cpu->registers.IP + 1];
    return err_success();
}

err_t RET(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    CHECK_RETURN_VALUE(cpu->registers.RSS > 0, err_format("not enough values on return stack"));
    cpu->registers.IP = cpu->registers.RS[--cpu->registers.RSS];
    u8 opcode = cpu->program_memory[cpu->registers.IP];
    CHECK_RETURN_VALUE(g_cpu_opcode_table[opcode].exec != NULL, err_format("returned to unknown opcode 0x%X", opcode));
    cpu->registers.IP += g_cpu_opcode_table[opcode].length;
    return err_success();
}

err_t RETI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    cpu->registers.IP = cpu->registers.IRA;
    return err_success();
}

err_t SEI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    cpu->registers.F.I = 1;
    return err_success();
}

err_t CLI(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    cpu->registers.F.I = 0;
    return err_success();
}

err_t NOP(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    cpu->registers.IP += g_cpu_opcode_table[OP_NOP].length;
    return err_success();
}

err_t HALT(struct cpu_t *cpu, struct bus_device_slist_node_t *bus) {
    assert(cpu != NULL);
    assert(bus != NULL);
    cpu->registers.F.H = 1;
    return err_success();
}
