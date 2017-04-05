PROGRAM test4 (INPUT,OUTPUT,ERR) ;

VAR x : INTEGER ;
VAR M : ARRAY [1..10,1..10,1..10] OF INTEGER ;
      
BEGIN
   
   M[1,1,1] := 5 ;
   
   M[10,10,10] := M[1,1,1] ;
   
   x := M[1,1,1] ;

   WHILE( x < 10 ) DO
   BEGIN
      M[x,x,x] := x + 1 ;
      PRINT( M[x,x,x] ) ;
      x := x + 1 
   END ;

   PRINT( M[1,1,1] ) ;
   PRINT( M[10,10,10] )
   
END.
