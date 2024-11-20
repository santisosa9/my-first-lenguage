	.text
	.globl getMax
	.type getMax, @function
getMax:
	enter	$(8*1), $0

	movq	8(%rbp), %r10
	movq	16(%rbp), %r11
	cmpq	%r11, %r10
	movq	$0, %r11
	movq	$1, %r10
	cmovg	%r10, %r11
	movq	%r11, -16(%rbp)

	cmpq	$0, -16(%rbp)
	je	l0

	movq	8(%rbp), %rax

	jmp	l1

l0:
	movq	16(%rbp), %rax

l1:
	leave
	ret

	.globl main
	.type main, @function
main:
	enter	$(8*3), $0

	movq	$6, %rdi
	pushq	%rdi

	movq	$9, %rsi
	pushq	%rsi

	call	getMax
	addq	$(8*2), %rsp
	movq	%rax, -24(%rbp)

	movq	-24(%rbp), %r10
	movq	%r10, -16(%rbp)

	movq	-16(%rbp), %rdi
	pushq	%rdi

	call	print_int
	addq	$(8*1), %rsp

	leave
	ret

