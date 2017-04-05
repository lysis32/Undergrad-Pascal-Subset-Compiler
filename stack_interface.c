/****************************************************************************/
/* stack_interface.c                                                        */
/****************************************************************************/

#include <stdlib.h>
#include "globals.h"
#include "stack.h"
#include "stack_interface.h"

extern status push_table( stack *p_S, tableptr ptr  ) {

  tableptr *p_T = ( tableptr * ) malloc( sizeof( tableptr ) ) ;

  if ( p_T == NULL ) return ERROR ;

  *p_T = ptr ;

  if ( push( p_S, ( generic_ptr ) p_T ) == ERROR ) {

    free( p_T ) ;
    return ERROR ;

  }

  return OK ;

}

extern status push_offset( stack *p_S, int offset  ) {

  int *p_T = ( int * ) malloc( sizeof( int ) ) ;

  if ( p_T == NULL ) return ERROR ;

  *p_T = offset ;

  if ( push( p_S, ( generic_ptr ) p_T ) == ERROR ) {

    free( p_T ) ;
    return ERROR ;

  }

  return OK ;


}


extern status pop_table( stack *p_S, tableptr *p_T ) {


  tableptr *ptr ;

  if ( pop( p_S, (generic_ptr *) &ptr ) == ERROR ) return ERROR ;

  *p_T = *ptr ;
  free( ptr ) ;
  return OK ;

}

extern status pop_offset( stack *p_S, int *p_Offset ) {

  int *ptr ;

  if ( pop( p_S, (generic_ptr *) &ptr ) == ERROR ) return ERROR ;

  *p_Offset = *ptr ;
  free( ptr ) ;
  return OK ;


}

extern status top_table( stack *p_S, tableptr *p_T ) {

  tableptr *ptr ;

  if ( top( p_S, (generic_ptr *) &ptr ) == ERROR ) return ERROR ;

  *p_T = *ptr ;
  return OK ;



}

extern status top_offset( stack *p_S, int *p_Offset ) {

  int *ptr ;

  if ( top( p_S, (generic_ptr *) &ptr ) == ERROR ) return ERROR ;

  *p_Offset = *ptr ;
  return OK ;



}
