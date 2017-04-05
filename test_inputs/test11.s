.LC0:
	.string "%d\n"
	.text
.globl main
#TAC_LABEL         main              
main:
#TAC_BEGINFUNC     main              
       pushl %ebp
       movl %esp, %ebp
#TAC_COPY             X     10       
       movl  $10, %eax
#TAC_LABEL           L3              
       movl  %eax, -12(%ebp)
L3:
#TAC_GE          (null)      X      2
       movl  -12(%ebp), %eax
       movl  $2, %ebx
       cmp  %eax, %ebx
       jle  L2
#TAC_COPY            T0      0       
       movl  $0, %ecx
#TAC_GOTO        (null)              
       movl  %ecx, -16(%ebp)
       jmp  L1
#TAC_LABEL           L2              
L2:
#TAC_COPY            T0      1       
       movl  $1, %eax
#TAC_LABEL           L1              
       movl  %eax, -16(%ebp)
L1:
#TAC_IFZ         (null)     T0       
       movl  -16(%ebp), %eax
       movl  $0, %ebx
       cmp   %eax, %ebx
       jz  L4
#TAC_SUB             T1      X      6
       movl  $6, %ebx
       movl  -12(%ebp), %ecx
       subl  %ebx, %ecx
#TAC_COPY             X     T1       
       movl  %ecx, -20(%ebp)
#TAC_PRINT            X              
       movl  %ecx, -12(%ebp)
       subl  $24, %esp
       pushl -12(%ebp)
       pushl $.LC0
       call printf
       addl $32, %esp
#TAC_GOTO        (null)              
       jmp  L3
#TAC_LABEL           L4              
L4:
#TAC_ENDFUNC       main              
       leave
       ret
#TAC_LABEL         fooL              
fooL:
#TAC_BEGINFUNC     fooL              
       pushl %ebp
       movl %esp, %ebp
#TAC_COPY           foo      a       
       movl  8(%ebp), %eax
#TAC_ENDFUNC                         
       leave
       ret
