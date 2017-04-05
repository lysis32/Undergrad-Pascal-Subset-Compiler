PROGRAM test1 ( input, output ) ;  

VAR X : INTEGER ;
VAR Y : INTEGER ;
VAR M : ARRAY[1..10,1..10] OF INTEGER ;

BEGIN

   M[1,1] := 10 ;
   M[2,2] := 3 ;
   M[3,3] := M[1,1] + M[2,2] ;
   
   PRINT( M[3,3] ) ;

   M[4,4] := M[1,1] DIV M[2,2] + M[3,3] ;
   M[5,5] := M[4,4] MOD 2 ;
   M[6,6] := 2 + M[5,5] ;

   PRINT( M[6,6] ) ;
   
   M[6,6] := M[6,6] - 2 ;

   PRINT( "Bro\n" ) ;
   
   PRINT( M[6,6] ) ;
   PRINT( M[4,4] ) ;
   PRINT( M[5,5] ) ;

   IF ( M[4,4] > M[6,6] ) THEN PRINT( M[4,4] ) ELSE PRINT( M[6,6] ) ;

   X := 3 ;
   X := X - 1 ;
   X := -M[4,4] ;
   
   PRINT( X ) ;
   PRINT( "Dog\n" ) 
   
END. 
