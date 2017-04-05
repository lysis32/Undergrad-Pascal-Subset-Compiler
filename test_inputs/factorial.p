PROGRAM test1 ( input, output ) ;  

VAR X : INTEGER ;

FUNCTION fact(n :  INTEGER ) : INTEGER ;
VAR T : INTEGER ;
BEGIN
   IF (( n = 1 ) OR ( n = 0 )) THEN fact := 1 ELSE fact := n * fact( n - 1 )

END ;
BEGIN

   X := fact( 4 ) ;
   PRINT( X )
   
END.
