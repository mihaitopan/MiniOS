#ifndef __TIMER_H__
#define __TIMER_H__
//
//  Author(s) : Mihai TOPAN
//
#include "Types.h"

#define TIMER_COMMAND      0x43
#define TIMER_DATA         0x40

VOID
TimerInit(void);

void IsrTimerEvent(void);

UINT64
TimerGetTickCount(void);

VOID
TimerSleep(
    UINT64 Milliseconds
);

// DOCS: https://wiki.osdev.org/APIC_timer
// GITS: https://github.com/sukwon0709/osdev/blob/master/tutorial/src/timer.c
#endif // __TIMER_H__