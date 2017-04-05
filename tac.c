/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* tac.c - Version .80 for Pascal Compiler, developed with     */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques                        */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol.h"
#include "tac.h"
#include "utility.h"
#include "stack_interface.h"
#include "cg.h"
#include "y.tab.h"

SYMB *mktmp()
/* Creates a temporary symb */
{

  SYMB *old_yylval = yylval.symb ;
  SYMBTABLE *curr_tab ;
  SYMB *tmp ;
  int offs ;

  char name[12] ;
  
  top_table( &symbol_table_stack, &curr_tab ) ;
  /*offset for temp variables as they may need to be spilled
   *onto the stack */
  pop_offset( &offset_stack, &offs ) ;

  sprintf( name, "T%d", next_tmp++ ) ;
  mkname( curr_tab, name, 0 ) ;
  /* yylval.symb -> token_type_2 = T_VAR ; */
  
  tmp =  yylval.symb ;
  yylval.symb = old_yylval ;

  tmp -> token_type_2 = T_VAR ;
  tmp -> offset = offs ;
  offs += 4 ;

  push_offset( &offset_stack, offs ) ;

  return tmp ;

}

SYMB *mktmpconst( char *s )
/* Creates a temporary constant with name s, where s is the value of the
   constant in string form */
{

  SYMB *old_yylval = yylval.symb ;
  SYMB *tmp ;

  mkconst( const_sym_tab, s, T_INT ) ;

  tmp = yylval.symb ;
  yylval.symb = old_yylval ;

  tmp -> token_type_1 = T_CONSTANT ;
  tmp -> token_type_2 = T_CONSTANT ;
  tmp -> int_value = atoi( s ) ;

  return tmp ;

}

SYMB *mklabel()
/* Make a temporary label that corresponds to a location in the output
   assembly  */
{

  SYMB *old_yylval = yylval.symb ;
  SYMBTABLE *curr_tab ;
  SYMB *tmp ;

  char name[12] ;
  
  top_table( &symbol_table_stack, &curr_tab ) ;

  sprintf( name, "L%d", next_label++ ) ;
  mkname( curr_tab, name, 0 ) ;

  tmp =  yylval.symb ;
  yylval.symb = old_yylval ;

  /*set the int value to the width of the current table
   *this is so we can access the top of stack of the current
   *label
   */
  tmp -> token_type_2 = T_LABEL ;
  tmp -> int_value = next_label - 1 ;

  return tmp ;

}

TAC *mktac( int op, SYMB *a, SYMB *b, SYMB *c )
/* Generate a TAC, or three address code, which is the
   primary data structure that is used to determine
   which assembly instructions to print */
{

  TAC *t = ( TAC * )safe_malloc( sizeof( TAC ) ) ;

  t -> prev = NULL ;
  t -> op = op ;
  t -> a.var = a ;
  t -> b.var = b ;
  t -> c.var = c ;
  tac_list = t ;

  return t ;

}

TAC *join_tac( TAC *c1, TAC *c2 )
/* Joins two tacs. The tac list is produced backwards in order to
   avoid stepping through long lists to insert. */
{

  TAC *t ;
  
  if( c1 == NULL )
    return c2 ;
  
  if( c2 == NULL )
    return c1 ;

  t = c2 ;

  while( t -> prev != NULL )
    t = t -> prev ;

  t -> prev = c1 ;
  return c2 ;

}

ENODE *mkenode()
/* Makes an 'enode', or expression node */
{

  return ( ENODE * )safe_malloc( sizeof( ENODE ) ) ;

}

