#ifndef __UTIL_H__
#define __UTIL_H__
//
//  Author(s) : Mihai TOPAN
//
#include <intrin.h>
#include "Types.h"

//
// exported functions from __init.asm
//
extern void __cli(void);           // clear interrupts
extern void __sti(void);           // set interrupts
extern void __magic(void);         // MAGIC breakpoint into BOCHS (XCHG BX,BX)
extern void __enableSSE(void);     // enable SSE instructions
extern void __halt(void);          // halt the execution

//
// serial port helper functions
//
extern BYTE __inb(UINT16 Port);
extern VOID __outb(UINT16 Port, BYTE Data);

//
// timer helper function
//
extern VOID __interlock_increment(volatile UINT64*);

//
// disk helper functions
//
extern VOID __read_disk_sector(PBYTE);
extern VOID __write_disk_sector(PBYTE);

//
// memory helper functions
//
extern VOID __alloc_page(UINT16 Page);
extern VOID __free_page(UINT16 Page);
extern VOID __invlpg(VOID*);

#endif // __UTIL_H__