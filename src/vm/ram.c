#include "ram.h"

#include <assert.h>
#include <stdlib.h>


struct ram_t {
    u8 data[0x7F * 0xFF];
    u8 page;
};

static err_t ram_read(struct bus_device_slist_node_t *dev, u8 addr, u8* out) {
    assert(out);
    assert(dev);
    assert(dev->userdata);

    struct ram_t* ram = dev->userdata;
    *out = (addr == 0x80) ? (ram->page) : (*(ram->data + (ram->page * 0xFF) + addr));
    
    return err_success();
}

static err_t ram_write(struct bus_device_slist_node_t *dev, u8 addr, u8 value) {
    assert(dev);
    assert(dev->userdata);

    struct ram_t* ram = dev->userdata;
    u8 *v = (addr == 0x80) ? (&(ram->page)) : (ram->data + (ram->page * 0xFF) + addr);
    *v = value;

    return err_success();
}

struct bus_device_slist_node_t ram_create() {
    void *instance = calloc(1, sizeof(struct ram_t));
    return (struct bus_device_slist_node_t){
        .name = "RAM",
        .addr_start = 0x00,
        .addr_end = 0x80,
        .read = ram_read,
        .write = ram_write,
        .userdata = instance
    };
}
