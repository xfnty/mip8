#include "vm.h"


static u8 ram_buffer[0x100];

err_t ram_read(struct bus_device_slist_node_t *dev, u8 addr, u8* out) {
    *out = ram_buffer[addr];
    LOG("RAM[0x%X] >> 0x%X", addr, *out);
    return err_success();
}

err_t ram_write(struct bus_device_slist_node_t *dev, u8 addr, u8 value) {
    ram_buffer[addr] = value;
    LOG("RAM[0x%X] << 0x%X (%d)", addr, value, value);
    return err_success();
}

static struct bus_device_slist_node_t ram_device = {
    .name = "Test RAM",
    .addr_start = 0x00,
    .addr_end = 0xFF,
    .read = ram_read,
    .write = ram_write,
};

void vm_init(struct vm_t *vm) {
    assert(vm);
    *vm = (struct vm_t){0};
    vm->cpu = (struct cpu_t){0};
    vm->devices = &ram_device;
}