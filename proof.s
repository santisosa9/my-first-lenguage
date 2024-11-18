	.file	"proof.c"
# GNU C17 (Debian 13.3.0-3) version 13.3.0 (x86_64-linux-gnu)
#	compiled by GNU C version 13.3.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.26-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mtune=generic -march=x86-64 -O0 -fasynchronous-unwind-tables
	.text
	.globl	x
	.data
	.align 4
	.type	x, @object
	.size	x, 4
x:
	.long	15674
	.globl	y
	.bss
	.align 4
	.type	y, @object
	.size	y, 4
y:
	.zero	4
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
# proof.c:7:     int z = 0;
	movl	$0, -4(%rbp)	#, z
# proof.c:8:     y = z;
	movl	-4(%rbp), %eax	# z, tmp86
	movl	%eax, y(%rip)	# tmp86, y
# proof.c:10:     return x + y;
	movl	x(%rip), %edx	# x, x.0_1
	movl	y(%rip), %eax	# y, y.1_2
	addl	%edx, %eax	# x.0_1, _6
# proof.c:11: }
	popq	%rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Debian 13.3.0-3) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
