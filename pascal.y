/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* pascal.y - Version .80 for Pascal Compiler, developed with  */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques                        */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

/* enough of the yacc grammar for C to support lex */

/* keywords */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symbol.h"
#include "tac.h"
#include "stack.h"
#include "stack_interface.h"
#define YYDEBUG 1

 SYMBTABLE *st ;
 SYMBTABLE *main_symbol_table ;
 SYMBTABLE *constant_symbol_table ;
 SYMBTABLE *table_pointer ;
 SYMBTABLE *table_ptr ;
 
 DIM_TYPE *dim_p ;
 
 PARAM_TYPE *parameter_list ;
 PARAM_TYPE *param ;
 SYMB *t ;

 ENODE *e_list ;
 ENODE *temp_enode ;
 TAC *t_list ;
 TAC *temp_tac ;

 int offset ;
 int len ;
 int temp_int ;
 stack symbol_table_stack ;
 struct decl_type temp ;

 int executable_statement_mode = 0 ;
 int parameter_mode = 0 ;
 int param_count = 0 ;
 int block_depth = 0 ;
 int param_os = 0 ; /* Offset for parameters, 0 is ret addr, 4 is ebp, 8 is arg 3, 12 is arg 2... */
 
 int c        ; /* Calculation of array element addressing */
 int elements ; /* Number of elements in an array.         */

 static void yyerror( char *str ){ fprintf( stderr, "pc: %s\n", str ) ; }

 /* This is the grammar, note that much of it still needs to be documented */

%}

%union{
  struct symb *symb ;
  struct decl_type decl_type ;
  struct dim_type *dim_type ;
  struct enode *enode ;
  struct tac *tac ;
}

%token AND ARRAY _BEGIN CASE CONST DIV DO DOWNTO ELSE
%token END _FILE FOR FORWARD FUNCTION GOTO IF IN LABEL
%token MOD NIL NOT OF OR PACKED PROCEDURE PROGRAM RECORD
%token REPEAT SET THEN TO TYPE UNTIL VAR WHILE WITH
%token INTEGER REAL PRINT

%token <symb> ID
%token <symb> NUM_INT
%token <symb> NUM_REAL
%token <symb> STRING

%token ASSIGN
%token NE
%token GE
%token LE
%token DOTDOT

%type <symb> main_identifier_list
%type <symb> declaration
%type <symb> parameters

%type <enode> factor
%type <enode> term
%type <enode> simple_expression
%type <enode> expression
%type <enode> expression_list
%type <enode> array_variable
%type <enode> array_variable_expression
%type <tac> compound_statement
%type <tac> procedure_statement
%type <tac> statement
%type <tac> statement_list
%type <tac> optional_statements
%type <tac> subprogram_head
%type <tac> subprogram_declarations
%type <tac> subprogram_declaration
%type <tac> optional_declarations
%type <tac> optional_parameters
%type <tac> optional_arguments
%type <tac> argument_list
%type <enode> variable

%type <decl_type> standard_type
%type <decl_type> type
%type <decl_type> declaration_identifier_list

%type <dim_type> dimension_declaration_list

%%
program: M1 PROGRAM ID'('main_identifier_list')'';'
         optional_declarations
         subprogram_declarations
	 compound_statement'.'
         {
	   /* Upon reduction, we pop the symbol table and offset stacks
	    we allocate space for a name ID, which is the name of the
	    program. We set the width of the popped symbol table to the
	    offset provided by the offset stack. We set token types for
	    ID accordingly and call do_prog, join the tacs, and call
	    cg() */
	   char *t_buffer ;
	   pop_table( &symbol_table_stack, &table_pointer ) ;
	   len = strlen( $3 -> name ) ;
	   table_pointer -> name = ( char * )malloc( sizeof( char ) * len + 1 ) ;
	   strcpy( table_pointer -> name, $3 -> name ) ;
	   table_pointer -> name[len] = '\0' ;
	   pop_offset( &offset_stack, &offset ) ;
	   table_pointer -> width = offset ;
	   $3 -> token_type_1 = T_SELF ;
	   $3 -> token_type_2 = T_PROGRAM ;
	   $3 -> data_type = T_UNDEF ;
	   constant_symbol_table -> width = c_offset ;
	   next_tmp = 0 ;
	   next_label = 0 ;
	   tac_list = do_prog( $3, NULL, $10, offset ) ;
	   tac_list = join_tac( tac_list, $9 ) ;
	   cg() ;
	 }
