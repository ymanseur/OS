# Yacine Manseur
# PS8
# Problem 2
# >as --64 -o a.o problem2.S
# >ld -m elf_x86_64 a.o

.global _start
.text
_start:
	movq	$1, %rax
	movq	$1, %rdi
	movq	$msg, %rsi
	movq	$len, %rdx
	syscall

.data

msg:
	.ascii "Hello World.\n"
	len = . - msg
