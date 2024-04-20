#ifndef BUS_H_
#define BUS_H_

#include "util.h"


struct bus_device_slist_node_t {
    struct slist_node_t node;
    const char* name;
    u8 addr_start;
    u8 addr_end;  // inclusive
    err_t (*read)(struct bus_device_slist_node_t *dev, u8 addr, u8* out);  // NULL is for "not supported", `addr` is relative to `addr_start`
    err_t (*write)(struct bus_device_slist_node_t *dev, u8 addr, u8 value);
    void *userdata;  // gets free()'d if non-zero
};

struct bus_device_slist_node_t* bus_get_device_at_addr(struct bus_device_slist_node_t *head, u8 addr);
err_t bus_read(struct bus_device_slist_node_t *head, u8 addr, u8* out);
err_t bus_write(struct bus_device_slist_node_t *head, u8 addr, u8 value);


#endif