ENODE *do_bin( int op, ENODE *op1, ENODE *op2 )
/* Generates tacs and temps for binary operations */
{
  
  ENODE *e = mkenode() ;
  SYMB *s = mktmp() ;
  TAC *t = mktac( op, s, op1 -> res, op2 -> res ) ;
  
  e -> res = s ;
  e -> tac = t ;
  e -> tac -> prev = join_tac( op1 -> tac, op2 -> tac ) ;

  /* s -> token_type_2 = T_VAR ; */
  switch( op ){
  case TAC_ADD:
    e -> res -> int_value = ( op1 -> res -> int_value ) + ( op2 -> res -> int_value ) ;
    break ;
  case TAC_SUB:
    e -> res -> int_value = ( op1 -> res -> int_value ) - ( op2 -> res -> int_value ) ;
    break ;
  case TAC_MUL:
    e -> res -> int_value = ( op1 -> res -> int_value ) * ( op2 -> res -> int_value ) ;
    break ;
  case TAC_DIV:
    //e -> res -> int_value = ( op1 -> res -> int_value ) / ( op2 -> res -> int_value ) ;
    break ;
  case TAC_MOD:
    //e -> res -> int_value = ( op1 -> res -> int_value ) % ( op2 -> res -> int_value ) ;
    break ;
  case TAC_AND:
    e -> res -> int_value = ( op1 -> res -> int_value ) && ( op2 -> res -> int_value ) ;
    break ;
  case TAC_OR:
    e -> res -> int_value = ( op1 -> res -> int_value ) || ( op2 -> res -> int_value ) ;
    break ;
    
  default: printf( "\nError in do_bin()\n" ) ;
  }

  s -> int_value = e -> res -> int_value ; 

  return e ;
  
}

ENODE *do_un( int op, ENODE *op1 )
/* Performs unary operations such as NEG, POS and NOT */
{
  
  ENODE *e = mkenode() ;
  SYMB *s = mktmp() ;
  TAC *t = mktac( op, op1 -> res, s, NULL ) ;
  
  e -> res = s ;
  e -> tac = t ;
  e -> tac -> prev = op1 -> tac ;
  
  /* s -> token_type_2 = T_VAR ; */

  switch( op ){
  case TAC_NEG:
    e -> res -> int_value = ( op1 -> res -> int_value ) * -1 ;
    break ;
  case TAC_POS:
    if( e -> res -> int_value < 0 )
      e -> res -> int_value = ( op1 -> res -> int_value ) * -1 ;
    else
      e -> res -> int_value = ( op1 -> res -> int_value ) ;
    break ;
  case TAC_NOT:
    e -> res -> int_value = !( op1 -> res -> int_value ) ;
    break;
  default: printf( "\nError in do_un()\n" ) ;
  }
  
  s -> int_value = e -> res -> int_value ;
  
  return e ;
 
}

TAC *do_assign( SYMB *var, ENODE *expr )
/* Generates tacs for assignment statements. Checks for
   assignment to non-variables */
{

  TAC *code ;

  if(( var -> token_type_2 != T_VAR ) && ( var -> token_type_2 != T_ARRAY )
     && ( var -> token_type_1 != T_SELF )){
    printf( "\nError: assignment to non-variable.\n" ) ;
  }

  /* var := array case
   *
   */
  if( expr -> offset != NULL ){
    code = mktac( TAC_VAR_ARR_CPY,( SYMB * )mktac( TAC_VAR_ARR_CPY, expr -> res, expr -> offset -> res, NULL ), var, NULL ) ;
    code -> prev = expr -> tac ;

    return code ;
  }

  code = mktac( TAC_COPY, var, expr -> res, NULL ) ;
  code -> prev = expr -> tac ;
  /* free( expr ) ; */

  return code ;

}


TAC *do_array_assign( SYMB *var, ENODE *offset, ENODE *expr )
/* For array assignments */
{

  TAC *code ;

  if(( var -> token_type_2 != T_VAR ) && ( var -> token_type_2 != T_ARRAY )){
    printf( "\nError: assignment to non-variable.\n" ) ;
  }

  /* Check if we are assigning an array to an array
   *
   */
  if(( expr -> offset != NULL )){
    //next of the C has the actual array
    code = mktac( TAC_ARR_ARR_CPY,
		( SYMB * )mktac( TAC_ARR_ARR_CPY, expr -> res,
		  expr -> offset -> res, NULL ), offset -> res, var ) ;
    code -> prev = join_tac( expr -> tac, offset -> tac ) ;
    return code ;
  }

  code = mktac( TAC_ARR_CPY, var, offset -> res, expr -> res ) ;
  code -> prev = join_tac( expr -> tac, offset -> tac ) ;
  /* free( expr ) ; */

  return code ;

}

