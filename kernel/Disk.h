#ifndef __DISK_H__
#define __DISK_H__
//
//  Author(s) : Mihai TOPAN
//
#include "Types.h"

// PIO mode ATA
// CHS mode
#define DSK_REGISTER_COMMAND 0x1F7
// buffer size = 512
#define DSK_COMMAND_READ 0x20
#define DSK_COMMAND_WRITE 0x30

void
DiskSendCommand(
    BYTE   Head,
    UINT16 Cylinder,
    BYTE   Sector,
    BYTE   Command
);

void
DiskRead(
    BYTE   Head,
    UINT16 Cylinder,
    BYTE   Sector,
    PBYTE  Buffer
);

void
DiskWrite(
    BYTE   Head,
    UINT16 Cylinder,
    BYTE   Sector,
    PBYTE  Buffer
);

// DOCS: https://wiki.osdev.org/ATA_PIO_Mode
// DOCS: https://wiki.osdev.org/ATA_read/write_sectors
// HELP: bszabo_CHS.txt
#endif // __DISK_H__