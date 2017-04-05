/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* symbol.c - Version .80 for Pascal Compiler, developed with  */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques.                       */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "stack_interface.h"
#include "utility.h"
#include "symbol.h"
#include "y.tab.h"

SYMBTABLE *mktable( SYMBTABLE *previous )
/* Creates a symbol table struct */
{

  SYMBTABLE *temp ;

  if( previous == NULL )
    return ( SYMBTABLE * )safe_malloc( sizeof( SYMBTABLE ) ) ;
  else{
    temp = ( SYMBTABLE * )safe_malloc( sizeof( SYMBTABLE ) ) ;
    temp -> parent = previous ;
    return temp ;
  }
    
}

SYMB *get_symb()
/* Allocates space for a struct symb */
{

  return ( SYMB * )safe_malloc( sizeof( SYMB ) ) ;

}

void mkname( SYMBTABLE *table, char *yytext, int tokentype )
/* Make a 'name' or symbol in table. First call lookup to see if the name
   already exists and set the yylval to point to that if it does.
   Otherwise check to see if the symbol exists in any previous tables.
   'hops' keeps track of the number of jumps away from the current table
   the symbol is. */
{

  SYMB *tempSymb ;
  SYMB *tempSymb2 ;
  SYMBTABLE *tempTb ;
  char *tempString ;
  int len ;
  int hops = 0 ;

  if(( tempSymb = lookup( table, yytext )) != NULL ){
      yylval.symb = tempSymb ;
      return ;
  }

  tempSymb2 = ( SYMB * )safe_malloc( sizeof( SYMB ) ) ;

  len = strlen( yytext ) ;
  tempString = ( char * )safe_malloc( sizeof( char ) * len + 1 ) ;
  strcpy( tempString, yytext ) ;
  tempString[len] = '\0' ;
  
  tempTb = table ;
  while( tempTb -> parent != NULL ){
    tempTb = tempTb -> parent ;
    ++hops ;
    if(( tempSymb = lookup( tempTb, yytext )) != NULL ){
      *tempSymb2 = *tempSymb ;
      tempSymb2 -> hops = hops ;
	if( tempSymb2 -> hops >= 1 ){
	  if(( tempSymb -> token_type_1 != T_SELF ) && 
	    ( tempSymb -> token_type_2 != T_FUNC ) &&
	    ( tempSymb -> token_type_2 != T_PROC )){
	  tempSymb2 -> token_type_1 = T_NOT_LOCAL ;
	  tempSymb2 -> token_type_2 = T_NOT_LOCAL ;
	  }
	  tempSymb2 -> line = line_num ;
	  tempSymb2 -> function_st_ptr = NULL ;
	  yylval.symb = tempSymb2 ;
	  insert_sym( table, tempSymb2 ) ;
	  return ;
	}
    }
  }
  
  tempSymb = ( SYMB * )safe_malloc( sizeof( SYMB ) ) ;

  tempSymb -> name = tempString ;

  tempSymb -> token_type_1 = T_LOCAL ;
  tempSymb -> token_type_2 = T_UNDEF ;
  tempSymb -> hops = 0 ;
  tempSymb -> line = line_num ;

  if( tokentype == T_SELF )
    tempSymb -> token_type_1 = T_SELF ;

  insert_sym( table, tempSymb ) ;
  yylval.symb = tempSymb ;

}

void mkconst( SYMBTABLE *table, char *yytext, int tokentype )
/* Make a constant symbol. We needn't worry about nonlocal names
   because all constants are stored in the constant symbol table */
{

  SYMB *tempSymb ;
  char *tempString ;
  int len ;

  if(( tempSymb = lookup( table, yytext )) != NULL ){
    yylval.symb = tempSymb ;
    return ;
  }

  tempSymb = ( SYMB * )safe_malloc( sizeof( SYMB ) ) ;
  
  len = strlen( yytext ) ;
  tempString = ( char * )safe_malloc( sizeof( char ) * len + 1 ) ;
  strcpy( tempString, yytext ) ;
  tempString[len] = '\0' ;

  tempSymb -> name = tempString ;
  tempSymb -> token_type_1 = T_CONSTANT ;
  tempSymb -> token_type_2 = T_CONSTANT ;
  tempSymb -> data_type = tokentype ;
  tempSymb -> line = line_num ;
  tempSymb -> hops = 0 ;
  tempSymb -> offset = c_offset ;

  if( tokentype == T_INT ){
    tempSymb -> size = SIZEOF_INT ;
    tempSymb -> int_value = atoi( tempString ) ;
    c_offset += SIZEOF_INT ;
  }else if( tokentype == T_REAL ){
    tempSymb -> size = SIZEOF_REAL ;
    tempSymb -> f_value = atof( tempString ) ;
    c_offset += SIZEOF_REAL ;
  }
    
  insert_sym( table, tempSymb ) ;
  yylval.symb = tempSymb ;

}

void init_pascal()
/* Just a few initiations, a function is reserved incase more need to be added later */
{

  line_num = 1 ;
  c_offset = 0 ;

}

DIM_TYPE *get_dim()
/* Get a DIM struct used to keep track of array dimensions */
{

  return ( DIM_TYPE * )safe_malloc( sizeof( DIM_TYPE ) ) ;

}

