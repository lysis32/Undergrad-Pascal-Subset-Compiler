#
#
# Makefile 

afpc: lex.yy.c y.tab.c main.c stack.o stack_interface.o list.o symbol.o utility.o tac.o cg.o
	cc -g lex.yy.c y.tab.c main.c stack.o stack_interface.o list.o symbol.o utility.o tac.o cg.o -o afpc -ll -ly

lex.yy.c: pascal.l
	lex pascal.l

y.tab.c: pascal.y
	yacc -d pascal.y

stack.o: stack.c stack.h
	gcc -c -g stack.c

stack_interface.o: stack_interface.c stack_interface.h
	gcc -c -g stack_interface.c

list.o: list.c list.h
	gcc -c -g list.c

symbol.o: symbol.c symbol.h
	gcc -c -g symbol.c

utility.o: utility.c utility.h
	gcc -c -g utility.c

tac.o: tac.c tac.h symbol.h
	gcc -c -g tac.c

cg.o: cg.c cg.h tac.h symbol.h
	gcc -c -g cg.c

clean:
	rm -f *.o *~ lex.yy.c y.tab.c afpc
