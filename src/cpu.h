#ifndef CPU_H_
#define CPU_H_

#include "bus.h"
#include "util.h"


enum cpu_opcode_id_t {
    OP_NOP   = 0x00,
    OP_HALT  = 0x01,
    OP_AND   = 0x02,
    OP_OR    = 0x03,
    OP_XOR   = 0x04,
    OP_ADD   = 0x05,
    OP_PUSH  = 0x06,
    OP_PUSHI = 0x07,
    OP_PUSHP = 0x08,
    OP_PUSHA = 0x09,
    OP_POP   = 0x0A,
    OP_POPA  = 0x0B,
    OP_POPI  = 0x0C,
    OP_DUP   = 0x0D,
    OP_OVER  = 0x0E,
    OP_DROP  = 0x0F,
    OP_JUMP  = 0x10,
    OP_CMP   = 0x11,
    OP_JZR   = 0x12,
    OP_JOV   = 0x13,
    OP_CALL  = 0x14,
    OP_RET   = 0x15,
    OP_RETI  = 0x16,
    OP_SEI   = 0x17,
    OP_CLI   = 0x18,
};

struct cpu_t {
    struct {
        u8 A;
        u8 IP;
        u8 IRA;
        struct {
            u8 Z:1;
            u8 O:1;
            u8 H:1;
            u8 I:1;
        } F;
        struct {
            u8 DSS:6;  // 0x00:0x1F
            u8 RSS:4;  // 0x0:0x7
        };
        u8 DS[32];
        u8 RS[8];
    } registers;
    u8 *program_memory;
};

struct cpu_opcode_t {
    const char *mnemonic;
    const char *description;
    u8 length;
    err_t (*exec)(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
};

extern const struct cpu_opcode_t g_cpu_opcode_table[0x100];  // NULL for illegal opcode

err_t cpu_load_program_memory_from_file(struct cpu_t *cpu, const char *path);
err_t cpu_exec_all(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t cpu_exec_next(struct cpu_t *cpu, struct bus_device_slist_node_t *bus);
err_t cpu_interrupt(struct cpu_t *cpu, struct bus_device_slist_node_t *bus, u8 addr);
void cpu_dump_state(const struct cpu_t *cpu);

#endif