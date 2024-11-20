	.data
	.globl z
	.align 8
z:
	.long 0
	.globl x
	.align 8
x:
	.long 14
	.globl i
	.align 8
i:
	.long 17
	.globl r
	.align 1
r:
	.byte 1
	.text
	.globl sumatory
	.type sumatory, @function
sumatory:
	enter	$(8*7), $0

	movq	$1, %r10
	movq	%r10, -16(%rbp)

	movq	$0, %r10
	movq	%r10, -24(%rbp)

l0:
	movq	i(%rip), %r10
	movq	8(%rbp), %r11
	cmpq	%r10, %r11
	movq	$0, %r11
	movq	$1, %r10
	cmovl	%r10, %r11
	movq	%r11, -40(%rbp)

	movq	i(%rip), %r10
	movq	8(%rbp), %r11
	cmpq	%r10, %r11
	movq	$0, %r11
	movq	$1, %r10
	cmove	%r10, %r11
	movq	%r11, -48(%rbp)

	movq	-40(%rbp), %r10
	cmpq	%r10, $1
	cmove	$1, %r11
	movq	-48(%rbp), %r10
	cmovne	%r10, %r11
	movq	%r11, -32(%rbp)

	cmpq	-32(%rbp), $0
	je	l1

	movq	-24(%rbp), %r10
	addl	i(%rip), %r10
	movq	%r10, -56(%rbp)

	movq	-56(%rbp), %r10
	movq	%r10, -24(%rbp)

	movq	i(%rip), %r10
	addl	$1, %r10
	movq	%r10, -64(%rbp)

	movq	-64(%rbp), %r10
	movq	%r10, i(%rip)

	jmp	l0

l1:
	leave
	ret

