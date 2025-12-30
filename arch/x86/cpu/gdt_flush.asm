BITS 32
global gdt_flush

gdt_flush:
    mov eax, [esp + 4]  ; arg: pointer to gdt_ptr_t
    lgdt [eax]          ; load the new GDT

    ; Reload segment registers with our dat aselector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS with our code selector (0x08)
    jmp 0x08:.flush_cs

.flush_cs:
    ret

section .note.GNU-stack noalloc noexec nowrite progbits

