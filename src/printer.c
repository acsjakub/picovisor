#include <device.h>
#include <mem_region.h>
#include <printer.h>

#include <stdio.h>

void printer_read(unsigned long long addr, size_t size, void *data) {
    // reading is no-op
    return;
}

void printer_write(unsigned long long addr, size_t size, void *data) {
    // addr should be offset
    if (addr == 0) {
        printf("%c", ((char*)data)[0]);
    }
}

int printer_init(struct mem_region* mr) {

    mr->mem_region_wr_hnd = printer_write;
    mr->mem_region_rd_hnd = printer_read;

    return 0;
}
