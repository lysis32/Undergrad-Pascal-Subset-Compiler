/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* cg.c - Version .80 for Pascal Compiler, developed with      */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques.                       */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "symbol.h"
#include "cg.h"
#include "tac.h"

SYMB *temp_zer ;

/* A general note: A lot of the array calculations clutter up the code
   it would be better to modify load_reg to make them obsolete in
   future versions */

void cg()
/* Reverse the tac_list, then step through and print out
   the commented tacs, followed by the corresponding x86
   assembly instructions */
{
  TAC *tls ;
  tac_list = init_cg() ;              /* Start of TAC */
  init_stack( &cg_offset_stack ) ;     /* init the offset stack */
  tls = tac_list ;
  
  cg_sys( "header" ) ;             /* Standard header */
  cg_strings() ;
  
  for( ; tls != NULL ; tls = tls->next )  /* Instructions in turn */
    {
      printf( "#%-15s", print_tac_instr( tls -> op ) ) ;
      if( tls -> a.var != NULL )
	printf( "%7s", tls -> a.var -> name ) ;
      else
	printf( "%7s", " " ) ;
      if( tls -> b.var != NULL )
	printf( "%7s", tls -> b.var -> name ) ;
      else
	printf( "%7s", " " ) ;
      if( tls -> c.var != NULL )
	printf( "%7s\n", tls -> c.var -> name ) ;
      else
	printf( "%7s\n", " " ) ;

      cg_instr( tls ) ;
    }
  
}      /* void  cg( TAC *tl ) */

TAC *init_cg()
/* Reverse the list and fill initialize all REGS to NULL */
{
        int  r ;
        TAC *c ;                         /* Current TAC instruction */
        TAC *p ;                         /* Previous TAC instruction */

        for( r = 0 ; r < R_MAX ; r++ )
                rdesc[r].name = NULL ;

        tos      = VAR_OFF ;             /* TOS allows space for link info */
        next_arg = -4 ;                   /* Next arg to load */
	frame_width = 0 ;
	array_flag = 0 ;
        /* Tidy up and reverse the code list */

        c = NULL ;                       /* No current */
        p = tac_list ;                         /* Preceding to do */

        while( p != NULL )
        {
                p->next = c ;            /* Set the next field */
                c       = p ;            /* Step on */
                p       = p->prev ;
        }

        return c ;

}       /* TAC *init_cg( TAC *tl ) */


void  cg_instr( TAC *c )
/* Generate code for a single TAC instruction. This is just a switch on all
   possible TAC instructions. Hopefully if we have written the front end
   correctly the default case will never be encountered. For most cases we just
   call a subsidiary routine "cg_xxx()" to do the code generation. */
{
  switch( c->op )
    {
    case TAC_UNDEF:
      
      error( "cannot translate TAC_UNDEF" ) ;
      return ;
      
    case TAC_ADD:
      
      cg_bin( "addl", c->VA, c->VB, c->VC ) ;
      return ;

    case TAC_AND:
      
      cg_bin( "and", c->VA, c->VB, c->VC ) ;
      return ;

    case TAC_OR:
      
      cg_bin( "or", c->VA, c->VB, c->VC ) ;
      return ;

    case TAC_NOT:
      
      cg_un( "not", c->VA, NULL ) ;
      return ;

    case TAC_POS:
      /* not used in this ver */
      cg_un( "pos", c->VA, c -> VB ) ;
      return ;

    case TAC_SUB:
      
      cg_bin( "subl", c->VA, c->VB, c->VC ) ;
      return ;
      
    case TAC_MUL:
      
      cg_bin( "imul", c->VA, c->VB, c->VC ) ;
      return ;
    
    case TAC_MOD:
      /* same as idiv but the remainder is in the DX */
      cg_bin( "mod", c->VA, c->VB, c->VC ) ;
      return ;

    case TAC_DIV:

      cg_bin( "idiv", c->VA, c->VB, c->VC ) ;
      return ;
      
    case TAC_NEG:
      
      cg_un( "neg", c -> VA, c -> VB ) ;
      return ;
      
    case TAC_COPY:
      
      cg_copy( c->VA, c->VB ) ;
      return ;

    case TAC_ARR_CPY:
      cg_arr_cpy( c -> VA, c -> VB, c -> VC ) ;
      return ;

    case TAC_ARR_ARR_CPY:
      cg_arr_arr_cpy( c -> LA, c -> VB, c -> VC ) ;
      return ;

    case TAC_VAR_ARR_CPY:
      cg_var_arr_cpy( c -> LA, c -> VB, c -> VC ) ;
      return ;
      
    case TAC_GOTO:
      
      cg_cond( "jmp", NULL, c->LA->VA->VAL1 ) ;
      return ;
      
    case TAC_IFZ:
      
      cg_cond( "jz", c->VB, c->LA->VA->VAL1 ) ;
      return ;
      
    case TAC_IFNZ:
      
      cg_cond( "jnz", c->VB, c->LA->VA->VAL1 ) ;
      return ;
   
    case TAC_PRINT_STR:
      cg_str( c -> VA ) ;
      return ;

    case TAC_LT:
      
      cg_relop( c -> op, c -> LA -> VA -> VAL1, c -> VB, c -> VC ) ;
      return ;
      
    case TAC_GT:
      
      cg_relop( c -> op, c -> LA -> VA -> VAL1, c -> VB, c -> VC ) ;
      return ;
      
    case TAC_LE:
      
      cg_relop( c -> op, c -> LA -> VA -> VAL1, c -> VB, c -> VC ) ;
      return ;
      
    case TAC_GE:
      
      cg_relop( c -> op, c -> LA -> VA -> VAL1, c -> VB, c -> VC ) ;
      return ;
      
    case TAC_NE:
      
      cg_relop( c -> op, c -> LA -> VA -> VAL1, c -> VB, c -> VC ) ;
      return ;
      
    case TAC_EQ:
      
      cg_relop( c -> op, c -> LA -> VA -> VAL1, c -> VB, c -> VC ) ;
      return ;
      
    case TAC_ARG:
      
      cg_arg( c->VA ) ;
      return ;
      
    case TAC_CALL:
      
      flush_all() ;
      cg_call( c->VA , c->VB ) ;
      return ;
      
    case TAC_RETURN:
      
      cg_return( c->VA ) ;
      return ;
      
    case TAC_LABEL:
      
      /* We generate an appropriate label. Note that we must flush
	 the register descriptor, since control may arrive at this
	 label from other points in the code. */
      
      flush_all() ;
      if( c -> VA -> name == NULL )
	printf( "L%u:\n", c->VA->VAL1 ) ;
      else
	printf( "%s:\n", c -> VA -> name ) ;

      return ;
      
    case TAC_VAR:
      
      /* Allocate 4 bytes for this variable to hold an integer on the
	 current top of stack */
      
      c->VA->ADDR2 = tos ;
      tos += 4 ;
      return ;
      
    case TAC_BEGINFUNC:
      
      /* At the start of a function we set the top of
	 stack to the width of the new function and push
	 the old offset onto the offset stack. */
      
      frame_width = c -> VA -> f_value ;
      push_offset( &cg_offset_stack, frame_width ) ;
      printf( "       pushl %s\n", get_xreg( R_EBP )) ;
      printf( "       movl %s, %s\n", get_xreg( R_ESP ), get_xreg( R_EBP )) ;
      return ;
      
    case TAC_ENDFUNC:
      
      /* At the end of the function we put in an implicit return
	 instruction. We pop the offset stack and reset the frame width. */
      
      cg_return( NULL ) ;
      pop_offset( &cg_offset_stack, &frame_width ) ;
      return ;
      
    case TAC_PRINT:
      if( c -> VA -> token_type_2 == T_ARRAY )
	cg_arr_print( c -> VA, c -> VB ) ;
      else
	cg_print( c -> VA ) ;
      return ;

    case TAC_HALT:
      return ;
      
    default:
      
      /* Don't know what this one is */
      
      error( "unknown TAC opcode to translate" ) ;
      return ;
    }
  
}       /* void  cg_instr( TAC *c ) */


