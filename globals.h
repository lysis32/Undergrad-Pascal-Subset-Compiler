/***************************************************************/
/* Alec Ferguson                                               */
/*                                                             */
/* globals.h - Version .80 for Pascal Compiler, developed with */
/* assistance from Prof. James Canning of UMass Lowell.        */
/*                                                             */
/* Heavily inspired by code from J.P. Bennett's                */
/* Introduction to Compiling Techniques.                       */
/*                                                             */
/* Last updated on 11 May 2012, 10:14 PM                       */
/*                                                             */
/***************************************************************/

#ifndef _globals
#define _globals

#define EOS '\0'

#define DATA( L ) ( ( L ) -> datapointer )
#define NEXT( L ) ( ( L ) -> next )

typedef enum { OK, ERROR } status ;
typedef enum { FALSE=0 , TRUE=1 } bool ;
typedef char *generic_ptr ;

#endif
