#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>



/* ----- Macros ----- */
#define LENGTH(array) (sizeof(array)/sizeof(array[0]))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define MIN(a, b) ((a < b) ? (a) : (b))

#define LOG(fmt, ...)           printf("\e[0m" fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_NOTICE(fmt, ...)    printf("\e[1;97m" fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_SUCCESS(fmt, ...)   printf("\e[1;92mok\e[1;97m: " fmt "\e[0m\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     printf("\e[1;91merror\e[1;97m: " fmt "\e[0m (At %s:%d:%s())\e[0m\n", ##__VA_ARGS__, __FILE__, __LINE__, __func__)

#define ASSERT(cond)                                             do { if (!(cond)) { LOG_ERROR("assertion \"%s\" failed", #cond); abort(); } } while(0)
#define __ASSERT_LOG_BOILERPLATE(expr, log_macro, fmt, end, ...) do { if (!(expr)) { log_macro(fmt, ##__VA_ARGS__); end; } } while(0)
#define ASSERT_LOG(cond, fmt, ...)                               __ASSERT_LOG_BOILERPLATE(cond, LOG_ERROR, fmt, abort(), ##__VA_ARGS__)
#define ASSERT_LOG_RETURN(cond, fmt, ...)                        __ASSERT_LOG_BOILERPLATE(cond, LOG_ERROR, fmt, return, ##__VA_ARGS__)
#define ASSERT_LOG_RETURN_VALUE(cond, value, fmt, ...)           __ASSERT_LOG_BOILERPLATE(cond, LOG_ERROR, fmt, return value, ##__VA_ARGS__)
#define CHECK_LOG(cond, fmt, ...)                                __ASSERT_LOG_BOILERPLATE(cond, LOG, fmt, ;, ##__VA_ARGS__)
#define CHECK_LOG_RETURN(cond, fmt, ...)                         __ASSERT_LOG_BOILERPLATE(cond, LOG, fmt, return, ##__VA_ARGS__)
#define CHECK_LOG_RETURN_VALUE(cond, value, fmt, ...)            __ASSERT_LOG_BOILERPLATE(cond, LOG, fmt, return value, ##__VA_ARGS__)
#define CHECK_RETURN(cond)                                       do { if (!(cond)) return; } while(0)
#define CHECK_RETURN_VALUE(cond, value)                          do { if (!(cond)) return (value); } while(0)
#define CHECK_PROPAGATE(cond)                                    do { bool _res = (cond); if (!_res) return _res; } while(0)
#define CHECK_GOTO(cond, label)                                  do { if (!(cond)) goto label; } while(0)
#define CHECK_SET_RESULT_GOTO(cond, var, value, label)           do { if (!(cond)) { (var) = (value); goto label; } } while(0)



/* ----- Types ----- */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

struct slist_node_t {
    struct slist_node_t* next;
};

struct bus_device_slist_node_t {
    struct slist_node_t node;
    const char* name;
    u8 addr_start;
    u8 addr_end;  // inclusive
    bool (*read)(u8 addr, u8* out);  // NULL is for "not supported", `addr` is relative to `addr_start`
    bool (*write)(u8 addr, u8 value);
};

typedef bool (*opcode_handler_f)(u8 opcode);



/* ----- Function Forward Declarations ----- */
static bool cpu_exec_next();
static void cpu_dump_state();
static bool load_rom_file(const char* filepath);
static struct bus_device_slist_node_t* bus_get_device_at_addr(u8 addr);
static bool bus_read(u8 addr, u8* out);
static bool bus_write(u8 addr, u8 value);

static bool ram_bus_device_read(u8 addr, u8* out);
static bool ram_bus_device_write(u8 addr, u8 value);

#define slist_foreach(head, it_type, it_name) for (it_type* it_name = (head); it_name; it_name = (it_type*)(((struct slist_node_t*)it_name)->next))
inline void slist_add(struct slist_node_t **head, struct slist_node_t *new_node) {
    new_node->next = *head;
    *head = new_node;
}

bool op_illegal(u8 opcode);
bool op_NOP(u8 opcode);
bool op_HALT(u8 opcode);



/* ----- Global Variables ----- */
static struct bus_device_slist_node_t* s_bus_device_list;
static opcode_handler_f                s_opcode_handlers[0x100];
static const char*                     s_opcode_mnemonics[0x100];
static u8                              s_ram[0x7F*0xFF];
static u8                              s_ram_page;
static struct {
    u8 A;
    u8 PC;
    struct {
        u8 DSP:5;
        u8 RSP:3;
    };
    u8 DS[32];
    u8 RS[8];
} s_cpu;
static struct {
    u8* data;
    u64 size;
} s_progmem;



/* ----- Function Definitions ----- */
int main(int argc, const char** argv) {
    CHECK_LOG_RETURN_VALUE(argc == 2, 0, "Usage: mip8 <rom>");

    CHECK_RETURN_VALUE(load_rom_file(argv[1]), 1);

    struct bus_device_slist_node_t ram_device = {
        .name = "RAM",
        .addr_start = 0x00,
        .addr_end   = 0x80,
        .read = ram_bus_device_read,
        .write = ram_bus_device_write
    };
    s_bus_device_list = &ram_device;

    while (cpu_exec_next() && getchar() != 'q');

    return 0;
}