void  cg_bin( char *op,                  /* Opcode to use */
              SYMB *a,                   /* Result */
              SYMB *b,                   /* Operands */
              SYMB *c )
/* Generate code for a binary operator

      a := b op c

   This is a typical code generation functions. We find and load a separate
   register for each argument, the second argument also being used for the
   result. We then generate the code for binary operator, updating the register
   descriptor appropriately. */
{

  int br, cr ;

  /* check to see if any of our operands are arrays and perform actions
     accordingly */
  if( b -> token_type_2 == T_ARRAY && c -> token_type_2 == T_ARRAY ){
    /* reserve two registers */
    spill_one( R_ECX ) ;
    clear_desc( R_ECX ) ;
    spill_one( R_ESI ) ;
    clear_desc( R_ESI ) ;

    /* b -> next -> offset is base of array, b -> next -> c is c,
       b -> f_value is calculated offset */
    printf( "       movl  %d(%s), %s\n", ( 0 - tos - ( b -> next -> offset ) + ( b -> next -> c ) 
	    - ( b -> f_value )), get_xreg( R_EBP ), get_xreg( R_ECX )) ;
    /* same deal here, just moving into ESI */
    printf( "       movl  %d(%s), %s\n", 
          ( 0 - tos - ( c -> next -> offset ) + ( c -> next -> c )
            - ( c -> f_value )), get_xreg( R_EBP ), get_xreg( R_ESI )) ;
    /* no need to set modified because these can be overwritten */
    array_flag = 1 ;
    /* check for the absence of special instructions */
    if(( strcmp( op, "idiv" ) == 1 ) && ( strcmp( op, "mod" ) == 1 )){
      printf( "      %s  %s, %s\n", op, get_xreg( R_ECX ), 
	      get_xreg( R_ESI )) ;
      insert_desc( R_ESI, a, MODIFIED ) ;
      array_flag = 0 ;
      return ;
    }

  }
  if( b -> token_type_2 == T_ARRAY && c -> token_type_2 != T_ARRAY && 
      c -> token_type_2 != T_FUNC && c -> token_type_1 != T_SELF ){
    /* reserve two registers */
    spill_one( R_ECX ) ;
    clear_desc( R_ECX ) ;
    spill_one( R_ESI ) ;
    clear_desc( R_ESI ) ;

    /* b -> next -> offset is base of array, b -> next -> c is c,
       b -> f_value is calculated offset */
    printf( "       movl  %d(%s), %s\n", ( 0 - tos - ( b -> next -> offset ) + ( b -> next -> c ) 
	    - ( b -> f_value )), get_xreg( R_EBP ), get_xreg( R_ECX )) ;
    /* move non_array symb into ESI */
    load_reg( R_ESI, c ) ;
    /* no need to set modified because these can be overwritten */
    array_flag = 1 ;
    /* check for the absence of special instructions */
    if(( strcmp( op, "idiv" ) == 1 ) && ( strcmp( op, "mod" ) == 1 )){
      printf( "      %s  %s, %s\n", op, get_xreg( R_ECX ), 
	      get_xreg( R_ESI )) ;
      insert_desc( R_ESI, a, MODIFIED ) ;
      array_flag = 0 ;
      return ;
    }
  }

  if( b -> token_type_2 != T_ARRAY && c -> token_type_2 == T_ARRAY && 
      b -> token_type_2 != T_FUNC && b -> token_type_1 != T_SELF  ){
    /* reserve two registers */
    spill_one( R_ECX ) ;
    clear_desc( R_ECX ) ;
    spill_one( R_ESI ) ;
    clear_desc( R_ESI ) ;
    
    /* move non-array symb into ECX */
    load_reg( R_ECX, b ) ;
    
    /* c -> next -> offset is base of array, c -> next -> c is c,
       c -> f_value is calculated offset */
    printf( "       movl  %d(%s), %s\n", ( 0 - tos - ( c -> next -> offset ) + ( c -> next -> c ) 
					   - ( c -> f_value )), get_xreg( R_EBP ), get_xreg( R_ESI )) ;
    /* no need to set modified because these can be overwritten */
    array_flag = 1 ;
    /* check for the absence of special instructions */
    if(( strcmp( op, "idiv" ) == 1 ) && ( strcmp( op, "mod" ) == 1 )){
      printf( "      %s  %s, %s\n", op, get_xreg( R_ECX ), 
	      get_xreg( R_ESI )) ;
      insert_desc( R_ESI, a, MODIFIED ) ;
      array_flag = 0 ;
      return ;
    }
  }

  /*special case for div instr
  format idiv src, divides DX:AX by src
  quotient in eax
  remainder in edx
  quotient sral 31 in edx */
  if( strcmp( op, "idiv" ) == 0 ){
    spill_one( R_EAX ) ; // spill current eax and edx
    clear_desc( R_EAX ) ;
    spill_one( R_EDX ) ;
    clear_desc( R_EDX ) ;

    /* array_flag == 1 indicates that we have arrays, b is already in ECX
      and c is already in ESI*/
    if( array_flag == 1 ){
      /* move ECX into EAX and EDX */
      printf( "       movl  %s, %s\n", get_xreg( R_ECX ), get_xreg( R_EAX )) ;
      printf( "       movl  %s, %s\n", get_xreg( R_ECX ), get_xreg( R_EDX )) ;
      printf( "       sar   $%d, %s\n", 31, get_xreg( R_EDX )) ;
      printf( "       idiv  %s\n", get_xreg( R_ESI )) ;

      /* eax has the quotient */
      insert_desc( R_EAX, a, MODIFIED ) ;
      array_flag = 0 ;
      return ;
    }
    spill_one( R_EBX ) ;
    clear_desc( R_EBX ) ;

    load_reg( R_EAX, b ) ;
    load_reg( R_EDX, b ) ;
    load_reg( R_EBX, c ) ;
    /* we are dividing by a whole word, so EAX must contain the upper byte
       and EDX the lower byte */
    printf( "       %s   $%d, %s\n", "sar", 31, get_xreg( R_EDX )) ;
    printf( "       %s  %s\n", op, get_xreg( R_EBX )) ;

    insert_desc( R_EAX, a, MODIFIED ) ;

    return ;
  }

  /* special case for modulus, uses idiv instruction */
  if( strcmp( op, "mod" ) == 0 ){
    spill_one( R_EAX ) ; // spill current eax and edx and bx
    clear_desc( R_EAX ) ;
    spill_one( R_EDX ) ;
    clear_desc( R_EDX ) ;

   /* array_flag == 1 indicates that we have arrays, b is already in ECX
      and c is already in ESI*/
    if( array_flag == 1 ){
      /* move ECX into EAX and EDX */
      printf( "       movl  %s, %s\n", get_xreg( R_ECX ), get_xreg( R_EAX )) ;
      printf( "       movl  %s, %s\n", get_xreg( R_ECX ), get_xreg( R_EDX )) ;
      printf( "       sar   $%d, %s\n", 31, get_xreg( R_EDX )) ;
      printf( "       idiv  %s\n", get_xreg( R_ESI )) ;

      /* eax has the quotient */
      insert_desc( R_EDX, a, MODIFIED ) ;
      array_flag = 0 ;
      return ;
    }

    spill_one( R_EBX ) ;
    clear_desc( R_EBX ) ;

    load_reg( R_EAX, b ) ;
    load_reg( R_EDX, b ) ;
    load_reg( R_EBX, c ) ;
    /* we are dividing by a whole word, so EAX must contain the upper byte
       and EDX the lower byte */
    printf( "       %s   $%d, %s\n", "sar", 31, get_xreg( R_EDX )) ;
    printf( "       %s  %s\n", "idiv", get_xreg( R_EBX )) ;

    insert_desc( R_EDX, a, MODIFIED ) ;

    return ;
  }

  /* for handling arrays */
  /* flag = 1 indicates array args */
  if( array_flag == 1 ){
    printf( "       %s  %s, %s\n", op, get_xreg( R_ECX ), get_xreg( R_ESI )) ;
    insert_desc( R_ESI, a, MODIFIED ) ;
    array_flag = 0 ;
    return ;
  }

  /* operations on two registers must be reversed with subl */
  if( strcmp( op, "subl" ) == 0 ){

    br = get_rreg( c ) ;        /* Result register */
    cr = get_areg( b, br ) ;    /* Second argument register */
  
    printf( "       %s  %s, %s\n", op, get_xreg( br ), get_xreg( cr )) ;
    
    /* Delete c from the descriptors and insert a */
    
    clear_desc( cr ) ;
    insert_desc( cr, a, MODIFIED ) ;
    return ;
    
  }

  /* for standard instructions */
  
  cr = get_rreg( c ) ;        /* Result register */
  br = get_areg( b, cr ) ;    /* Second argument register */

  printf( "       %s  %s, %s\n", op, get_xreg( br ), get_xreg( cr )) ;
  
  /* Delete c from the descriptors and insert a */
  
  clear_desc( cr ) ;
  insert_desc( cr, a, MODIFIED ) ;

}       /* void  cg_bin( char *op,
                         SYMB *a,
                         SYMB *b,
                         SYMB *c ) */