int limit( SYMB *s, int m )
/* Calculates the number of dimensions in dimension m of array symb s */
{

  DIM_TYPE *dim_t ;
  int i ;

  //check if the dimension ( m ) is greater than the number of dimensions in s
  if( s -> dimensions < m ){
    printf( "\nError: Array out of bounds access.\n" ) ;
    exit( 0 ) ;
  }

  //set the dim_t ptr to the dim_list
  dim_t = s -> dimension_list ;
  //loop through the dimension list to find dimension m
  for( i = 1 ; i < m ; ++i ){
    if( dim_t == NULL ){
      printf( "\nError: Array out of bounds access.\n" ) ;
      exit( 0 ) ;
    }
    dim_t = dim_t -> next ;
  }

  return dim_t -> n ;

}

void print_tac_list()
/* Prints the tac list */
{

  TAC *t ;
  int tac_num = 1 ;
  t = tac_list ;

  if( tac_list == NULL )
    return ;
  
  printf( "\n\n" ) ;
  printf( "%5s%15s%7s%7s%7s\n", "TAC", "OPERATOR", "DEST", "OP1", "OP2" ) ;
  while( t != NULL ){
    printf( "%5d", tac_num++ ) ;
    printf( "%15s", print_tac_instr( t -> op ) ) ;
    if( t -> a.var != NULL )
      printf( "%7s", t -> a.var -> name ) ;
    else
      printf( "%7s", " " ) ;
    if( t -> b.var != NULL )
      printf( "%7s", t -> b.var -> name ) ;
    else
      printf( "%7s", " " ) ;
    if( t -> c.var != NULL )
      printf( "%7s", t -> c.var -> name ) ;
    else
      printf( "%7s", " " ) ;

    printf( "\n" ) ;
    
    t = t -> next ;
  }

}

char *print_tac_instr( int op )
/* Returns a string corresponding to the TAC designated by op */
{

  switch( op ){
  case TAC_UNDEF:
    return "TAC_UNDEF" ;
    break ;
  case TAC_ADD:
    return "TAC_ADD" ;
    break ;
  case TAC_SUB:
    return "TAC_SUB" ;
    break ;
  case TAC_MUL:
    return "TAC_MUL" ;
    break ;
  case TAC_DIV:
    return "TAC_DIV" ;
    break ;
  case TAC_NEG:
    return "TAC_NEG" ;
    break ;
  case TAC_COPY:
    return "TAC_COPY" ;
    break ;
  case TAC_GOTO:
    return "TAC_GOTO" ;
    break ;
  case TAC_IFZ:
    return "TAC_IFZ" ;
    break ;
  case TAC_IFNZ:
    return "TAC_IFNZ" ;
    break ;
  case TAC_ARG:
    return "TAC_ARG" ;
    break ;
  case TAC_CALL:
    return "TAC_CALL" ;
    break ;
  case TAC_RETURN:
    return "TAC_RETURN" ;
    break ;
  case TAC_MOD:
    return "TAC_MOD" ;
    break ;
  case TAC_AND:
    return "TAC_AND" ;
    break ;
  case TAC_OR:
    return "TAC_OR" ;
    break ;
  case TAC_POS:
    return "TAC_POS" ;
    break ;
  case TAC_NOT:
    return "TAC_NOT" ;
    break ;
  case TAC_ARR_CPY:
    return "TAC_ARR_CPY" ;
    break ;
  case TAC_LABEL:
    return "TAC_LABEL" ;
    break ;
  case TAC_BEGINFUNC:
    return "TAC_BEGINFUNC" ;
    break ;
  case TAC_ENDFUNC:
    return "TAC_ENDFUNC" ;
    break ;
  case TAC_PRINT:
    return "TAC_PRINT" ;
    break ;
  case TAC_HALT:
    return "TAC_HALT" ;
    break ;
  case TAC_LT:
    return "TAC_LT" ;
    break ;
  case TAC_GT:
    return "TAC_GT" ;
    break ;
  case TAC_EQ:
    return "TAC_EQ" ;
    break ;
  case TAC_NE:
    return "TAC_NE" ;
    break ;
  case TAC_GE:
    return "TAC_GE" ;
    break ;
  case TAC_LE:
    return "TAC_LE" ;
    break ;
  case TAC_ARR_ARR_CPY:
    return "TAC_ARR_ARR_CPY" ;
    break ;
  case TAC_VAR_ARR_CPY:
    return "TAC_VAR_ARR_CPY" ;
    break ;
  case TAC_PRINT_STR:
    return "TAC_PRINT_STR" ;
    break ;

  default:
    return "ERROR" ;
  }

}

