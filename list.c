/*****************************************************************/
/* Programmer:  Jim Canning                                      */
/*                                                               */
/* list.c                                                        */
/*****************************************************************/

#include <stdlib.h>
#include "globals.h"
#include "list.h"

typedef char byte ;

void memcopy( byte *to, byte *from, int count ) {

  while ( count-- > 0 ) *to++ = *from++ ;

}

extern status allocate_node ( list *p_L, generic_ptr data ) {

  list L = (list) malloc ( sizeof ( node ) ) ;

  if ( L == NULL ) return ERROR ;

  *p_L = L ;

  DATA(L) = data ;

  NEXT(L) = NULL ;

  return ( OK ) ;

}

extern void free_node ( list *p_L ) { free (*p_L) ; *p_L = NULL ; }

extern status init_list ( list *p_L ) { *p_L = NULL ; return ( OK ) ; }

extern bool empty_list ( list L ) { return ( ( L == NULL) ? TRUE : FALSE  ) ; }

extern status insert ( list *p_L , generic_ptr data  ) {

  list L ;

  if ( allocate_node ( &L , data ) == ERROR ) return ERROR ;

  NEXT( L) = *p_L ;
  *p_L = L ;
  return ( OK ) ;
}

extern status append ( list *p_L , generic_ptr data ) {

  list L, tmplist ;
 
  if ( allocate_node ( &L , data ) == ERROR ) return ERROR ;

  if ( empty_list( *p_L ) == TRUE ) *p_L = L ;
  else {
    for ( tmplist = *p_L ; NEXT(tmplist) != NULL ; tmplist = NEXT(tmplist) );
    NEXT(tmplist) = L ;
  }
  return ( OK ) ;
}

extern status delete_node ( list *p_L , list node ) {

  if (empty_list(*p_L) == TRUE ) return ( ERROR ) ;
  if ( *p_L == node ) *p_L = NEXT ( *p_L ) ;
  else {

    list L ;
    for ( L = *p_L ; L != NULL && NEXT(L) != node ; L = NEXT(L) ) ;
    if ( L == NULL ) return ( ERROR ) ;
    else
      NEXT(L) = NEXT(node) ;
  }
  free_node(&node) ;
  return ( OK ) ;
}

extern status delete ( list *p_L, generic_ptr *p_data ) {

  if ( empty_list ( *p_L ) ) return ERROR ;

  *p_data = DATA( *p_L );

  return ( delete_node ( p_L , *p_L ) ) ;

}

extern status traverse ( list L , status ( *p_func_f) ( ) ) {

  if ( empty_list ( L ) ) return ( OK ) ; 

  if ( ( *p_func_f) (DATA( L) ) == ERROR ) return ERROR ;
  else
    return ( traverse ( NEXT(L), p_func_f ) ) ;
}

extern list list_iterator( list L, list lastreturn ) { return ( lastreturn == NULL ) ? L : NEXT ( lastreturn ) ; }

extern status find_key ( list L , generic_ptr key, int (*p_cmp_f)() , list *p_keynode  ) {

  list curr = NULL ;

  while (  (curr = list_iterator(L, curr)) != NULL ) {

    if (( *p_cmp_f)( key, DATA(curr) ) == 0 ) {  *p_keynode = curr ; return( OK ) ; }

  }
  return ( ERROR ) ;

}

extern void destroy ( list *p_L , void ( *p_func_f ) ( ) ) {

  if ( empty_list( *p_L ) == FALSE ) { 

     destroy( &NEXT(*p_L), p_func_f ) ; 
     if (  p_func_f != NULL ) (*p_func_f)(DATA(*p_L)) ;
     free_node(p_L) ; 

  }
}

extern bool equal( list L1, list L2, int (*p_cmp_f)() ) {

  if ( ( L1 == NULL) && (L2 == NULL) )   return TRUE  ;
  if ( ( L1 != NULL ) && (L2 == NULL ) ) return FALSE ;
  if ( ( L1 == NULL ) && (L2 != NULL ) ) return FALSE ;

  return( ((*p_cmp_f)( DATA(L1), DATA(L2) ) == 0 ) && equal( NEXT(L1), NEXT(L2), p_cmp_f ) ) ;

} 

extern bool set_equal( list L1, list L2, int (*p_cmp_f)() ) {

  list temp = NULL ;
  list p_keynode ;

  while ( ( temp = list_iterator( L1, temp )) != NULL  ) {

    if ( find_key( L2, DATA( temp ), p_cmp_f, &p_keynode ) == ERROR ) return FALSE ;

  }

  temp = NULL ;

  while ( ( temp = list_iterator( L2, temp )) != NULL ) {

    if ( find_key( L1, DATA( temp ), p_cmp_f, &p_keynode ) == ERROR ) return FALSE ;

  }
  
  return TRUE ;

}


extern status copylist( list L1, list *p_L2, int elementsize ) {

  generic_ptr data ;

  if ( L1 == NULL ) { *p_L2 = NULL ; return OK ; }

  data = (generic_ptr) malloc( elementsize ) ;

  if ( data == NULL ) return ERROR ;

  memcopy( (byte *) data, (byte *) DATA( L1 ), elementsize ) ;

  insert( p_L2 , data ) ;

  return copylist( NEXT(L1), &NEXT(*p_L2), elementsize ) ; 
}

extern status appendlist( list *p_L1, list *p_L2 ) {

  list temp ;

  if ( *p_L1 == NULL ) return OK ;
  if ( *p_L2 == NULL ) *p_L2 = *p_L1 ;

  temp = *p_L2 ;

  while ( NEXT( temp ) != NULL ) temp = NEXT( temp ) ; 
 
  NEXT( temp ) =  *p_L1 ;

  *p_L1 = NULL ;

  return OK ;

}
