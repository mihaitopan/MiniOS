#include "Util.h"
#include "SerialLogging.h"
#include "VideoMemory.h"
#include "Interrupts.h"
#include "Timer.h"
#include "Console.h"
#include "MemoryManager.h"


void KernelMain()
{
    LogString("~");
    LogString("~LongMode entered");
    PrintFormatString("Video memory: %x\n", 0xB8000);

    EnableInterrupts();
    LogString("~Interrupts enabled");
    PrintFormatString("Interrupts enabled\n");
    // __debugbreak(); // PrintFormatString("Interrupts working");

    // PrintFormatString("Sleep: 2s");
    // TimerSleep(2000);
    // LogString("~Sleep test done");

    VirtualMemoryManagerInit();

    ConsoleRun();
}