TAC *do_func( SYMB *func, TAC *args, int offs )
/* Generates tacs for functions, the standard procedure for functions
   and procedures is the same as do_prog. We change the name to the name
   of the function with an L appended at the end to avoid confusion in the
   symbol table. */
{

  TAC *tlist ; //backpatch list
  
  TAC *tlab ; //label at start of function
  TAC *tbegin ; //BEGINFUNC marker

  if( func -> token_type_1 == T_UNDEF ){
    printf( "\nError: function already declared.\n" ) ;
    return NULL ;
  }

  tlab = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  tbegin = mktac( TAC_BEGINFUNC, tlab -> VA, NULL, NULL ) ;

  tlab -> VA -> token_type_1 = T_FUNC ;
  tlab -> VA -> f_value = offs ;
  func -> f_value = offs ;
  tlab -> VA -> name = ( char * )malloc( strlen( func -> name ) * sizeof( char ) + 2 ) ;
  strcpy( tlab -> VA -> name, func -> name ) ;
  tlab -> VA -> name[strlen( func -> name)] = 'L' ;
  tlab -> VA -> name[strlen( func -> name ) + 1] = '\0' ;

  tlab = join_tac( tlab, tbegin ) ;
  tlab = join_tac( tlab, args ) ;

  return tlab ;

}

TAC *do_prog( SYMB *func, TAC *args, TAC *code, int offs )
/* Generates tacs for program */
{

  TAC *tlist ; //backpatch list
  
  TAC *tlab ; //label at start of function
  TAC *tbegin ; //BEGINFUNC marker
  TAC *tend ; //ENDFUNC marker

  if( func -> token_type_1 == T_UNDEF ){
    printf( "\nError: function already declared.\n" ) ;
    return NULL ;
  }

  tlab = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  tbegin = mktac( TAC_BEGINFUNC, tlab -> VA, NULL, NULL ) ;
  tend = mktac( TAC_ENDFUNC, tlab -> VA, NULL, NULL ) ;

  tlab -> VA -> token_type_1 = T_FUNC ;
  tlab -> VA -> f_value = offs ;
  tlab -> VA -> name = ( char * )malloc( strlen( "main" ) * sizeof( char ) + 1 ) ;
  //prog will always have 'main' for a name
  strcpy( tlab -> VA -> name, "main" ) ;
  tlab -> VA -> name[strlen( "main" )] = '\0' ;

  tbegin -> prev = tlab ;
  tend -> prev = join_tac( tbegin, code ) ;

  return tend ;

}

TAC *do_test( ENODE *expr, TAC *stat1, TAC *stat2 )
/* if expr then stat1 else stat2 */
{

  TAC *label1, *label2, *code1, *code2 ;

  label1 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  label2 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  
  /* VA - label for if
   * VB - expression to test
   */
  code1 = mktac( TAC_IFZ, ( SYMB * )label1, expr -> res, NULL ) ;

  /* IF expr -> res false, goto label2
   */
  code2 = mktac( TAC_GOTO, ( SYMB * )label2, NULL, NULL ) ;

  /* join the tacs together
   */
  
  code1->prev  = expr->tac ;                      /* Join the code */
  code1        = join_tac( code1, stat1 ) ;
  code2->prev  = code1 ;
  label1->prev = code2 ;
  label1       = join_tac( label1, stat2 ) ;
  label2->prev = label1 ;

  //free( expr ) ;

  return label2 ;

}

