%{#include <iostream>#include <string>#include <stdlib.h>#include <stdio.h>#include <map>#include <list>#include "myc--.h"using namespace std;extern FILE *yyin;int yylex();void yyerror(const char* s);NBlock *root;int lineNumber = 1;%}%start program%union {  float number;  char literalChar;  string *literalString;    list<NVarDecl*> *paramList;  list<NExpression*> *exprList;    ValueType valueType;  char *ident;  NExpression *expr;  NStatement *stmt;  NBlock *block;}%error-verbose%token <number> NUMBER%token <literalChar> CHAR_LITERAL%token <literalString> STRING_LITERAL%token <ident> ID%token LEQ GEQ EQ NEQ AND OR%token NOT LT GT MINUS PLUS TIMES DIVIDE MODULO ASSIGN%token LPARENS RPARENS LBRACE RBRACE LINDEX RINDEX DQUOTE SQUOTE %token SEMICOLON COMMA%token TRUE FALSE RETURN WHILE FOR DO IF ELSE %token TYPE_BOOL TYPE_INT TYPE_FLOAT TYPE_STRING TYPE_CHAR%type <block> program%type <block> block%type <block> nonempty_block%type <stmt> stmt%type <stmt> return_stmt%type <stmt> for_stmt%type <stmt> for_init%type <expr> for_cond%type <expr> for_incr%type <stmt> branch_stmt%type <stmt> assn_stmt%type <stmt> var_decl_stmt%type <stmt> arr_decl_stmt%type <stmt> func_decl_stmt%type <paramList> func_param_list%type <expr> expr%type <expr> expr9%type <expr> expr8%type <expr> expr7%type <expr> expr6%type <expr> expr5%type <expr> expr4%type <expr> expr3%type <expr> expr2%type <expr> expr1%type <expr> call_expr%type <exprList> call_arg_list%type <expr> index_expr%type <expr> expr0%type <expr> literal%type <valueType> type%%program : block { root = $$; }block:    /* empty */ { $$ = new NBlock(); }  | nonempty_block { $$ = $1; }  nonempty_block:    stmt nonempty_block {       $2->statements->push_front($1);      $$ = $2;    }   | stmt { $$ = new NBlock($1); }stmt:     return_stmt  | WHILE LPARENS expr RPARENS LBRACE block RBRACE { $$ = new NWhile($3, $6); }  | DO LBRACE block RBRACE WHILE LPARENS expr RPARENS { $$ = new NDoWhile($7, $3); }  | for_stmt  | branch_stmt  | assn_stmt   | var_decl_stmt  | arr_decl_stmt  | call_expr SEMICOLON { $$ = (NStatement*) $1; }  | func_decl_stmt  return_stmt: RETURN expr SEMICOLON { $$ = new NReturn($2); }for_stmt: FOR LPARENS for_init for_cond SEMICOLON for_incr RPARENS LBRACE block RBRACE {  $$ = new NFor($3, $4, $6, $9);}for_init: var_decl_stmt { $$ = $1; }for_cond: expr { $$ = $1; }for_incr: expr { $$ = $1; }  branch_stmt:     IF LPARENS expr RPARENS LBRACE block RBRACE {       $$ = new NBranch($3, $6, NULL);    }  | IF LPARENS expr RPARENS LBRACE block RBRACE ELSE LBRACE block RBRACE {      $$ = new NBranch($3, $6, $10);    }  assn_stmt:     ID ASSIGN expr SEMICOLON { $$ = new NAssign($1, $3); }  | ID LINDEX expr RINDEX ASSIGN expr SEMICOLON { $$ = new NIndexAssign($1, $3, $6); }var_decl_stmt:     type ID SEMICOLON { $$ = new NVarDecl($1, $2); }  | type ID ASSIGN expr SEMICOLON { $$ = new NVarDecl($1, $2, $4); }arr_decl_stmt:    type ID LINDEX expr RINDEX SEMICOLON { $$ = new NArrayDecl($1, $2, $4); }func_decl_stmt:     type ID LPARENS RPARENS LBRACE block RBRACE {      $$ = new NFuncDecl($1, $2, $6);    }  | type ID LPARENS func_param_list RPARENS LBRACE block RBRACE {       $$ = new NFuncDecl($1, $2, $7, $4);     }func_param_list:    type ID {       $$ = new list<NVarDecl*>;      $$->push_front(new NVarDecl($1, $2));    }  | type ID COMMA func_param_list {      $$ = $4;      $$->push_front(new NVarDecl($1, $2));    }  type:     TYPE_BOOL { $$ = BOOL; }  | TYPE_INT { $$ = INT; }  | TYPE_FLOAT { $$ = FLOAT; }  | TYPE_CHAR { $$ = CHAR; }  | TYPE_STRING { $$ = STRING; }expr: expr9expr9: expr8  | ID ASSIGN expr9 { $$ = new NAssign($1, $3); }expr8: expr7  | expr8 OR expr7 { $$ = new NBinaryOp($1, $3, OP_OR); }expr7: expr6  | expr7 AND expr6 { $$ = new NBinaryOp($1, $3, OP_AND); }expr6: expr5  | expr6 EQ expr5 { $$ = new NBinaryOp($1, $3, OP_EQ); }  | expr6 NEQ expr5 { $$ = new NBinaryOp($1, $3, OP_NEQ); }expr5: expr4  | expr5 LT expr4 { $$ = new NBinaryOp($1, $3, OP_LT); }  | expr5 GT expr4 { $$ = new NBinaryOp($1, $3, OP_GT); }  | expr5 LEQ expr4 { $$ = new NBinaryOp($1, $3, OP_LEQ); }  | expr5 GEQ expr4 { $$ = new NBinaryOp($1, $3, OP_GEQ); }expr4: expr3  | expr4 PLUS expr3 { $$ = new NBinaryOp($1, $3, OP_PLUS); }  | expr4 MINUS expr3 { $$ = new NBinaryOp($1, $3, OP_MINUS); }expr3: expr2  | expr3 TIMES expr2 { $$ = new NBinaryOp($1, $3, OP_TIMES); }  | expr3 DIVIDE expr2 { $$ = new NBinaryOp($1, $3, OP_DIVIDE); }expr2: expr1  | NOT expr1 { $$ = new NUnaryOp($2, OP_NOT); }expr1: expr0  | call_expr  | index_expr  call_expr:     ID LPARENS RPARENS { $$ = new NFuncCall($1); }  | ID LPARENS call_arg_list RPARENS { $$ = new NFuncCall($1, $3); }call_arg_list:     expr {       $$ = new list<NExpression*>;      $$->push_front($1);    }  | expr COMMA call_arg_list {      $$ = $3;      $$->push_front($1);    }index_expr: expr0 LINDEX expr RINDEX { $$ = new NIndex($1, $3); }expr0:     ID { $$ = new NIdentifier($1); }  | LPARENS expr RPARENS { $$ = $2; }  | literalliteral:     NUMBER { $$ = new NNumber($1); }  | TRUE { $$ = new NBoolean(true); }  | FALSE { $$ = new NBoolean(false); }  | CHAR_LITERAL { $$ = new NChar($1); }%%int main(int argc, char **argv) {  if (argc > 1) yyin = fopen(argv[1], "r");  yyparse();  cout << "===========input program" << endl << endl;;  root->print();  cout << endl << "============evaluation of program" << endl << endl;  root->evaluate()->print();
  
  cout << endl << endl << "=============final state of program" << endl << endl;  map<string, Value*>::iterator it;
  for (it=state.begin(); it!=state.end(); ++it) {
    std::cout << it->first << " => ";    it->second->print();     std::cout << '\n';  }}void yyerror(const char *s) {  fprintf(stderr, "line %d: %s\n", lineNumber, s);}