bool cpu_exec_next() {
    CHECK_RETURN_VALUE(((u64)s_cpu.PC) + 1 < s_progmem.size, false);
    u8 opcode = s_progmem.data[s_cpu.PC++];

    if (!s_opcode_handlers[opcode])
        return op_illegal(opcode);

    LOG("[0x%X] %s (0x%X)", s_cpu.PC, s_opcode_mnemonics[opcode], opcode);
    return s_opcode_handlers[opcode](opcode);
}

void cpu_dump_state() {
    LOG("CPU State:\nPC=%lu A=0x%X DSP=%u RSP=%u", s_cpu.PC, s_cpu.A, s_cpu.DSP, s_cpu.RSP);

    printf("DS=[");
    for (int i = 0; i < s_cpu.DSP - 1; i++)
        printf("0x%X, ", s_cpu.DS[i]);
    if (s_cpu.DSP >= 1)
        printf("0x%X", s_cpu.DS[s_cpu.DSP - 1]);
    printf("]\n");

    printf("RS=[");
    for (int i = 0; i < s_cpu.RSP - 1; i++)
        printf("0x%X, ", s_cpu.RS[i]);
    if (s_cpu.RSP >= 1)
        printf("0x%X", s_cpu.RS[s_cpu.RSP - 1]);
    printf("]\n");

    LOG("RAMpage=%d", s_ram_page);

    LOG("Bus devices:");
    slist_foreach(s_bus_device_list, struct bus_device_slist_node_t, dev) {
        LOG("  %s (0x%X:0x%X)", dev->name, dev->addr_start, dev->addr_end);
    }
}

bool load_rom_file(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    ASSERT_LOG_RETURN_VALUE(file, false, "could not open \"%s\"", filepath);

    ASSERT(fseek(file, 0, SEEK_END) == 0);
    i64 file_size = ftell(file);
    rewind(file);
    ASSERT_LOG_RETURN_VALUE(file_size >= 0, false, "could not read \"%s\" (%d)", filepath, errno);
    ASSERT_LOG_RETURN_VALUE(file_size <= 0xFF, false, "ROM file is too big (%ld bytes)", file_size);

    s_progmem.size = MAX(file_size, 0xFF);
    s_progmem.data = calloc(1, s_progmem.size);
    file_size = fread(s_progmem.data, 1, file_size, file);
    ASSERT_LOG_RETURN_VALUE(file_size <= s_progmem.size, false, "failed to read \"%s\"", filepath);

    LOG_SUCCESS("loaded ROM file \"%s\" (%ld bytes)", filepath, file_size);
    return true;
}

static struct bus_device_slist_node_t* bus_get_device_at_addr(u8 addr) {
    slist_foreach(s_bus_device_list, struct bus_device_slist_node_t, dev)
        if (addr >= dev->addr_start && addr <= dev->addr_end)
            return dev;
    return NULL;
}

bool bus_read(u8 addr, u8* out) {
    assert(out != NULL);
    *out = 0;
    struct bus_device_slist_node_t* dev = bus_get_device_at_addr(addr);
    ASSERT_LOG_RETURN_VALUE(dev, false, "read on unmapped address 0x%X", addr);
    ASSERT_LOG_RETURN_VALUE(dev->read, false, "device does not support read operation");
    return dev->read(addr - dev->addr_start, out);
}

bool bus_write(u8 addr, u8 value) {
    struct bus_device_slist_node_t* dev = bus_get_device_at_addr(addr);
    ASSERT_LOG_RETURN_VALUE(dev, false, "write 0x%X on unmapped address 0x%X", value, addr);
    ASSERT_LOG_RETURN_VALUE(dev->write, false, "device does not support write operation");
    return dev->write(addr - dev->addr_start, value);
}

bool ram_bus_device_read(u8 addr, u8* out) {
    assert(out != NULL);
    assert(addr <= 0x80);
    *out = (addr == 0x80) ? (s_ram_page) : (s_ram[s_ram_page * 0xFF + addr]);
    return true;
}

bool ram_bus_device_write(u8 addr, u8 value) {
    assert(addr <= 0x80);
    u8* v = (addr == 0x80) ? (&s_ram_page) : (&s_ram[s_ram_page * 0xFF + addr]);
    *v = value;
    return true;
}

bool op_illegal(u8 opcode) {
    LOG_ERROR("attempted to execute illegal opcode 0x%X", opcode);
    cpu_dump_state();
    return false;
}

bool op_NOP(u8 opcode) {
    return true;
}

bool op_HALT(u8 opcode) {
    LOG("Halted");
    cpu_dump_state();
    return false;
}

static opcode_handler_f s_opcode_handlers[0x100] = {
    [0x00] = op_NOP,
    [0x01] = op_HALT,
};

static const char* s_opcode_mnemonics[0x100] = {
    [0x00] = "NOP",
    [0x01] = "HALT",
};
