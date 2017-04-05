#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define AND 257
#define ARRAY 258
#define _BEGIN 259
#define CASE 260
#define CONST 261
#define DIV 262
#define DO 263
#define DOWNTO 264
#define ELSE 265
#define END 266
#define _FILE 267
#define FOR 268
#define FORWARD 269
#define FUNCTION 270
#define GOTO 271
#define IF 272
#define IN 273
#define LABEL 274
#define MOD 275
#define NIL 276
#define NOT 277
#define OF 278
#define OR 279
#define PACKED 280
#define PROCEDURE 281
#define PROGRAM 282
#define RECORD 283
#define REPEAT 284
#define SET 285
#define THEN 286
#define TO 287
#define TYPE 288
#define UNTIL 289
#define VAR 290
#define WHILE 291
#define WITH 292
#define INTEGER 293
#define REAL 294
#define PRINT 295
#define ID 296
#define NUM_INT 297
#define NUM_REAL 298
#define STRING 299
#define ASSIGN 300
#define NE 301
#define GE 302
#define LE 303
#define DOTDOT 304
typedef union{
  struct symb *symb ;
  struct decl_type decl_type ;
  struct dim_type *dim_type ;
  struct enode *enode ;
  struct tac *tac ;
} YYSTYPE;
extern YYSTYPE yylval;
