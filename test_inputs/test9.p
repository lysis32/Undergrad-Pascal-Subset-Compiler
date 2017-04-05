PROGRAM test4 (INPUT,OUTPUT,ERR) ;

VAR M : ARRAY [1..10] OF INTEGER ;
VAR x : INTEGER ;

BEGIN

   M[1] := 1 ;
   M[10] := 10 ;

   x := M[1] ;
   M[1] := x + 1 ;

   PRINT( M[1] ) ;
   PRINT( M[10] ) ;
   PRINT( x ) ;

   INPUT := 5 ;

   PRINT( INPUT )

   
END.
