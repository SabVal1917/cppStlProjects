  .intel_syntax noprefix

  .text
  .global add

add:
    mov rax, rdi
    add rax, rsi
    ret