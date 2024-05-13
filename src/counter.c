#include <device.h>
#include <mem_region.h>
#include <counter.h>

#include <stdio.h>

unsigned char counter = 0;
struct mem_region* assigned_region = NULL;

void counter_read(unsigned long long addr, size_t size, void *data) {
    // hooking reading is not possible in the model currently used
    return;
}

void counter_write(unsigned long long addr, size_t size, void *data) {
    // writing increases the counter
    if (addr == 4 && assigned_region != NULL) {
        counter++;
        *(unsigned char *)assigned_region->host_userspace_addr = counter;
    }
}

int counter_init(struct mem_region* mr) {
    printf("Counter init\n");
    *(unsigned int *)mr->host_userspace_addr = 0x00000000;
    assigned_region = mr;

    mr->mem_region_wr_hnd = counter_write;
    mr->mem_region_rd_hnd = counter_read;

    return 0;
}
