#include <core/bus.h>
#include <core/util.h>

#include "test.h"


static u8 next_byte_to_be_read = 0;
static u8 last_written_byte = 0;
static u8 last_write_addr = 0;
static u8 last_read_addr = 0;
static const char* dev_name = "test-device";
static struct bus_device_slist_node_t *device = NULL;

static err_t dev_read(struct bus_device_slist_node_t *dev, u8 addr, u8* out) {
    TEST_ASSERT_EQ(dev, device);
    TEST_ASSERT_STREQ(dev->name, dev_name);
    TEST_ASSERT_LEQ(addr, dev->addr_end - dev->addr_start);
    *out = next_byte_to_be_read;
    last_read_addr = dev->addr_start + addr;
    return err_success();
}

static err_t dev_write(struct bus_device_slist_node_t *dev, u8 addr, u8 value) {
    TEST_ASSERT_EQ(dev, device);
    TEST_ASSERT_STREQ(dev->name, dev_name);
    TEST_ASSERT_LEQ(addr, dev->addr_end - dev->addr_start);
    last_written_byte = value;
    last_write_addr = dev->addr_start + addr;
    return err_success();
}

int main(int argc, char const *argv[]) {
    struct bus_device_slist_node_t dev = {
        .name = dev_name,
        .addr_start = 0x10,
        .addr_end = 0x20,
        .read = dev_read,
        .write = dev_write,
    };
    device = &dev;

    next_byte_to_be_read = 0xAA;
    u8 val = 0;
    err_t err = bus_read(device, 0x18, &val);
    TEST_ASSERT_EQ(err.succeded, true);
    TEST_ASSERT_EQ(val, next_byte_to_be_read);
    TEST_ASSERT_EQ(last_read_addr, 0x18);

    err = bus_write(device, 0x1F, 0xAA);
    TEST_ASSERT_EQ(err.succeded, true);
    TEST_ASSERT_EQ(last_write_addr, 0x1F);
    TEST_ASSERT_EQ(last_written_byte, 0xAA);

    TEST_SUCCEED();
}
