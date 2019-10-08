%{#include <iostream>#include <string>#include <stdlib.h>#include <stdio.h>#include <map>#include <list>#include "myc--.h"using namespace std;extern FILE *yyin;int yylex();void yyerror(const char* s);NBlock *root;int lineNumber = 1;%}%start program%union {  float number;  char *ident;  NExpression *expr;  NStatement *stmt;  NBlock *block;}%error-verbose%token <number> NUMBER%token <ident> ID%token MINUS PLUS TIMES DIVIDE LPARENS RPARENS LBRACE RBRACE EQUALS SEMICOLON PRINT
%type <block> program%type <block> block;%type <stmt> stmt%type <expr> expr%%program : block { root = $$; };block: stmt block {     $2->statements->push_front($1);    $$ = $2;  }  | stmt {    $$ = new NBlock($1);  };stmt: ID EQUALS expr SEMICOLON { $$ = new NAssign($1, $3); };expr:     ID { $$ = new NIdentifier($1); }  | NUMBER { $$ = new NNumber($1); }
  | expr PLUS expr { $$ = new NBinaryOp($1, $3, OP_PLUS); }
  | expr MINUS expr { $$ = new NBinaryOp($1, $3, OP_MINUS); }
  | expr TIMES expr { $$ = new NBinaryOp($1, $3, OP_TIMES); }
  | expr DIVIDE expr { $$ = new NBinaryOp($1, $3, OP_DIVIDE); };%%int main(int argc, char **argv) {  if (argc > 1) yyin = fopen(argv[1], "r");  yyparse();  cout << "input program" << endl << endl;;  root->print();  cout << endl << "evaluation of program" << endl << endl;  root->evaluate();
  
  cout << endl << endl << "final state of program" << endl << endl;  map<string, Value*>::iterator it;
  for (it=state.begin(); it!=state.end(); ++it) {
    std::cout << it->first << " => ";    it->second->print();     std::cout << '\n';  }}void yyerror(const char *s) {  fprintf(stderr, "line %d: %s\n", lineNumber, s);}