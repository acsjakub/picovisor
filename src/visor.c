//#include "visor.h"
#include <fcntl.h>
#include <linux/kvm.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <printer.h>
#include <vm.h>
#include <vcpu.h>

#define RAM_SIZE 0x1000

int init_kvm() {
    int kvm_fd, api_ver, mem_slots;
    kvm_fd = open("/dev/kvm", O_RDWR);
    if (kvm_fd < 0) {
        return -1;
    }

    api_ver = ioctl(kvm_fd, KVM_GET_API_VERSION, 0);
    // let's just assume it succeeds for now
    printf("API Version: %d\n", api_ver);

    mem_slots = ioctl(kvm_fd, KVM_CHECK_EXTENSION, KVM_CAP_NR_MEMSLOTS);
    printf("MEM slots: %d\n", mem_slots);

    return kvm_fd;
}

int main(void) {
  int kvm_fd;
  struct VM vm;

  int binary_fd;
  char guest_prog[4096];
  size_t guest_prog_size;

  kvm_fd = init_kvm();
  if (kvm_fd < 0){
      return -1;
  }

  if (vm_create(&vm, kvm_fd, RAM_SIZE) < 0) {
      return -1;
  }
  struct device printer_dev = {
        .name = "PRINTER",
        .dev_init = printer_init
  };

  vm_add_device(&vm, &printer_dev, 0x4000, 0x1000);

  if (vm_create_vcpu(&vm) < 0) {
      return -1;
  }

  if (vcpu_init_real_mode(&vm.vcpu, 0) < 0) {
      return -1;
  }

  // TODO allocate the memory for guest program dynamically
  // TODO allow specifying the guest binary as an argument
  binary_fd = open("guest", O_RDONLY);
  guest_prog_size = read(binary_fd, &guest_prog, 4096);
  vm_load_guest_program(&vm, guest_prog, guest_prog_size);

  vm_run_vcpu(&vm, &vm.vcpu);

  return 0;
}
