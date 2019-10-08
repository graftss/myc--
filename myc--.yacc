%{#include <iostream>#include <string>#include <stdlib.h>#include <stdio.h>#include <map>#include <list>#include "myc--.h"using namespace std;extern FILE *yyin;int yylex();void yyerror(const char* s);NBlock *root;int lineNumber = 1;%}%start program%union {  float number;  char *ident;  NExpression *expr;  NStatement *stmt;  NBlock *block;}%error-verbose%token <number> NUMBER%token <ident> ID%token MINUS PLUS TIMES DIVIDE LPARENS RPARENS LBRACE RBRACE %token EQUALS SEMICOLON PRINT MODULO
%type <block> program%type <block> block;%type <stmt> stmt%type <expr> expr%type <expr> expr9%type <expr> expr8%type <expr> expr7%type <expr> expr6%type <expr> expr5%type <expr> expr4%type <expr> expr3%type <expr> expr2%type <expr> expr1%type <expr> expr0%%program : block { root = $$; };block: stmt block {     $2->statements->push_front($1);    $$ = $2;  }  | stmt {    $$ = new NBlock($1);  };stmt: ID EQUALS expr SEMICOLON { $$ = new NAssign($1, $3); };expr: expr9;expr9: expr8;expr8: expr7;expr7: expr6;expr6: expr5;expr5: expr4;expr4: expr3  | expr4 PLUS expr3 { $$ = new NBinaryOp($1, $3, OP_PLUS); }  | expr4 MINUS expr3 { $$ = new NBinaryOp($1, $3, OP_MINUS); };expr3: expr2  | expr3 TIMES expr2 { $$ = new NBinaryOp($1, $3, OP_TIMES); }  | expr3 DIVIDE expr2 { $$ = new NBinaryOp($1, $3, OP_DIVIDE); };expr2: expr1;expr1: expr0;expr0:     ID { $$ = new NIdentifier($1); }  | NUMBER { $$ = new NNumber($1); };%%int main(int argc, char **argv) {  if (argc > 1) yyin = fopen(argv[1], "r");  yyparse();  cout << "input program" << endl << endl;;  root->print();  cout << endl << "evaluation of program" << endl << endl;  root->evaluate();
  
  cout << endl << endl << "final state of program" << endl << endl;  map<string, Value*>::iterator it;
  for (it=state.begin(); it!=state.end(); ++it) {
    std::cout << it->first << " => ";    it->second->print();     std::cout << '\n';  }}void yyerror(const char *s) {  fprintf(stderr, "line %d: %s\n", lineNumber, s);}