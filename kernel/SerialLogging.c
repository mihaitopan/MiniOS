#include "SerialLogging.h"
#include "Util.h"

#define COM_1   0x3f8
#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)
#define SERIAL_LINE_ENABLE_DLAB         0x80

VOID
InitSerialPort()
{
    __outb(SERIAL_LINE_COMMAND_PORT(COM_1), SERIAL_LINE_ENABLE_DLAB);
    __outb(SERIAL_DATA_PORT(COM_1), (1 >> 8) & 0x00FF); // speed is 115200 bits/s
    __outb(SERIAL_DATA_PORT(COM_1), 1 & 0x00FF);

    __outb(SERIAL_LINE_COMMAND_PORT(COM_1), 0x03);
    __outb(SERIAL_FIFO_COMMAND_PORT(COM_1), 0xc7);
    __outb(SERIAL_MODEM_COMMAND_PORT(COM_1), 0x03);

    //__outb(COM_1 + 1, 0x00);
    //__outb(COM_1 + 3, 0x80);
    //__outb(COM_1 + 0, 0x01);
    //__outb(COM_1 + 1, 0x00);
    //__outb(COM_1 + 3, 0x03);
    //__outb(COM_1 + 2, 0xc7);
    //__outb(COM_1 + 4, 0x0b);
}

VOID
WaitForTransmisionReady()
{
    //while (0 == (__inb(COM_1 + 5) & 0x20));
    while (0 == (__inb(SERIAL_LINE_STATUS_PORT(COM_1)) & 0x20));
}

VOID
SerialLoggingSendByte(
    BYTE Data
)
{
    WaitForTransmisionReady();
    // __magic(); // out 0x65 -> 0x5 WTF
    __outb(COM_1, Data);
}


VOID LogString(
    PCHAR String
)
{
    UINT16 i = 0;
    while (String[i] != '\0')
    {
        SerialLoggingSendByte(String[i]);
        i++;
    }
    SerialLoggingSendByte('\0');
}

VOID
LogStringContext(
    PSTRING_CONTEXT Context
)
{
    UINT16 i = 0;
    CHAR stringToLog[16] = { 0 };
    switch (Context->Type)
    {
    case typeChar:
        SerialLoggingSendByte(Context->Character);
        break;
    case typeString:
        while (Context->String[i] != '\0')
        {
            SerialLoggingSendByte(Context->String[i]);
            i++;
        }
        break;
    case typeNumber:
        QwordToString(Context->Number, stringToLog);
        for (i = 0; i < 16; i++)
        {
            SerialLoggingSendByte(stringToLog[i]);
        }
        break;
    }
}
