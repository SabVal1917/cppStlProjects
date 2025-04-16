.intel_syntax noprefix

.text
.global dot_product

dot_product:
    mov rcx, rdi
    mov rax, rdi
    vxorps ymm0, ymm0, ymm0
    vxorps xmm1, xmm1, xmm1
    shr rcx, 3
    cmp rcx, 0
    jle calc_inter

ord_block:
    cmp rcx, 0
    jle calc_inter
    vmovups ymm1, [rsi]
    vmovups ymm2, [rdx]
    add rsi, 32
    add rdx, 32
    vmulps ymm2, ymm1, ymm2
    vaddps ymm0, ymm0, ymm2
    sub rcx, 1
    jmp ord_block

calc_inter:
    and rax, 7
    cmp rax, 0
    je calc_final


byte_block:
    cmp rax, 0
    jle calc_final
    vmovss xmm2, DWORD PTR [rsi]
    vmovss xmm3, DWORD PTR [rdx]
    vmulss xmm2, xmm2, xmm3
    vaddss xmm1, xmm1, xmm2
    add rsi, 4
    add rdx, 4
    sub rax, 1
    jmp byte_block
calc_final:
    vextractf128 xmm4, ymm0, 1
    vaddps xmm0, xmm0, xmm4
    vhaddps xmm0, xmm0, xmm0
    vhaddps xmm0, xmm0, xmm0
    vaddps xmm0, xmm0, xmm1
    vmovd rax, xmm0
    ret