TAC *do_while( ENODE *expr, TAC *stat )
/* Generates tacs for WHILE expr DO stat statements */
{

  TAC *label, *code ;
  
  label = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  code = mktac( TAC_GOTO, ( SYMB * )label, NULL, NULL ) ;
  
  code -> prev = stat ;
  
  return join_tac( label, do_if( expr, code ) ) ;

}

TAC *do_if( ENODE *expr, TAC *stat )
/* Generates tacs for IF expr DO stat, really only used by do_while */
{

  TAC *label, *code ; 
  label = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  code = mktac( TAC_IFZ, ( SYMB * )label, expr -> res, NULL ) ;

  code -> prev = expr -> tac ;
  code = join_tac( code, stat ) ;
  label -> prev = code ;

  //free( expr ) ;

  return label ;

}

ENODE *do_LT( ENODE *expr1, ENODE *expr2 )
/* Generates TACs for LT, should be more or less the same as other do_XX,
 * differences will be in the CG. All the relop construction functions
 * are more or less the same.
 *
 * It would be easier to compress them into one function, do_RELOP,
 * for future versions.
 *
 */
{

  SYMB *temp = mktmp() ;
  TAC *label1 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *label2 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *code1 = mktac( TAC_LT, ( SYMB * ) label2, expr1 -> res, expr2 -> res ) ;
  TAC *code2 = mktac( TAC_COPY, temp, mktmpconst( "0" ), NULL ) ;
  
  TAC *code3 = mktac( TAC_GOTO, ( SYMB * )label1, NULL, NULL ) ;
  TAC *code4 = mktac( TAC_COPY, temp, mktmpconst( "1" ), NULL ) ;

  code1 -> prev = join_tac( expr1 -> tac, expr2 -> tac ) ;
  code2 = join_tac( code1, code2 ) ;
  code3 = join_tac( code2, code3 ) ;
  label2 = join_tac( code3, label2 ) ;
  code4 = join_tac( label2, code4 ) ;
  label1 = join_tac( code4, label1 ) ;
  
  expr1 -> res = temp ;
  expr1 -> tac = label1 ;

  return expr1 ;

}

ENODE *do_GT( ENODE *expr1, ENODE *expr2 )
/* Generates TACs for GT, should be more or less the same as do_LT,
 * differences will be in the CG.
 *
 */
{

  SYMB *temp = mktmp() ;
  TAC *label1 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *label2 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *code1 = mktac( TAC_GT, ( SYMB * ) label2, expr1 -> res, expr2 -> res ) ;
  TAC *code2 = mktac( TAC_COPY, temp, mktmpconst( "0" ), NULL ) ;
  
  TAC *code3 = mktac( TAC_GOTO, ( SYMB * )label1, NULL, NULL ) ;
  TAC *code4 = mktac( TAC_COPY, temp, mktmpconst( "1" ), NULL ) ;

  code1 -> prev = join_tac( expr1 -> tac, expr2 -> tac ) ;
  code2 = join_tac( code1, code2 ) ;
  code3 = join_tac( code2, code3 ) ;
  label2 = join_tac( code3, label2 ) ;
  code4 = join_tac( label2, code4 ) ;
  label1 = join_tac( code4, label1 ) ;
  
  expr1 -> res = temp ;
  expr1 -> tac = label1 ;

  return expr1 ;

}

ENODE *do_EQ( ENODE *expr1, ENODE *expr2 )
/* Generates TACs for EQ, should be more or less the same as do_LT,
 * differences will be in the CG.
 *
 */
{

  SYMB *temp = mktmp() ;
  TAC *label1 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *label2 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *code1 = mktac( TAC_EQ, ( SYMB * ) label2, expr1 -> res, expr2 -> res ) ;
  TAC *code2 = mktac( TAC_COPY, temp, mktmpconst( "0" ), NULL ) ;
  
  TAC *code3 = mktac( TAC_GOTO, ( SYMB * )label1, NULL, NULL ) ;
  TAC *code4 = mktac( TAC_COPY, temp, mktmpconst( "1" ), NULL ) ;

  code1 -> prev = join_tac( expr1 -> tac, expr2 -> tac ) ;
  code2 = join_tac( code1, code2 ) ;
  code3 = join_tac( code2, code3 ) ;
  label2 = join_tac( code3, label2 ) ;
  code4 = join_tac( label2, code4 ) ;
  label1 = join_tac( code4, label1 ) ;
  
  expr1 -> res = temp ;
  expr1 -> tac = label1 ;

  return expr1 ;

}

