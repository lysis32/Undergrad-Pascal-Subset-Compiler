PROGRAM test3 (Input, Output) ;

VAR x : INTEGER ;
VAR y : INTEGER ;
VAR z : INTEGER ;

PROCEDURE bar(a	: INTEGER ) ;

VAR x, y, z : INTEGER ; 
VAR N : ARRAY [1..100] OF INTEGER ;
      
BEGIN

   x := 1 
   
END ;


BEGIN

   x := 0 ;
   y := 0 ;
   z := 5 ;
   

   
   IF (( NOT ( x = 0 )) AND (y = 0) AND ( z = 5 )) THEN x := x + 2 ELSE x := 573;
   IF (( y <> 0 ) OR ( y = 0 )) THEN y := y + 2 ELSE y := 573;
   IF ( z >= 0 ) THEN z := z + 2 ELSE z := 573;

   PRINT( z ) ;
   
   IF ( z >= 5 ) THEN z := z + 2 ELSE z := 573;

   PRINT( z ) ;
   
   PRINT( x ) ;
   PRINT( y ) ;

   y := 0 ;

   WHILE ( y < 10 ) DO y := y + 1 ;

   PRINT( y )
      
END.
