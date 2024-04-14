#include "bus.h"


struct bus_device_slist_node_t* bus_get_device_at_addr(struct bus_device_slist_node_t *head, u8 addr) {
    assert(head != NULL);
    slist_foreach(head, struct bus_device_slist_node_t, dev) {
        if (addr >= dev->addr_start && addr <= dev->addr_end) {
            return dev;
        }
    }
    return NULL;
}

err_t bus_read(struct bus_device_slist_node_t *head, u8 addr, u8 *out) {
    assert(head != NULL);
    assert(out != NULL);
    *out = 0;
    struct bus_device_slist_node_t *dev = bus_get_device_at_addr(head, addr);
    CHECK_RETURN_VALUE(dev, err_format("read on unmapped address 0x%X.", addr));
    CHECK_RETURN_VALUE(dev->read, err_format("device does not support read operation."));
    return dev->read(dev, addr - dev->addr_start, out);
}

err_t bus_write(struct bus_device_slist_node_t *head, u8 addr, u8 value) {
    assert(head != NULL);
    struct bus_device_slist_node_t *dev = bus_get_device_at_addr(head, addr);
    CHECK_RETURN_VALUE(dev, err_format("write 0x%X on unmapped address 0x%X.", value, addr));
    CHECK_RETURN_VALUE(dev->write, err_format("device does not support write operation."));
    return dev->write(dev, addr - dev->addr_start, value);
}
