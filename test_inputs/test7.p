PROGRAM main( input ) ;

VAR a, b, c :  INTEGER ;

PROCEDURE goo(i	: INTEGER ) ;

VAR a : INTEGER ;

BEGIN

   a := 6 ;
   a := a + 6 ;
   PRINT( a )
   
END ;

PROCEDURE boo(i	: INTEGER ) ;

   VAR c : INTEGER ;
  
   PROCEDURE hoo( i  : INTEGER ) ;

   VAR b : INTEGER ;

      PROCEDURE loo(i : INTEGER ) ;

	 PROCEDURE foo(i : INTEGER ) ;
	 
	 BEGIN

	    goo( 8 )

	 END ;

      BEGIN

	 foo( 1 );
	 goo( 9 ) 
	 
      END;
   
   BEGIN

      b := 200 ;
      loo( 2 ) ;
      goo( 9 ) 

   END;

BEGIN

   hoo( 3 ) ;
   goo( 1 )

END;

BEGIN

   a := 77 ;
   b := 78 ;
   c := 79 ;
   
   boo( 4 ) ;

   goo( 5) 
   
END.
