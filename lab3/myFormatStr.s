	.file	"myFormatStr.c"
	.text
	.section	.rodata
.LC0:
	.string	"secret!!"
	.align 8
.LC1:
	.string	"locations: buf = %lu secret = %lu\n"
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbpS
	movq	%rsp, %rbp
	subq	$80, %rsp
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	leaq	-32(%rbp), %rax
	movl	$20, %edx
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	strncpy@PLT
	movq	-80(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rcx
	leaq	-64(%rbp), %rax
	movl	$32, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	strncpy@PLT
	leaq	-64(%rbp), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$10, %edi
	call	putchar@PLT
	leaq	-32(%rbp), %rdx
	leaq	-64(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	stdout(%rip), %rax
	movq	%rax, %rdi
	call	fflush@PLT
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
