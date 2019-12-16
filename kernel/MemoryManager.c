#include "MemoryManager.h"
#include "Util.h"
#include "VideoMemory.h"


UINT64 gPhysicalBitmap = 0;
UINT16 gPhysicalFreeCount = 64;
#define IS_BIT_SET(map, pos) !!((map & ((UINT64)1 << pos)))
//                         0 == (map & ((UINT64)1 << pos))
#define SET_BIT(map, pos)       (map |= ((UINT64)1) << pos)
#define CLEAR_BIT(map, pos)     (map &= ~(((UINT64)1) << pos))


PVOID
AllocPhysicalPages(
    UINT16 noPages
)
{
    if (UINT64_MAX == gPhysicalBitmap || 0 == noPages)
    {
        return NULL;
    }

    UINT16 count = 1;
    UINT16 foundIndex = 0;
    for (UINT16 i = 0; i < NO_MAX_PAGES; i++)
    {
        if (!IS_BIT_SET(gPhysicalBitmap, i))
        {
            foundIndex = i;
            while (count < noPages)
            {
                if (NO_MAX_PAGES == i ||
                    IS_BIT_SET(gPhysicalBitmap, i))
                {
                    foundIndex = NO_MAX_PAGES;
                    // PrintFormatString("REFUSED index: %x\n", i);
                    break;
                }
                else
                {
                    i++;
                }
                count++;
            }

            if (NO_MAX_PAGES != foundIndex)
            {
                // PrintFormatString("FOUND index: %x\n", foundIndex);
                break;
            }

            count = 1;
            foundIndex = i;
        }
    }

    if (NO_MAX_PAGES == foundIndex)
    {
        return NULL;
    }

    void* addr = NULL;
    for (UINT16 i = 0; i < noPages; i++)
    {
        __alloc_page(i + foundIndex);
        // PrintFormatString("allocated: %x\n", (i + foundIndex));
        SET_BIT(gPhysicalBitmap, (i + foundIndex));
        gPhysicalFreeCount--;

        if (NULL == addr)
        {
            addr = (void*)((UINT64)HEAP_BASE_ADDRESS + (UINT64)PAGE_SIZE * (i + foundIndex));
        }
    }

    return addr;
}

BOOL
FreePhysicalPages(
    PVOID Page, 
    UINT16 noPages
)
{
    UINT64 pageIt = (UINT64)Page;
    for (UINT16 i = 0; i < noPages; i++)
    {
        if (((UINT64)pageIt - HEAP_BASE_ADDRESS) % PAGE_SIZE)
        {
            return FALSE;
        }

        UINT16 index = (UINT16)((BYTE*)pageIt - (BYTE*)HEAP_BASE_ADDRESS) / PAGE_SIZE;
        if (index > NO_MAX_PAGES || !IS_BIT_SET(gPhysicalBitmap, index))
        {
            return FALSE;
        }

        __free_page(index);
        // PrintFormatString("freed: %x\n", (index));
        CLEAR_BIT(gPhysicalBitmap, index);
        gPhysicalFreeCount++;

        pageIt += PAGE_SIZE;
    }

    Page = NULL;
    return TRUE;
}


PVOID
AllocVirtualPages(
    UINT16 noPages
)
{
    UINT64 PhysicalPage = (UINT64)AllocPhysicalPages(noPages);
    return (PVOID)(PhysicalPage + VIRTUAL_HEAP_OFFSET);
}

BOOL
FreeVirtualPages(
    PVOID Page, 
    UINT16 noPages
)
{
    UINT64 PhysicalPage = (UINT64)Page - VIRTUAL_HEAP_OFFSET;
    return FreePhysicalPages((PVOID)PhysicalPage, noPages);
}

/// move logic up to virtual allocator
typedef struct _VMAP
{
    void* physicalAddr;
    UINT16 noPages;
} VMAP;
VMAP gMapping[64];

VOID
VirtualMemoryManagerInit(void)
{
    for (UINT16 i = 0; i < PAGE_SIZE; i++)
    {
        gMapping[i].physicalAddr = NULL;
        gMapping[i].noPages = 0;
    }
}


PVOID
MMAlloc(
    UINT16 Space
)
{
    UINT16 noPages = (0 == Space % PAGE_SIZE) ? Space / PAGE_SIZE : Space / PAGE_SIZE + 1;
    // PrintFormatString("noPages: %x\n", noPages);
    void* addr = AllocVirtualPages(noPages);
    if (NULL == addr)
    {
        return NULL;
    }
    
    for (UINT16 i = 0; i < PAGE_SIZE; i++)
    {
        if (NULL == gMapping[i].physicalAddr)
        {
            gMapping[i].physicalAddr = addr;
            gMapping[i].noPages = noPages;

            // PrintFormatString("virtualAddr: %x\n", addr);
            break;
        }
    }

    return addr;
}

BOOL
MMFree(
    PVOID Addr
)
{
    BOOL found = FALSE;
    UINT16 noPages = 0;
    for (UINT16 i = 0; i < PAGE_SIZE; i++)
    {
        if (Addr == gMapping[i].physicalAddr)
        {
            gMapping[i].physicalAddr = 0;
            noPages = gMapping[i].noPages;
            gMapping[i].noPages = 0;
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        return FALSE;
    }

    return FreeVirtualPages(Addr, noPages);
}
