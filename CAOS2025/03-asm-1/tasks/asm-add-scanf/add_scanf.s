  .intel_syntax noprefix

  .text
  .global add_scanf


add_scanf:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rdi, OFFSET FLAT:scanf_format_string
  lea rdx, QWORD PTR [rbp - 8]
  lea rsi, QWORD PTR [rbp - 16]
  call scanf
  mov rax, QWORD PTR [rbp - 8]
  add rax, QWORD PTR [rbp - 16]
  mov rsp, rbp
  pop rbp
  ret


scanf_format_string:
  .string "%lld %lld"
