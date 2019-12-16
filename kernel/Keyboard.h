#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
//
//  Author(s) : Mihai TOPAN
//
#include "Types.h"

#define KEYBOARD_PORT 0x60

void IsrKeyboardEvent(void);

VOID
KeyboardHandleKey(
    CHAR Code
);

// DOCS: https://wiki.osdev.org/PS2_Keyboard
#endif // __KEYBOARD_H__