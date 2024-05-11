#ifndef MEM_REGION_H_
#define MEM_REGION_H_
#include <linux/kvm.h>
#include <unistd.h>

struct mem_region {
    int flags;
    unsigned long long guest_phys_addr;
    size_t size;
    void *host_userspace_addr;
    void (*mem_region_wr_hnd)(unsigned long long addr, size_t size, void* data);
    void (*mem_region_rd_hnd)(unsigned long long addr, size_t size, void* data);
};

int mr_new_private(struct mem_region*, int, unsigned long long, size_t);
int mr_new_shared(struct  mem_region*, int, unsigned long long, size_t);
int mr_new_file_mapped(struct mem_region*, int, unsigned long long, size_t);



#endif // MEM_REGION_H_
