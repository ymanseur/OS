# Yacine Manseur
# PS8
# Problem 3
# >as --64 -o a.o problem3.S
# >ld -m elf_x86_64 a.o

# ANSWER TO THE QUESTION:
# After the write, a seg fault occurred.
#  This is because there was no return 
#  value when the program exited.

.global _start
.text
_start:
	movq	$1, %rax
	movq	$1, %rdi
	movq	$msg, %rsi
	movq	$len, %rdx
	syscall

	movq	$60, %rax
	movq	$1, %rdi
	syscall

.data

msg:
	.ascii "Hello World.\n"
	len = . - msg
