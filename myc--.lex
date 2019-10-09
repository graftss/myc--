%option interactive

%{
	#include <string.h>
  #include "myc--.h"	#include "y.tab.h"

	extern int lineNumber;
%}

%option noyywrap

DIGIT [0-9]
ID [a-zA-Z][a-zA-Z0-9]*
STR [^"]*
CHAR [^']

%x ERROR

%%

\'[^']\' {
  yylval.literalChar = yytext[1];
  return CHAR_LITERAL;
}

["][^"]*["] {
  // TODO fix string literals
  std::string literalString(yytext);
  yylval.literalString = &literalString;
  return STRING_LITERAL;
}

"<=" { return LEQ; }
">=" { return GEQ; }
"==" { return EQ; }
"!=" { return NEQ; }
"&&" { return AND; }
"||" { return OR; }
"!" { return NOT; }
"<" { return LT; }
">" { return GT; }
"-" { return MINUS; }
"+" { return PLUS; }
"*" { return TIMES; }
"/" { return DIVIDE; }
"%" { return MODULO; }
"=" { return ASSIGN; }
"(" { return LPARENS; }
")" { return RPARENS; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"[" { return LINDEX; }
"]" { return RINDEX; }
"\"" { return DQUOTE; }
"\'" { return SQUOTE; }
";" { return SEMICOLON; }
"," { return COMMA; }
"return" { return RETURN; }
"true" { return TRUE; }
"false" { return FALSE; }
"bool" { return TYPE_BOOL; }
"int" { return TYPE_INT; }
"float" { return TYPE_FLOAT; }
"string" { return TYPE_STRING; }
"char" { return TYPE_CHAR; }

{DIGIT}+ { 
  yylval.number = atof(yytext);
  return NUMBER;
}

{ID} {
  yylval.ident = strdup(yytext);
  return ID;
}

[ \t\f\r] ;
\n { lineNumber++; }

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

