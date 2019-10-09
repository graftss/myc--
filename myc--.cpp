#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include "myc--.h"

using namespace std;map<string, Value*> state;

// Type

string Type::toString(ValueType valueType) {
  switch (valueType) {
    case BOOL: return "bool";
    case INT: return "int";
    case FLOAT: return "float";
    case STRING: return "string";
    case CHAR: return "char";
    case FUNC: return "func";
  }
}

// Value 

Value* Value::fromBool(bool b) {
  Value *v = new Value;
  v->value.b = b;
  v->valueType = BOOL;
  return v;
}

Value* Value::fromInt(int i) {
  Value *v = new Value;
  v->value.i = i;
  v->valueType = INT;
  return v;
}

Value* Value::fromFloat(float f) {
  Value *v = new Value;
  v->value.f = f;
  v->valueType = FLOAT;
  return v;
}

Value* Value::fromChar(char c) {
  Value *v = new Value;
  v->value.c = c;
  v->valueType = CHAR;
  return v;
}

Value* Value::fromString(string *s) {
  Value *v = new Value;
  v->value.s = s;
  v->valueType = STRING;
  return v;
}

Value* Value::fromFunc(NFuncDecl *func) {
  Value *v = new Value;
  v->value.func = func;
  v->valueType = FUNC;
  return v;
}

Value* Value::fromVoid() {
  Value *v = new Value;
  v->valueType = VOID;
  return v;
}

bool Value::toBool() { return value.b; }
int Value::toInt() { return value.i; }
float Value::toFloat() { return value.f; }
char Value::toChar() { return value.c; }
string* Value::toString() { return value.s; }
Value* Value::callFunc() { return value.func->evaluate(); }

void Value::print() {
  switch (valueType) {
    case BOOL: cout << (value.b ? "true" : "false"); break;
    case INT: cout << value.i; break;
    case FLOAT: cout << value.f; break;
    case CHAR: cout << "'" << value.c << "'"; break;
    case STRING: cout << '"' << *(toString()) << '"'; break;
    case FUNC: cout << "() -> " << Type::toString(value.func->returnType); break;
    case VOID: cout << "VOID"; break;
  }
}

bool Value::isTrue() {  switch (valueType) {
    case BOOL: return toBool();
    case INT: return toInt() != 0;
    case FLOAT: return toFloat() != 0;
    case CHAR: return toChar() != 0;
  }
  
  return true;
}

int Value::compare(Value *l, Value *r) {
  int lt = l->valueType, rt = r->valueType;
  if (lt != rt) return -2;

  switch (lt) {
    case FLOAT: {
      float lf = l->toFloat(), rf = r->toFloat();
      if (lf == rf) return 0;
      return lf < rf ? -1 : 1;
    }
    case CHAR: {
      char lc = l->toChar(), rc = r->toChar();
      if (lc == rc) return 0;
      return lc < rc ? -1 : 1;
    }
  }
  
  return -2;
}

// NNumber

NNumber::NNumber(float num) : num(num) {}
void NNumber::print() { cout << num; }
Value* NNumber::evaluate() { return Value::fromFloat(num); }

// NBoolean

NBoolean::NBoolean(bool b) : b(b) {}
void NBoolean::print() { cout << (b ? "true" : "false"); }
Value* NBoolean::evaluate() { return Value::fromBool(b); }

// NChar

NChar::NChar(char c) : c(c) {}
void NChar::print() { cout << "'" << c << "'"; }
Value* NChar::evaluate() { return Value::fromChar(c); }

// NString

NString::NString(string *s) : s(s) {}
void NString::print() { cout << '"' << *s << '"'; }
Value* NString::evaluate() { return Value::fromString(s); }

// NIdentifier

NIdentifier::NIdentifier(string id) : id(id) {}
void NIdentifier::print() { cout << id; }
Value* NIdentifier::evaluate() { return state[id]; }

// NBinaryOp

NBinaryOp::NBinaryOp(NExpression *left, NExpression *right, int tag)   : left(left), right(right), tag(tag) {}
  void NBinaryOp::print() {
  string op;
  switch (tag) {
    case OP_PLUS: op = "+"; break;
    case OP_MINUS: op = "-"; break;
    case OP_TIMES: op = "*"; break;
    case OP_DIVIDE: op = "/"; break;
    case OP_MODULO: op = "%"; break;

    case OP_LEQ: op = "<="; break;
    case OP_GEQ: op = ">="; break;
    case OP_EQ: op = "=="; break;
    case OP_NEQ: op = "!="; break;
    case OP_AND: op = "&&"; break;
    case OP_OR: op = "||"; break;
    case OP_LT: op = "<"; break;
    case OP_GT: op = ">"; break;
  }
  
  cout << "(";
  left->print();
  cout << " " << op << " ";
  right->print();
  cout << ")";
}

