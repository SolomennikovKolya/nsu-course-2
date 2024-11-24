.data

msg:
    .ascii "Hello world\n"
    len = . - msg

.text
    .global _start

_start:
    movq $4, %rax # 4 - это номер системного вызова (write)
    movq $1, %rbx # 1 - номер дескриптора (stdout)
    movq $msg, %rcx  # $msg - адрес буфера
    movq $len, %rdx # $len - длина буфера
    int $0x80 # вызов прерыввания, 0x80 - номер обработчика прерывания

    movq $1, %rax # 1 - номер системного вызова (exit)
    xorq %rdi, %rdi # код возврата (0)
    int $0x80

# Для компиляции и линковки:
# gcc hello.S -c -o hello.o
# ld hello.o -o hello.out