PARAM_TYPE *get_param()
/* Get a PARAM struct used to keep track of parameters */
{

  return ( PARAM_TYPE * )safe_malloc( sizeof( PARAM_TYPE ) ) ;

}

SYMB *lookup( SYMBTABLE *tableptr, char *name )
/* Lookup the name in the current symbol table, uses a hash */
{

  int hv = hash( name ) ;
  SYMB *t = tableptr -> symtab[hv] ;

  while( t != NULL )
    if( strcmp( t -> name, name ) == 0 )
      break ;
    else
      t = t -> next ;

  return t ;

}

void insert_sym( SYMBTABLE *tb, SYMB *s )
/* Insert sym with name into symbol table tb */
{

  int hv = hash( s -> name ) ;

  s -> next = tb -> symtab[hv] ;
  tb -> symtab[hv] = s ;

}

int hash( char *s )
/* Hashes the name */
{

  int hv = 0 ;
  int i, v ;
  for( i = 0 ; s[i] != EOS ; ++i ){
    
    v = ( hv >> 28 ) ^ ( s[i] & 0xf ) ;

    hv = ( hv << 4 ) | v ;

  }

  hv = hv & 0x7fffffff ;
  return hv % HASHSIZE ;

}

void dump_table( SYMBTABLE *tableptr )
/* Dumps the table onto the screen incase we want to look at it,
   the call is currently commented out in main.c */
{

  int i ;
  SYMBTABLE *temp ;

  if( tableptr == NULL )
    return ;

  temp = tableptr ;

  if( temp -> name != NULL )
    printf( "\nTABLE NAME:    %s    WIDTH:    %d    PARAMETERS:    %d\n", temp -> name, temp -> width, temp -> parameters ) ;
  
  printf( "%8s%14s%14s%14s%6s%8s%6s%13s%12s%3s\n", "NAME", "TOKEN_TYPE_1", "TOKEN_TYPE_2", 
	  "DATA_TYPE", "HOPS", "OFFSET", "SIZE", "LINE_NUMBER", "DIMENSIONS", "C" ) ;
    
    for( i = 0 ; i < HASHSIZE ; ++i )
      if( temp -> symtab[i] != NULL ){
	if( temp -> symtab[i] -> token_type_1 == T_NOT_LOCAL ){
	  temp -> symtab[i] -> data_type = T_NOT_LOCAL ;
	  temp -> symtab[i] -> offset = 0 ;
	  temp -> symtab[i] -> size = 0 ;
	  temp -> symtab[i] -> line = 0 ;
	  temp -> symtab[i] -> dimensions = 0 ;
	  temp -> symtab[i] -> c = 0 ;
	} 
	printf( "%8s", ( temp -> symtab[i] ) -> name ) ;
	printf( "%14s", lookup_type(( temp -> symtab[i] ) -> token_type_1 )) ;
	printf( "%14s", lookup_type(( temp -> symtab[i] ) -> token_type_2 )) ;
	printf( "%14s",  lookup_type(( temp -> symtab[i] ) -> data_type )) ;
	printf( "%6d", ( temp -> symtab[i] ) -> hops ) ;
	printf( "%8d", ( temp -> symtab[i] ) -> offset ) ;
	printf( "%6d", ( temp -> symtab[i] ) -> size ) ;
	printf( "%13d", ( temp -> symtab[i] ) -> line ) ;
	printf( "%12d", ( temp -> symtab[i] ) -> dimensions ) ;
	printf( "%3d\n", ( temp -> symtab[i] ) -> c ) ;
	
      }
    
    for( i = 0 ; i < HASHSIZE ; ++i )
      if( temp -> symtab[i] != NULL )
	if(( temp -> symtab[i] ) -> function_st_ptr != NULL )
	  dump_table(( temp -> symtab[i] ) -> function_st_ptr ) ;

}

char *lookup_type( int type )
/* Allocates space for a string and returns it to the caller */
{

  char *s = ( char * )safe_malloc( sizeof( char ) * 12 ) ;

  switch( type ){
  case T_UNDEF: s = "T_UNDEF\0" ; return s ;
  case T_VAR: s = "T_VAR\0" ; return s ;
  case T_PROC: s = "T_PROC\0" ; return s ;
  case T_FUNC: s = "T_FUNC\0" ; return s ;
  case T_INT: s = "T_INT\0" ; return s ;
  case T_LABEL: s = "T_LABEL\0" ; return s ;
  case T_REAL: s = "T_REAL\0" ; return s ;
  case T_ARRAY: s = "T_ARRAY\0" ; return s ;
  case T_STRING: s = "T_STRING\0" ; return s ;
  case T_BOOL: s = "T_BOOL\0" ; return s ;
  case T_SPECIAL: s = "T_SPECIAL\0" ; return s ;
  case T_PROGRAM: s = "T_PROGRAM\0" ; return s ;
  case T_NOT_LOCAL: s = "T_NOT_LOCAL\0" ; return s ;
  case T_PARAM: s = "T_PARAM\0" ; return s ;
  case T_ARG: s = "T_ARG\0" ; return s ;
  case T_LOCAL: s = "T_LOCAL\0" ; return s ;
  case T_FILE: s = "T_FILE\0" ; return s ;
  case T_CONSTANT: s = "T_CONSTANT\0" ; return s ;
  case T_SELF: s = "T_SELF\0" ; return s ;
  default: exit( 0 ) ;
  }


}
