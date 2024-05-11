#include <mem_region.h>

#include <sys/mman.h>


int mr_new_private(struct mem_region* mr,
                   int flags,
                   unsigned long long guest_phys_addr,
                   size_t size)
{
    mr->flags = flags;
    mr->guest_phys_addr = guest_phys_addr;
    mr->size = size;
    mr->host_userspace_addr = mmap(NULL,
                                   mr->size,
                                   PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                                   -1,
                                   0);

    if (mr->host_userspace_addr == MAP_FAILED) {
        return -1;
    }

    return 0;
}

int mr_new_shared(struct mem_region* mr,
                  int flags,
                  unsigned long long guest_phys_addr,
                  size_t size)
{
    // TODO implement
    //
    //shared_fd = shm_open("shared", O_CREAT | O_RDWR, 0666);
    //ftruncate(shared_fd, 4096);
    //guest_mmio_memory = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED , shared_fd, 0);
    //vm_add_memory_region(&vm, 0x2000, (unsigned long long) guest_mmio_memory, 1, 4096, 0);


    return -1;

}

int mr_new_file_mapped(struct mem_region* mr,
                  int flags,
                  unsigned long long guest_phys_addr,
                  size_t size)
{
    // TODO implement
    return -1;

}
