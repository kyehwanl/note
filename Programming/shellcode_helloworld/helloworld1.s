#cpuid.s Sample program to extract the processor Vendor ID
.section .text
.globl _start
_start:
  call mark_below
  .ascii "Hello, world"

mark_below:
  popl %ecx
  movl $4, %eax
  movl $1, %ebx
  movl $15,%edx
  int $0x80

  movl $1, %eax
  movl $0, %ebx
  int $0x80
