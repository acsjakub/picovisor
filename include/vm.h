#ifndef VM_H_
#define VM_H_
#include "mem_region.h"
#include "device.h"
#include "vcpu.h"

#define MAX_MEM_REGIONS 10
#define MAX_DEVICES 10

struct VM {
    int kvm_fd;
    int vm_fd;
    int nr_mem_regions;
    struct vcpu vcpu;
    struct mem_region* mem_regions[MAX_MEM_REGIONS];
    struct device devices[MAX_DEVICES];
};

int vm_add_memory_region(struct VM*, struct mem_region*, int);
int vm_add_device(struct VM*, struct device*);
int vm_create(struct VM*, int, size_t);
int vm_create_vcpu(struct VM*);
int vm_load_guest_program(struct VM*, char *, size_t);
int vm_run_vcpu(struct VM*, struct vcpu*);


#endif // VM_H_
