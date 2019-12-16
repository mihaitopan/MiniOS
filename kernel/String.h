#ifndef __STRING_H__
#define __STRING_H__
//
//  Author(s) : Mihai TOPAN
//
#include "Types.h"

typedef struct _STRING {
    UINT16 Length;
    PCHAR  Buffer;
} STRING, *PSTRING;

typedef enum _STRING_TYPE {
    typeChar,
    typeString,
    typeNumber,
} STRING_TYPE, *PSTRING_TYPE;

typedef struct _STRING_CONTEXT
{
    STRING_TYPE Type;
    union
    {
        CHAR   Character;
        PCHAR  String;
        UINT64 Number;
    };
} STRING_CONTEXT, *PSTRING_CONTEXT;

typedef VOID(*PFUNC_StringContext)(PSTRING_CONTEXT Context);

VOID
AutoFillFormatString(
    PCHAR FormatString,
    UINT16 FormatStringLength,
    PFUNC_StringContext Callback,
    ...
);

VOID
QwordToString(UINT64 Qword, CHAR String[16]);


#define CONST_STRING(string) {sizeof(string)/sizeof(CHAR), string}

BOOL AreStringsEqual(
    PSTRING String1,
    UINT16 String2_Length,
    PCHAR  String2_Buffer
);

// DOCS: https://code.woboq.org/userspace/glibc/stdio-common/fprintf.c.html
#endif // __STRING_H__