;

M1:
         {
	   /* This is the first action that is performed when parsing
	    stacks are initialized, tables are created and named, global
	    variables are set, etc. */
	   init_stack( &symbol_table_stack ) ;
	   init_stack( &offset_stack ) ;
	   constant_symbol_table = mktable( NULL ) ;
	   table_ptr = mktable( NULL ) ;
	   main_symbol_table = table_ptr ;
	   len = strlen( "CONSTANT SYMBOLS" ) ;
	   constant_symbol_table -> name = ( char * )malloc( sizeof( char ) * len + 1 ) ;
	   strcpy( constant_symbol_table -> name, "CONSTANT SYMBOLS" ) ;
	   main_symbol_table -> width = 0 ;
	   main_symbol_table -> parameters = 0 ;
	   constant_symbol_table -> width = 0 ;
	   push_table( &symbol_table_stack, table_ptr ) ;
	   push_offset( &offset_stack, 0 ) ;
	   parameter_mode = 0 ;
	   main_stack = &symbol_table_stack ;
	   const_sym_tab = constant_symbol_table ;
	 }


main_identifier_list: 
          ID
	  {
	    /* These are the parameters of main. The most important thing
	       to note here is that they have a negative offset that is
	       not incorporated into the width of the table. Their offset
	       is set with param_os, beginning at -8 and being decremented
	       to -12, -16 etc. */
	    $1 -> token_type_1 = T_PARAM ;
	    $1 -> token_type_2 = T_FILE ;
	    $1 -> data_type = T_UNDEF ;
	    $1 -> offset = param_os - 8 ;
	    param_os = 0 ;
	    $$ = $1 ;
	    main_symbol_table -> parameters = 1 ;
	  }
          | main_identifier_list',' ID
	  {
	    $3 -> token_type_1 = T_PARAM ;
	    $3 -> token_type_2 = T_FILE ;
	    $3 -> data_type = T_UNDEF ;
	    param_os -= 4 ;
	    $3 -> offset = param_os - 8 ;
	    $$ = $1 ;
	    main_symbol_table -> parameters = main_symbol_table -> parameters + 1 ;
	  } 
;

optional_declarations: declarations
                     |
                     ;

declaration_identifier_list:
            ',' ID declaration_identifier_list
	    {
	      /* Declaration lists, parameter mode is set when declaring
	      parameters and actions are taken much the same as they are in
	      the main identifier list. Otherwise we perform a series of
	      calculations to get token types, c, size, data types, etc. */
	      if( parameter_mode == 1 )
		{ $2 -> token_type_1 = T_PARAM ; param_count++ ; 
		  $2 -> hops = 0 ;
		  param_os -= 4 ;
		  $2 -> offset = param_os - 8 ;
		}
	      else{
		$2 -> token_type_1 = T_LOCAL ;
		$2 -> hops = 0 ;
		$2 -> line = line_num ;
	      }

	      if( parameter_mode == 1 ){
		param = get_param() ;
		param -> param_symb = $2 ;
		param -> next = parameter_list ;
		parameter_list = param ;
	      }
	      
	      $$.data_type = $3.data_type ;
	      $$.token_type_1 = $3.token_type_1 ;
	      $$.token_type_2 = $3.token_type_2 ;

	      $$.size = $2 -> size = $3.size ;
	      $$.dimensions = $2 -> dimensions = $3.dimensions ;
	      $$.dimension_list = $2 -> dimension_list = $3.dimension_list ;
	      $2 -> line = line_num ;

	      $2 -> token_type_2 = $3.token_type_2 ;

	      if( $3.token_type_2 == T_ARRAY ){
		$2 -> token_type_2 = $3.token_type_2 ;
		$2 -> c            = $3.c ;
		$$.c               = $3.c ;
		$2 -> size         = $3.elements * $3.size ;
		$$.elements        = $3.elements ;
	      }
	      else{
		$2 -> token_type_2 = T_VAR ;
		$2 -> size = $3.size ;
	      }

	      if( parameter_mode == 0 ){
		$2 -> data_type = $3.data_type ;
		pop_offset( &offset_stack, &offset ) ;
		$2 -> offset = offset ;
		offset = offset + $2 -> size ;
		push_offset( &offset_stack, offset ) ;
	      }
	    }
            | ':' type
            {
	      $$ = $2 ;
	    }
            ;

