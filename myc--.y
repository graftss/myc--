%{
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <list>
#include "myc--.h"

using namespace std;

extern FILE *yyin;
extern int yyleng;
int yylex();
void yyerror(const char* s);

NBlock *root;
int lineNumber = 1;
%}

%start program

%union {
  float number;
  char literalChar;
  char *literalString;
  
  list<NVarDecl*> *paramList;
  list<NExpression*> *exprList;
  
  ValueType valueType;
  char *ident;
  NFuncCall *callExpr;
  NExpression *expr;
  NStatement *stmt;
  NBlock *block;
}

%error-verbose

%token <number> NUMBER
%token <literalChar> CHAR_LITERAL
%token <literalString> STRING_LITERAL
%token <ident> ID
%token LEQ GEQ EQ NEQ AND OR
%token NOT LT GT MINUS PLUS TIMES DIVIDE MODULO ASSIGN
%token LPARENS RPARENS LBRACE RBRACE LINDEX RINDEX DQUOTE SQUOTE 
%token SEMICOLON COMMA
%token PRINT READ TRUE FALSE RETURN WHILE FOR DO IF ELSE
%token TYPE_BOOL TYPE_INT TYPE_FLOAT TYPE_STRING TYPE_CHAR TYPE_VOID

%type <block> program
%type <block> block
%type <block> nonempty_block
%type <stmt> stmt
%type <stmt> return_stmt
%type <stmt> while_stmt
%type <stmt> do_while_stmt
%type <stmt> for_stmt
%type <stmt> for_init
%type <expr> for_cond
%type <expr> for_incr
%type <stmt> branch_stmt
%type <stmt> assn_stmt
%type <stmt> var_decl_stmt
%type <stmt> arr_decl_stmt
%type <stmt> func_decl_stmt
%type <stmt> print_stmt
%type <stmt> read_stmt
%type <paramList> func_param_list
%type <expr> expr
%type <expr> expr9
%type <expr> expr8
%type <expr> expr7
%type <expr> expr6
%type <expr> expr5
%type <expr> expr4
%type <expr> expr3
%type <expr> expr2
%type <expr> expr1
%type <callExpr> call_expr
%type <exprList> comma_sep_exprs
%type <expr> index_expr
%type <expr> expr0
%type <expr> literal
%type <valueType> type

%%

program : block { root = $$; }

block:
    /* empty */ { $$ = new NBlock(); }
  | nonempty_block { $$ = $1; }
;
  
nonempty_block:
    stmt nonempty_block { 
      $2->statements->push_front($1);
      $$ = $2;
    } 
  | stmt { $$ = new NBlock($1); }
;

stmt: 
    return_stmt
  | while_stmt
  | do_while_stmt
  | for_stmt
  | branch_stmt
  | assn_stmt 
  | var_decl_stmt
  | arr_decl_stmt
  //| call_expr SEMICOLON { $$ = $1; }
  | func_decl_stmt
  | print_stmt
  | read_stmt
;

return_stmt: 
  RETURN expr SEMICOLON { $$ = new NReturn($2); }
;

while_stmt: WHILE LPARENS expr RPARENS LBRACE block RBRACE { 
  $$ = new NWhile($3, $6); 
}
;

do_while_stmt: DO LBRACE block RBRACE WHILE LPARENS expr RPARENS SEMICOLON { 
  $$ = new NDoWhile($7, $3); 
}
;

for_stmt: FOR LPARENS for_init for_cond SEMICOLON for_incr RPARENS LBRACE block RBRACE {
  $$ = new NFor($3, $4, $6, $9);
}
;

for_init: var_decl_stmt { $$ = $1; }
;

for_cond: expr { $$ = $1; }
;

for_incr: expr { $$ = $1; }
; 

branch_stmt: 
    IF LPARENS expr RPARENS LBRACE block RBRACE { 
      $$ = new NBranch($3, $6, NULL);
    }
  | IF LPARENS expr RPARENS LBRACE block RBRACE ELSE LBRACE block RBRACE {
      $$ = new NBranch($3, $6, $10);
    }
;  

assn_stmt: 
    ID ASSIGN expr SEMICOLON { $$ = new NAssign($1, $3); }
  | ID LINDEX expr RINDEX ASSIGN expr SEMICOLON { $$ = new NIndexAssign($1, $3, $6); }
;

var_decl_stmt: 
    type ID SEMICOLON { $$ = new NVarDecl($1, $2); }
  | type ID ASSIGN expr SEMICOLON { $$ = new NVarDecl($1, $2, $4); }
;

arr_decl_stmt:
    type ID LINDEX expr RINDEX SEMICOLON { $$ = new NArrayDecl($1, $2, $4); }
;

func_decl_stmt: 
    type ID LPARENS RPARENS LBRACE block RBRACE {
      $$ = new NFuncDecl($1, $2, $6);
    }
  | type ID LPARENS func_param_list RPARENS LBRACE block RBRACE { 
      $$ = new NFuncDecl($1, $2, $7, $4); 
    }
;

func_param_list:
    type ID { 
      $$ = new list<NVarDecl*>;
      $$->push_front(new NVarDecl($1, $2));
    }
  | type ID COMMA func_param_list {
      $$ = $4;
      $$->push_front(new NVarDecl($1, $2));
    }
;

print_stmt: 
  PRINT comma_sep_exprs SEMICOLON { $$ = new NPrint($2); }
;

read_stmt: 
    READ TYPE_INT ID SEMICOLON { $$ = new NRead(INT, $3); }
  | READ TYPE_FLOAT ID SEMICOLON { $$ = new NRead(FLOAT, $3); }
  | READ TYPE_CHAR ID SEMICOLON { $$ = new NRead(CHAR, $3); }
  | READ TYPE_STRING ID SEMICOLON { $$ = new NRead(STRING, $3); }
;

type: 
    TYPE_BOOL { $$ = BOOL; }
  | TYPE_INT { $$ = INT; }
  | TYPE_FLOAT { $$ = FLOAT; }
  | TYPE_CHAR { $$ = CHAR; }
  | TYPE_STRING { $$ = STRING; }
  | TYPE_VOID { $$ = VOID; }
;

expr: expr9
;

expr9: expr8
  | ID ASSIGN expr9 { $$ = new NAssign($1, $3); }
;

expr8: expr7
  | expr8 OR expr7 { $$ = new NBinaryOp($1, $3, OP_OR); }
;

expr7: expr6
  | expr7 AND expr6 { $$ = new NBinaryOp($1, $3, OP_AND); }
;

expr6: expr5
  | expr6 EQ expr5 { $$ = new NBinaryOp($1, $3, OP_EQ); }
  | expr6 NEQ expr5 { $$ = new NBinaryOp($1, $3, OP_NEQ); }
;

expr5: expr4
  | expr5 LT expr4 { $$ = new NBinaryOp($1, $3, OP_LT); }
  | expr5 GT expr4 { $$ = new NBinaryOp($1, $3, OP_GT); }
  | expr5 LEQ expr4 { $$ = new NBinaryOp($1, $3, OP_LEQ); }
  | expr5 GEQ expr4 { $$ = new NBinaryOp($1, $3, OP_GEQ); }
;

expr4: expr3
  | expr4 PLUS expr3 { $$ = new NBinaryOp($1, $3, OP_PLUS); }
  | expr4 MINUS expr3 { $$ = new NBinaryOp($1, $3, OP_MINUS); }
;

expr3: expr2
  | expr3 TIMES expr2 { $$ = new NBinaryOp($1, $3, OP_TIMES); }
  | expr3 DIVIDE expr2 { $$ = new NBinaryOp($1, $3, OP_DIVIDE); }
  | expr3 MODULO expr2 { $$ = new NBinaryOp($1, $3, OP_MODULO); }
;

expr2: expr1
  | NOT expr1 { $$ = new NUnaryOp($2, OP_NOT); }
;

expr1: expr0
  | call_expr
  | index_expr
;

call_expr: 
    ID LPARENS RPARENS { $$ = new NFuncCall($1); }
  | ID LPARENS comma_sep_exprs RPARENS { $$ = new NFuncCall($1, $3); }
;

comma_sep_exprs: 
    expr { 
      $$ = new list<NExpression*>;
      $$->push_front($1);
    }
  | expr COMMA comma_sep_exprs {
      $$ = $3;
      $$->push_front($1);
    }
;

index_expr: 
  expr0 LINDEX expr RINDEX { $$ = new NIndex($1, $3); }
;

expr0: 
    ID { $$ = new NIdentifier($1); }
  | LPARENS expr RPARENS { $$ = $2; }
  | literal
;

literal: 
    NUMBER { $$ = new NNumber($1); }
  | TRUE { $$ = new NBoolean(true); }
  | FALSE { $$ = new NBoolean(false); }
  | CHAR_LITERAL { $$ = new NChar($1); }
  | STRING_LITERAL { 
    string s ($1 + 1, yyleng - 2);
    $$ = new NString(s); 
  }
;

%%

int main(int argc, char **argv) {
  if (argc > 1) yyin = fopen(argv[1], "r");

  yyparse();
  
  /*
  cout << "----------source" << endl << endl;
  root->print();
  
  cout << endl << "----------AST" << endl << endl;
  root->printNode();

  cout << endl << "----------evaluation" << endl << endl;
  Value* value = root->evaluate();
  if (value->valueType != VOID) value->print();
  
  cout << endl << endl << "----------final state" << endl << endl;
  map<string, Value*>::iterator it;
  for (it=state.begin(); it!=state.end(); ++it) {
    std::cout << it->first << " => ";
    it->second->print(); 
    std::cout << '\n';
  }
  */

  CFG* cfgRootNode = root->makeCFG();

  cout << endl << endl << "----------CFG Label/AST Map Verification" << endl << endl;
  map<int, CFG*> labelNodeMap = cfgRootNode->createLabelNodeMap();
  CFG::printLabelNodeMap(labelNodeMap);

  cout << endl << endl << "----------CFG EdgeMap Verification" << endl << endl;
  list<tuple<int, int>> edgeList = *(cfgRootNode->extraEdges);
  CFG::printLabelEdgeMap(edgeList);
  
  list<int> *labels = cfgRootNode->labelsTo(12);
  list<int>::iterator itt;
  list<int> *allLabels = cfgRootNode->allLabels();
  
  /*
  cout << endl << "assignments to x: ";
  labels = cfgRootNode->assignmentsToId("x");
  for (itt = labels->begin(); itt != labels->end(); ++itt) {
    cout << (*itt) << " ";
  }
  cout << endl;
  
  cout << "all labels: ";
  for (itt = allLabels->begin(); itt != allLabels->end(); ++itt) {
    cout << (*it1) << " ";
  }
  cout << endl;
  
  cout << "all identifiers: ";
  list<string> *ids = cfgRootNode->allIds();
  list<string>::iterator it2;
  for (it2 = ids->begin(); it2 != ids->end(); ++it2) {
    cout << (*it2) << " ";
  }
  cout << endl;
  */
  
  int label;
  
  cout << endl << endl << "----------entry equations" << endl << endl;
  for (itt = allLabels->begin(); itt != allLabels->end(); ++itt) {
    label = *itt;
    cfgRootNode->printRDEntryEqn(label);
    cout << endl;
  }
  
  cout << endl << endl << "----------exit equations" << endl << endl;
  for (itt = allLabels->begin(); itt != allLabels->end(); ++itt) {
    label = *itt;
    cfgRootNode->printRDExitEqn(label);
    cout << endl;
  }
  
  cout << endl << endl << "-----------RD solutions" << endl << endl;
  tuple<RDAnalysis*, RDAnalysis*> soln = Worklist::solveRD(cfgRootNode);

  cout << "entry solutions:" << endl;
  printRDAnalysis(get<0>(soln));

  cout << endl << endl << "exit solutions:" << endl;
  printRDAnalysis(get<1>(soln));
}

void yyerror(const char *s) {
  fprintf(stderr, "line %d: %s\n", lineNumber, s);
}