ENODE *do_NE( ENODE *expr1, ENODE *expr2 )
/* Generates TACs for NE, should be more or less the same as do_LT,
 * differences will be in the CG.
 *
 */
{

  SYMB *temp = mktmp() ;
  TAC *label1 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *label2 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *code1 = mktac( TAC_NE, ( SYMB * ) label2, expr1 -> res, expr2 -> res ) ;
  TAC *code2 = mktac( TAC_COPY, temp, mktmpconst( "0" ), NULL ) ;
  
  TAC *code3 = mktac( TAC_GOTO, ( SYMB * )label1, NULL, NULL ) ;
  TAC *code4 = mktac( TAC_COPY, temp, mktmpconst( "1" ), NULL ) ;

  code1 -> prev = join_tac( expr1 -> tac, expr2 -> tac ) ;
  code2 = join_tac( code1, code2 ) ;
  code3 = join_tac( code2, code3 ) ;
  label2 = join_tac( code3, label2 ) ;
  code4 = join_tac( label2, code4 ) ;
  label1 = join_tac( code4, label1 ) ;
  
  expr1 -> res = temp ;
  expr1 -> tac = label1 ;

  return expr1 ;

}

ENODE *do_GE( ENODE *expr1, ENODE *expr2 )
/* Generates TACs for GE, should be more or less the same as do_LT,
 * differences will be in the CG.
 *
 */
{

  SYMB *temp = mktmp() ;
  TAC *label1 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *label2 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *code1 = mktac( TAC_GE, ( SYMB * ) label2, expr1 -> res, expr2 -> res ) ;
  TAC *code2 = mktac( TAC_COPY, temp, mktmpconst( "0" ), NULL ) ;
    
  TAC *code3 = mktac( TAC_GOTO, ( SYMB * )label1, NULL, NULL ) ;
  TAC *code4 = mktac( TAC_COPY, temp, mktmpconst( "1" ), NULL ) ;

  code1 -> prev = join_tac( expr1 -> tac, expr2 -> tac ) ;
  code2 = join_tac( code1, code2 ) ;
  code3 = join_tac( code2, code3 ) ;
  label2 = join_tac( code3, label2 ) ;
  code4 = join_tac( label2, code4 ) ;
  label1 = join_tac( code4, label1 ) ;
  
  expr1 -> res = temp ;
  expr1 -> tac = label1 ;

  return expr1 ;

}

ENODE *do_LE( ENODE *expr1, ENODE *expr2 )
/* Generates TACs for LE, should be more or less the same as do_LT,
 * differences will be in the CG.
 *
 */
{

  SYMB *temp = mktmp() ;
  TAC *label1 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *label2 = mktac( TAC_LABEL, mklabel( next_label ), NULL, NULL ) ;
  TAC *code1 = mktac( TAC_LE, ( SYMB * ) label2, expr1 -> res, expr2 -> res ) ;
  TAC *code2 = mktac( TAC_COPY, temp, mktmpconst( "0" ), NULL ) ;
  
  TAC *code3 = mktac( TAC_GOTO, ( SYMB * )label1, NULL, NULL ) ;
  TAC *code4 = mktac( TAC_COPY, temp, mktmpconst( "1" ), NULL ) ;

  code1 -> prev = join_tac( expr1 -> tac, expr2 -> tac ) ;
  code2 = join_tac( code1, code2 ) ;
  code3 = join_tac( code2, code3 ) ;
  label2 = join_tac( code3, label2 ) ;
  code4 = join_tac( label2, code4 ) ;
  label1 = join_tac( code4, label1 ) ;
  
  expr1 -> res = temp ;
  expr1 -> tac = label1 ;

  return expr1 ;

}
