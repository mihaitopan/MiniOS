#include "Timer.h"
#include "Util.h"
#include "Pic.h"
#include "SerialLogging.h"
#include "VideoMemory.h"

static volatile UINT64 gTickCount = 0;


VOID
TimerInit(void)
{
    UINT32 frequency = 100; // 14,551‬
    UINT32 timerBaseFreq = 1193182;

    UINT32 divisor = timerBaseFreq / frequency;
    BYTE lowByte = ((BYTE*)(&divisor))[0];
    BYTE highByte = ((BYTE*)(&divisor))[1];

    __outb(TIMER_COMMAND, 0x36);
    __outb(TIMER_DATA, lowByte);
    __outb(TIMER_DATA, highByte);
}

void IsrTimerEvent(void)
{
    __interlock_increment(&gTickCount);
    __outb(PIC1, PIC_EOI);
}

UINT64
TimerGetTickCount(void)
{
    return gTickCount;
}

VOID
TimerSleep(
    UINT64 Milliseconds
)
{

    UINT64 intialTimerTickCount = TimerGetTickCount();
    while (TimerGetTickCount() - intialTimerTickCount <= Milliseconds);
}
