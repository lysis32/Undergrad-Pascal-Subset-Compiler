.LC0:
	.string "%d\n"
	.text
.globl main
.LC1:
.string "Bro\n"
.text
.LC2:
.string "Dog\n"
.text
#TAC_LABEL         main              
main:
#TAC_BEGINFUNC     main              
       pushl %ebp
       movl %esp, %ebp
#TAC_COPY            T3     44       
       movl  $44, %eax
#TAC_MUL             T0      1     10
       movl  $10, %ebx
       movl  $1, %ecx
       imul  %ecx, %ebx
#TAC_ADD             T1      1     T0
       movl  %ebx, -420(%ebp)
       addl  %ecx, %ebx
#TAC_MUL             T2     T1      4
       movl  $4, %edx
       imul  %ebx, %edx
#TAC_ARR_CPY         T3     T2     10
       movl  %edx, -428(%ebp)
       movl  $10, %edx
       movl  %edx, -20(%ebp)
#TAC_COPY            T7     44       
       movl  $44, %esi
#TAC_MUL             T4      2     10
       movl  %edx, -16(%ebp)
       movl  $2, %edi
       imul  %edi, %edx
#TAC_ADD             T5      2     T4
       movl  %edx, -436(%ebp)
       addl  %edi, %edx
#TAC_MUL             T6     T5      4
       movl  $4, %ecx
       imul  %edx, %ecx
#TAC_ARR_CPY         T7     T6      3
       movl  %edx, -440(%ebp)
       movl  $3, %edx
       movl  %edx, -64(%ebp)
#TAC_COPY           T15     44       
       movl  $44, %edi
#TAC_MUL            T12      1     10
       movl  %eax, -432(%ebp)
       movl  $10, %eax
       movl  %ebx, -424(%ebp)
       movl  $1, %ebx
       imul  %ebx, %eax
