%{
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

int yylex();
void yyerror(const char *);

bool error_flag = false;
%}

%define api.value.type {double}
%token NUM

%left '+' '-'
%left '*' '/'
%right '^'
%precedence UMINUS 

%%

input:
  %empty
| input line
;

line:
  '\n'
| exp '\n' { if (!error_flag) printf("%.10g\n", $1); error_flag = false; }
;

exp:
  NUM              { $$ = $1; }
| exp '+' exp     { $$ = $1 + $3; }
| exp '-' exp     { $$ = $1 - $3; }
| exp '*' exp     { $$ = $1 * $3; }
| exp '/' exp     { 
    if ($3 == 0) {
        yyerror("Division by zero");
        error_flag = true;
        $$ = 0;
    } else {
        $$ = $1 / $3;
    }
  }
| exp '^' exp     { $$ = pow($1, $3); }
| '(' exp ')'     { $$ = $2; }
| '-' exp %prec UMINUS { $$ = -$2; } 
;

%%

#include <ctype.h>
#include <stdlib.h>

int yylex() {
    int c;
    while ((c = getchar()) == ' ' || c == '\t');

    if (isdigit(c) || c == '.') {
        ungetc(c, stdin);
        if (scanf("%lf", &yylval) != 1)
            abort();
        return NUM;
    }
    return c;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    return yyparse();
}