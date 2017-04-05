/*******************************************************************************************/
/* stack_interface.h                                                                       */
/*******************************************************************************************/

#ifndef _stackinterface
#define _stackinterface

#include "globals.h"
#include "stack.h"
#include "symbol.h"

extern status push_table( stack *p_S , tableptr ptr  ) ;
extern status push_offset( stack *p_S, int offset     ) ;

extern status pop_table( stack *p_S, tableptr *ptr    ) ;
extern status pop_offset( stack *p_S, int *p_Offset     ) ;

extern status top_table( stack *p_S, tableptr *ptr    ) ;
extern status top_offset( stack *p_S, int *p_Offset     ) ;

#endif