/* For unary operations such as neg, NOT, etc */
void cg_un( char *op, SYMB *a, SYMB *b ){

  int ar, br ;

  /* Ignore this code, apparently POS or unary + is not in the grammar */
  if( strcmp( op, "pos" ) == 0 ){
    ar = get_rreg( a ) ;
    insert_desc( ar, a, MODIFIED ) ;
    
    printf( "       %s  $%d, %s\n", "movl", 
	    ( a -> int_value ) * -1, get_xreg( ar )) ;
    
    spill_one( ar ) ;
    clear_desc( ar ) ;
    
    return ;

  }

  if( a -> token_type_2 == T_ARRAY )
    array_flag = 1 ;

  /* For some reason both the neg operation and subtracting from zero
     do not generate twos complement numbers, this is a temporary fix */
  if( strcmp( op, "neg" ) == 0 ){
    if( array_flag == 0 )
      ar = get_rreg( a ) ;
    else{
      /* special case for arrays */
      spill_one( R_EBX ) ;
      clear_desc( R_EBX ) ;
      spill_one( R_ECX ) ;
      clear_desc( R_ECX ) ;
      printf( "       movl  %d(%s), %s\n", 
	      ( 0 - tos - ( a -> next -> offset ) + ( a -> next -> c ) 
		- ( a -> f_value )), get_xreg( R_EBP ), get_xreg( R_EBX )) ;
      printf( "       neg   %s\n", get_xreg( R_EBX )) ;
      printf( "       movl  %s,  %s\n", get_xreg( R_EBX ), get_xreg( R_ECX )) ;
      
      insert_desc( R_ECX, b, MODIFIED ) ;
      spill_one( R_ECX ) ;
      clear_desc( R_ECX ) ;
      array_flag = 0 ;
      return ;

    }
    br = get_areg( b, ar ) ;
    
    printf( "       %s   %s\n", "neg", get_xreg( ar )) ;
    printf( "       %s  %s, %s\n", "movl", get_xreg( ar ), get_xreg( br ));
    
    insert_desc( br, b, MODIFIED ) ;
    spill_one( br ) ;
    clear_desc( br ) ;
    
    return ;

  }
  ar = get_rreg( a ) ;
  insert_desc( ar, a, MODIFIED ) ;
  
  printf( "       %s  %s\n", op, get_xreg( ar )) ;
  
  spill_one( ar ) ;
  clear_desc( ar ) ;
  
  array_flag = 0 ;

}

