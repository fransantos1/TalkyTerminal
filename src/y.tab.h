#ifndef _yy_defines_h_
#define _yy_defines_h_

#define FIM 257
#define CD 258
#define CLEAR 259
#define LS 260
#define PWD 261
#define MKDIR 262
#define ARGS 263
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE{
	char* comval;
	char* str;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
