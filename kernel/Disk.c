#include "Disk.h"
#include "Util.h"


void
DiskSendCommand(
    BYTE   Head,
    UINT16 Cylinder,
    BYTE   Sector,
    BYTE   Command
)
{
    __outb(0x1F6, (Head | 0b10100000));     // Master DRV | CHS mode 
    __outb(0x1F2, 1);                       // sector count = 1 sector
    __outb(0x1F3, Sector);                  // sector number = Sector
    __outb(0x1F4, ((PBYTE)&Cylinder)[0]);   // Cylinder low port
    __outb(0x1F5, ((PBYTE)&Cylinder)[1]);   // Cylinder high port
    __outb(DSK_REGISTER_COMMAND, Command);

    while (TRUE)
    {
        BYTE retVal = __inb(DSK_REGISTER_COMMAND); // wait for disk to be ready
        if (retVal & 8)
            break;
    }
}

void
DiskRead(
    BYTE   Head,
    UINT16 Cylinder,
    BYTE   Sector,
    PBYTE  Buffer
)
{
    DiskSendCommand(Head, Cylinder, Sector, DSK_COMMAND_READ);
    __read_disk_sector(Buffer);
}

void
DiskWrite(
    BYTE   Head,
    UINT16 Cylinder,
    BYTE   Sector,
    PBYTE  Buffer
)
{
    DiskSendCommand(Head, Cylinder, Sector, DSK_COMMAND_WRITE);
    __write_disk_sector(Buffer);
}
