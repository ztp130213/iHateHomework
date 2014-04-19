%{
/************************************************************
calc.y
Simple calculator. Features floating point arithmetic using
the addition, subtraction, multiplication and divide
operators, and unary minus. Expressions can be grouped
using parentheses, and simple error recovery is supported.
************************************************************/

#include <ctype.h>
#include <stdio.h>
#define YYSTYPE double /* double type for YACC stack */
%}

%token NUMBER

%%
lines	: lines expr '\n'			{ printf("Success. The result is %g\n", $2); }
		| lines '\n'
		| /* e */
		| error '\n'				{ yyerror("reenter last line:"); yyerrok(); }
		;

expr	: expr '+' term				{ $$ = $1 + $3; printf("E->E+T %g=%g+%g\n",$$,$1,$3);}
		| expr '-' term				{ $$ = $1 - $3; printf("E->E-T %g=%g-%g\n",$$,$1,$3);}
		| term                      { printf("E->T %g\n",$$ );}
		;

term	: term '*' factor			{ $$ = $1 * $3; printf("T->T*F %g=%g*%g\n",$$,$1,$3);}
		| term '/' factor			{ $$ = $1 / $3; printf("T->T/F %g=%g/%g\n",$$,$1,$3);}
		| factor                    { printf("T->F %g\n",$$);}
		;

factor	: '(' expr ')'				{ $$ = $2; printf("F->(E) %g\n", $$);}
		| '(' expr error			{ $$ = $2; yyerror("missing ')'"); yyerrok(); }
		| '-' factor				{ $$ = -$2;printf("F->-F -%g\n", $$); }
		| NUMBER                    { printf("F->num %g\n",$$);}
		; 

%%
int main(void)
{
    printf("Please input the expression: ");
	return yyparse();
}

int yylex(void)
{
	int c;
	while ((c = getchar()) == ' ');
	if (c == '.' || isdigit(c)) {
		ungetc(c, stdin);
		scanf("%lf", &yylval);
		return NUMBER;
	}
	return c;
}
