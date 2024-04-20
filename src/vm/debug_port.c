#include "debug_port.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


static err_t debug_port_read(struct bus_device_slist_node_t *dev, u8 addr, u8* out) {
    assert(dev);
    assert(out);

    switch (addr) {
        case 0: {
            *out = getchar();
        } break;
        case 1: {
            int v = 0;
            CHECK_RETURN_VALUE(scanf("%d", &v) != EOF, err_format("invalid scanf(\"%d\", ...) input"));
            CHECK_RETURN_VALUE(v >= 0 && v <= 0xFF, err_format("invalid integer value %d", v));
            *out = (u8)v;
        } break;
    }
    
    return err_success();
}

static err_t debug_port_write(struct bus_device_slist_node_t *dev, u8 addr, u8 value) {
    assert(dev);

    switch (addr) {
    case 0:
        printf("%c", value);
        break;
    case 1:
        printf("%d\n", (int)value);
        break;
    }

    return err_success();
}

struct bus_device_slist_node_t debug_port_create() {
    return (struct bus_device_slist_node_t){
        .name = "Debug Port",
        .addr_start = 0xFE,
        .addr_end = 0xFF,
        .read = debug_port_read,
        .write = debug_port_write,
    };
}
