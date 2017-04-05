/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* symbol.h - Version .80 for Pascal Compiler, developed with  */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques.                       */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#ifndef _symbol
#define _symbol

#define HASHSIZE  997 

#define T_UNDEF      0 
#define T_VAR        1
#define T_PROC       2
#define T_FUNC       3
#define T_INT        4
#define T_LABEL      5
#define T_REAL       6
#define T_ARRAY      7
#define T_STRING     8
#define T_BOOL       9
#define T_SPECIAL    10
#define T_PROGRAM    11
#define T_NOT_LOCAL  12
#define T_PARAM      13
#define T_ARG        14
#define T_LOCAL      15
#define T_FILE       16
#define T_CONSTANT   17
#define T_SELF       18
#define T_TEXT       19

#define SIZEOF_INT     4
#define SIZEOF_REAL    4
#define SIZEOF_POINTER 4

#include <stdio.h>
#include "stack.h"

typedef struct dim_type {

  struct dim_type *next ;
  struct dim_type *prev ;

  int  high      ;
  int  low       ;
  int  dimension ; 
  int  c         ;  /* Maybe not needed here. */
  int  n         ;

} DIM_TYPE ; 

typedef struct decl_type {

  int data_type ;
  int token_type_1 ;
  int token_type_2 ;
  int size ;
  int c    ;             /* To be used in arrays. Precalculated c constant */

  int elements ;         /* Total number of elements in an array.          */
  int dimensions ;
  struct dim_type *dimension_list ;

} DECL_TYPE ;

typedef struct param_type {

  struct param_type *next ;
  struct symb *param_symb ;  
  int param ;                    /* The number of the parameter. */

} PARAM_TYPE ;

typedef struct symb {

  struct symb *next ;                    
  struct symb *root_symb ;                /* pointer to the non local variable        */

  struct symbol_table *function_st_ptr ;  /* pointer to the func proc symbol table    */

  struct dim_type *dimension_list ;       /* not implemented yet                      */
  struct param_type *parameter_list ;     /* I may not need this here.                */

  int token_type_1  ;                     /* type of the symbol                       */
  int token_type_2  ;                     /* more type of the symbol                  */
  int data_type     ;
  int offset        ;                     /* offset from the base                     */
  int size          ;
  int dimensions    ;                     /* arrays - number of dimensions            */
  int parameters    ;                     /* number of parameters                     */
  int int_value     ;                     /* value of the integer constant            */
  int hops          ;                     /* how many routines am I nested inside     */
  int line          ;                     /* the line this symbol was first seen.     */
  int c             ;                     /* If array, precalculated constant         */
  int f_value       ;
  double dbl_value  ;                     /* value of the real constant               */
  char *string_value  ;                   /* the string                               */
  char *name        ;                     /* pointer to the symbol's name             */

} SYMB ;

typedef struct symbol_table {
 
  char   *name  ;                /* name of the symbol table's function or procedure  */
  struct symbol_table *parent ;  /* pointer to the enclosing parent's sym. table      */
  int width ;                    /* how many bytes of storage are needed.             */
  int parameters;                /* The number of parameters the procedure has.       */
  struct param_type *parameter_list;  /* parameter list                               */
  SYMB  *symtab[HASHSIZE] ;      /* hash table of symbols within this procedure       */
 
} SYMBTABLE ;


typedef SYMBTABLE *tableptr ;

SYMBTABLE *const_sym_tab ;
FILE *strings ;
stack *main_stack ;
stack offset_stack ;
int line_num ;
int c_offset ;
int next_string ;

extern SYMBTABLE *mktable( SYMBTABLE *previous ) ;
extern void mkname( SYMBTABLE *table, char *yytext , int tokentype ) ;
extern void mkconst( SYMBTABLE *table, char *yytext , int tokentype ) ;
extern void init_constant_table( void ) ;
extern DIM_TYPE *get_dim( void ) ;
extern PARAM_TYPE *get_param( void ) ;
extern SYMB *lookup( SYMBTABLE *tableptr, char *name ) ;
extern void insert_sym( SYMBTABLE *tb, SYMB *s ) ;
extern int hash( char *s ) ;
extern void dump_table( SYMBTABLE *tableptr ) ;
extern char *lookup_type( int type ) ;
extern SYMB *get_symb() ;

#endif