declaration: ID declaration_identifier_list
             {
	       /* This function is similar to the previous, it is split
		  into two rules to allow for synthesized attributes */
	       if( parameter_mode == 1 )
		 { $1 -> token_type_1 = T_PARAM ; param_count++ ;
		   $1 -> hops = 0 ;
		   $1 -> offset = param_os - 8 ;
		   param_os = 0 ;
		 }
	       else{
		 $1 -> token_type_1 = T_LOCAL ;
		 $1 -> hops = 0 ;
		 $1 -> line = line_num ;
	       }

	       if( parameter_mode == 1 ){
		 param = get_param() ;
		 param -> param_symb = $1 ;
		 param -> next = parameter_list ;
		 parameter_list = param ;
	       }

	       $1 -> line = line_num ;
	       $1 -> line = line_num ;

	       if( $2.token_type_2 == T_ARRAY ){
		 $1 -> token_type_2 = $2.token_type_2 ;
		 $1 -> c            = $2.c ;
		 $1 -> size         = $2.elements * $2.size ;
	       }

	       else if( parameter_mode == 0 ){
		 $1 -> token_type_2 = T_VAR ;
		 $1 -> offset       = offset ;
		 $1 -> size         = $2.size ;
	       }

	       else{
		 $1 -> token_type_2 = T_VAR ;
		 $1 -> size = $2.size ;
	       }

	       $1 -> data_type = $2.data_type ;
	       $1 -> dimensions = $2.dimensions ;
	       $1 -> dimension_list = $2.dimension_list ;

	       if( parameter_mode == 0 ){
		 pop_offset( &offset_stack, &offset ) ;
		 $1 -> offset = offset ;
		 offset = offset + $1 -> size ;
		 push_offset( &offset_stack, offset ) ;
	       }
	     }
;

declarations: declarations VAR declaration';'
            | VAR declaration ';'
            ;     

type: standard_type
      {
	$$ = $1 ;
      }
      | ARRAY dimension_declaration_list ']' OF standard_type
      { 
	/* Type information for arrays, we need to keep track of dimemensions
	   and the dimension lists, as well as the size and C constant. */
	$$ = $5 ;
	$$.token_type_2 = T_ARRAY ;
	$$.dimension_list = $2 ;
	$$.dimensions = $2 -> dimension ;
	$$.elements = 1 ;

	$$.c = $2 -> c * $5.size ;

	DIM_TYPE *t ;
	t = $2 ;

	while( t != NULL ){
	  /* Iterate through the dimension list, each time updating
	     the number of elements in the array by the number of elements
	     in each dimension */
	  $$.elements = $$.elements * t -> n ;
	  t = t -> next ;
	}

      }
      ;

dimension_declaration_list: dimension_declaration_list ',' NUM_INT DOTDOT NUM_INT
                          {
			    /* For array declarations. For each one of these rules
			       we need a new dimension struct. We set the next field
			       of this dimension struct to the first terminal. */
			    $$ = get_dim() ;
			    $$ -> high = $5 -> int_value ; /* Upper bound */
			    $$ -> low = $3 -> int_value ; /* Lower bound */
			    $$ -> n = $$ -> high - $$ -> low + 1 ; /* Number of elements */
			    $$ -> dimension = $1 -> dimension + 1 ; /* Which dimension is this? */
			    $$ -> next = $1 ; /* Insert into LL */
			    $1 -> prev = $$ ;
			    $$ -> c = $1 -> c * $$ -> n + $$ -> low ; /* C calculation */
			  }
                          | '[' NUM_INT DOTDOT NUM_INT
                          {
			    /* Similar to the previous, this is the first
			       dimension */
			    $$ = get_dim() ;
			    $$ -> high = $4 -> int_value ;
			    $$ -> low = $2 -> int_value ;
			    $$ -> n = $$ -> high - $$ -> low + 1 ;
			    $$ -> dimension = 1 ;
			    $$ -> c = $$ -> low ;
			    $$ -> next = NULL ;
			  }
                          ;

