;;-----------------_DEFINITIONS ONLY_-----------------------
;; IMPORT FUNCTIONS FROM C
%macro IMPORTFROMC 1-*
%rep  %0
    %ifidn __OUTPUT_FORMAT__, win32 ; win32 builds from Visual C decorate C names using _ 
    extern _%1
    %1 equ _%1
    %else
    extern %1
    %endif
%rotate 1 
%endrep
%endmacro

;; EXPORT TO C FUNCTIONS
%macro EXPORT2C 1-*
%rep  %0
    %ifidn __OUTPUT_FORMAT__, win32 ; win32 builds from Visual C decorate C names using _ 
    global _%1
    _%1 equ %1
    %else
    global %1
    %endif
%rotate 1 
%endrep
%endmacro

%define break xchg bx, bx

IMPORTFROMC KernelMain

TOP_OF_STACK                equ 0x200000               ; goes left
KERNEL_BASE_PHYSICAL        equ 0x200000               ; goes right

; PML4T                       equ 0x001000
; PDPT                        equ 0x002000
; PDT                         equ 0x003000
; PT                          equ 0x004000
; PT2                         equ 0x005000
; PT3                         equ 0x006000
; MAX_NO_PAGES                equ 64

STRUC TRAP_FRAME
    .Rax:   resq 1
    .Rbx:   resq 1
    .Rcx:   resq 1
    .Rdx:   resq 1 
    .Rsi:   resq 1 
    .Rdi:   resq 1 
    .R8:    resq 1
    .R9:    resq 1
    .R10:   resq 1
    .R11:   resq 1
    .R12:   resq 1
    .R13:   resq 1
    .R14:   resq 1
    .R15:   resq 1
    .Rbp:   resq 1  
    .Flags: resq 1
    .Size:
ENDSTRUC
;;-----------------^DEFINITIONS ONLY^----------------------- !!!

segment .text
[BITS 32]
ASMEntryPoint:
    cli
    MOV     DWORD [0x000B8000], 'O1S1'
%ifidn __OUTPUT_FORMAT__, win32
    MOV     DWORD [0x000B8004], '3121'                  ; 32 bit build marker (with color 1)
%else
    MOV     DWORD [0x000B8004], '6141'                  ; 64 bit build marker (with color 1)
%endif



    MOV     ESP, TOP_OF_STACK                           ; just below the kernel  (ca sa am loc - va scadea de aici)
    
    ; https://wiki.osdev.org/Setting_Up_Long_Mode
.is_CPUID_supported?:
    ; Check if CPUID is supported by attempting to flip the ID bit(bit 21) in the FLAGS register. If we can flip it, CPUID is available.
    ;break
    pushfd					; Copy FLAGS in to EAX via stack
    pop eax
    mov ecx, eax			; Copy to ECX as well for comparing later on
    xor eax, 1 << 21 		; Flip the ID bit
    push eax				; Copy EAX to FLAGS via the stack
    popfd
    pushfd					; Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
    pop eax
    push ecx				; Restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit back if it was ever flipped).
    popfd
    xor eax, ecx			; Compare EAX and ECX. If they are equal then that means the bit wasn't flipped, and CPUID isn't supported.
    jnz .CPUID_supported
    jmp .CLI_and_HLT 
    
.CPUID_supported:
.is_LongModeFunc_supported?:
    mov eax, 0x80000000    	; Set the A-register to 0x80000000.
    cpuid                  	; CPU identification.
    cmp eax, 0x80000001    	; Compare the A-register with 0x80000001.
    jnb .LongModeFunc_supported
    jmp .CLI_and_HLT	   	; It is less, there is no long mode.

.LongModeFunc_supported:
.is_LongMode_supported?:
    mov eax, 0x80000001    	; Set the A-register to 0x80000001.
    cpuid                  	; CPU identification.
    test edx, 1 << 29      	; Test if the LM-bit, which is bit 29, is set in the D-register.
    jnz .LongMode_supported
    jmp .CLI_and_HLT	   	; They aren't, there is no long mode.

.CLI_and_HLT:
    break
    cli                     ; this should not be reached 
    hlt

.LongMode_supported:
.DisableOldPaging:
    mov eax, cr0       		; Set the A-register to control register 0.
    and eax, 1 << 31 		; Clear the PG-bit, which is bit 31.
    mov cr0, eax        	; Set control register 0 to the A-register.
.SetUpTables:
    mov edi, 0x1000    		
    mov cr3, edi       		; PML4T - 0x1000
    xor eax, eax       		; Nullify the A-register.
    mov ecx, 4096      		; Set the C-register to 4096.
    rep stosd          		; Clear the memory.
    ;break
    