void  cg_copy( SYMB *a,
               SYMB *b )
/* Generate code for a copy instruction

      a := b

   We load b into an register, then update the descriptors to indicate that a
   is also in that register. We need not do the store until the register is
   spilled or flushed. */
{
  /* If copying into a SELF, we are returning, load the symb b into
     eax for return. This code may be obsolete. */
  if( a -> token_type_1 == T_SELF )
    {
      spill_one( R_EAX ) ;
      load_reg( R_EAX, b ) ;
      /* indicate unmodified, we don't want to spill to a bad location */
      insert_desc( R_EAX, a, UNMODIFIED ) ;
      return ;
    }
  
  if( b -> token_type_2 == T_FUNC ){
    insert_desc( R_EAX, a, MODIFIED ) ;
    return ;
  }
  
  int  br = get_rreg( b ) ;        /* Load b into a register */
    
  insert_desc( br, a, MODIFIED ) ; /* Indicate a is there */

}       /* void  cg_copy( SYMB *a,
                          SYMB *b ) */
 
void cg_arr_cpy( SYMB *a, SYMB *b, SYMB *c )
/* For array[..] := var
 *
 * a = C constant temp
 * a -> next = array symb
 * b = calculated offset of array
 * c = var
 *
 */
{

  int br, cr ;

  spill_one( R_EDX ) ;
  clear_desc( R_EDX ) ;
  load_reg( R_EDX, c ) ;
  insert_desc( R_EDX, c, MODIFIED ) ;

  printf( "       %s  %s, %d(%s)\n", "movl", get_xreg( R_EDX ), 
	  ( 0 - VAR_OFF - ( a -> next -> offset ) + ( a -> next -> c ) - ( b -> int_value )), 
	  get_xreg( R_EBP ));

  return ;

}