standard_type: INTEGER
               {
		 /* Type information, only integers are currently supported */
		 $$.data_type      = T_INT ;
		 $$.size           = SIZEOF_INT ;
		 $$.dimensions     = 0 ;
		 $$.dimension_list = NULL ;
	       }
             | REAL
	       {
		 $$.data_type      = T_REAL ;
		 $$.size           = SIZEOF_INT ;
		 $$.dimensions     = 0 ;
		 $$.dimension_list = NULL ;
	       }
             ;

subprogram_declarations: subprogram_declarations subprogram_declaration
                       {
			 $$ = join_tac( $1, $2 ) ;
		       }
                       |
                       {
			 $$ = NULL ;
		       }
                       ;

subprogram_declaration: subprogram_head optional_declarations subprogram_declarations compound_statement';'
	                {
			  /* A 'subprogram declaration' is essentially a function
			     or procedure declarations. Optional declarations are
			     bariable declarations. Subprogram declarations is $3
			     to allow for nested functions, although with the way
			     tacs are joined they are not currently supported */
			  
			  /* pop the symbol table and offset stacks at this point,
			     set the width of the table to the offset */
			  pop_table( &symbol_table_stack, &table_pointer ) ;
			  pop_offset( &offset_stack, &offset ) ;
			  table_pointer -> width = offset ;
			  temp_tac = $1 ;
			  while( temp_tac -> op != TAC_LABEL )
			    temp_tac = temp_tac -> prev ;
			  temp_tac -> VA -> f_value = offset ;
			  $$ = $1 ;
			  $$ = join_tac( $$, $4 ) ;
			  $$ = join_tac( $$, mktac( TAC_ENDFUNC, NULL, NULL, NULL ) ) ;
			  $$ = join_tac( $$, $3 ) ;
			}
                      ;

subprogram_head: FUNCTION ID N1 optional_parameters':'standard_type';'
               {
		 /* A function reduction, set the name of the function label
		    to ID and the function_st_ptr to point to the table_pointer
		    function symbs all end up in the main symbol table and we
		    can follow the function_st_ptr to reach them */

		 top_table( &symbol_table_stack, &table_pointer ) ;
		 len = strlen( $2 -> name ) ;
		 table_pointer -> name = ( char * )malloc( sizeof( char ) * len + 1 ) ;
		 strcpy( table_pointer -> name, $2 -> name ) ;
		 table_pointer -> name[len] = '\0' ;
		 top_offset( &offset_stack, &offset ) ;
		 table_pointer -> width = 0 ;
		 $2 -> token_type_1 = T_LOCAL ;
		 $2 -> token_type_2 = T_FUNC ;
		 $2 -> data_type = $6.data_type ;
		 $2 -> function_st_ptr = table_pointer ;

		 /* This routine inserts the function name into its
		    own table for recursive calls and returns */
		 SYMB *t = get_symb() ;
		 t -> name = $2 -> name ;
		 t -> token_type_1 = T_SELF ;
		 t -> token_type_2 = T_FUNC ;
		 t -> data_type = $6.data_type ;
		 t -> offset = -1 ;
		 t -> function_st_ptr = NULL ;

		 insert_sym( table_pointer, t ) ;
		 $$ = do_func( $2, NULL, 0 ) ;


	       }
               | PROCEDURE ID N1 optional_parameters';'
	       {
		 /* A procedure reduction, set the name of the function label
		    to ID and the function_st_ptr to point to the table_pointer
		    function symbs all end up in the main symbol table and we
		    can follow the function_st_ptr to reach them. Procedures
		    are the same as functions except they cannot return values.
		    They can, however, still be recursive. */

		 top_table( &symbol_table_stack, &table_pointer ) ;
		 len = strlen( $2 -> name ) ;
		 table_pointer -> name = ( char * )malloc( sizeof( char ) * len + 1 ) ;
		 strcpy( table_pointer -> name, $2 -> name ) ;
		 table_pointer -> name[len] = '\0' ;
		 top_offset( &offset_stack, &offset ) ;
		 table_pointer -> width = 0 ;

		 $2 -> token_type_1 = T_LOCAL ;
		 $2 -> token_type_2 = T_PROC ;
		 $2 -> data_type = T_UNDEF ;
		 $2 -> function_st_ptr = table_pointer ;

		 /* This routine inserts the function name into its
		    own table for recursive calls */
		 t = get_symb() ;
		 t -> name = $2 -> name ;
		 t -> token_type_1 = T_SELF ;
		 t -> token_type_2 = T_PROC ;
		 t -> data_type = T_UNDEF ;
		 t -> offset = -1 ;
		 t -> function_st_ptr = NULL ;

		 insert_sym( table_pointer, t ) ;
		 $$ = do_func( $2, NULL, 0 ) ;

	       }
               ;

