#include "Keyboard.h"
#include "Util.h"
#include "Pic.h"
#include "VideoMemory.h"
#include "SerialLogging.h"
#include "Console.h"


void IsrKeyboardEvent(void)
{
    BYTE key = __inb(KEYBOARD_PORT);
    KeyboardHandleKey(key);
    __outb(PIC1, PIC_EOI);
}

static const char gDigits[] = "1234567890";
static const char gLettersQP[] = "qwertyuiop";
static const char gLettersAL[] = "asdfghjkl";
static const char gLettersZM[] = "zxcvbnm";


BOOL
KeyboardIsKeyReleased(
    CHAR Code
)
{
    return (Code & 0x80);
}

VOID
KeyboardHandleKey(
    CHAR Code
)
{
    CHAR character = 0;
    if (!ConsoleIsReceivingInput() || KeyboardIsKeyReleased(Code))
    {
        return;
    }
    else
    {
        if (Code >= 0x02 && Code <= 0x0B)       // 1234567890
        {
            character = gDigits[Code - 0x02];
        }
        else if (Code >= 0x10 && Code <= 0x19)  // QWERTYUIOP
        {
            character = gLettersQP[Code - 0x10];
        }
        else if (Code >= 0x1E && Code <= 0x26)  // ASDFGHJKL
        {
            character = gLettersAL[Code - 0x1E];
        }
        else if (Code >= 0x2C && Code <= 0x32)  // ZXCVBNM
        {
            character = gLettersZM[Code - 0x2C];
        }
        else if (Code == 0x39)                  // Space
        {
            character = ' ';
        }
        else if (Code == 0x1C)                  // Enter
        {
            ConsoleSignalEnd();
        }
        else if (Code == 0xE)                   // Backspace
        {
            ConsoleBackspace();
            return;
        }
        else
        {
            return;
        }
    }

    ConsolePutCharacter(character);
}
