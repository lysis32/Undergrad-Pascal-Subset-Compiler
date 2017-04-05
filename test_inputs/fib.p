PROGRAM test1 ( input, output ) ;  

VAR C : INTEGER ;

FUNCTION fib(n :  INTEGER ) : INTEGER ;
VAR X : INTEGER ;
VAR Y : INTEGER ;
BEGIN
   IF ( n <= 0 ) THEN fib := 0
   ELSE IF ( n = 1 ) THEN fib := 1 ELSE
      fib := fib( n - 1 ) + fib( n - 2 )
	 
END ;
BEGIN

   C := fib( 4 ) ;
   PRINT( C )
   
END.
