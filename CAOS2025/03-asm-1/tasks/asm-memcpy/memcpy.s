.intel_syntax noprefix

.text
.global my_memcpy

my_memcpy:
    xor rcx, rcx

loop_start:
    cmp edx, 8
    jl validate_rcx
    mov rax, [rsi + rcx * 8]
    mov [rdi + rcx * 8], rax
    sub edx, 8
    add rcx, 1
    jmp loop_start
validate_rcx:
    shl rcx, 3
    jmp loop_cp_block
loop_cp_block:
    cmp edx, 0
    jle loop_end
    mov al, [rsi + rcx]
    mov [rdi + rcx], al
    add rcx, 1
    sub edx, 1
    jmp loop_cp_block

loop_end:
    mov rax, rdi
    ret
