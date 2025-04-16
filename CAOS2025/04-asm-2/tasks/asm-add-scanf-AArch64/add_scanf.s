  .text
  .global add_scanf

add_scanf:
  sub sp, sp, #32
  str x29, [sp, #16]
  str x30, [sp, #24]
  mov x29, sp
  sub sp, sp, #16
  adr x0, scanf_format_string
  add x1, sp, #0
  add x2, sp, #8
  bl scanf
  ldr x1, [sp, #0]
  ldr x2, [sp, #8]
  add x0, x1, x2
  add sp, sp, #16
  ldr x29, [sp, #16]
  ldr x30, [sp, #24]
  add sp, sp, #32
  ret
  .section .rodata

scanf_format_string:
  .string "%lld %lld"
