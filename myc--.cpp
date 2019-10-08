#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include "myc--.h"

using namespace std;map<string, Value*> state;

// Value 

Value* Value::fromFloat(float f) {
  Value *v = new Value;
  v->value.f = f;
  v->valueType = FLOAT;
  return v;
}

Value* Value::fromBool(bool b) {
  Value *v = new Value;
  v->value.b = b;
  v->valueType = BOOL;
  return v;
}

Value* Value::fromString(string *s) {
  Value *v = new Value;
  v->value.s = s;
  v->valueType = STRING;
  return v;
}

Value* Value::fromChar(char c) {
  Value *v = new Value;
  v->value.c = c;
  v->valueType = CHAR;
  return v;
}

bool Value::toBool() { return value.b; }
float Value::toFloat() { return value.f; }
char Value::toChar() { return value.c; }
string* Value::toString() { return value.s; }

void Value::print() {
  switch (valueType) {
    case BOOL: cout << (value.b ? "true" : "false"); break;
    case FLOAT: cout << value.f; break;
    case CHAR: cout << "'" << value.c << "'"; break;
    case STRING: cout << '"' << *(toString()) << '"'; break;
  }
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