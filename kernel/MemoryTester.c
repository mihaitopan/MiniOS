#include "MemoryTester.h"
#include "MemoryManager.h"
#include "String.h"
#include "Util.h"
#include "VideoMemory.h"
#include "Timer.h"


BOOL
WriteStringToPhysicalMemoryPage(
    PSTRING String,
    UINT16 First,
    UINT16 Last,
    PVOID Page
)
{
    if (!String || First > Last || Last > String->Length)
    {
        return FALSE;
    }

    for (UINT16 i = First; i < Last; i++)
    {
        ((PCHAR)Page)[i % PAGE_SIZE] = String->Buffer[i];
    }

    return TRUE;
}


BOOL
WriteStringToMemoryPage(
    PSTRING String,
    UINT16 First,
    UINT16 Last,
    PVOID Page
)
{
    UINT64 physicalPage = (UINT64)Page - VIRTUAL_HEAP_OFFSET;
    return WriteStringToPhysicalMemoryPage(String, First, Last, (PVOID)physicalPage);
}

BOOL
WriteStringToMemory(
    PSTRING String, 
    PVOID Addr
)
{
    if (!String)
    {
        return FALSE;
    }

    BOOL flag = TRUE;
    UINT16 stringIndex = 0;
    UINT16 noPages = (0 == String->Length % PAGE_SIZE) ? String->Length / PAGE_SIZE : String->Length / PAGE_SIZE + 1;
    if (0 == noPages)
    {
        return FALSE;
    }

    UINT64 NextPage = (UINT64)Addr;
    for (UINT16 i = 0; i < noPages - 1; i++)
    {
        if (!WriteStringToMemoryPage(String, stringIndex, stringIndex + PAGE_SIZE, (PVOID)NextPage))
        {
            flag = FALSE;
        }

        stringIndex += PAGE_SIZE;
        NextPage += PAGE_SIZE;
    }

    if (!flag)
    {
        return FALSE;
    }

    return WriteStringToMemoryPage(String, stringIndex, stringIndex + (String->Length % PAGE_SIZE), (PVOID)NextPage);
}


VOID
TestPhysicalPageAllocators(void)
{
    STRING stringP1 = CONST_STRING("11111111");
    STRING stringP2 = CONST_STRING("22222222");
    STRING stringP3 = CONST_STRING("33333333");
    
    void* p1 = AllocPhysicalPages(1);
    PrintFormatString("p1: %x\n", p1);
    WriteStringToPhysicalMemoryPage(&stringP1, 0, stringP1.Length, p1);
    // WriteStringToPhysicalMemoryPage(&stringP2, 0, stringP2.Length, (void*)0x401000); // should Page fault 

    void* p2 = AllocPhysicalPages(1);
    PrintFormatString("p2: %x\n", p2);
    WriteStringToPhysicalMemoryPage(&stringP2, 0, stringP2.Length, p2);
    FreePhysicalPages((void*)p2, 1);
    // WriteStringToPhysicalMemoryPage(&stringP2, 0, stringP2.Length, (void*)0x401000); // should Page fault 

    void* p3 = AllocPhysicalPages(1);
    PrintFormatString("p3: %x\n", p3);
    WriteStringToPhysicalMemoryPage(&stringP3, 0, stringP3.Length, (void*)0x401000); // should not Page fault, because 0x401000 is reused

    // __magic();
    FreePhysicalPages((void*)p3, 1);
    FreePhysicalPages((void*)p1, 1);

    PrintFormatString("Test succedded.\n");
    __magic(); // TimerSleep(5000);
}

void TestVirtualPageAllocators()
{
    STRING stringP1 = CONST_STRING("11111111");
    STRING stringP2 = CONST_STRING("22222222");
    STRING stringP3 = CONST_STRING("33333333");

    void* p1 = AllocVirtualPages(1);
    PrintFormatString("p1: %x\n", p1);
    WriteStringToMemoryPage(&stringP1, 0, stringP1.Length, p1);
    // WriteStringToMemoryPage(&stringP2, 0, stringP2.Length, (void*)0x9401000); // should Page fault 

    void* p2 = AllocVirtualPages(1);
    PrintFormatString("p2: %x\n", p2);
    WriteStringToMemoryPage(&stringP2, 0, stringP2.Length, p2);
    FreeVirtualPages((void*)p2, 1);
    // WriteStringToMemoryPage(&stringP2, 0, stringP2.Length, (void*)0x9401000); // should Page fault 

    void* p3 = AllocVirtualPages(1);
    PrintFormatString("p3: %x\n", p3);
    WriteStringToMemoryPage(&stringP3, 0, stringP3.Length, (void*)0x9401000); // should not Page fault, because 0x401000 is reused

    void* p4 = AllocVirtualPages(1);
    PrintFormatString("p4: %x\n", p4);
    // __magic();
    FreeVirtualPages((void*)p1, 1);
    FreeVirtualPages((void*)p3, 1);

    void* p5 = AllocVirtualPages(2); // should allocate pages with index 0 and 1
    PrintFormatString("p5: %x\n", p5);

    // __magic();
    FreeVirtualPages((void*)p4, 1);
    FreeVirtualPages((void*)p5, 2);

    PrintFormatString("Test succedded.\n");
    __magic(); // TimerSleep(5000);
}

void TestVirtualByteAllocators()
{
    STRING stringAA = CONST_STRING("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    STRING stringBB = CONST_STRING("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");

    CHAR* bbbb = MMAlloc(5000);
    PrintFormatString("bbbb: %x\n", bbbb);
    WriteStringToMemory(&stringBB, (void*)bbbb);

    CHAR* aaaa = MMAlloc(30);
    PrintFormatString("aaaa: %x\n", aaaa);
    WriteStringToMemory(&stringAA, (void*)aaaa);

    MMFree(aaaa);
    MMFree(bbbb);

    PrintFormatString("Test succedded.\n");
    __magic(); // TimerSleep(5000);
}
