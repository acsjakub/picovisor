# RUN 
compile visor
```
make
```

assemble guest
```
nasm test/hello_guest.S -o guest
```

run (for now, this reads guest code from hardcoded location: `guest` in `$PWD`)
```
./visor
```

## References
heavily sourced from: https://github.com/dpw/kvm-hello-world/blob/master/kvm-hello-world.c

KVM API: https://docs.kernel.org/virt/kvm/api.html

lab description: https://linux-kernel-labs.github.io/refs/heads/master/so2/assign7-kvm-vmm.html

