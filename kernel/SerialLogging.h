#ifndef __SERIAL_LOGGING_H__
#define __SERIAL_LOGGING_H__
//
//  Author(s) : Mihai TOPAN
//
#include "Types.h"
#include "String.h"

VOID
InitSerialPort();

VOID
LogString(
    PCHAR String
);

VOID
LogStringContext(
    PSTRING_CONTEXT Context
);

#define LogFormatString(Format, ...) AutoFillFormatString(Format, sizeof(Format), LogStringContext, ## __VA_ARGS__)

// DOCS: https://wiki.osdev.org/Serial_Ports
// DOCS: https://littleosbook.github.io/#configuring-the-serial-port
#endif // __SERIAL_LOGGING_H__