; 1000 (PML4T) -> 2000 (PDPT) -> 3000 (PDT) -> 4000 (PT)  =>       0 - 200.000
; 											-> 5000 (PT2) => 200.000 - 400.000
; 											-> 6000 (PT3) => 400.000 - 600.000
    ; break
    mov edi, cr3       		
    mov DWORD [edi], 0x2003 ; PDPT - 0x2000 -> 3 = first two bits should be set = readable & writable page
    add edi, 0x1000         
    mov DWORD [edi], 0x3003 ; PDT - 0x3000
    ;mov DWORD [edi + 8], 0x4003
    add edi, 0x1000
    
    mov DWORD [edi], 0x4003 ; PT - 0x4000
    mov DWORD [edi + 8], 0x5003 ; PT2 - 0x5000
    mov DWORD [edi + 16], 0x6003 ; PT3 - 0x6000 -> used for heap
    add edi, 0x1000
    
;.SetUpIdentityMapForTheFirstTwoMegabytes:
    mov ebx, 0x00000003     ; Set the B-register to 0x00000003.
    mov ecx, 512            ; how many times to loop
.SetEntry:
    mov DWORD [edi], ebx    ; Set the uint32_t at the destination index to the B-register.
    add ebx, 0x1000         ; Add 0x1000 to the B-register.
    add edi, 8              ; Add eight to the destination index.
    loop .SetEntry          ; Set the next entry.
    
    ;break
    ;mov DWORD [edi], 0x5003 ; PT2 - 0x5000
    ;add edi, 0x1000      
;.SetUpIdentityMapForTheSecondTwoMegabytes:
    mov ebx, 0x00200007     ; Set the B-register to 0x00000003.
    mov ecx, 512            ; how many times to loop
.SetEntry2:
    mov DWORD [edi], ebx    ; Set the uint32_t at the destination index to the B-register.
    add ebx, 0x1000         ; Add 0x1000 to the B-register.
    add edi, 8              ; Add eight to the destination index.
    loop .SetEntry2         ; Set the next entry.	

    ; break
;.SetUpIdentityMapForTheThirdTwoMegabytes:
    mov ebx, 0x00400002     ; page NOT present - will be put present after
    mov ecx, 512            ; how many times to loop
.SetEntry3:
    mov DWORD [edi], ebx    ; Set the uint32_t at the destination index to the B-register.
    add ebx, 0x1000         ; Add 0x1000 to the B-register.
    add edi, 8              ; Add eight to the destination index.
    loop .SetEntry3         ; Set the next entry.	

.EnablePAE:
    ; break
    mov eax, cr4            ; Set the A-register to control register 4.
    or eax, 1 << 5          ; Set the PAE-bit, which is the 6th bit (bit 5).
    mov cr4, eax            ; Set control register 4 to the A-register.
.EnableCompatibilityMode:
    mov ecx, 0xC0000080     ; Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                   ; Read from the model-specific register.
    or eax, 1 << 8          ; Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                   ; Write to the model-specific register.	
.EnablePaging:
    mov eax, cr0            ; Set the A-register to control register 0.
    or eax, 1 << 31 | 1 << 0; enable paging (bit 31) and protected mode (bit 0)
    mov cr0, eax            ; Set control register 0 to the A-register.	
.LoadDescriptorTable:
    ;break
    lgdt [GDT64.Pointer]    ; Load the 64-bit global descriptor table.
    jmp GDT64.Code:Realm64  ; Set the code segment and enter 64-bit long mode.

[BITS 64] 
Realm64:
    cli                     ; Clear the interrupt flag.
    mov ax, GDT64.Data      ; Set the A-register to the data descriptor.
    mov ds, ax              ; Set the data segment to the A-register.
    mov es, ax              ; Set the extra segment to the A-register.
    mov fs, ax              ; Set the F-segment to the A-register.
    mov gs, ax              ; Set the G-segment to the A-register.
    mov ss, ax              ; Set the stack segment to the A-register.
    mov edi, 0xB8000        ; Set the destination index to 0xB8000.
    mov rax, 0x1F201F201F201F20   ; Set the A-register to 0x1F201F201F201F20.
    mov ecx, 500            ; Set the C-register to 500.
    rep stosq               ; Clear the screen.
    
__enableSSE:                ;; enable SSE instructions (CR4.OSFXSR = 1)
    ;break
    MOV     RAX, CR4
    OR      RAX, 0x00000200
    MOV     CR4, RAX

    ;break
    mov		rax, KernelMain
    call	rax
    break
    cli                     ; this should not be reached 
    hlt

;;--------------------------------------------------------

__cli:
    CLI
    RET

__sti:
    STI
    RET

__magic:
    XCHG    BX,BX
    RET
    
__halt:
    XCHG    BX,BX
    RET
    cli                     ;; this should never be reached 
    hlt

__inb:
    mov     DX, CX
    in      AL, DX
    ret

__outb:
    mov     RAX, RDX
    mov     RDX, RCX
    out     DX, AL
    ret

__interlock_increment:
    lock inc QWORD [rcx]
    ret

EXPORT2C ASMEntryPoint, __cli, __sti, __magic, __halt, __inb, __outb, __interlock_increment

