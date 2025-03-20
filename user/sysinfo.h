#ifndef SYSINFO_H
#define SYSINFO_H
#include "types.h"

struct sysinfo {
    uint64 freemem;  // Amount of free memory in bytes
    uint64 nproc;    // Number of active processes
};

#endif