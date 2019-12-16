#ifndef __VIDEO_MEMORY_H__
#define __VIDEO_MEMORY_H__
//
//  Author(s) : Mihai TOPAN
//
#include "String.h"

#define MAX_LINES       0x25
#define MAX_COLUMNS     0x50

#pragma pack(push, 1)
typedef struct _PIXEL
{
    CHAR Character;
    BYTE Colour;
} PIXEL, *PPIXEL;

typedef struct _SCREEN
{
    PIXEL* VideoMemory;
    UINT16 Cursor;
} SCREEN, *PSCREEN;
#pragma pack(pop)

VOID
ClearScreen();

VOID
PrintCharacter(
    CHAR Character
);

VOID
PrintBackspace();

VOID
PrintStringContext(
    PSTRING_CONTEXT Context
);

#define PrintFormatString(str, ...) AutoFillFormatString(str, (sizeof(str) - sizeof(char)), PrintStringContext, ##__VA_ARGS__)

// DOCS: https://wiki.osdev.org/Printing_to_Screen
#endif // __VIDEO_MEMORY_H__