N1:
   {
     /* Initialization of fields for when we encounter the beginning of a
	subprogram head. We make a new table and push it onto the table stack,
	and push 0 onto the offset stack to reflect the width of the new
	table */
     top_table( &symbol_table_stack, &table_pointer ) ;
     top_offset( &offset_stack, &offset ) ;
     table_pointer = mktable( table_pointer ) ;
     table_pointer -> width = 0 ;
     table_pointer -> parameters = 0 ;
     push_table( &symbol_table_stack, table_pointer ) ;
     push_offset( &offset_stack, 0 ) ;
   } 
   

optional_parameters: '(' N2 parameters ')'
                     {
		       /* Turn off parameter mode, note that N2 turns on
			  parameter mode, so it is turned off when this is
			  reduced. Set the parameters of the current table */
		       top_table( &symbol_table_stack, &table_pointer ) ;
		       table_pointer -> parameters = param_count ;
		       table_pointer -> parameter_list = parameter_list ;
		       parameter_mode = 0 ;
		     }
                     |
                     ;
N2: { parameter_mode = 1 ; param_count = 0 ; parameter_list = NULL ; }

parameters: parameters ';' declaration
              {
		/* Much like in declaring parameters for the main
		   symbol table, they have a negative offset */
		param_os -= 4 ;
		$3 -> offset = param_os - 8 ;
	      }
              | declaration
              {
		$1 -> offset = param_os - 8 ;
		param_os = 0 ;
	      }
              ;

compound_statement: _BEGIN M2 optional_statements END M3
                  {
		    /* A simple compound statement, like a block in
		       C */
		    $$ = $3 ;
		  }
                  ;

M2: { block_depth++ ; executable_statement_mode = 1 ; }
M3: { if( --block_depth == 0 ) executable_statement_mode = 0 ; }

optional_statements: statement_list
                   {
		     $$ = $1 ;
		   }
                   |
                   {
		     $$ = NULL ;
		   }
                   ;

statement_list: statement
              {
		$$ = $1 ;
	      }
              | statement_list';'statement
              {
		$$ = join_tac( $1, $3 ) ;
	      }
              ;

statement: variable ASSIGN expression
         {
           /* var := var or var := int */
	   if( $1 -> offset == NULL && $3 -> offset == NULL )
	     $$ = do_assign( $1 -> res, $3 ) ;
           /* array index := var or int */
	   else if( $1 -> offset != NULL && $3 -> offset == NULL )
	     $$ = do_array_assign( $1 -> res, $1 -> offset, $3 ) ;
           /* array index := array index */
           /* next field of the C struct contains the array itself */
	   else if( $1 -> offset != NULL && $3 -> offset != NULL )
	     $$ = do_array_assign( $1 -> res, $1 -> offset, $3 ) ;
           /* var := array index */
	   else if( $1 -> offset == NULL && $3 -> offset != NULL )
	     $$ = do_assign( $1 -> res, $3 ) ;
	 }
         | procedure_statement
         {
	   $$ = $1 ;
	 }
         | compound_statement
         {
	   $$ = $1 ;
	 }
         | IF expression THEN statement ELSE statement
	 {
	   $$ = do_test( $2, $4, $6 ) ;
	 }
         | WHILE expression DO statement
	 {
	   $$ = do_while( $2, $4 ) ;
	 }
         | PRINT'('expression')' 
         {
	   $$ = mktac( TAC_PRINT, $3 -> res, NULL, NULL ) ;
	   $$ -> prev = $3 -> tac ;
	   
	   /* for arrays, add an aditional field needed */
	   if( $3 -> offset != NULL )
	     $$ -> VB = $3 -> offset -> res ;
	 }
         | PRINT'('STRING')'
         {
	   /* Print the string literal in $3 to the strings file */
	   $$ = mktac( TAC_PRINT_STR, $3, NULL, NULL ) ;
	   fprintf( strings, ".LC%d:\n.string %s\n.text\n", next_string, $3 -> name ) ;
	   $$ -> VA -> int_value = next_string++ ;
	 } 
         ;

