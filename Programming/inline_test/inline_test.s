	.file	"inline_test.c"
	.text
.Ltext0:
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"a=%d and b=%d\n"
	.text
	.globl	test_inline_func1
	.type	test_inline_func1, @function
test_inline_func1:
.LFB11:
	.file 1 "inline_test.c"
	.loc 1 7 0
	.cfi_startproc
.LVL0:
	subl	$28, %esp
	.cfi_def_cfa_offset 32
	.loc 1 8 0
	movl	36(%esp), %eax
	movl	%eax, 8(%esp)
	movl	32(%esp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	.loc 1 9 0
	addl	$28, %esp
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE11:
	.size	test_inline_func1, .-test_inline_func1
	.globl	test_inline_func2
	.type	test_inline_func2, @function
test_inline_func2:
.LFB12:
	.loc 1 11 0
	.cfi_startproc
.LVL1:
	movl	4(%esp), %eax
	.loc 1 12 0
	imull	%eax, %eax
	.loc 1 13 0
	ret
	.cfi_endproc
.LFE12:
	.size	test_inline_func2, .-test_inline_func2
	.section	.rodata.str1.1
.LC1:
	.string	"square val=%d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB13:
	.loc 1 15 0
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$16, %esp
.LVL2:
.LBB6:
.LBB7:
	.loc 1 8 0
	movl	$4, 8(%esp)
	movl	$2, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
.LVL3:
.LBE7:
.LBE6:
	.loc 1 22 0
	movl	$25, 4(%esp)
	movl	$.LC1, (%esp)
	call	printf
	.loc 1 25 0
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE13:
	.size	main, .-main
.Letext0:
	.file 2 "/usr/lib/gcc/i686-redhat-linux/4.6.3/include/stddef.h"
	.file 3 "/usr/include/bits/types.h"
	.file 4 "/usr/include/libio.h"
	.file 5 "/usr/include/stdio.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x36d
	.value	0x3
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF50
	.byte	0x1
	.long	.LASF51
	.long	.LASF52
	.long	.Ltext0
	.long	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF8
	.byte	0x2
	.byte	0xd4
	.long	0x30
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF1
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF2
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF3
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF4
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF5
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF6
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF7
	.uleb128 0x2
	.long	.LASF9
	.byte	0x3
	.byte	0x38
	.long	0x61
	.uleb128 0x2
	.long	.LASF10
	.byte	0x3
	.byte	0x8d
	.long	0x85
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.long	.LASF11
	.uleb128 0x2
	.long	.LASF12
	.byte	0x3
	.byte	0x8e
	.long	0x6f
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x6
	.byte	0x4
	.long	0x9f
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF13
	.uleb128 0x7
	.long	.LASF43
	.byte	0x94
	.byte	0x4
	.value	0x111
	.long	0x22d
	.uleb128 0x8
	.long	.LASF14
	.byte	0x4
	.value	0x112
	.long	0x5a
	.byte	0
	.uleb128 0x8
	.long	.LASF15
	.byte	0x4
	.value	0x117
	.long	0x99
	.byte	0x4
	.uleb128 0x8
	.long	.LASF16
	.byte	0x4
	.value	0x118
	.long	0x99
	.byte	0x8
	.uleb128 0x8
	.long	.LASF17
	.byte	0x4
	.value	0x119
	.long	0x99
	.byte	0xc
	.uleb128 0x8
	.long	.LASF18
	.byte	0x4
	.value	0x11a
	.long	0x99
	.byte	0x10
	.uleb128 0x8
	.long	.LASF19
	.byte	0x4
	.value	0x11b
	.long	0x99
	.byte	0x14
	.uleb128 0x8
	.long	.LASF20
	.byte	0x4
	.value	0x11c
	.long	0x99
	.byte	0x18
	.uleb128 0x8
	.long	.LASF21
	.byte	0x4
	.value	0x11d
	.long	0x99
	.byte	0x1c
	.uleb128 0x8
	.long	.LASF22
	.byte	0x4
	.value	0x11e
	.long	0x99
	.byte	0x20
	.uleb128 0x8
	.long	.LASF23
	.byte	0x4
	.value	0x120
	.long	0x99
	.byte	0x24
	.uleb128 0x8
	.long	.LASF24
	.byte	0x4
	.value	0x121
	.long	0x99
	.byte	0x28
	.uleb128 0x8
	.long	.LASF25
	.byte	0x4
	.value	0x122
	.long	0x99
	.byte	0x2c
	.uleb128 0x8
	.long	.LASF26
	.byte	0x4
	.value	0x124
	.long	0x265
	.byte	0x30
	.uleb128 0x8
	.long	.LASF27
	.byte	0x4
	.value	0x126
	.long	0x26b
	.byte	0x34
	.uleb128 0x8
	.long	.LASF28
	.byte	0x4
	.value	0x128
	.long	0x5a
	.byte	0x38
	.uleb128 0x8
	.long	.LASF29
	.byte	0x4
	.value	0x12c
	.long	0x5a
	.byte	0x3c
	.uleb128 0x8
	.long	.LASF30
	.byte	0x4
	.value	0x12e
	.long	0x7a
	.byte	0x40
	.uleb128 0x8
	.long	.LASF31
	.byte	0x4
	.value	0x132
	.long	0x3e
	.byte	0x44
	.uleb128 0x8
	.long	.LASF32
	.byte	0x4
	.value	0x133
	.long	0x4c
	.byte	0x46
	.uleb128 0x8
	.long	.LASF33
	.byte	0x4
	.value	0x134
	.long	0x271
	.byte	0x47
	.uleb128 0x8
	.long	.LASF34
	.byte	0x4
	.value	0x138
	.long	0x281
	.byte	0x48
	.uleb128 0x8
	.long	.LASF35
	.byte	0x4
	.value	0x141
	.long	0x8c
	.byte	0x4c
	.uleb128 0x8
	.long	.LASF36
	.byte	0x4
	.value	0x14a
	.long	0x97
	.byte	0x54
	.uleb128 0x8
	.long	.LASF37
	.byte	0x4
	.value	0x14b
	.long	0x97
	.byte	0x58
	.uleb128 0x8
	.long	.LASF38
	.byte	0x4
	.value	0x14c
	.long	0x97
	.byte	0x5c
	.uleb128 0x8
	.long	.LASF39
	.byte	0x4
	.value	0x14d
	.long	0x97
	.byte	0x60
	.uleb128 0x8
	.long	.LASF40
	.byte	0x4
	.value	0x14e
	.long	0x25
	.byte	0x64
	.uleb128 0x8
	.long	.LASF41
	.byte	0x4
	.value	0x150
	.long	0x5a
	.byte	0x68
	.uleb128 0x8
	.long	.LASF42
	.byte	0x4
	.value	0x152
	.long	0x287
	.byte	0x6c
	.byte	0
	.uleb128 0x9
	.long	.LASF53
	.byte	0x4
	.byte	0xb6
	.uleb128 0xa
	.long	.LASF44
	.byte	0xc
	.byte	0x4
	.byte	0xbc
	.long	0x265
	.uleb128 0xb
	.long	.LASF45
	.byte	0x4
	.byte	0xbd
	.long	0x265
	.byte	0
	.uleb128 0xb
	.long	.LASF46
	.byte	0x4
	.byte	0xbe
	.long	0x26b
	.byte	0x4
	.uleb128 0xb
	.long	.LASF47
	.byte	0x4
	.byte	0xc2
	.long	0x5a
	.byte	0x8
	.byte	0
	.uleb128 0x6
	.byte	0x4
	.long	0x234
	.uleb128 0x6
	.byte	0x4
	.long	0xa6
	.uleb128 0xc
	.long	0x9f
	.long	0x281
	.uleb128 0xd
	.long	0x30
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x4
	.long	0x22d
	.uleb128 0xc
	.long	0x9f
	.long	0x297
	.uleb128 0xd
	.long	0x30
	.byte	0x27
	.byte	0
	.uleb128 0xe
	.byte	0x1
	.long	.LASF54
	.byte	0x1
	.byte	0x7
	.byte	0x1
	.byte	0x3
	.long	0x2b8
	.uleb128 0xf
	.string	"a"
	.byte	0x1
	.byte	0x7
	.long	0x5a
	.uleb128 0xf
	.string	"b"
	.byte	0x1
	.byte	0x7
	.long	0x5a
	.byte	0
	.uleb128 0x10
	.byte	0x1
	.long	.LASF55
	.byte	0x1
	.byte	0xb
	.byte	0x1
	.long	0x5a
	.byte	0x3
	.long	0x2d4
	.uleb128 0xf
	.string	"x"
	.byte	0x1
	.byte	0xb
	.long	0x5a
	.byte	0
	.uleb128 0x11
	.long	0x297
	.long	.LFB11
	.long	.LFE11
	.byte	0x1
	.byte	0x9c
	.long	0x2f8
	.uleb128 0x12
	.long	0x2a5
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x12
	.long	0x2ae
	.byte	0x2
	.byte	0x91
	.sleb128 4
	.byte	0
	.uleb128 0x11
	.long	0x2b8
	.long	.LFB12
	.long	.LFE12
	.byte	0x1
	.byte	0x9c
	.long	0x314
	.uleb128 0x12
	.long	0x2ca
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x13
	.byte	0x1
	.long	.LASF56
	.byte	0x1
	.byte	0xf
	.long	0x5a
	.long	.LFB13
	.long	.LFE13
	.byte	0x1
	.byte	0x9c
	.long	0x356
	.uleb128 0x14
	.string	"tmp"
	.byte	0x1
	.byte	0x11
	.long	0x5a
	.uleb128 0x15
	.long	0x297
	.long	.LBB6
	.long	.LBE6
	.byte	0x1
	.byte	0x13
	.uleb128 0x16
	.long	0x2ae
	.byte	0x4
	.uleb128 0x16
	.long	0x2a5
	.byte	0x2
	.byte	0
	.byte	0
	.uleb128 0x17
	.long	.LASF48
	.byte	0x5
	.byte	0xa5
	.long	0x26b
	.byte	0x1
	.byte	0x1
	.uleb128 0x17
	.long	.LASF49
	.byte	0x5
	.byte	0xa6
	.long	0x26b
	.byte	0x1
	.byte	0x1
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF22:
	.string	"_IO_buf_end"
.LASF9:
	.string	"__quad_t"
.LASF30:
	.string	"_old_offset"
.LASF50:
	.string	"GNU C 4.6.3 20120306 (Red Hat 4.6.3-2)"
.LASF25:
	.string	"_IO_save_end"
.LASF5:
	.string	"short int"
.LASF8:
	.string	"size_t"
.LASF52:
	.string	"/users/kyehwanl/Programming/inline_test"
.LASF35:
	.string	"_offset"
.LASF19:
	.string	"_IO_write_ptr"
.LASF14:
	.string	"_flags"
.LASF21:
	.string	"_IO_buf_base"
.LASF26:
	.string	"_markers"
.LASF16:
	.string	"_IO_read_end"
.LASF6:
	.string	"long long int"
.LASF34:
	.string	"_lock"
.LASF11:
	.string	"long int"
.LASF31:
	.string	"_cur_column"
.LASF47:
	.string	"_pos"
.LASF46:
	.string	"_sbuf"
.LASF43:
	.string	"_IO_FILE"
.LASF1:
	.string	"unsigned char"
.LASF4:
	.string	"signed char"
.LASF7:
	.string	"long long unsigned int"
.LASF0:
	.string	"unsigned int"
.LASF44:
	.string	"_IO_marker"
.LASF33:
	.string	"_shortbuf"
.LASF18:
	.string	"_IO_write_base"
.LASF42:
	.string	"_unused2"
.LASF15:
	.string	"_IO_read_ptr"
.LASF40:
	.string	"__pad5"
.LASF2:
	.string	"short unsigned int"
.LASF13:
	.string	"char"
.LASF56:
	.string	"main"
.LASF54:
	.string	"test_inline_func1"
.LASF55:
	.string	"test_inline_func2"
.LASF45:
	.string	"_next"
.LASF36:
	.string	"__pad1"
.LASF37:
	.string	"__pad2"
.LASF38:
	.string	"__pad3"
.LASF39:
	.string	"__pad4"
.LASF51:
	.string	"inline_test.c"
.LASF3:
	.string	"long unsigned int"
.LASF20:
	.string	"_IO_write_end"
.LASF12:
	.string	"__off64_t"
.LASF10:
	.string	"__off_t"
.LASF27:
	.string	"_chain"
.LASF24:
	.string	"_IO_backup_base"
.LASF48:
	.string	"stdin"
.LASF29:
	.string	"_flags2"
.LASF41:
	.string	"_mode"
.LASF17:
	.string	"_IO_read_base"
.LASF32:
	.string	"_vtable_offset"
.LASF23:
	.string	"_IO_save_base"
.LASF28:
	.string	"_fileno"
.LASF49:
	.string	"stdout"
.LASF53:
	.string	"_IO_lock_t"
	.ident	"GCC: (GNU) 4.6.3 20120306 (Red Hat 4.6.3-2)"
	.section	.note.GNU-stack,"",@progbits
