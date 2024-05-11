#ifndef VCPU_H_
#define VCPU_H_

struct vcpu {
    int vcpu_fd;
    struct kvm_run* runtime_control;
};

int vcpu_init_real_mode(struct vcpu*, unsigned short ip);


#endif // VCPU_H_
