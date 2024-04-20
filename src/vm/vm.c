#include "vm.h"

#include "ram.h"
#include "debug_port.h"


typedef struct bus_device_slist_node_t (*create_device_func_t)();

void vm_init(struct vm_t *vm) {
    assert(vm);
    *vm = (struct vm_t){0};
    
    static const create_device_func_t create_device_funcs[] = {
        ram_create, debug_port_create
    };

    for (int i = 0; i < LENGTH(create_device_funcs); i++) {
        struct bus_device_slist_node_t *dev = malloc(sizeof(*dev));
        *dev = create_device_funcs[i]();
        slist_add(&(vm->devices), dev);
    }
}

void vm_destroy(struct vm_t *vm) {
    // FIXME: who does it anyway..
    
    // slist_foreach(vm->devices, struct bus_device_slist_node_t, dev) {
    //     if (dev->userdata)
    //         free(dev->userdata);
    // }
}
