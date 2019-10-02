%{#include <iostream>#include <string>#include <stdlib.h>#include <stdio.h>#include <map>#include <list>#include "myc--.h"using namespace std;extern FILE *yyin;int yylex();void yyerror(const char* s);NStatement *root;int lineNumber = 1;%}%start program%union {  float number;  char *ident;  NExpression *expr;  NStatement *stmt;}%error-verbose%token <number> NUMBER%token <ident> ID%token MINUS PLUS TIMES DIVIDE LPARENS RPARENS LBRACE RBRACE EQUALS SEMICOLON PRINT
%type <stmt> program
%type <stmt> stmt%type <expr> expr%%program : stmt { root = $$; };stmt: ID EQUALS expr { $$ = new NAssign($1, $3); };expr: NUMBER { $$ = new NNumber($1); }
  | expr PLUS expr { $$ = new NBinaryNumOp($1, $3, OP_PLUS); }
  | expr MINUS expr { $$ = new NBinaryNumOp($1, $3, OP_MINUS); }
  | expr TIMES expr { $$ = new NBinaryNumOp($1, $3, OP_TIMES); }
  | expr DIVIDE expr { $$ = new NBinaryNumOp($1, $3, OP_DIVIDE); };%%int main(int argc, char **argv) {  if (argc > 1) yyin = fopen(argv[1], "r");  yyparse();  cout << "input program" << endl << endl;;  root->print();  cout << endl << endl << "evaluation of program" << endl << endl;  root->evaluate();
  
  cout << endl << endl << "final state of program" << endl << endl;
  for (map<string,float>::iterator it=state.begin(); it!=state.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';}void yyerror(const char *s) {  fprintf(stderr, "line %d: %s\n", lineNumber, s);}