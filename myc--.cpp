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

void Value::print() {
  switch (valueType) {
    case BOOL: cout << (value.b ? "true" : "false"); break;
    case INT: cout << value.i; break;
    case FLOAT: cout << value.f; break;
    case CHAR: cout << "'" << value.c << "'"; break;
    case STRING: cout << '"' << *(toString()) << '"'; break;
    case VOID: cout << "VOID"; break;
  }
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

void NBlock::print() {
  list<NStatement*>::iterator it;

  for (it=statements->begin(); it != statements->end(); ++it) {
    (*it)->print();
    cout << endl;
  }
}

void NBlock::evaluate() {
  list<NStatement*>::iterator it;

  for (it=statements->begin(); it != statements->end(); ++it) {
    (*it)->evaluate();
  }} 

// NAssign 
NAssign::NAssign(string id, NExpression *expr)   : id(id), expr(expr) {}void NAssign::print() {  cout << id << " = ";  expr->print();
  cout << ";";}void NAssign::evaluate() {
  Value *v = expr->evaluate();

  cout << "ASSIGN: " << id << " <= ";
  v->print();
  cout << endl;
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
  cout << ";";
}

void NVarDecl::evaluate() {  Value *v = expr == NULL ? Value::fromVoid() : expr->evaluate();
  state[id] = v;
}