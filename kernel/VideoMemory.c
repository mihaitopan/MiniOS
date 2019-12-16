#include "VideoMemory.h"
#include "SerialLogging.h"
#include "Util.h"

static SCREEN gScreen = { (PIXEL*)(0xB8000), 0 };


VOID
ClearScreen()
{
    UINT16 i = 0;
    for (i = 0; i < MAX_COLUMNS * MAX_LINES; i++)
    {
        gScreen.VideoMemory[i].Character = 0;
    }
    gScreen.Cursor = 0;
}

VOID
PrintCharacter(
    CHAR Character
)
{
    if (Character == '\n')
    {
        gScreen.Cursor += (MAX_COLUMNS - gScreen.Cursor % MAX_COLUMNS);
        return;
    }

    gScreen.VideoMemory[gScreen.Cursor].Character = Character;
    gScreen.Cursor++;
}

VOID
PrintBackspace()
{
    if (0 == gScreen.Cursor) { return; }
    gScreen.Cursor--;
    gScreen.VideoMemory[gScreen.Cursor].Character = 0;
}

VOID
PrintStringContext(
    PSTRING_CONTEXT Context
)
{
    UINT16 i = 0;
    CHAR stringToPrint[16] = { 0 };
    switch (Context->Type)
    {
    case typeChar:
        PrintCharacter(Context->Character);
        break;
    case typeString:
        while (Context->String[i] != '\0')
        {
            PrintCharacter(Context->String[i]);
            i++;
        }
        break;
    case typeNumber:
        QwordToString(Context->Number, stringToPrint);
        for (i = 0; i < 16; i++)
        {
            PrintCharacter(stringToPrint[i]);
        }
        break;
    }
}
