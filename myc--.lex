%option interactive

%{
	#include <string.h>
  #include "myc--.h"	#include "y.tab.h"

	extern int lineNumber;
%}

%option noyywrap

DIGIT [0-9]
LETTER [a-zA-Z]

%x ERROR

%%

print { 
	return PRINT; 
}

{DIGIT}+ { 
  yylval.number = atof(yytext);
  return NUMBER;
}

{LETTER}[{DIGIT}{LETTER}]* {
  yylval.ident = strdup(yytext);
  return ID;
}

[ \t\f\r] ;
\n { lineNumber++; }

"-" { return MINUS; }
"+" { return PLUS; }
"*" { return TIMES; }
"/" { return DIVIDE; }
"%" { return MODULO; }
"(" { return LPARENS; }
")" { return RPARENS; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"=" { return EQUALS; }
";" { return SEMICOLON; }

. { 
  BEGIN(ERROR);
  yymore();
}

<ERROR>[^{DIGIT}{LETTER}+\-/*(){}= \t\n\f\r] { yymore(); }
<ERROR>(.|\n) { 
  yyless(yyleng - 1);
  printf("error token: %s on line %d\n", yytext, lineNumber);
  BEGIN(INITIAL);
}

%%

