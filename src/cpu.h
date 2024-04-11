#ifndef CPU_H_
#define CPU_H_

#include "bus.h"
#include "util.h"


enum cpu_opcode_id_t {
    OP_NOP,   // 00
    OP_HALT,  // 01
    OP_AND,   // 02
    OP_OR,    // 03
    OP_XOR,   // 04
    OP_ADD,   // 05
    OP_PUSH,  // 06
    OP_PUSHI, // 07
    OP_PUSHP, // 08
    OP_PUSHA, // 09
    OP_POP,   // 0A
    OP_POPA,  // 0B
    OP_DUP,   // 0C
    OP_OVER,  // 0D
    OP_DROP,  // 0E
    OP_JUMP,  // 0F
    OP_CMP,   // 10
    OP_JZR,   // 11
    OP_JOV,   // 12
    OP_CALL,  // 13
    OP_RET,   // 14
};

struct cpu_t {
    struct {
        u8 A;
        u8 IP;
        struct {
            u8 Z:1;
            u8 O:1;
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

// FIXME: SOA?
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
void cpu_dump_state(const struct cpu_t *cpu);

#endif