PROGRAM test2 (Input, Output) ;
VAR  
   x : INTEGER ;

FUNCTION foo(a : INTEGER ) : INTEGER ;

VAR z : INTEGER ;

BEGIN

   foo := a

END ;

FUNCTION bar(a	: INTEGER ; b : INTEGER ) : INTEGER ;

VAR x, y, z : INTEGER ; 
      
BEGIN

   PRINT( a ) ;
   
   bar := foo( 9 ) 
   
END ;
	       
BEGIN
   
   x := 7;
   x := 8 ;

   x := bar( 3 + 4, 4 + 5 ) ;
   
   PRINT( x ) 
   
END.
