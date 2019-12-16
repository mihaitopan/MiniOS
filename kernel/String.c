#include "String.h"
#include "Util.h"
#include "VideoMemory.h"


VOID
AutoFillFormatString(
    PCHAR  FormatString,
    UINT16 FormatStringLength,
    PFUNC_StringContext Callback,
    ...
)
{
#pragma warning(suppress:4047) // iterating byte by byte
    PBYTE* current = &Callback + 1;
    STRING_CONTEXT ctx = { 0 };
    
    UINT16 i = 0;
    BOOL isSpecifier = FALSE;
    while (i < FormatStringLength)
    {
        if (!isSpecifier)
        {
            switch (FormatString[i])
            {
            case '%':
                isSpecifier = TRUE;
                break;
            default:
                ctx.Type = typeChar;
                ctx.Character = FormatString[i];
                Callback(&ctx);
                break;
            }
        }
        else
        {
            switch (FormatString[i])
            {
            case 's':
                ctx.Type = typeString;
                ctx.String = *((PCHAR*)current);
                Callback(&ctx);
                current++;
                break;
            case 'x':
                ctx.Type = typeNumber;
                ctx.Number = *((PUINT64)current);
                Callback(&ctx);
                current++;
                break;
            }
            isSpecifier = FALSE;
        }
        i++;
    }
}

VOID
QwordToString(
    UINT64 Qword, 
    CHAR String[16]
)
{
    UINT16 i = 16;
    while (Qword)
    {
        BYTE rem = Qword % 0x10;
        Qword = Qword / 0x10;

        if (rem >= 0xA)
        {
            String[i - 1] = rem - 0xA + 'A';
        }
        else
        {
            String[i - 1] = rem + '0';
        }
        i--;
    }

    while (i)
    {
        String[i - 1] = '0';
        i--;
    }
}

BOOL
AreStringsEqual(
    PSTRING String1, 
    UINT16 String2_Length, 
    PCHAR String2_BufferString2
)
{
    if (!String1 || 0 == String2_Length || !String2_BufferString2)
    {
        return FALSE;
    }

    if (String1->Length != String2_Length)
    {
        return FALSE;
    }

    for (UINT16 i = 0; i < String1->Length; ++i)
    {
        if (String1->Buffer[i] != String2_BufferString2[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}