variable: ID
        {
	  $$ = mkenode() ;
	  $$ -> res = $1 ;
	  $1 -> token_type_2 = T_VAR ;
	}
        | array_variable
        {
	  $$ = $1 ;
	  $$ -> res -> token_type_2 = T_ARRAY ; 
	}
        ;

array_variable: array_variable_expression']'
              { 
		/* Calculation of array offsets/C is fairly complex
		   and explained in detail in the documentation below */
		char *t_buffer ;
		int cval ;
		temp_enode = mkenode() ;                   /* width enode */
		temp_enode -> res = mktmpconst( "4" ) ;

		/* set the offset to $1.place * width */
		$$ -> offset = do_bin( TAC_MUL, $1, temp_enode ) ;
		
		/* make a new temp */
		$$ -> res = mktmp() ; 
		
		/* assign new temp value of C */
		cval = $1 -> array -> c ; 
		t_buffer = ( char * )malloc( sizeof( char ) * 80 ) ;
		sprintf( t_buffer, "%d", cval ) ;
		t = mktmpconst( t_buffer ) ;
		
		//make a new enode and set the value to C
		temp_enode = mkenode() ;
		temp_enode -> res = t ;
		$$ -> res -> token_type_2 = T_ARRAY ;

		//copy the value of C into array_variable_expression.place
		temp_tac = mktac( TAC_COPY, $1 -> res, temp_enode -> res, NULL ) ;

		//set the next field of the C to the array so that we can access it later
		$1 -> res -> next = $1 -> array ;
		/* set the f_value of the C to the offset for simplifying binary operations */
		$1 -> res -> f_value = $$ -> offset -> res -> int_value ;

		//join with the offset tac
		$$ -> tac = join_tac( temp_tac, $$ -> offset -> tac ) ;
		
	      }
              ;

array_variable_expression: array_variable_expression',' expression
                         {
			   char *t_buffer ;
			   int ival ;
			   ENODE *t_enode1, *t_enode2 ;
			   //t := newtemp
			   temp_enode = mkenode() ;
			   t_enode1 = mkenode() ;
			   t_enode2 = mkenode() ;
			   
			   //assign $1 number of dimensions +1 to temp_int
			   temp_int = $1 -> ndim + 1 ;
			  
			   ival = limit( $1 -> array, temp_int ) ;

			   //change the name of the new temp symb to temp_int 
			   t_buffer = ( char * )malloc( sizeof( char ) * 80 ) ;
			   sprintf( t_buffer, "%d", ival ) ;

			   t = mktmpconst( t_buffer ) ;
			   //assign t to the res field of temp_enode
			   temp_enode -> res = t ;
                           // emit t := elist1.place * limit( elist1.array, m )
			   t_enode1 = do_bin( TAC_MUL, $1, temp_enode ) ;
			   // emit t := t + e.place 
			   t_enode2 = do_bin( TAC_ADD, $3, t_enode1 ) ;
			   
			   $$ = t_enode2 ;
			   // $$ -> tac = temp_tac ;
			   $$ -> array = $1 -> array ;
			   // $$ -> res = t ;
			   $$ -> ndim = temp_int ;
			   //join the two binary expr tacs together
			   $$ -> tac = t_enode2 -> tac ;

			 }
                         | ID'['expression
                         {
			   $$ = $3 ;
			   $$ -> array = $1 ;
			   $$ -> ndim = 1 ;
			 }
                         ;

