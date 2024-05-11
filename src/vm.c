#include <vm.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

int vm_add_device(struct VM* vm,
                  struct device* dev)
{
    // call device's init function?
    // which will add memory regions and the handlers?
    //
    // TODO check if allocation successful
    struct mem_region *mmio = malloc(sizeof(struct mem_region));
    if (mr_new_private(mmio, KVM_MEM_READONLY, 0x4000, 0x1000) < 0) {
        printf("Failed to create MMIO region\n");
        return -1;
    }

    if (vm_add_memory_region(vm, mmio, vm->nr_mem_regions) < 0) {
        printf("Failed to add MMIO region\n");
        return -1;
    }

    if (dev->dev_init(mmio) < 0) {
        printf("Failed to init device\n");
        return -1;
    }

    return 0;
}

int vm_add_memory_region(struct VM* vm,
                         struct mem_region* mem_region,
                         int slot)
{
    struct kvm_userspace_memory_region kvm_mem_region;

    if (vm->nr_mem_regions >= MAX_MEM_REGIONS) {
        // TODO explain errors somewhere?
        return -2;
    }

    kvm_mem_region.slot = slot;
    kvm_mem_region.flags = mem_region->flags;
    kvm_mem_region.guest_phys_addr = mem_region->guest_phys_addr;
    kvm_mem_region.memory_size = mem_region->size;
    kvm_mem_region.userspace_addr = (unsigned long long) mem_region->host_userspace_addr;

    if (ioctl(vm->vm_fd, KVM_SET_USER_MEMORY_REGION, &kvm_mem_region) < 0) {
        perror("KVM_SET_USER_MEMORY_REGION");
        return -1;
    }

    // This introduces multiple owners of mem_region pointer
    vm->mem_regions[vm->nr_mem_regions++] = mem_region;

    return 0;
}

int vm_create(struct VM* vm, int kvm_fd, size_t RAM_size) {
    // I decided to include RAM in the creation of VM,
    // since I believe having a machine without RAM
    // makes little sense
    struct mem_region * ram_mr;

    vm->kvm_fd = kvm_fd;
    vm->vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0);

    if (vm->vm_fd < 0) {
        perror("KVM_CREATE_VM");
        return -1;
    }

    // I don't fully understand why or if is this needed atm,
    // found inconclusive and slightly contradicting material
    // on this
    if ( ioctl(vm->vm_fd, KVM_SET_TSS_ADDR, 0xfffbd000) < 0) {
        perror("KVM_SET_TSS_ADDR");
        return -1;
    }

    vm->nr_mem_regions = 0;

    ram_mr = malloc(sizeof(struct mem_region));

    if (mr_new_private(ram_mr, 0, 0, RAM_size) < 0) {
        printf("Failed to create RAM mem region\n");
        return -1;
    }

    if (vm_add_memory_region(vm, ram_mr, 0) < 0) {
        printf("Failed to add RAM region\n");
        return -1;
    }


    return 0;
}


int vm_create_vcpu(struct VM* vm) {
    int kvm_vcpu_mmap_size;
    vm->vcpu.vcpu_fd = ioctl(vm->vm_fd, KVM_CREATE_VCPU, 0);
    if (vm->vcpu.vcpu_fd < 0) {
        perror("KVM_CREATE_VCPU");
        return -1;
    }

    // READ the global size of kvm_run size
    kvm_vcpu_mmap_size = ioctl(vm->kvm_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
    if (kvm_vcpu_mmap_size <= 0) {
        perror("KVM_GET_VCPU_MMAP_SIZE");
        return -1;
    }
    vm->vcpu.runtime_control = mmap(NULL, kvm_vcpu_mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vm->vcpu.vcpu_fd, 0);
    if (vm->vcpu.runtime_control == MAP_FAILED) {
        perror("KVM_MMAP_RUN");
        return -1;
    }

    return 0;
}

int vm_load_guest_program(struct VM* vm, char * guest_program, size_t size) {
    // TODO check if program fits into the available memory
    // TODO assuming the memory in slot 0 is RAM - where the guest
    // program is expected
    //
    if (vm->nr_mem_regions < 1) {
        return -1;
    }
    memcpy((void *) vm->mem_regions[0]->host_userspace_addr, guest_program, size);

    return 0;
}

int vm_run_vcpu(struct VM* vm, struct vcpu* vcpu) {
    struct kvm_regs regs;
    int halt = 0;

    printf("\n -- RUNNING -- \n");
    while (halt == 0) {
        if (ioctl(vcpu->vcpu_fd, KVM_RUN, 0) < 0) {
            perror("KVM_RUN");
            return -1;
        }

        if (ioctl(vcpu->vcpu_fd, KVM_GET_REGS, &regs) < 0) {
            perror("KVM_GET_REGS");
            return -1;
        }

        switch (vcpu->runtime_control->exit_reason) {

            case KVM_EXIT_HLT:
                printf("\n -- vCPU halted. -- \n");
                halt = 1;
                break;

            // TODO distribute accesses to devices when implemented
            case KVM_EXIT_MMIO:
                // iterate vm->memory_regions to understand where it falls
                // and call the handler
                //printf("MMIO exit\n");
                unsigned long long phys_addr = (unsigned long long) vcpu->runtime_control->mmio.phys_addr;
                //printf("addr: 0x%llx\n", phys_addr);

                for (int i = 0; i < vm->nr_mem_regions; i++) {
                    // TODO add debugging output capability
                    //printf("vm mem region: 0x%llx\n", (unsigned long long) vm->mem_regions[i]);
                    //printf("vm mem region->guest_phys_addr: 0x%llx\n", (unsigned long long) vm->mem_regions[i]->guest_phys_addr);
                    if (phys_addr >= vm->mem_regions[i]->guest_phys_addr &&
                        phys_addr < vm->mem_regions[i]->guest_phys_addr + vm->mem_regions[i]->size) {

                        if (vcpu->runtime_control->mmio.is_write) {
                            //printf("Calling write handler\n");
                            // subtracting because we want to pass the offset
                            vm->mem_regions[i]->mem_region_wr_hnd(phys_addr - vm->mem_regions[i]->guest_phys_addr, vcpu->runtime_control->mmio.len, vcpu->runtime_control->mmio.data);
                        } else {
                            //printf("Calling read handler\n");
                            vm->mem_regions[i]->mem_region_rd_hnd(phys_addr, vcpu->runtime_control->mmio.len, vcpu->runtime_control->mmio.data);

                        }
                    }
                }
                //struct kvm_run * runtime_control = vcpu->runtime_control;
                //printf("Phys_addr: 0x%llx\n", runtime_control->mmio.phys_addr);
                //printf("Data: 0x%llx\n", *((unsigned long long *) runtime_control->mmio.data));
                //printf("len: 0x%u\n", runtime_control->mmio.len);
                //printf("is_write: 0x%u\n", runtime_control->mmio.is_write);

                break;

            default:
                printf("Exit reason: %d\n", vcpu->runtime_control->exit_reason);
                halt = 1;
        }

        //printf("RIP: %llx\n", regs.rip);
        //printf("RAX: %llx\n", regs.rax);

    }

    return 0;
}
