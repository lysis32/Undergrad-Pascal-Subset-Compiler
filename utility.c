/* utility.c */

#include <stdlib.h>
#include <stdio.h>

void error( char *str ) {

  fprintf( stderr, "Trouble in River City: %s \n", str ) ;
  
}

void *safe_malloc( int n ) {

  void *t = malloc( n ) ;

  if ( t == NULL ) { error("malloc failed") ; exit( 0 ) ; }

  return t ;

}

