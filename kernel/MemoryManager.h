#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

#include "Types.h"

#define HEAP_PT             0x00006000
#define HEAP_BASE_ADDRESS   0x00400000
#define VIRTUAL_HEAP_OFFSET 0x09000000
// also hardcoded in __init.asm

#define NO_MAX_PAGES      64


PVOID
AllocPhysicalPages(
    UINT16 noPages
);

BOOL
FreePhysicalPages(
    PVOID Page, 
    UINT16 noPages
);


VOID
VirtualMemoryManagerInit(void);

PVOID
AllocVirtualPages(
    UINT16 noPages
);

BOOL
FreeVirtualPages(
    PVOID Page, 
    UINT16 noPages
);


PVOID
MMAlloc(
    UINT16 noPages
);

BOOL
MMFree(
    PVOID Addr
);

// DOCS: https://wiki.osdev.org/Memory_management
// DOCS: https://wiki.osdev.org/Memory_Management_Unit
// DOCS: https://wiki.osdev.org/Memory_Allocation
// DOCS: https://wiki.osdev.org/Writing_a_memory_manager
#endif // __MEMORY_MANAGER_H__