void cg_arr_arr_cpy( TAC *a, SYMB *b, SYMB *c )
/* For array[..] := array[..]
 *
 * a -> VA = C value of array on RHS
 * a -> VA -> next = array
 * a -> VB -> int_value = calculated offset of subscript
 * a -> VC = NULL
 * b = calculated offset of target array
 * c = C constant temp
 * c -> next = array symb
 *
 */
{

  spill_one( R_EDX ) ;
  clear_desc( R_EDX ) ;

  printf( "       %s  %d(%s), %s\n", "movl", 
	  ( 0 - VAR_OFF - ( a -> VA -> next -> offset ) + ( a -> VA -> next -> c )
	    - ( a -> VB -> int_value )), get_xreg( R_EBP ), get_xreg( R_EDX )) ;   
  printf( "       %s  %s, %d(%s)\n", "movl", get_xreg( R_EDX ), 
	  ( 0 - VAR_OFF - ( c -> next -> offset ) + ( c -> next -> c ) - ( b -> int_value )), 
	  get_xreg( R_EBP ));

  

  return ;

}

void cg_var_arr_cpy( TAC *a, SYMB *b, SYMB *c )
/* For var := array[..]
 *
 * a -> VA = C value of array on RHS
 * a -> VA -> next = array symb
 * a -> VB -> int_value = calculated offset of subscript
 * a -> VC = NULL
 * b = var symb
 * c = NULL
 *
 */
{

  spill_one( R_EDX ) ;
  clear_desc( R_EDX ) ;

  printf( "       %s  %d(%s), %s\n", "movl", 
	  ( 0 - VAR_OFF - ( a -> VA -> next -> offset ) + ( a -> VA -> next -> c )
	    - ( a -> VB -> int_value )), get_xreg( R_EBP ), get_xreg( R_EDX )) ;   
  printf( "       %s  %s, %d(%s)\n", "movl", get_xreg( R_EDX ),
	( 0 - VAR_OFF - b -> offset ), get_xreg( R_EBP ));

  return ;

}

void  cg_cond( char *op,
               SYMB *a,                  /* Condition */
               int   l )                 /* Branch destination */
/* Generate for "goto", "ifz" or "ifnz". We must spill registers before the
   branch. In the case of unconditional goto we have no condition, and so "b"
   is NULL. We set the condition flags if necessary by explicitly loading "a"
   into a register to ensure the zero flag is set. A better approach would be
   to keep track of what is in the status register, so saving this load. */
{
  flush_all() ;
  
  if( a != NULL )
    {
      int  cr, br ;

      cr = get_rreg( a ) ;
      
      spill_one( R_EBX ) ;
      printf( "       movl  $0, %s\n", get_xreg( R_EBX )) ;
      printf( "       cmp   %s, %s\n", get_xreg( cr ), get_xreg( R_EBX )) ;
      
    }
  
  printf( "       %s  L%u\n", op, l ) ;   /* Branch */
  
}       /* void  cg_cond( char *op,
                          SYMB *a,
                          int   l ) */

void cg_relop( int op, int lab, SYMB *b, SYMB *c )
/* Generates code for all RELOPs, namely GT, LT, etc.
 * Note that the CG action taken does not always necessarily
 * reflect the nature of the tac.
 */
{
 
  int br, cr ;

  flush_all() ;

  /* check to see if any of our operands are arrays and perform actions
     accordingly */
  if( b -> token_type_2 == T_ARRAY && c -> token_type_2 == T_ARRAY ){
    /* reserve two registers */
    spill_one( R_ECX ) ;
    clear_desc( R_ECX ) ;
    spill_one( R_ESI ) ;
    clear_desc( R_ESI ) ;

    /* b -> next -> offset is base of array, b -> next -> c is c,
       b -> f_value is calculated offset */
    printf( "       movl  %d(%s), %s\n", 
	    ( 0 - tos - ( b -> next -> offset ) + ( b -> next -> c ) 
	    - ( b -> f_value )), get_xreg( R_EBP ), get_xreg( R_ECX )) ;
    /* same deal here, just moving into ESI */
    printf( "       movl  %d(%s), %s\n", 
          ( 0 - tos - ( c -> next -> offset ) + ( c -> next -> c )
            - ( c -> f_value )), get_xreg( R_EBP ), get_xreg( R_ESI )) ;
    /* no need to set modified because these can be overwritten */
    array_flag = 1 ;

  }
  if( b -> token_type_2 == T_ARRAY && c -> token_type_2 != T_ARRAY && 
      c -> token_type_2 != T_FUNC && c -> token_type_1 != T_SELF ){
    /* reserve two registers */
    spill_one( R_ECX ) ;
    clear_desc( R_ECX ) ;
    spill_one( R_ESI ) ;
    clear_desc( R_ESI ) ;

    /* b -> next -> offset is base of array, b -> next -> c is c,
       b -> f_value is calculated offset */
    printf( "       movl  %d(%s), %s\n", 
	    ( 0 - tos - ( b -> next -> offset ) + ( b -> next -> c ) 
	      - ( b -> f_value )), get_xreg( R_EBP ), get_xreg( R_ECX )) ;
    /* move non_array symb into ESI */
    load_reg( R_ESI, c ) ;
    /* no need to set modified because these can be overwritten */
    array_flag = 1 ;

  }

  if( b -> token_type_2 != T_ARRAY && c -> token_type_2 == T_ARRAY && 
      b -> token_type_2 != T_FUNC && b -> token_type_1 != T_SELF  ){
    /* reserve two registers */
    spill_one( R_ECX ) ;
    clear_desc( R_ECX ) ;
    spill_one( R_ESI ) ;
    clear_desc( R_ESI ) ;
    
    /* move non-array symb into ECX */
    load_reg( R_ECX, b ) ;
    
    /* c -> next -> offset is base of array, c -> next -> c is c,
       c -> f_value is calculated offset */
    printf( "       movl  %d(%s), %s\n", 
	    ( 0 - tos - ( c -> next -> offset ) + ( c -> next -> c ) 
	      - ( c -> f_value )), get_xreg( R_EBP ), get_xreg( R_ESI )) ;
    /* no need to set modified because these can be overwritten */
    array_flag = 1 ;
  }
  
  if( array_flag == 0 ){
    cr = get_rreg( b ) ;
    br = get_areg( c, br ) ;
  }else{
    cr = R_ECX ;
    br = R_ESI ;
    array_flag = 0 ;
  }

  switch( op ){
  case TAC_LT:
    printf( "       cmp  %s, %s\n", get_xreg( cr ), get_xreg( br )) ;
    printf( "       jg   L%u\n", lab ) ;
    return ;
  case TAC_GT:
    printf( "       cmp  %s, %s\n", get_xreg( cr ), get_xreg( br )) ;
    printf( "       jl   L%u\n", lab ) ;
    return ;
  case TAC_EQ:
    printf( "       cmp  %s, %s\n", get_xreg( cr ), get_xreg( br )) ;
    printf( "       je  L%u\n", lab ) ;
    return ;
  case TAC_NE:
    printf( "       cmp  %s, %s\n", get_xreg( cr ), get_xreg( br )) ;
    printf( "       jne   L%u\n", lab ) ;
    return ;
  case TAC_GE:
    printf( "       cmp  %s, %s\n", get_xreg( cr ), get_xreg( br )) ;
    printf( "       jle  L%u\n", lab ) ;
    return ;
  case TAC_LE:
    printf( "       cmp  %s, %s\n", get_xreg( cr ), get_xreg( br )) ;
    printf( "       jge  L%u\n", lab ) ;
    return ;
  }

}

