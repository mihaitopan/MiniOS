#include "Console.h"
#include "Util.h"
#include "VideoMemory.h"
#include "SerialLogging.h"
#include "Timer.h"
#include "Disk.h"
#include "MemoryTester.h"


static USER_COMMAND gUserCommand = { 0 };

static STRING trapBreak = CONST_STRING("break");
static STRING timeSleep = CONST_STRING("sleep");
static STRING readDisk  = CONST_STRING("read");
static STRING writeDisk = CONST_STRING("write");
static STRING Tphysical = CONST_STRING("physical");
static STRING Tvirtual  = CONST_STRING("virtual");
static STRING Tmalloc   = CONST_STRING("malloc");

VOID
ConsolePrintHelp(void)
{
    PrintFormatString("Available commands:\n"
                      "> break    - __debugbreak exception\n"
                      "> sleep    - sleep 5 seconds\n"
                      "> read     - read stored info on disk\n"
                      "> write    - write info to disk\n"
                      "> physical - test physiscal page memory allocator\n"
                      "> virtual  - test virtual page memory allocator\n"
                      "> malloc   - test virtual byte memory allocator\n");
}


VOID
ConsoleReadCommand(void)
{
    ClearScreen();
    ConsolePrintHelp();
    PrintFormatString("> ");

    ConsoleResetBuffer();
    gUserCommand.IsReceivingInput = TRUE;
    while (ConsoleIsReceivingInput());
}

VOID
ConsoleMatchCommand(void)
{
    ClearScreen();

    if (AreStringsEqual(&trapBreak, gUserCommand.Cursor - 1, gUserCommand.Buffer))
    {
        ConsoleTrapBreak();
    }
    else if (AreStringsEqual(&timeSleep, gUserCommand.Cursor - 1, gUserCommand.Buffer))
    {
        ConsoleTimeSleep();
    }
    else if (AreStringsEqual(&readDisk, gUserCommand.Cursor - 1, gUserCommand.Buffer))
    {
        ConsoleReadDisk();
    }
    else if (AreStringsEqual(&writeDisk, gUserCommand.Cursor - 1, gUserCommand.Buffer))
    {
        ConsoleWriteDisk();
    }
    else if (AreStringsEqual(&Tphysical, gUserCommand.Cursor - 1, gUserCommand.Buffer))
    {
        ConsoleTestPhysicalPageAllocators();
    }
    else if (AreStringsEqual(&Tvirtual, gUserCommand.Cursor - 1, gUserCommand.Buffer))
    {
        ConsoleTestVirtualPageAllocators();
    }
    else if (AreStringsEqual(&Tmalloc, gUserCommand.Cursor - 1, gUserCommand.Buffer))
    {
        ConsoleTestVirtualByteAllocators();
    }
    else
    {
        PrintFormatString("Invalid command!\n");
        ConsolePrintHelp();
        TimerSleep(2000);
    }
}

VOID
ConsoleResetBuffer(void)
{
    gUserCommand.IsReceivingInput = FALSE;
    gUserCommand.Cursor = 0;
    for (UINT16 i = 0; i < sizeof(gUserCommand.Buffer); i++) { gUserCommand.Buffer[i] = 0; }
}

VOID
ConsolePutCharacter(
    CHAR Character
)
{
    if (gUserCommand.Cursor == sizeof(gUserCommand.Buffer)) { return; }
    gUserCommand.Buffer[gUserCommand.Cursor] = Character;
    gUserCommand.Cursor++;
    PrintCharacter(Character);
}

VOID
ConsoleBackspace(void)
{
    if (gUserCommand.Cursor == 0) { return; }
    gUserCommand.Cursor--;
    gUserCommand.Buffer[gUserCommand.Cursor] = 0;
    PrintBackspace();
}

BOOL
ConsoleIsReceivingInput(void)
{
    return gUserCommand.IsReceivingInput;
}

VOID
ConsoleSignalEnd(void)
{
    ConsolePutCharacter(0);
    gUserCommand.IsReceivingInput = FALSE;
}


VOID
ConsoleRun(void)
{
    while (TRUE)
    {
        ConsoleReadCommand();
        ConsoleMatchCommand();
    }
}


VOID
ConsoleTrapBreak(void)
{
    PrintFormatString("__debugbreak");
    LogString("~__debugbreak");
    __debugbreak();
}

VOID
ConsoleTimeSleep(void)
{
    PrintFormatString("sleeping 5 seconds");
    LogString("~sleeping 5 seconds");
    TimerSleep(5000);
}

VOID
ConsoleReadDisk(void)
{
    BYTE head = 0;
    BYTE sector = 1;
    UINT16 cylinder = 0;
    
    DiskRead(head, cylinder, sector, (PBYTE)gUserCommand.Buffer);
    LogString("~disk read");
    for (UINT16 i = 0; i < sizeof(gUserCommand.Buffer); i++) { PrintCharacter(gUserCommand.Buffer[i]); }
    __magic(); // TimerSleep(5000);
}

VOID
ConsoleWriteDisk(void)
{
    BYTE head = 0;
    BYTE sector = 1;
    UINT16 cylinder = 0;

    PrintFormatString("input string to write\n");
    ConsoleResetBuffer();
    gUserCommand.IsReceivingInput = TRUE;
    while (ConsoleIsReceivingInput());

    DiskWrite(head, cylinder, sector, (PBYTE)gUserCommand.Buffer);
    LogString("~disk written");
    PrintFormatString("\ndisk written");
    // __magic(); // TimerSleep(2000);
}

VOID
ConsoleTestPhysicalPageAllocators(void)
{
    TestPhysicalPageAllocators();
}

VOID
ConsoleTestVirtualPageAllocators(void)
{
    TestVirtualPageAllocators();
}

VOID
ConsoleTestVirtualByteAllocators(void)
{
    TestVirtualByteAllocators();
}
