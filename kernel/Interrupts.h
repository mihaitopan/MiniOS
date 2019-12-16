#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__
//
//  Author(s) : Mihai TOPAN
//
#include "Types.h"
#include "Pic.h"


#pragma pack(push, 1)
typedef struct _IDT_ENTRY
{
    UINT16 OffsetLow;
    UINT16 Selector;
    BYTE IST;
    BYTE Attributes;
    UINT16 OffsetMid;
    UINT32 OffsetHigh;
    UINT32 Reserved;
} IDT_ENTRY, *PIDT_ENTRY;

typedef struct _IDT_DESCRIPTOR
{
    UINT16 Limit;
    UINT64 Base;
} IDT_DESCRIPTOR, *PIDT_DESCRIPTOR;

typedef struct _TRAP_FRAME {
    UINT64 RAX;
    UINT64 RBX;
    UINT64 RCX;
    UINT64 RDX;
    UINT64 RSI;
    UINT64 RDI;
    UINT64 R8;
    UINT64 R9;
    UINT64 R10;
    UINT64 R11;
    UINT64 R12;
    UINT64 R13;
    UINT64 R14;
    UINT64 R15;
    UINT64 RBP;
    UINT64 Flags;
} TRAP_FRAME, *PTRAP_FRAME;
#pragma pack(pop)

VOID
EnableInterrupts();

VOID
DumpTrapFrame(
    PTRAP_FRAME TrapFrame, 
    UINT64 InterruptIndex
);

// DOCS: https://wiki.osdev.org/Interrupts
// DOCS: https://wiki.osdev.org/Interrupt_Descriptor_Table
// GITS: https://github.com/pritamzope/OS/blob/master/Interrupt_Descriptor_Table/isr.h
#endif // __INTERRUPTS_H__