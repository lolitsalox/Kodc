bits 64

section .data
x dd 5

section .text
default rel

global f
f:
push rbp
push rbx
mov rbp, rsp
sub rsp, 0


_end_f:
add rsp, 0
mov rsp, rbp
pop rbx
pop rbp
ret

global main
main:
push rbp
push rbx
mov rbp, rsp
sub rsp, 0

mov dword [rbp + 24], ecx
mov qword [rbp + 28], rdx

_end_main:
add rsp, 0
mov rsp, rbp
pop rbx
pop rbp
ret