#TAC_ADD            T13      1    T12
       movl  %eax, -468(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T14    T13      4
       movl  $4, %ebx
       imul  %eax, %ebx
#TAC_COPY           T19     44       
       movl  %eax, -472(%ebp)
       movl  $44, %eax
#TAC_MUL            T16      2     10
       movl  %eax, -496(%ebp)
       movl  $10, %eax
       movl  %ebx, -476(%ebp)
       movl  $2, %ebx
       imul  %ebx, %eax
#TAC_ADD            T17      2    T16
       movl  %eax, -484(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T18    T17      4
       movl  $4, %ebx
       imul  %eax, %ebx
#TAC_ADD            T20    T15    T19
       movl  %ecx, -444(%ebp)
       movl  %esi, -448(%ebp)
       movl  -20(%ebp), %ecx
       movl  -64(%ebp), %esi
       addl  %ecx, %esi
#TAC_COPY           T11     44       
       movl  $44, %ecx
#TAC_MUL             T8      3     10
       movl  %eax, -488(%ebp)
       movl  $10, %eax
       imul  %edx, %eax
#TAC_ADD             T9      3     T8
       movl  %eax, -452(%ebp)
       addl  %edx, %eax
#TAC_MUL            T10     T9      4
       movl  %eax, -456(%ebp)
       movl  $4, %eax
       movl  %ebx, -492(%ebp)
       movl  -456(%ebp), %ebx
       imul  %ebx, %eax
#TAC_ARR_CPY        T11    T10    T20
       movl  %edx, -32(%ebp)
       movl  %esi, %edx
       movl  %edx, -108(%ebp)
#TAC_COPY           T24     44       
       movl  $44, %esi
#TAC_MUL            T21      3     10
       movl  $10, %ebx
       movl  %eax, -460(%ebp)
       movl  $3, %eax
       imul  %eax, %ebx
#TAC_ADD            T22      3    T21
       movl  %ebx, -504(%ebp)
       addl  %eax, %ebx
#TAC_MUL            T23    T22      4
       movl  $4, %eax
       imul  %ebx, %eax
#TAC_PRINT          T24    T23       
       movl  %eax, -512(%ebp)
       movl  %ebx, -508(%ebp)
       movl  %ecx, -464(%ebp)
       movl  %edx, -500(%ebp)
       movl  %esi, -516(%ebp)
       movl  %edi, -480(%ebp)
       subl  $856, %esp
       pushl  -108(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_COPY           T32     44       
       movl  $44, %eax
#TAC_MUL            T29      1     10
       movl  $10, %ebx
       movl  $1, %ecx
       imul  %ecx, %ebx
#TAC_ADD            T30      1    T29
       movl  %ebx, -536(%ebp)
       addl  %ecx, %ebx
#TAC_MUL            T31    T30      4
       movl  $4, %edx
       imul  %ebx, %edx
#TAC_COPY           T36     44       
       movl  $44, %esi
#TAC_MUL            T33      2     10
       movl  $10, %edi
       movl  $2, %ecx
       imul  %ecx, %edi
#TAC_ADD            T34      2    T33
       movl  %edi, -552(%ebp)
       addl  %ecx, %edi
#TAC_MUL            T35    T34      4
       movl  $4, %ecx
       imul  %edi, %ecx
#TAC_DIV            T37    T32    T36
       movl  %ecx, -560(%ebp)
       movl  %esi, -564(%ebp)
       movl  -20(%ebp), %ecx
       movl  -64(%ebp), %esi
       movl  %eax, -548(%ebp)
       movl  %edx, -544(%ebp)
       movl  %ecx, %eax
       movl  %ecx, %edx
       sar   $31, %edx
       idiv  %esi
#TAC_COPY           T41     44       
       movl  $44, %ecx
#TAC_MUL            T38      3     10
       movl  $10, %edx
       movl  $3, %esi
       imul  %esi, %edx
#TAC_ADD            T39      3    T38
       movl  %edx, -572(%ebp)
       addl  %esi, %edx
#TAC_MUL            T40    T39      4
       movl  $4, %esi
       imul  %edx, %esi
#TAC_ADD            T42    T37    T41
       movl  %ecx, -584(%ebp)
       movl  %esi, -580(%ebp)
       movl  %eax, %ecx
       movl  -108(%ebp), %esi
       addl  %ecx, %esi
#TAC_COPY           T28     44       
       movl  $44, %eax
#TAC_MUL            T25      4     10
       movl  %eax, -532(%ebp)
       movl  $10, %eax
       movl  %ebx, -540(%ebp)
       movl  $4, %ebx
       imul  %ebx, %eax
#TAC_ADD            T26      4    T25
       movl  %eax, -520(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T27    T26      4
       imul  %eax, %ebx
#TAC_ARR_CPY        T28    T27    T42
       movl  %edx, -576(%ebp)
       movl  %esi, %edx
       movl  %edx, -152(%ebp)
#TAC_COPY           T50     44       
       movl  $44, %esi
#TAC_MUL            T47      4     10
       movl  %eax, -524(%ebp)
       movl  $10, %eax
       movl  %ebx, -528(%ebp)
       movl  $4, %ebx
       imul  %ebx, %eax
#TAC_ADD            T48      4    T47
       movl  %eax, -608(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T49    T48      4
       imul  %eax, %ebx
#TAC_MOD            T51    T50      2
       movl  %ecx, -568(%ebp)
       movl  %esi, -620(%ebp)
       movl  -152(%ebp), %ecx
       movl  $2, %esi
       movl  %eax, -612(%ebp)
       movl  %edx, -588(%ebp)
       movl  %ecx, %eax
       movl  %ecx, %edx
       sar   $31, %edx
       idiv  %esi
#TAC_COPY           T46     44       
       movl  $44, %eax
#TAC_MUL            T43      5     10
       movl  $10, %ecx
       movl  $5, %esi
       imul  %esi, %ecx
#TAC_ADD            T44      5    T43
       movl  %ecx, -592(%ebp)
       addl  %esi, %ecx
#TAC_MUL            T45    T44      4
       movl  $4, %esi
       imul  %ecx, %esi
#TAC_ARR_CPY        T46    T45    T51
       movl  %edx, -624(%ebp)
       movl  -624(%ebp), %edx
       movl  %edx, -196(%ebp)
#TAC_COPY           T59     44       
       movl  %eax, -604(%ebp)
       movl  $44, %eax
#TAC_MUL            T56      5     10
       movl  %eax, -656(%ebp)
       movl  $10, %eax
       movl  %ebx, -616(%ebp)
       movl  $5, %ebx
       imul  %ebx, %eax
#TAC_ADD            T57      5    T56
       movl  %eax, -644(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T58    T57      4
       movl  $4, %ebx
       imul  %eax, %ebx
#TAC_ADD            T60      2    T59
       movl  %ecx, -596(%ebp)
       movl  %esi, -600(%ebp)
       movl  $2, %ecx
       movl  -196(%ebp), %esi
       addl  %ecx, %esi
#TAC_COPY           T55     44       
       movl  $44, %ecx
#TAC_MUL            T52      6     10
       movl  %eax, -648(%ebp)
       movl  $10, %eax
       movl  %ebx, -652(%ebp)
       movl  $6, %ebx
       imul  %ebx, %eax
#TAC_ADD            T53      6    T52
       movl  %eax, -628(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T54    T53      4
       movl  $4, %ebx
       imul  %eax, %ebx
#TAC_ARR_CPY        T55    T54    T60
       movl  %edx, -624(%ebp)
       movl  %esi, %edx
       movl  %edx, -240(%ebp)
#TAC_COPY           T64     44       
       movl  $44, %esi
#TAC_MUL            T61      6     10
       movl  %eax, -632(%ebp)
       movl  $10, %eax
       movl  %ebx, -636(%ebp)
       movl  $6, %ebx
       imul  %ebx, %eax
#TAC_ADD            T62      6    T61
       movl  %eax, -664(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T63    T62      4
       movl  $4, %ebx
       imul  %eax, %ebx
#TAC_PRINT          T64    T63       
       movl  %eax, -668(%ebp)
       movl  %ebx, -672(%ebp)
       movl  %ecx, -640(%ebp)
       movl  %edx, -660(%ebp)
       movl  %esi, -676(%ebp)
       movl  %edi, -556(%ebp)
       subl  $856, %esp
       pushl  -240(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_COPY           T72     44       
       movl  $44, %eax
#TAC_MUL            T69      6     10
       movl  $10, %ebx
       movl  $6, %ecx
       imul  %ecx, %ebx
#TAC_ADD            T70      6    T69
       movl  %ebx, -696(%ebp)
       addl  %ecx, %ebx
#TAC_MUL            T71    T70      4
       movl  $4, %edx
       imul  %ebx, %edx
#TAC_SUB            T73    T72      2
       movl  -240(%ebp), %ecx
       movl  $2, %esi
      subl  %ecx, %esi
#TAC_COPY           T68     44       
       movl  $44, %ecx
#TAC_MUL            T65      6     10
       movl  $10, %edi
       movl  %eax, -708(%ebp)
       movl  $6, %eax
       imul  %eax, %edi
#TAC_ADD            T66      6    T65
       movl  %edi, -680(%ebp)
       addl  %eax, %edi
#TAC_MUL            T67    T66      4
       movl  $4, %eax
       imul  %edi, %eax
#TAC_ARR_CPY        T68    T67    T73
       movl  %edx, -704(%ebp)
       movl  %esi, %edx
       movl  %edx, -240(%ebp)
#TAC_PRINT_STR  "Bro\n"              
       subl  $856, %esp
       pushl $.LC1
       call printf
       addl $864, %esp
#TAC_COPY           T77     44       
       movl  $44, %esi
#TAC_MUL            T74      6     10
       movl  %eax, -688(%ebp)
       movl  $10, %eax
       movl  %ebx, -700(%ebp)
       movl  $6, %ebx
       imul  %ebx, %eax
#TAC_ADD            T75      6    T74
       movl  %eax, -716(%ebp)
       addl  %ebx, %eax
#TAC_MUL            T76    T75      4
       movl  $4, %ebx
       imul  %eax, %ebx
#TAC_PRINT          T77    T76       
       movl  %eax, -720(%ebp)
       movl  %ebx, -724(%ebp)
       movl  %ecx, -692(%ebp)
       movl  %edx, -712(%ebp)
       movl  %esi, -728(%ebp)
       movl  %edi, -684(%ebp)
       subl  $856, %esp
       pushl  -240(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_COPY           T81     44       
       movl  $44, %eax
#TAC_MUL            T78      4     10
       movl  $10, %ebx
       movl  $4, %ecx
       imul  %ecx, %ebx
#TAC_ADD            T79      4    T78
       movl  %ebx, -732(%ebp)
       addl  %ecx, %ebx
#TAC_MUL            T80    T79      4
       imul  %ebx, %ecx
#TAC_PRINT          T81    T80       
       movl  %eax, -744(%ebp)
       movl  %ebx, -736(%ebp)
       movl  %ecx, -740(%ebp)
       subl  $856, %esp
       pushl  -152(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_COPY           T85     44       
       movl  $44, %eax
#TAC_MUL            T82      5     10
       movl  $10, %ebx
       movl  $5, %ecx
       imul  %ecx, %ebx
#TAC_ADD            T83      5    T82
       movl  %ebx, -748(%ebp)
       addl  %ecx, %ebx
#TAC_MUL            T84    T83      4
       movl  $4, %edx
       imul  %ebx, %edx
#TAC_PRINT          T85    T84       
       movl  %eax, -760(%ebp)
       movl  %ebx, -752(%ebp)
       movl  %edx, -756(%ebp)
       subl  $856, %esp
       pushl  -196(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_COPY           T89     44       
       movl  $44, %eax
#TAC_MUL            T86      4     10
       movl  $10, %ebx
       movl  $4, %ecx
       imul  %ecx, %ebx
#TAC_ADD            T87      4    T86
       movl  %ebx, -764(%ebp)
       addl  %ecx, %ebx
#TAC_MUL            T88    T87      4
       imul  %ebx, %ecx
#TAC_COPY           T93     44       
       movl  $44, %edx
#TAC_MUL            T90      6     10
       movl  $10, %esi
       movl  $6, %edi
       imul  %edi, %esi
#TAC_ADD            T91      6    T90
       movl  %esi, -780(%ebp)
       addl  %edi, %esi
#TAC_MUL            T92    T91      4
       movl  $4, %edi
       imul  %esi, %edi
#TAC_GT          (null)    T89    T93
       movl  %eax, -776(%ebp)
       movl  %ebx, -768(%ebp)
       movl  %ecx, -772(%ebp)
       movl  %edx, -792(%ebp)
       movl  %esi, -784(%ebp)
       movl  %edi, -788(%ebp)
       movl  -152(%ebp), %ecx
       movl  -240(%ebp), %esi
       cmp  %ecx, %esi
       jl   L1
#TAC_COPY           T94      0       
       movl  $0, %eax
#TAC_GOTO        (null)              
       movl  %eax, -796(%ebp)
       jmp  L0
#TAC_LABEL           L1              
L1:
#TAC_COPY           T94      1       
       movl  $1, %eax
#TAC_LABEL           L0              
       movl  %eax, -796(%ebp)
L0:
#TAC_IFZ         (null)    T94       
       movl  -796(%ebp), %eax
       movl  $0, %ebx
       cmp   %eax, %ebx
       jz  L2
#TAC_COPY           T98     44       
       movl  $44, %ebx
#TAC_MUL            T95      4     10
       movl  $10, %ecx
       movl  $4, %edx
       imul  %edx, %ecx
#TAC_ADD            T96      4    T95
       movl  %ecx, -800(%ebp)
       addl  %edx, %ecx
#TAC_MUL            T97    T96      4
       imul  %ecx, %edx
#TAC_PRINT          T98    T97       
       movl  %ebx, -812(%ebp)
       movl  %ecx, -804(%ebp)
       movl  %edx, -808(%ebp)
       subl  $856, %esp
       pushl  -152(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_GOTO        (null)              
       jmp  L3
#TAC_LABEL           L2              
L2:
#TAC_COPY          T102     44       
       movl  $44, %eax
#TAC_MUL            T99      6     10
       movl  $10, %ebx
       movl  $6, %ecx
       imul  %ecx, %ebx
#TAC_ADD           T100      6    T99
       movl  %ebx, -816(%ebp)
       addl  %ecx, %ebx
#TAC_MUL           T101   T100      4
       movl  $4, %edx
       imul  %ebx, %edx
#TAC_PRINT         T102   T101       
       movl  %eax, -828(%ebp)
       movl  %ebx, -820(%ebp)
       movl  %edx, -824(%ebp)
       subl  $856, %esp
       pushl  -240(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_LABEL           L3              
L3:
#TAC_COPY             X      3       
       movl  $3, %eax
#TAC_SUB           T103      X      1
       movl  $1, %ebx
       subl  %ebx, %eax
#TAC_COPY             X   T103       
       movl  %eax, -832(%ebp)
#TAC_COPY          T107     44       
       movl  $44, %ecx
#TAC_MUL           T104      4     10
       movl  $10, %edx
       movl  $4, %esi
       imul  %esi, %edx
#TAC_ADD           T105      4   T104
       movl  %edx, -836(%ebp)
       addl  %esi, %edx
#TAC_MUL           T106   T105      4
       imul  %edx, %esi
#TAC_NEG           T107   T108       
       movl  %ecx, -848(%ebp)
       movl  -152(%ebp), %ebx
       neg   %ebx
       movl  %ebx,  %ecx
       movl  %ecx, -852(%ebp)
#TAC_COPY             X   T108       
       movl  -852(%ebp), %ebx
#TAC_PRINT            X              
       movl  %ebx, -12(%ebp)
       movl  %edx, -840(%ebp)
       movl  %esi, -844(%ebp)
       subl  $856, %esp
       pushl -12(%ebp)
       pushl $.LC0
       call printf
       addl $864, %esp
#TAC_PRINT_STR  "Dog\n"              
       subl  $856, %esp
       pushl $.LC2
       call printf
       addl $864, %esp
#TAC_ENDFUNC       main              
       leave
       ret
