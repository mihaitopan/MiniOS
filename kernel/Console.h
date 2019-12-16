#ifndef __CONSOLE_H__
#define __CONSOLE_H__
//
//  Author(s) : Mihai TOPAN
//
#include "Types.h"
#include "VideoMemory.h"

#pragma pack(push, 1)
typedef struct _USER_COMMAND {
    volatile BOOL IsReceivingInput;
    CHAR          Buffer[MAX_LINES * MAX_COLUMNS];
    UINT16        Cursor;
} USER_COMMAND, *PUSER_COMMAND;
#pragma pack(pop)

VOID
ConsolePrintHelp(void);

VOID
ConsoleReadCommand(void);

VOID
ConsoleMatchCommand(void);

VOID
ConsoleResetBuffer(void);

VOID
ConsolePutCharacter(
    CHAR Character
);

VOID
ConsoleBackspace(void);

BOOL
ConsoleIsReceivingInput(void);

VOID
ConsoleSignalEnd(void);

VOID
ConsoleRun(void);

VOID
ConsoleTrapBreak(void);

VOID
ConsoleTimeSleep(void);

VOID
ConsoleReadDisk(void);

VOID
ConsoleWriteDisk(void);

VOID
ConsoleTestPhysicalPageAllocators(void);

VOID
ConsoleTestVirtualPageAllocators(void);

VOID
ConsoleTestVirtualByteAllocators(void);

#endif // __CONSOLE_H__