void cg_arg( SYMB *a )
/* Generate for an ARG instruction. We load the argument into a register, and
   then write it onto the new stack frame, which is 1 word past the current
   top of stack. We keep track of which arg this is in the global variable
   "next_arg". We assume that ARG instructions are always followed by other ARG
   instructions or CALL instructions. */
{
  /* Flush all as we are leaving the scope */
  flush_all() ;
  
  int  r  = get_rreg( a ) ;

  printf( "       movl  %s, %d(%s)\n", get_xreg( r ), 0 - frame_width - VAR_OFF - next_arg,
	  get_xreg( R_EBP )) ;
  next_arg += 4 ;

}       /* void  cg_arg( SYMB *a ) */

void  cg_call( SYMB *res,
               SYMB *func )
/* The standard call sequence is

   push args...
   pushl %ebp
   movl  %esp, %ebp

   We flush out the registers prior to a call and then execute the standard
   CALL sequence. Flushing involves spilling modified registers, and then
   clearing the register descriptors. After the call if there is a result it will be in R_EAX
   so enter this in the descriptors.  We reset "next_arg" before the call,
   since we know we have finished all the arguments now. */
{
  
  int result_reg ;
  
  flush_all() ;
  /* Move the stack pointer to the top of the current frame, past the arguments. */
  printf( "       subl  $%d, %s\n", (0 + VAR_OFF + frame_width + next_arg - 4 ), get_xreg( R_ESP )) ;
  printf( "       call  %sL\n", func -> name ) ;
  next_arg = 0 ;
  /* Do a result if there is one */
  if( res != NULL ){                     
    insert_desc( R_EAX, res, MODIFIED ) ;
    spill_one( R_EAX ) ;
    clear_desc( R_EAX ) ;
  }

}       /* void  cg_call( int   f,
                          SYMB *res ) */

void  cg_return( SYMB *a )
/* The standard return sequence is

   leave
   ret

   If "a" is NULL we don't load anything into the result register.
*/
{
        if( a != NULL )
        {
                spill_one( R_EAX ) ;
                load_reg( R_EAX, a ) ;
        }
	
	printf( "       leave\n" ) ;
	printf( "       ret\n" ) ;

}       /* void  cg_return( SYMB *a ) */


void  cg_sys( char *fn )                 /* File name */
/* This routine is used to copy standard header and library files into the
   generated code. */
{
        FILE *fd = fopen( fn, "r" ) ; /* The library file */
        int  c ;

        if( fd == NULL )
        {
                error( "cannot open system file" ) ;
                exit( 0 ) ;
        }

        while((c = getc( fd )) != EOF )
                putchar( c ) ;

        fclose( fd ) ;

}       /* void  cg_sys( char *fn ) */

void  cg_strings()
/* cg_strings() places all the strings at the start of the file. Strings
   are located in the global temporary file 'strings.txt', which is
   initialized in main with w+ and then unlinked. */
{
        int  c ;

	rewind( strings ) ;

        while((c = getc( strings )) != EOF )
                putchar( c ) ;

        fclose( strings ) ;

}

/* These are the support routines for the code generation. "flush_all()' is
   used to write all modified registers and clear the registers at points where
   their validity can not be guaranteed (after labels and function calls).
   "spill_all()" is used to write all modified registers at points where we
   wish memory to be consistent (prior to a branch).  "spill_one()" is used to
   write a specific register out if it is modified. */

void  flush_all( void )
/* Spill all registers, and clear their descriptors. Although we don't spill
   the result register (it is only set on a return, and therefore never needs
   saving for future use), we do clear its descriptor! */
{
        int  r ;

        spill_all() ;

        for( r = R_GEN ; r < R_MAX ; r++ )   /* Clear the descriptors */
                clear_desc( r ) ;

        clear_desc( R_EAX ) ;                /* Clear result register */

}       /* void  flush_all( void ) */

