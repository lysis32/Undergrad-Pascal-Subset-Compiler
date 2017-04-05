/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* main.c - Version .80 for Pascal Compiler, developed with    */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques                        */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "tac.h"

extern int yyparse (void) ;
extern SYMBTABLE *main_symbol_table ;
extern SYMBTABLE *constant_symbol_table ;
extern FILE *yyin ;
extern void init_table( void ) ;
extern int yydebug ;

int main ( int argc, char *argv[] ) {

  init_pascal() ;

  yydebug = 0 ;

  yyin = fopen( argv[1], "r" ) ;

  strings = fopen( "/tmp/strings.txt", "w+" ) ;
  unlink( "/tmp/strings.txt" ) ;
  next_string = 1 ;

  if ( !yyparse() ){
    /* These segments are used for dumping the symbol table
     * For constants and variables. They are currently commented
     * out to avoid polluting the assembly output.
     * 
     * printf("\nYes, it parses! \n");
     *
     * dump_table( main_symbol_table ) ;
     * printf( "\n" ) ;
     * dump_table( constant_symbol_table ) ; */

  } else printf("\nNo, it does not parse.\n") ;
  return ( 0 ) ;

}