GDT64:                           ; Global Descriptor Table (64-bit).
    .Null: equ $ - GDT64         ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 1                         ; Granularity.
    db 0                         ; Base (high).
    .Code: equ $ - GDT64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .Data: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .Pointer:                    ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.


;;-----------------_INTERRUPTS AND TRAPFRAME_-----------------------
%macro SAVE_CONTEXT 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rbp
    pushf
%endmacro

%macro RESUME_CONTEXT 0
    popf
    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro FILL_UP_TRAP_FRAME 1
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.Rax], rax
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.Rbx], rbx
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.Rcx], rcx
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.Rdx], rdx
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.Rsi], rsi
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.Rdi], rdi
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R8],  r8
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R9],  r9
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R10], r10
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R11], r11
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R12], r12
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R13], r13
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R14], r14
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.R15], r15
    mov [%1 + TRAP_FRAME.Size + TRAP_FRAME.Rbp], rbp
    pushf
    pop QWORD [%1 + TRAP_FRAME.Size + TRAP_FRAME.Flags]
%endmacro

extern DumpTrapFrame

%macro DEFINE_ISR_EXCEPTION 1
global isr_%1
isr_%1:
    cli
    sub rsp, TRAP_FRAME.Size 	; reserve space for trap frame

    FILL_UP_TRAP_FRAME rsp		; fill trapframe onto stack
    mov rcx, rsp
    add rcx, TRAP_FRAME.Size	; get a pointer to the trapframe on stack
    mov rdx, %1
    call DumpTrapFrame

    add rsp, TRAP_FRAME.Size	; restore stack
    sti
    iretq
%endmacro

%macro DEFINE_ISR_MASTER_PIC_HANDLER 2
extern %2
global %1
%1:
    cli
    SAVE_CONTEXT
    
    call %2
    
    RESUME_CONTEXT
    sti
    iretq
%endmacro

DEFINE_ISR_EXCEPTION 0
DEFINE_ISR_EXCEPTION 1
DEFINE_ISR_EXCEPTION 2
DEFINE_ISR_EXCEPTION 3
DEFINE_ISR_EXCEPTION 4
DEFINE_ISR_EXCEPTION 5
DEFINE_ISR_EXCEPTION 6
DEFINE_ISR_EXCEPTION 7
DEFINE_ISR_EXCEPTION 8
DEFINE_ISR_EXCEPTION 9
DEFINE_ISR_EXCEPTION 10
DEFINE_ISR_EXCEPTION 11
DEFINE_ISR_EXCEPTION 12
DEFINE_ISR_EXCEPTION 13
DEFINE_ISR_EXCEPTION 14
DEFINE_ISR_EXCEPTION 16
DEFINE_ISR_EXCEPTION 17
DEFINE_ISR_EXCEPTION 18
DEFINE_ISR_EXCEPTION 19
DEFINE_ISR_EXCEPTION 20

DEFINE_ISR_MASTER_PIC_HANDLER IsrTimer, IsrTimerEvent
DEFINE_ISR_MASTER_PIC_HANDLER IsrKeyboard, IsrKeyboardEvent


global __read_disk_sector
__read_disk_sector:
    SAVE_CONTEXT
    mov rdi, rcx                ; to read 256 words = 1 sector
    mov rcx, 256                ; RCX is counter for INSW
    mov rdx, 1F0h               ; Data port, in and out
    rep insw                    ; in to [RDI]
    RESUME_CONTEXT
    ret

global __write_disk_sector
__write_disk_sector:
    SAVE_CONTEXT
    mov rsi, rcx                ; to write 256 words = 1 sector
    mov rcx, 256                ; RCX is counter for INSW
    mov rdx, 1F0h               ; Data port, in and out
    rep outsw                   ; out from [RSI]
    RESUME_CONTEXT
    ret

global __alloc_page
__alloc_page:
    SAVE_CONTEXT
    mov rbx, rcx				; page index
    shl rbx, 3					; page index size in PT3 is 8bits
    add rbx, 0x6000				; PT3 (heap) base address
    mov rdi, rbx
    
    mov rbx, rcx				; page index
    shl rbx, 12					; page size is 4096
    add rbx, 0x00400003			; physical heap base address ; page present
    mov DWORD [edi], ebx
    invlpg [rbx]
    RESUME_CONTEXT
    ret

global __free_page
__free_page:
    SAVE_CONTEXT
    mov rbx, rcx				; page index
    shl rbx, 3					; page index size in PT3 is 8bits
    add rbx, 0x6000				; PT3 (heap) base address
    mov rdi, rbx
    
    mov rbx, rcx				; page index
    shl rbx, 12					; page size is 4096
    add rbx, 0x00400002			; physical heap base address ; page NOT present
    mov DWORD [edi], ebx
    invlpg [rbx]
    RESUME_CONTEXT
    ret

global __invlpg
__invlpg:
    SAVE_CONTEXT
    shl rcx, 12
    add rcx, 0x00400000			; heap base address
    invlpg [rcx]
    RESUME_CONTEXT
    ret
