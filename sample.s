	.text
	.globl sumatory
	.type sumatory, @function
sumatory:
	enter	$(8*7), $0

	movq	$0, %r10
	movq	%r10, -16(%rbp)

	movq	$0, %r10
	movq	%r10, -24(%rbp)

l0:
	movq	-16(%rbp), %r10
	movq	8(%rbp), %r11
	cmpq	%r10, %r11
	movq	$0, %r11
	movq	$1, %r10
	cmovl	%r10, %r11
	movq	%r11, -40(%rbp)

	movq	-16(%rbp), %r10
	movq	8(%rbp), %r11
	cmpq	%r10, %r11
	movq	$0, %r11
	movq	$1, %r10
	cmove	%r10, %r11
	movq	%r11, -48(%rbp)

	movq	-40(%rbp), %r10
	cmpq	$1, %r10
	cmove	$1, %r11
	movq	-48(%rbp), %r10
	cmovne	%r10, %r11
	movq	%r11, -32(%rbp)

	cmpq	$0, -32(%rbp)
	je	l1

	movq	-24(%rbp), %r10
	addq	-16(%rbp), %r10
	movq	%r10, -56(%rbp)

	movq	-56(%rbp), %r10
	movq	%r10, -24(%rbp)

	movq	-16(%rbp), %r10
	addq	$1, %r10
	movq	%r10, -64(%rbp)

	movq	-64(%rbp), %r10
	movq	%r10, -16(%rbp)

	jmp	l0

l1:
	movq	-24(%rbp), %rax

	leave
	ret

	.globl main
	.type main, @function
main:
	enter	$(8*3), $0

	movq	$2, %rdi
	pushq	%rdi

	call	sumatory
	addq	$(8*1), %rsp

	movq	-24(%rbp), %r10
	movq	%r10, -16(%rbp)

	movq	-16(%rbp), %rdi
	pushq	%rdi

	call	print_int
	addq	$(8*1), %rsp

	leave
	ret

