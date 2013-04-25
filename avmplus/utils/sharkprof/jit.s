// something
.text
.align 12
.globl __jitStart
.globl _jitStart
__jitStart:
_jitStart:
.fill 128 * 1024 * 1024, 1, 0xcc
.globl __jitEnd
.globl _jitEnd
__jitEnd:
_jitEnd:
