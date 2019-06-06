[extern isr_handler] ; Defined in isr.h
[extern irq_handler] ; Defined in isr.h
[extern irq_multitask_timer_handler]

[global irq_multitask_timer_stub]

isr_common_stub:
    ; 1. Save CPU state
	pusha           ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds      ; Lower 16-bits of eax = ds.
	push eax        ; save the data segment descriptor
	mov ax, 0x10    ; kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
    ; 2. Call C handler
	call isr_handler
	
    ; 3. Restore state
	pop eax 
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa        ; Pop edi,esi,ebp,esp,ebx,edx,ecx,eax
	add esp, 8  ; Cleans up the pushed error code and pushed ISR number
	sti
	iret        ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

irq_common_stub:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler ; Different than the ISR code

    pop ebx          ; Different than the ISR code
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    sti
    iret

irq_multitask_timer_stub:
    pushad
    push esp
    call irq_multitask_timer_handler
    pop esp
    popad
    iret

%macro ISR_WITHOUT_ERRCODE 1
    [global isr%1]
    isr%1:
        push byte 0
        push byte %1
        jmp isr_common_stub
%endmacro

%macro ISR_WITH_ERRCODE 1
    [global isr%1]
    isr%1:
        push byte %1
        jmp isr_common_stub
%endmacro

%macro IRQ 1
    [global irq%1]
    irq%1:
        push byte 0
        push byte %1
        jmp irq_common_stub
%endmacro
	
; Code generation

%assign n 0
%rep 8 - n
    ISR_WITHOUT_ERRCODE n
    %assign n n + 1
%endrep

ISR_WITH_ERRCODE    8
ISR_WITHOUT_ERRCODE 9

%assign n 10
%rep 15 - n
    ISR_WITH_ERRCODE n
    %assign n n + 1
%endrep

%rep 32 - n
    ISR_WITHOUT_ERRCODE n
    %assign n n + 1
%endrep

%rep 48 - n
    IRQ n
    %assign n n + 1
%endrep