procedure_statement: ID optional_arguments
                   {
		     /* VA is function ID, procedures do not return */
		     $$ = mktac( TAC_CALL, NULL, $1, NULL ) ;
		     $$ = join_tac( $2, $$ ) ;
		   } 
                   ;

optional_arguments: argument_list ')'
                  {
		    $$ = $1 ;
		  }
                  ;

argument_list: argument_list ',' expression
             {
	       /* For arguments */
	       $$ = mktac( TAC_ARG, $3 -> res, NULL, NULL ) ;
	       $$ = join_tac( $3 -> tac, $$ ) ;
	       $$ = join_tac( $$, $1 ) ;
	     }
             | '(' expression
             {
	       $$ = mktac( TAC_ARG, $2 -> res, NULL, NULL ) ;
	       $$ -> prev = $2 -> tac ;
	     }
             ;

expression_list: expression
               {
		 /* NOTE, expression lists are not used and will currently
		    make yacc give a warning flag for these two rules.
		    They are kept in the grammar incase we need them later */
                 $$ = $1 ;
	       }
               | expression_list','expression
	       {
		 $1 -> next = $3 ;
		 $$ = $1 ;
	       }
               ;

expression: simple_expression
          {
	    /* The rest of the grammar is fairly self-explanatory and simple
	       from this point onwards */
	    $$ = $1 ;
	  }
          | simple_expression NE simple_expression
          {
	    $$ = do_NE( $1, $3 ) ;
	  }
          | simple_expression GE simple_expression
          {
	    $$ = do_GE( $1, $3 ) ;
	  }
          | simple_expression LE simple_expression
          {
	    $$ = do_LE( $1, $3 ) ;
	  }
          | simple_expression'='simple_expression
          {
	    $$ = do_EQ( $1, $3 ) ;
	  }
          | simple_expression'<'simple_expression
          {
	    $$ = do_LT( $1, $3 ) ;
	  }
          | simple_expression'>'simple_expression
          {
	    $$ = do_GT( $1, $3 ) ;
	  }
          ;

simple_expression: term
                 {
		   $$ = $1 ;
		 }
                 | '+'term
                 {
		   $$ = do_un( TAC_POS, $2 ) ;
		 }
                 | '-'term
		 {
		   $$ = do_un( TAC_NEG, $2 ) ;
		 }
                 | simple_expression'+'term
                 {
		   $$ = do_bin( TAC_ADD, $1, $3 ) ;
		 }
                 | simple_expression'-'term
                 {
		   $$ = do_bin( TAC_SUB, $1, $3 ) ;
		 }
                 | simple_expression OR term
                 {
		   $$ = do_bin( TAC_OR, $1, $3 ) ;
		 }
                 ;

term: factor
    {
      $$ = $1 ;
    }
    | term'*'factor
    {
      $$ = do_bin( TAC_MUL, $1, $3 ) ;
    }
    | term'/'factor
    {
      $$ = do_bin( TAC_DIV, $1, $3 ) ;
    }
    | term MOD factor
    {
      $$ = do_bin( TAC_MOD, $1, $3 ) ;
    }
    | term AND factor
    {
      $$ = do_bin( TAC_AND, $1, $3 ) ;
    }
    | term DIV factor
    {
      $$ = do_bin( TAC_DIV, $1, $3 ) ;
    }
    ;

factor: ID
      {
	$$ = mkenode() ;
	$$ -> res = $1 ;
      }
      | ID optional_arguments 
      {
	$$ = mkenode() ;
	$$ -> res = mktmp() ;
	$$ -> tac = mktac( TAC_CALL, $$ -> res, $1, NULL ) ;
	$$ -> tac = join_tac( $2, $$ -> tac ) ;
      }
      | NUM_INT
      {
	$$ = mkenode() ;
	$$ -> res = $1 ;
      }
      | NUM_REAL
      {
	$$ = mkenode() ;
	$$ -> res = $1 ;
      }
      | '('expression')'
      {
	$$ = $2 ;
      }
      | NOT factor
      {
	$$ = do_un( TAC_NOT, $2 ) ;
      }
      | array_variable
      {
	$$ = $1 ;
	$$ -> res -> token_type_2 = T_ARRAY ; 
      }
      ;

%%
