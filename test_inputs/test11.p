PROGRAM test11 ( INPUT, OUTPUT, ERR ) ;

VAR X : INTEGER ;

FUNCTION foo(a : INTEGER ) : INTEGER ;

VAR z : INTEGER ;

BEGIN

   foo := a

END ;

BEGIN

   X := 10 ;

   WHILE( X > 2 ) DO
   BEGIN
      X := X - 8 ;
      PRINT( X )
   END

END.
