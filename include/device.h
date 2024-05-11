#ifndef DEVICE_H_
#define DEVICE_H_

#include <mem_region.h>

struct device {
    char name[256];
    int (*dev_init)(struct mem_region*);
};



#endif // DEVICE_H_
