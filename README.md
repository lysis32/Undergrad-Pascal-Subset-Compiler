Alec Ferguson
Compiler Construction
91.406

README
Version .80

The current binary executable is 'afpc'. Output is directed to standard output by default,
to direct it to a file, simply use the output redirection operator.

Ex. ./afpc test1.p > test1.s
    gcc test1.s
    ./a.out

Is the sequence to build and execute an x86 assembly file.

NOTE: There should be a few warnings from yacc when building the executable.
Ignore them, they have no effect on the program.

It is capable of:
+ All arithmetic statements, including MOD and DIV.

+ If, then, else, while, do.

+ Conditional statements, comparisons and logical operations. 
  Note that the structure of the grammer enforces parenthesis
  in cases like this ( X < 0 ) AND ( Y < 0 ).

+ Printing integers.
  The syntax is PRINT( X )

+ Printing strings.
  The syntax is PRINT( "c-style-string" )

+ Function and procedure calls. Arguments are functional and values can be returned.
  Functions and procedures can call other functions and procedures.
  NOTE that a function must be DEFINED before it can be called or recognized:
  Which is to say, the function definition for the calling function must come after
  that of the caller. 
  
+ Recursive calls. For some reason I was unable to get my recursive
  fibonacci to work, but recursive factorial works fine.

+ N-dimensional arrays on both sides of the assignment operator, plus expressions.
  For example, statements of the form: N[1 + 2, 3, 4] := A[2,3] + X DIV N[3,4].

It is currently incapable of:
- Access to nonlocal names.

- Parsing nested functions.

- Passing arrays as arguments.

I felt like I learned a lot from this project. I had a heavy course load this semester
so I was unable to get everything done, but I am satisfied with what I was able to
produce. I am happy to say that I now have a better understanding of programming
language design, compilers, and how CS theory applies to them.









