PROGRAM test6( input, output ) ;

   VAR x,y,z : INTEGER ;
   VAR a,b,c,d : INTEGER    ;
   VAR M	    : ARRAY [1..10] OF INTEGER ;
   VAR N    : ARRAY [1..10,1..5] OF INTEGER ; 
   VAR P    : ARRAY[1..10,1..10,1..10] OF INTEGER ;
   
FUNCTION foo(a , b : INTEGER ; c : INTEGER ) : INTEGER ;

VAR x, y, z : INTEGER ;
	    
BEGIN
   
   a := 10 ;
   b := a MOD 3 ;
   d := 3 ;
   PRINT( a ) ;
   PRINT( b ) ;
   PRINT( d )
   
END ;


PROCEDURE bar(a	: INTEGER ) ;

VAR x, y, z : INTEGER ; 
VAR N : ARRAY [1..100] OF INTEGER ;
      
BEGIN

   PRINT( a ) ;
   foo( a, a, a )
   
END ;
	       

BEGIN
      
   M[10] := 5 ;
   bar( 1000 ) ;
   N[2,3] := 3 ;
   P[2,3,4] := 4 ;

   PRINT( P[2,3,4] + 4 )
   
END.
