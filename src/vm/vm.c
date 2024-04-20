#include "vm.h"

#include "ram.h"


void vm_init(struct vm_t *vm) {
    assert(vm);
    *vm = (struct vm_t){0};
    
    struct bus_device_slist_node_t *ram = malloc(sizeof(*ram));
    *ram = ram_create();

    vm->devices = ram;
}

void vm_destroy(struct vm_t *vm) {
    // FIXME: who does it anyway..
    
    // slist_foreach(vm->devices, struct bus_device_slist_node_t, dev) {
    //     if (dev->userdata)
    //         free(dev->userdata);
    // }
}
