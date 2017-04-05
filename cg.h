/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* cg.h - Version .80 for Pascal Compiler, developed with      */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques.                       */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#ifndef CG_H
#define CG_H

#include <stdio.h>
#include <ctype.h>
#include "symbol.h"
#include "tac.h"
#include "globals.h"
#include "stack.h"
#include "stack_interface.h"

/* Constants used here. First we define some of the registers. We reserve
   register ESP as the stack pointer and EBP as the base pointer*/

#define R_EBP           0                /* ebp */
#define R_ESP           1                /* esp */
#define R_GEN           2                /* start of general purpose regs */
#define R_EAX           2                /* eax is the first gen reg */
#define R_EBX           3                /* ebx */
#define R_ECX           4                /* ecx */
#define R_EDX           5                /* edx */
#define R_ESI           6                /* esi */
#define R_EDI           7                /* edi */
#define R_MAX           8                /* 8 registers */       

/* The stack frame holds the dynamic link at offset zero and the return address
   at offset 4. */

#define P_OFF           0                /* Offset of stack pointer on frame */
#define PC_OFF          4                /* Offset of ret address on frame */
#define S_LINK          8                /* Offset of the static link    */
#define VAR_OFF         12               /* Offset of variables on frame */

/* To make the code clearer we define flags MODIFIED and UNMODIFIED as TRUE and
   FALSE respectively for setting the mod field of the register descriptor. */

#define MODIFIED     TRUE                /* Entries for descriptors */
#define UNMODIFIED  FALSE

/* These are static variables used throughout this section. The register
   descriptor is an array of anonymous structures with a field to hold the most
   recent item slaved in the register and a field to mark whether the register
   has been modified since last written to memory.

   "tos" is the top of stack in the current function and "next_arg" is the
   number of the next argument to load on the stack. */

struct                                   /* Reg descriptor */
{
        struct symb *name ;              /* Thing in reg */
        int          modified ;          /* If needs spilling */
}    rdesc[R_MAX] ;
int  tos ;                               /* Top of stack */
int  next_arg ;                          /* Next argument to load */
int  frame_width ;                       /* Current frame width */
int  array_flag ;                        /* For binary ops with arrays */

stack cg_offset_stack ;

/* These are the prototypes of routines defined here. Routines to translate TAC
   instructions generally have the form "cg_xxx()" where xxx is the name of a
   TAC instruction of group of TAC instructions. */

void  cg() ;
TAC  *init_cg() ;
void  cg_instr( TAC *c ) ;
void  cg_bin( char *op,
              SYMB *a,
              SYMB *b,
              SYMB *c ) ;
void  cg_un( char *op, 
	     SYMB *a,
	     SYMB *b ) ;
void  cg_copy( SYMB *a,
               SYMB *b ) ;
void  cg_cond( char *op,
               SYMB *a,
               int   l ) ;
void  cg_arg( SYMB *a ) ;
void  cg_call( SYMB *func,
               SYMB *res ) ;
void  cg_return( SYMB *a ) ;
void  cg_sys( char *fn ) ;
void  cg_strings() ;
void  cg_str( SYMB *s ) ;
void  flush_all( void ) ;
void  spill_all( void ) ;
void  spill_one( int  r ) ;
void  load_reg( int   r,
                SYMB *n ) ;
void  clear_desc( int   r ) ;
void  insert_desc( int   r,
                   SYMB *n,
                   int   mod ) ;
int   get_rreg( SYMB *c ) ;
int   get_areg( SYMB *b,
               int   cr ) ;

char* get_xreg( int op ) ;
void cg_print( SYMB *c ) ;
void cg_arr_print( SYMB *a, SYMB *b ) ;
void cg_arr_cpy( SYMB *a, SYMB *b, SYMB *c ) ;
void cg_arr_arr_cpy( TAC *a, SYMB *b, SYMB *c ) ;
void cg_var_arr_cpy( TAC *a, SYMB *b, SYMB *c ) ;
void cg_relop( int op, int lab, SYMB *b, SYMB *c ) ;

#endif
