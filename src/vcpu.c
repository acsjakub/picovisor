#include <vcpu.h>

#include <linux/kvm.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>


int vcpu_init_real_mode(struct vcpu* vcpu, unsigned short ip) {
    struct kvm_sregs sregs;
    struct kvm_regs regs;

    // Read registers
    if (ioctl(vcpu->vcpu_fd, KVM_GET_SREGS, &sregs) < 0) {
        perror("KVM_GET_SREGS");
        return -1;
    }

    sregs.cs.selector = 0;
    sregs.cs.base = 0;

    if (ioctl(vcpu->vcpu_fd, KVM_SET_SREGS, &sregs)< 0) {
        perror("KVM_SET_SREGS");
        return -1;
    }

    memset(&regs, 0, sizeof(regs));

    regs.rflags = 2;
    regs.rip = (unsigned long long) ip;

    if (ioctl(vcpu->vcpu_fd, KVM_SET_REGS, &regs) < 0) {
        perror("KVM_SET_REGS");
        return -1;
    }

    return 0;
}