void  spill_all( void )
/* Spill all the registers */
{
        int  r ;

        for( r = R_GEN ; r < R_MAX ; r++ )
                spill_one( r ) ;

}       /* spill_all( void ) */

void  spill_one( int  r )
/* Spill the value in register r if there is one and it's modifed */
{
        if( (rdesc[r].name != NULL) && rdesc[r].modified )
        {
	  if( rdesc[r].name -> token_type_1 == T_PARAM ){
	    printf( "       movl  %s, %d(%s)\n", get_xreg( r ), (rdesc[r].name->ADDR2)*-1, get_xreg( R_EBP ));
	    rdesc[r].modified = UNMODIFIED ;
	    return ;
	  }
	  printf( "       movl  %s, %d(%s)\n", get_xreg( r ), (rdesc[r].name->ADDR2 + tos)*-1,
		  get_xreg( R_EBP )) ;
	  rdesc[r].modified = UNMODIFIED ;
        }

}       /* void  spill_one( int  r ) */

void  load_reg( int   r,                 /* Register to be loaded */
                SYMB *n )                /* Name to load */
/* "load_reg()" loads a value into a register.

   We update the register descriptor accordingly */
{
        int  s ;

        /* Look for a register */
        for( s = 0 ; s < R_MAX ; s++ )  
	  if( rdesc[s].name == n )
	    {
	      printf( "       movl  %s, %s\n", get_xreg( s ), get_xreg( r )) ;
	      insert_desc( r, n, rdesc[s].modified ) ;
	      return ;
	    }
	
        /* Not in a reg. Load appropriately */
	
	/* For parameters, parameters have a positive offset from the EBP */
	if( n -> token_type_1 == T_PARAM ){
	  printf( "       movl  %d(%s), %s\n", ( 0 - n -> offset ), get_xreg( R_EBP ), get_xreg( r ));
	  insert_desc( r, n, UNMODIFIED ) ;
	  return ;
	}
        switch( n->token_type_2 )
	  {
	  case T_CONSTANT:
	    printf( "       movl  $%d, %s\n", n->VAL1, get_xreg( r ) ) ;
	    break ;	    
	  case T_VAR:
	    printf( "       movl  %d(%s), %s\n", ( 0 - VAR_OFF - n -> offset ), get_xreg( R_EBP ), get_xreg( r )) ;
	    break ;
	  case T_TEXT:
	    printf( "       movl  %s, %s\n", get_xreg( n->VAL2 ), get_xreg( r )) ;
	    break ;
	  case T_ARRAY:
	    printf( "       movl  %d(%s), %s\n", ( 0 - VAR_OFF - n -> offset ), get_xreg( R_EBP ), get_xreg( r )) ;
	    break ;
	  }
	
        insert_desc( r, n, UNMODIFIED ) ;

}       /* void  load_reg( int   r,
                           SYMB *n ) */


/* We have three routines to handle the register descriptor. "clear_desc()"
   removes any slave information in a register, "insert_desc()" inserts slave
   information. */

void  clear_desc( int   r )              /* Register to delete */
/* Clear the descriptor for register r */
{
        rdesc[r].name = NULL ;

}       /* void  clear_desc( int   r ) */

void  insert_desc( int   r,
                   SYMB *n,
                   int   mod )
/* Insert a descriptor entry for the given name. First as a precaution delete
   it from any existing descriptor. */
{
        int  or ;                       /* Old register counter */

        /* Search through each register in turn looking for "n". There should
           be at most one of these. */

        for( or = R_GEN ; or < R_MAX ; or++ )
        {
                if( rdesc[or].name == n )
                {
                        /* Found it, clear it and break out of the loop. */

                        clear_desc( or ) ;
                        break ;
                }
        }

        /* We should not find any duplicates, but check, just in case. */

        for( or++ ; or < R_MAX ; or++ )
        
                if( rdesc[or].name == n )
                {
                        error( "Duplicate slave found" ) ;
                        clear_desc( or ) ;
                }

        /* Finally insert the name in the new descriptor */

        rdesc[r].name     = n ;
        rdesc[r].modified = mod ;

}       /* void  insert_desc( int   r,
                              SYMB *n,
                              int   mod ) */


/* These two routines implement the simple register allocation algorithm
   described in chapter 10. "get_rreg()" gets a register that will hold an
   operand and be overwritten by a result. "get_areg()" gets a register that
   will hold an operand that will no be overwritten. */

int  get_rreg( SYMB *c )
/* Get a register to hold the result of the computation

      a := b op c

   This must initially hold c and will be overwritten with a. If c is already
   in a register we use that, spilling it first if necessary, otherwise we
   chose in order of preference from

      An empty register
      An unmodified register
      A modified register

   In the last case we spill the contents of the register before it is used. If
   c is not in the given result register we load it. We avoid using the
   reserved registers. Note that since c may be the same as b we must update
   the address and register descriptors. */ 
{
        int        r ;                   /* Register for counting */

        for( r = R_GEN ; r < R_MAX ; r++ )   /* Already in a register */
                if( rdesc[r].name == c )
                {
                        spill_one( r ) ;
                        return r ;
                }

        for( r = R_GEN ; r < R_MAX ; r++ )
                if( rdesc[r].name == NULL )  /* Empty register */
                {
                        load_reg( r, c ) ;
                        return r ;
                }

        for( r = R_GEN ; r < R_MAX ; r++ )
                if( !rdesc[r].modified )     /* Unmodifed register */
                {
                        clear_desc( r ) ;
                        load_reg( r, c ) ;
                        return r ;
                }

        spill_one( R_GEN ) ;                 /* Modified register */
        clear_desc( R_GEN ) ;
        load_reg( R_GEN, c ) ;
        return R_GEN ;

}       /* int  get_rreg( SYMB *c ) */


