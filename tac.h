/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* tac.h - Version .80 for Pascal Compiler, developed with     */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques.                       */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#ifndef _TAC_H
#define _TAC_H

#include <stdlib.h>
#include <stdio.h>
#include "symbol.h"

#define TAC_UNDEF     0
#define TAC_ADD       1
#define TAC_SUB       2
#define TAC_MUL       3
#define TAC_DIV       4
#define TAC_NEG       5
#define TAC_COPY      6
#define TAC_GOTO      7
#define TAC_IFZ       8
#define TAC_IFNZ      9
#define TAC_ARG      10
#define TAC_CALL     11
#define TAC_RETURN   12
#define TAC_MOD      13
#define TAC_AND      14
#define TAC_OR       15
#define TAC_POS      16
#define TAC_NOT      17
#define TAC_ARR_CPY  18
#define TAC_VAR      19
#define TAC_LABEL    20
#define TAC_BEGINFUNC 21
#define TAC_ENDFUNC  22
#define TAC_HALT     23
#define TAC_PRINT    24
#define TAC_LT       25
#define TAC_GT       26
#define TAC_EQ       27
#define TAC_NE       28
#define TAC_GE       29
#define TAC_LE       30
#define TAC_ARR_ARR_CPY 31
#define TAC_VAR_ARR_CPY 32
#define TAC_PRINT_STR 33

#define VAL1   int_value                  /* Value val1 */
#define TEXT1  name                       /* Text val1 */
#define VAL2   int_value                  /* Value val2 */
#define LABEL2 int_value                  /* Label val2 */
#define ADDR2  offset                     /* Address val2 */
#define ETYPE  res->token_type_1          /* Type of expr result */
#define EVAL1  res->int_value             /* Value field in expr */
#define VA a.var
#define LA a.lab
#define VB b.var
#define LB b.lab
#define VC c.var
#define LC c.lab

#define  LIB_PRINTN     0                /* Index into library entry points */
#define  LIB_PRINTS     1
#define  LIB_MAX        2

int next_tmp ;
int next_label ;
extern stack symbol_table_stack ;

typedef struct tac{
  struct tac *next ;
  struct tac *prev ;
  int op ;

  union{
    SYMB *var ;
    struct tac *lab ;
  } a ;
  union{
    SYMB *var ;
    struct tac *lab ;
  } b ;
  union{
    SYMB *var ;
    struct tac *lab ;
  } c ;

} TAC ;

TAC *tac_list ;

typedef struct enode{

  struct enode *next ;
  struct enode *offset ;
  TAC *tac ;
  SYMB *res ;

  SYMB *array ;
  int ndim ;

} ENODE ;

SYMB *mktmp() ;
SYMB *mklabel() ;
SYMB *mktmpconst( char *s ) ;
TAC *mktac( int op, SYMB *a, SYMB *b, SYMB *c ) ;
TAC *join_tac( TAC *c1, TAC *c2 ) ;
ENODE *mkenode() ;
ENODE *do_bin( int op, ENODE *op1, ENODE *op2 ) ;
ENODE *do_un( int op, ENODE *op1 ) ;
TAC *do_assign( SYMB *var, ENODE *expr ) ;
TAC *do_array_assign( SYMB *var, ENODE *offset, ENODE *expr ) ;
int limit( SYMB *s, int m ) ;
char *print_tac_instr( int op ) ;
void print_tac_list() ;
TAC *do_func( SYMB *func, TAC *args, int offs ) ;
TAC *do_prog( SYMB *func, TAC *args, TAC *code, int offs ) ;

/* relops */
ENODE *do_LT( ENODE *expr1, ENODE *expr2 ) ; 
ENODE *do_GT( ENODE *expr1, ENODE *expr2 ) ; 
ENODE *do_EQ( ENODE *expr1, ENODE *expr2 ) ;
ENODE *do_NE( ENODE *expr1, ENODE *expr2 ) ;
ENODE *do_GE( ENODE *expr1, ENODE *expr2 ) ;
ENODE *do_LE( ENODE *expr1, ENODE *expr2 ) ;    

/* if then */
TAC *do_test( ENODE *expr, TAC *stat1, TAC *stat2 ) ;
TAC *do_if( ENODE *expr, TAC *stat ) ;

/* do while */
TAC *do_while( ENODE *expr, TAC *stat ) ;

#endif