Value* NBinaryOp::evaluate() {  Value* l = left->evaluate();
  Value* r = right->evaluate();
  
  switch (tag) {
    case OP_PLUS: return Value::fromFloat(l->toFloat() + r->toFloat());
    case OP_MINUS: return Value::fromFloat(l->toFloat() - r->toFloat());
    case OP_TIMES: return Value::fromFloat(l->toFloat() * r->toFloat());
    case OP_DIVIDE: return Value::fromFloat(l->toFloat() / r->toFloat());
//    case OP_MODULO: return Value::fromFloat(l->toFloat() % r->toFloat());

    case OP_AND: return Value::fromBool(l->toBool() && r->toBool());
    case OP_OR: return Value::fromBool(l->toBool() || r->toBool());
    
    case OP_GT: return Value::fromBool(Value::compare(l, r) == 1);
    case OP_LT: return Value::fromBool(Value::compare(l, r) == -1);
    case OP_EQ: return Value::fromBool(Value::compare(l, r) == 0);
    case OP_NEQ: return Value::fromBool(Value::compare(l, r) != 0);
    case OP_LEQ: {
      int comp = Value::compare(l, r);
      return Value::fromBool(comp == -1 || comp == 0);
    }
    case OP_GEQ: {
      int comp = Value::compare(l, r);
      return Value::fromBool(comp == 1 || comp == 0);
    }
  }
}

// NUnaryOp

NUnaryOp::NUnaryOp(NExpression *expr, int tag) : expr(expr), tag(tag) {}

void NUnaryOp::print() {  string op;
  switch (tag) {
    case OP_NOT: op = "!"; break;
  }
  
  cout << "( " << op;
  expr->print();
  cout << " )";
}

Value* NUnaryOp::evaluate() {  Value* v = expr->evaluate();
  
  switch (tag) {
    case OP_NOT: return Value::fromBool(!v->toBool());
  }
}


// NBlock 
NBlock::NBlock(NStatement *head) {
  statements = new list<NStatement*>;
  statements->push_front(head);
}

void NBlock::print(int indent) {
  list<NStatement*>::iterator it;

  for (it=statements->begin(); it != statements->end(); ++it) {
    cout << std::string(indent, ' ');
    (*it)->print();
    cout << ";" << endl;
  }
}

void NBlock::print() {
  print(0);
}

Value* NBlock::evaluate() {
  list<NStatement*>::iterator it;

  for (it=statements->begin(); it != statements->end(); ++it) {
    Value *v = (*it)->evaluate();

    if ((*it)->isReturn) return v;
  }
  
  return Value::fromVoid();} 

// NReturn 

NReturn::NReturn(NExpression *expr) : expr(expr) {
  isReturn = true;
}

void NReturn::print() {  cout << "return ";
  expr->print();
}

Value* NReturn::evaluate() {
  return expr->evaluate();
}

// NAssign 
NAssign::NAssign(string id, NExpression *expr)   : id(id), expr(expr) {}void NAssign::print() {  cout << id << " = ";  expr->print();
}Value* NAssign::evaluate() {
  Value *v = expr->evaluate();
  state[id] = v;}
// NVarDecl

NVarDecl::NVarDecl(ValueType type, string id) 
  : id(id), type(type) {}
  
NVarDecl::NVarDecl(ValueType type, string id, NExpression *expr) 
  : type(type), id(id), expr(expr) {}void NVarDecl::print() {
  cout << Type::toString(type) << " " << id;
  if (expr != NULL) {
    cout << " = ";
    expr->print();
  }
}

Value* NVarDecl::evaluate() {  Value *v = expr == NULL ? Value::fromVoid() : expr->evaluate();
  state[id] = v;
  
  return Value::fromVoid();
}

// NFuncDecl

NFuncDecl::NFuncDecl(ValueType returnType, string id, NBlock *body)
  : returnType(returnType), id(id), body(body) {}
  
void NFuncDecl::print() {
  cout << Type::toString(returnType) << " " << id << "() {" << endl;
  body->print(2);
  cout << "}";
}

Value* NFuncDecl::evaluate() {
  state[id] = Value::fromFunc(this);
  
  return Value::fromVoid();
}

Value* NFuncDecl::call() {
  return body->evaluate();
}

// NFuncCall

NFuncCall::NFuncCall(string id) : id(id) {}

void NFuncCall::print() {
  cout << id << "()";
}

Value* NFuncCall::evaluate() {
  return state[id]->value.func->call();
}

// NWhile

NWhile::NWhile(NExpression *cond, NBlock *body)
  : cond(cond), body(body) {}

void NWhile::print() {  cout << "while (";
  cond->print();
  cout << ") {" << endl;
  body->print(2);
  cout << "}" << endl;
}

Value* NWhile::evaluate() {
  while (cond->evaluate()->isTrue()) {
    body->evaluate();
  }
  
  return Value::fromVoid();}

// NDoWhile

NDoWhile::NDoWhile(NExpression *cond, NBlock *body)
  : cond(cond), body(body) {}

void NDoWhile::print() {
  cout << "do {" << endl;
  body->print(2);
  cout << "} while (";
  cond->print();
  cout << ")" << endl;
}

Value* NDoWhile::evaluate() {
  do {
    body->evaluate();
  } while (cond->evaluate()->isTrue());

  return Value::fromVoid();
}

// NFor

NFor::NFor(NStatement *init, NExpression *cond, NExpression *incr, NBlock *body)
  : init(init), cond(cond), incr(incr), body(body) {}

void NFor::print() {
  cout << "for (";
  init->print();
  cout << "; ";
  cond->print();
  cout << "; ";
  incr->print();
  cout << ") {" << endl;
  body->print(2);
  cout << "}";
}

Value* NFor::evaluate() {
  for (init->evaluate(); cond->evaluate()->isTrue(); incr->evaluate()) {
    body->evaluate();
  }

  return Value::fromVoid();
}