int  get_areg( SYMB *b,
               int   cr )                /* Register already holding b */
/* Get a register to hold the second argument of the computation

      a := b op c

   This must hold b and will not be overwritten. If b is already in a register
   we use that, otherwise we chose in order of preference from 

      An empty register
      An unmodified register
      A modified register

   In the last case we spill the contents of the register before it is used. If
   b is not in the given argument register we load it.
   We may not use cr unless it already contains b. */
{
        int        r ;                   /* Register for counting */

        for( r = R_EBP ; r < R_MAX ; r++ )
                if( rdesc[r].name == b )              /* Already in register */
                        return r ;

        for( r = R_GEN ; r < R_MAX ; r++ )
                if( rdesc[r].name == NULL )           /* Empty register */
                {
                        load_reg( r, b ) ;
                        return r ;
                }

        for( r = R_GEN ; r < R_MAX ; r++ )
                if( !rdesc[r].modified && (r != cr))  /* Unmodifed register */
                {
                        clear_desc( r ) ;
                        load_reg( r, b ) ;
                        return r ;
                }

        for( r = R_GEN ; r < R_MAX ; r++ )
                if( r != cr )                          /* Modified register */
                {
                        spill_one( r ) ;
                        clear_desc( r ) ;
                        load_reg( r, b ) ;
                        return r ;
                }

}       /* int  get_areg( SYMB *b,
                          int   cr ) */

char* get_xreg( int op ){
/* convert a register ID into a string and return it */
  
  switch( op ){
  case R_EBP:
    return "%ebp" ;
  case R_ESP:
    return "%esp" ;
  case R_EAX:
    return "%eax" ;
  case R_EBX:
    return "%ebx" ;
  case R_ECX:
    return "%ecx" ;
  case R_EDX:
    return "%edx" ;
  case R_ESI:
    return "%esi" ;
  case R_EDI:
    return "%edi" ;
  }

}

void cg_print( SYMB *c )
/* For printing variables and constants
 * c - pointer to symbol to print
 *
 */
{
  if( c -> token_type_1 == T_CONSTANT ){
    flush_all() ;
    
    printf( "       subl  $%d, %s\n", tos + frame_width, get_xreg( R_ESP )) ;
    printf( "       pushl $%d\n", c -> int_value ) ;
    printf( "       pushl $.LC0\n" ) ;
    printf( "       call printf\n" ) ;

    printf( "       addl $%d, %s\n", tos + frame_width + 8, get_xreg( R_ESP )) ;
    
  } /* Parameters have a positive offset, and do not take into account VAR_OFF */ 
  else if( c -> token_type_1 == T_PARAM ){
    get_rreg( c ) ;

    flush_all() ;
    
    printf( "       subl  $%d, %s\n", tos + frame_width, get_xreg( R_ESP )) ;
    printf( "       pushl %d(%s)\n", ( c -> offset ) * -1, get_xreg( R_EBP )) ;
    printf( "       pushl $.LC0\n" ) ;
    printf( "       call printf\n" ) ;

    printf( "       addl $%d, %s\n", tos + frame_width + 8, get_xreg( R_ESP )) ;

  } 
  else if( c -> token_type_2 == T_VAR ){
    get_rreg( c ) ;

    flush_all() ;
    
    printf( "       subl  $%d, %s\n", tos + frame_width, get_xreg( R_ESP )) ;
    printf( "       pushl %d(%s)\n", ( c -> offset + VAR_OFF ) * -1, get_xreg( R_EBP )) ;
    printf( "       pushl $.LC0\n" ) ;
    printf( "       call printf\n" ) ;

    printf( "       addl $%d, %s\n", tos + frame_width + 8, get_xreg( R_ESP )) ;

  }

  return ;

}

void cg_arr_print( SYMB *a, SYMB *b )
/* For printing arrays
 * a - a pointer to the SYMB holding the C value, the next field of which is
 * a pointer to the array itself
 *
 * b - a pointer to the offset symb
 */
{
  flush_all() ;
  
  /* note that tos is analagous with VAR_OFF, or 12 */
  printf( "       subl  $%d, %s\n", tos + frame_width, get_xreg( R_ESP )) ;
  printf( "       %s  %d(%s)\n", "pushl", 
  ( 0 - tos - ( a -> next -> offset ) + ( a -> next -> c ) - ( b -> int_value )), 
  get_xreg( R_EBP )) ;
  printf( "       pushl $.LC0\n" ) ;
  printf( "       call printf\n" ) ;
  printf( "       addl $%d, %s\n", tos + frame_width + 8, get_xreg( R_ESP )) ;

}

void cg_str( SYMB *s )
/* For printing strings
 *
 *
 */
{

    printf( "       subl  $%d, %s\n", tos + frame_width, get_xreg( R_ESP )) ;
    printf( "       pushl $.LC%d\n", s -> int_value ) ;
    printf( "       call printf\n" ) ;

    printf( "       addl $%d, %s\n", tos + frame_width + 8, get_xreg( R_ESP )) ;
  
}
