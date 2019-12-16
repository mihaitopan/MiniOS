%define break xchg bx, bx

KERNEL_SECTORS equ 72 ; 72 ; 55 ; 50

[org 0x7E00]
[bits 16]
SSL:
	; break
	; memory from 0x7E00 - 0x9200 used by SLL
	mov    ax,    0x1000
	mov	   es,    ax				; es:bx -> es * 16 + bx
    mov    bx,    00h           	; from 0x10000, KERNEL_SECTORS are dedicated to our kernel
	; https://stackoverflow.com/questions/3645803/bios-int-13h-with-ah-2-can-only-read-72-sectors-each-time-why
	; http://members.tripod.com/vitaly_filatov/ng/asm/asm_024.3.html
	
    mov    ah,    02h               ; parameters for calling int13 (ReadSectors) - read the Kernel
    mov    al,    KERNEL_SECTORS    ; read KERNEL_SECTORS sectors (hardcoded space for Kernel)
    mov    ch,    00h     
    mov    cl,    0Bh               ; starting from sector 11 - skip first 10 sectors (the MBR + SSL)
    mov    dh,    00h
	mov    dl,    00h
    int    13h    
	; break
    jnc    .success        
	jmp .CLI_and_HLT                    
 
.success:                      
    cli                    ; starting RM to PM32 transition
    lgdt   [GDT]
    mov    eax,    cr0
    or     al,     1
    mov    cr0,    eax     ; we are in protected mode but we need to set the CS register  
    jmp    8:.bits32       ; we change the CS to 8 (index of FLAT_DESCRIPTOR_CODE32 entry) (daca nu suntem in protected mode)

.bits32:
[bits 32]
    mov    ax,    16       ; index of FLAT_DESCRIPTOR_DATA32 entry
    mov    ds,    ax
    mov    es,    ax      
    mov    gs,    ax      
    mov    ss,    ax      
    mov    fs,    ax 
    
    cld
    mov    ecx,    KERNEL_SECTORS*512  ; KERNEL_SECTORS * sector_size
    mov    esi,    0x10000             ; source
    mov    edi,    0x200000            ; destination
    rep    movsb                       ; copy the kernel from 0x10000 to 0x200000
    
    mov    [ds:0xb8000], BYTE 'O'
    mov    [ds:0xb8002], BYTE 'K'
    ;break
    
.is_A20_on?:           ; legacy shit (16bit) 
                      ; -> erau doar 19 la adresare, s-a extins la 20 si trebuia activat ; bochs o activeaza automat ; cod pe OSR 
    pushad
    mov edi,0x112345  ;odd megabyte address.
    mov esi,0x012345  ;even megabyte address.
    mov [esi],esi     ;making sure that both addresses contain diffrent values.
    mov [edi],edi     ;(if A20 line is cleared the two pointers would point to the address 0x012345 that would contain 0x112345 (edi)) 
    cmpsd             ;compare addresses to see if the're equivalent.
    popad
    jne .A20_on        ;if not equivalent , A20 line is set.
	jmp .CLI_and_HLT 
 
.A20_on:
	;break
    mov    eax, 0x201000    ; the hardcoded ASMEntryPoint of the Kernel ; prima sectiune sa inceapa la 1000h
	                        ; MultiBoot face transparent pentru noi (poate si hardcodarea)
    call   eax
	
.CLI_and_HLT:
	break
	cli                     ; this should not be reached 
    hlt

;
; Data
;
GDT:
    .limit  dw  GDTTable.end - GDTTable - 1
    .base   dd  GDTTable

FLAT_DESCRIPTOR_CODE32  equ 0x00CF9A000000FFFF  ; Code: Execute/Read              ; A - 2 - B - 3 se schimba biti pentru 32/64/..
FLAT_DESCRIPTOR_DATA32  equ 0x00CF92000000FFFF  ; Data: Read/Write                ; 00 - base ; CF si FF - maxim
FLAT_DESCRIPTOR_CODE16  equ 0x00009B000000FFFF  ; Code: Execute/Read, accessed
FLAT_DESCRIPTOR_DATA16  equ 0x000093000000FFFF  ; Data: Read/Write, accessed

; de la _base_ la _base_+_size_ o zona e descrisa de un descriptor -> pentru securitate; modelul FLAT: de la 0 la SIZE_MAX
; CS(8) descrie ca toata memoria fiind 8; DS -> tot toata memoria (pentru date)
; adresa se calculeaza altfel decat la assembly (CS:IP) SSSS0 + offset (ca sa accesez memorie fizica)
; in modul protejat adresa e o adresa virtuala
; pe 32bit CS:EIP -> _base_descriptor_(0) + EIP
; ca sa trecem in 64 pornim de la protected32
; READ ABOUT PROTECTED AND REAL MODE ON 16BIT (despre protejat si real...)
; because of the pagination translation, windows, linux, .. tell that CS, DS, .. start from 0 and end at 4gb and question just eip
; trebuie sa pornesc paginarea
; 32biti: 10 + 10 + 12
; primii 10: page directory PDBR (page directory base register) - CR3 register - tells us where it starts
; alt tabel cu 32; primii 20 duc in PT (page table) - urmatorii sunt considerati 0 - se folosesc ca si flag-uri cu informatii despre pagina
; urmatorii 10 sunt indexare in PT (care are la fel 20 si 10) ; cei 20 duc in memoria fizica
; ultimii 12 sunt indecsi in fizica
; ultima pagina are 2MB: un singur tabel (un flag seteaza ca e ultima pagina) si 22biti offset
; pentru 64 !!! ot folosi bitul si pagini uriase ca sa nu fac multe tabele :)

; pentru 64 tot asa jmp (far cu segment si offset) dar setam tot un bit din alt registru
    
GDTTable:
    .null     dq 0                         ;  0
    .code32   dq FLAT_DESCRIPTOR_CODE32    ;  8
    .data32   dq FLAT_DESCRIPTOR_DATA32    ; 16
    .code16   dq FLAT_DESCRIPTOR_CODE16    ; 24
    .data16   dq FLAT_DESCRIPTOR_DATA16    ; 32 ; TODO!!! add 64-bits descriptors
    .end:
