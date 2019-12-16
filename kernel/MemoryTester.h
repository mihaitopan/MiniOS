#ifndef __MEMORY_TESTER_H__
#define __MEMORY_TESTER_H__
//
//  Author(s) : Mihai TOPAN
//
#include "String.h"


BOOL
WriteStringToPhysicalMemoryPage(
    PSTRING String,
    UINT16 First,
    UINT16 Last, 
    PVOID Page
);

BOOL
WriteStringToMemoryPage(
    PSTRING String,
    UINT16 First,
    UINT16 Last, 
    PVOID Page
);

BOOL
WriteStringToMemory(
    PSTRING String,
    PVOID Addr
);

VOID
TestPhysicalPageAllocators(void);

VOID
TestVirtualPageAllocators(void);

VOID
TestVirtualByteAllocators(void);

#endif // __MEMORY_TESTER_H__