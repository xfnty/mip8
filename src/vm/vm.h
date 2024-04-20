#ifndef VM_VM_H_
#define VM_VM_H_

#include "core/cpu.h"
#include "core/util.h"


struct vm_t {
    struct cpu_t cpu;
    struct bus_device_slist_node_t *devices;
};

void vm_init(struct vm_t *vm);
void vm_destroy(struct vm_t *vm);

#endif