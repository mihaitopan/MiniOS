#include "Interrupts.h"
#include "Util.h"
#include "SerialLogging.h"
#include "VideoMemory.h"
#include "Timer.h"


// Exception handlers
extern void isr_0(void);            // 0x00 - Divide By Zero
extern void isr_1(void);            // 0x01 - Debug
extern void isr_2(void);            // 0x02 - Non-Maskable Interrupt
extern void isr_3(void);            // 0x03 - Breakpoint
extern void isr_4(void);            // 0x04 - Overflow
extern void isr_5(void);            // 0x05 - Bound Range Exceeded
extern void isr_6(void);            // 0x06 - Invalid Opcode
extern void isr_7(void);            // 0x07 - Device Not Available
extern void isr_8(void);            // 0x08 - Double Fault
extern void isr_9(void);            // 0x09 - Coprocessor Segment Overrun (Old - out of use)
extern void isr_10(void);           // 0x0A - Invalid TTS
extern void isr_11(void);           // 0x0B - Segment Not Present
extern void isr_12(void);           // 0x0C - Stack Segment Fault
extern void isr_13(void);           // 0x0D - General Protection Fault
extern void isr_14(void);           // 0x0E - Page Fault
extern void isr_16(void);           // 0x0F - x87 Floating-Point Exception
extern void isr_17(void);           // 0x10 - Allignment Check
extern void isr_18(void);           // 0x11 - Machine Check
extern void isr_19(void);           // 0x12 - SIMD Floating Point Exception
extern void isr_20(void);           // 0x13 - Virtualization Exception

// Timer and Keyboard handlers
extern void IsrTimer(void);
extern void IsrKeyboard(void);

static IDT_ENTRY gIdtEntries[256] = { 0 };
static IDT_DESCRIPTOR gIdt = { 0 };


VOID
InitializeIdtEntry(PIDT_ENTRY IdtEntry, PVOID InterruptHandler)
{
    IdtEntry->Selector = 0x08;
    IdtEntry->IST = 0;
    IdtEntry->Attributes = 0x8E;
    IdtEntry->OffsetLow = (UINT16)InterruptHandler;
    IdtEntry->OffsetMid = (UINT16)((UINT64)InterruptHandler >> 16);
    IdtEntry->OffsetHigh = (UINT16)((UINT64)InterruptHandler >> 32);
}

VOID
InitializeIdtEntries()
{
    InitializeIdtEntry(&gIdtEntries[0], isr_0);
    InitializeIdtEntry(&gIdtEntries[1], isr_1);
    InitializeIdtEntry(&gIdtEntries[2], isr_2);
    InitializeIdtEntry(&gIdtEntries[3], isr_3);
    InitializeIdtEntry(&gIdtEntries[4], isr_4);
    InitializeIdtEntry(&gIdtEntries[5], isr_5);
    InitializeIdtEntry(&gIdtEntries[6], isr_6);
    InitializeIdtEntry(&gIdtEntries[7], isr_7);
    InitializeIdtEntry(&gIdtEntries[8], isr_8);
    InitializeIdtEntry(&gIdtEntries[9], isr_9);
    InitializeIdtEntry(&gIdtEntries[10], isr_10);
    InitializeIdtEntry(&gIdtEntries[11], isr_11);
    InitializeIdtEntry(&gIdtEntries[12], isr_12);
    InitializeIdtEntry(&gIdtEntries[13], isr_13);
    InitializeIdtEntry(&gIdtEntries[14], isr_14);
    InitializeIdtEntry(&gIdtEntries[16], isr_16);
    InitializeIdtEntry(&gIdtEntries[17], isr_17);
    InitializeIdtEntry(&gIdtEntries[18], isr_18);
    InitializeIdtEntry(&gIdtEntries[19], isr_19);
    InitializeIdtEntry(&gIdtEntries[20], isr_20);
    
    InitializeIdtEntry(&gIdtEntries[32], IsrTimer);
    InitializeIdtEntry(&gIdtEntries[33], IsrKeyboard);
}


VOID
EnableInterrupts()
{
    unsigned char a1, a2;

    InitializeIdtEntries();

    a1 = __inb(PIC1_DATA);            // save masks
    a2 = __inb(PIC2_DATA);

    __outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    __outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    __outb(PIC1_DATA, 0x20);          // ICW2: Master PIC vector offset
    __outb(PIC2_DATA, 0x28);          // ICW2: Slave PIC vector offset
    __outb(PIC1_DATA, 4);             // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    __outb(PIC2_DATA, 2);             // ICW3: tell Slave PIC its cascade identity (0000 0010)

    __outb(PIC1_DATA, ICW4_8086);
    __outb(PIC2_DATA, ICW4_8086);

    __outb(PIC1_DATA, a1);            // restore saved masks.
    __outb(PIC2_DATA, a2);

    __outb(PIC1_DATA, 0xFC);          // timer & keyboard
    __outb(PIC2_DATA, 0xFF);          // slave completely masked

    gIdt.Base = (UINT64)gIdtEntries;
    gIdt.Limit = (sizeof(IDT_ENTRY) * 34) - 1;
    __lidt((PVOID)&gIdt);
    
    TimerInit();
    LogString("~Timer enabled");
    PrintFormatString("Timer enabled\n");

    __sti();
}

// stdcall
VOID
DumpTrapFrame(
    PTRAP_FRAME TrapFrame, 
    UINT64 InterruptIndex
)
{
    LogString("~Exception occurred");

    PrintFormatString("\n--- EXCEPTION OCCURRED ---\n"
        "Exception Code: 0x%x\n"
        "RAX = 0x%x    RBX = 0x%x\n"  
        "RCX = 0x%x    RDX = 0x%x\n"
        "RSI = 0x%x    RDI = 0x%x\n"
        "R8  = 0x%x    R9  = 0x%x\n"
        "R10 = 0x%x    R11 = 0x%x\n"
        "R12 = 0x%x    R13 = 0x%x\n"
        "R14 = 0x%x    R15 = 0x%x\n"
        "RBP = 0x%x    FLAGS = 0x%x",
        InterruptIndex,
        TrapFrame->RAX,
        TrapFrame->RBX,
        TrapFrame->RCX,
        TrapFrame->RDX,
        TrapFrame->RSI,
        TrapFrame->RDI,
        TrapFrame->R8,
        TrapFrame->R9,
        TrapFrame->R10,
        TrapFrame->R11,
        TrapFrame->R12,
        TrapFrame->R13,
        TrapFrame->R14,
        TrapFrame->R15,
        TrapFrame->RBP,
        TrapFrame->Flags);
    __halt();
}
