# Yacine Manseur
# PS8
# Problem 4
# >as --64 -o a.o problem4.S
# >ld -m elf_x86_64 a.o

.global _start
.text
_start:
	movq	$1, %rax
	movq	$1, %rdi
	movq	$1, %rsi
	movq	$len, %rdx
	syscall

	movq	$60, %rax
	movq	$1, %rdi
	syscall

.data

msg:
	.ascii "Hello World.\n"
	len = . - msg
