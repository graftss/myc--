#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <cmath>
#include "myc--.h"

using namespace std;
map<string, Value*> state;

int blockDepth = 0;
string blockIndent() { return std::string(blockDepth * 2, ' '); }

int treeDepth = 0;
string treeIndent() { return std::string(treeDepth * 2, ' '); }

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

Value* Value::fromArray(ValueArray *array) {
  Value *v = new Value;
  v->value.array = array;
  v->valueType = ARRAY;
  return v;
}

Value* Value::fromVoid() {
  Value *v = new Value;
  v->valueType = VOID;
  return v;
}

bool Value::toBool() { return value.b; }

int Value::toInt() { 
  if (valueType == FLOAT) return std::floor(value.f);
  return value.i; 
}

float Value::toFloat() { return value.f; }

char Value::toChar() { return value.c; }

string* Value::toString() { return value.s; }

Value* Value::callFunc() { return value.func->evaluate(); }

ValueArray* Value::toArray() { return value.array; }

void Value::print() {
  switch (valueType) {
    case BOOL: cout << (value.b ? "true" : "false"); break;
    case INT: cout << value.i; break;
    case FLOAT: cout << value.f; break;
    case CHAR: cout << "'" << value.c << "'"; break;
    case STRING: cout << '"' << *(toString()) << '"'; break;
    case FUNC: cout << "() -> " << Type::toString(value.func->returnType); break;
    case VOID: cout << "VOID"; break;
    case ARRAY: {
      ValueArray *arr = toArray();
      cout << Type::toString(arr->valueType) << "[" << arr->size << "]";
      break;
    }
  }
}

bool Value::isTrue() {
  switch (valueType) {
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

// ValueArray

ValueArray::ValueArray(ValueType valueType, int size) 
  : valueType(valueType), size(size) {
  std::vector<Value*> v (size, Value::fromVoid());
  
  values = v;
}

Value* ValueArray::getValue(int index) {
  return values.at(index);
}

void ValueArray::setValue(int index, Value* v) {
  values.at(index) = v;
}

// NNumber

NNumber::NNumber(float num) : num(num) {}
void NNumber::print() { cout << num; }
void NNumber::printNode() { cout << treeIndent() << "NNumber " << num << endl; }
Value* NNumber::evaluate() { return Value::fromFloat(num); }

// NBoolean

NBoolean::NBoolean(bool b) : b(b) {}
void NBoolean::print() { cout << (b ? "true" : "false"); }
void NBoolean::printNode() { cout << treeIndent() << "NBoolean " << b << endl; }
Value* NBoolean::evaluate() { return Value::fromBool(b); }

// NChar

NChar::NChar(char c) : c(c) {}
void NChar::print() { cout << "'" << c << "'"; }
void NChar::printNode() { cout << treeIndent() << "NChar " << c << endl; }
Value* NChar::evaluate() { return Value::fromChar(c); }

// NString

NString::NString(string *s) : s(s) {}
void NString::print() { cout << '"' << *s << '"'; }
void NString::printNode() { cout << treeIndent() << "NString " << s << endl; }
Value* NString::evaluate() { return Value::fromString(s); }

// NIdentifier

NIdentifier::NIdentifier(string id) : id(id) {}
void NIdentifier::print() { cout << id; }
void NIdentifier::printNode() { cout << treeIndent() << "NIdentifier " << id << endl; }
Value* NIdentifier::evaluate() { return state[id]; }

// NBinaryOp

NBinaryOp::NBinaryOp(NExpression *left, NExpression *right, int tag)   : left(left), right(right), tag(tag) {}
  
string NBinaryOp::opString() {
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
  return op;
}
  void NBinaryOp::print() {
  cout << "(";
  left->print();
  cout << " " << opString() << " ";
  right->print();
  cout << ")";
}

void NBinaryOp::printNode() {
  cout << treeIndent() << "NBinaryOp " << opString() << endl;

  treeDepth += 1;
  left->printNode();
  right->printNode();
  treeDepth -= 1;
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

string NUnaryOp::opString() {  string op;
  switch (tag) {
    case OP_NOT: op = "!"; break;
  }
  
  return op;
}

void NUnaryOp::print() {  cout << "( " << opString();
  expr->print();
  cout << " )";
}

void NUnaryOp::printNode() {
  cout << treeIndent() << "NUnaryOp " << opString() << endl;
  treeDepth += 1;
  expr->printNode();
  treeDepth -= 1;
}

Value* NUnaryOp::evaluate() {  Value* v = expr->evaluate();
  
  switch (tag) {
    case OP_NOT: return Value::fromBool(!v->toBool());
  }
}

// NIndex

NIndex::NIndex(NExpression *arrayExpr, NExpression *indexExpr) 
  : arrayExpr(arrayExpr), indexExpr(indexExpr) {}
  
void NIndex::print() {  arrayExpr->print();
  cout << "[";
  indexExpr->print();
  cout << "]";
}

void NIndex::printNode() {
  cout << treeIndent() << "NIndex" << endl;
  treeDepth += 1;
  arrayExpr->printNode();
  indexExpr->printNode();
  treeDepth -= 1;
}

Value* NIndex::evaluate() {  Value* array = arrayExpr->evaluate();
  Value* index = indexExpr->evaluate(); 
  return array->toArray()->getValue(index->toInt());
}

// NBlock 

NBlock::NBlock() {
  statements = new list<NStatement*>;
}

NBlock::NBlock(NStatement *head) {
  statements = new list<NStatement*>;
  statements->push_front(head);
}

void NBlock::print() {
  list<NStatement*>::iterator it;

  for (it = statements->begin(); it != statements->end(); ++it) {
    cout << blockIndent();
    (*it)->print();
    cout << endl;
  }
}

void NBlock::printIndented() {
  blockDepth += 1;
  cout << " {" << endl;
  print();
  blockDepth -= 1;
  cout << blockIndent() << "}";
}

void NBlock::printNode() {
  list<NStatement*>::iterator it;

  cout << treeIndent() << "NBlock" << endl;

  treeDepth += 1;
  for (it = statements->begin(); it != statements->end(); ++it) {
    (*it)->printNode();
  }
  treeDepth -= 1;
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

void NReturn::printNode() {  cout << treeIndent() << "NReturn";
  treeDepth += 1;
  expr->printNode();
  treeDepth -= 1;
}

Value* NReturn::evaluate() {
  return expr->evaluate();
}

// NAssign 
NAssign::NAssign(string id, NExpression *expr)   : id(id), expr(expr) {}void NAssign::print() {  cout << id << " = ";  expr->print();
}

void NAssign::printNode() {  cout << treeIndent() << "NAssign " << id << endl;
  treeDepth += 1;
  expr->printNode();
  treeDepth -= 1;
}Value* NAssign::evaluate() {
  Value *v = expr->evaluate();
  state[id] = v;
  return v;}

// NIndexAssign

NIndexAssign::NIndexAssign(string id, NExpression *indexExpr, NExpression *expr)
  : id(id), indexExpr(indexExpr), expr(expr) {}
  
void NIndexAssign::print() {  cout << id << "[";
  indexExpr->print();
  cout << "] = ";
  expr->print();
}

void NIndexAssign::printNode() {  cout << treeIndent() << "NIndexAssign " << id << endl;
  treeDepth += 1;
  indexExpr->printNode();
  expr->printNode();
  treeDepth -= 1;
}

Value* NIndexAssign::evaluate() {  ValueArray *array = state[id]->toArray();
  int index = indexExpr->evaluate()->toInt();
  Value* value = expr->evaluate();

  array->setValue(index, value);
}
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

void NVarDecl::printNode() {  cout << treeIndent() << "NVarDecl " << Type::toString(type) << " " << id << endl;
  treeDepth += 1;
  expr->printNode();
  treeDepth -= 1;
}

Value* NVarDecl::evaluate() {  Value *v = expr == NULL ? Value::fromVoid() : expr->evaluate();
  state[id] = v;
  
  return Value::fromVoid();
}

// NArrayDecl

NArrayDecl::NArrayDecl(ValueType type, string id, NExpression* sizeExpr) 
  : type(type), id(id), sizeExpr(sizeExpr) {}

void NArrayDecl::print() {
  cout << Type::toString(type) << " " << id << "[";
  sizeExpr->print();
  cout << "]";
}

void NArrayDecl::printNode() {
  cout << treeIndent() << "NArrayDecl " << Type::toString(type) << " " << id << endl;  treeDepth += 1;
  sizeExpr->printNode();
  treeDepth -= 1;
}
  
Value* NArrayDecl::evaluate() {
  int size = sizeExpr->evaluate()->toInt();
  ValueArray *array = new ValueArray(type, size);
  Value *v = Value::fromArray(array);
  
  state[id] = v;
    
  return Value::fromVoid();}
  
// NFuncDecl

NFuncDecl::NFuncDecl(
  ValueType returnType, 
  string id, 
  NBlock *body
) : returnType(returnType), id(id), body(body) {
  arguments = new list<NVarDecl*>;
}

NFuncDecl::NFuncDecl(
  ValueType returnType, 
  string id, 
  NBlock *body, 
  list<NVarDecl*> *arguments
) : returnType(returnType), id(id), body(body), arguments(arguments) {}
  
void NFuncDecl::print() {
  cout << Type::toString(returnType) << " " << id << "(";
  printArguments();
  cout << ")";
  body->printIndented();
  cout << endl;
}

void NFuncDecl::printArguments() {
  list<NVarDecl*>::iterator it = arguments->begin();
  
  while (it != arguments->end()) {    (*it)->print();
    if (++it != arguments->end()) {
      cout << ", ";
    }
  }
}

void NFuncDecl::printNode() {  cout << treeIndent() << "NFuncDecl " << Type::toString(returnType) << " " << id << endl;
  treeDepth += 1;  
  body->printNode();
  treeDepth -= 1;  
}

Value* NFuncDecl::evaluate() {
  state[id] = Value::fromFunc(this);
  
  return Value::fromVoid();
}

Value* NFuncDecl::call(list<Value*> *args) {
  map<string, Value*> temp = *(new map<string, Value*>);
  
  // save existing argument variables in global scope to temp state
  list<NVarDecl*>::iterator paramIt = arguments->begin();
  list<Value*>::iterator argIt = args->begin();
  for (; paramIt != arguments->end(); ++paramIt, ++argIt) {
    string id = (*paramIt)->id;

   if (state[id] != NULL) temp[id] = state[id];

    state[id] = (*argIt);
  }
  
  Value* result = body->evaluate();
  
  // return saved scope values to global scope
  for (paramIt = arguments->begin(); paramIt != arguments->end(); ++paramIt) {
    string id = (*paramIt)->id;

    if (temp[id] != NULL) state[id] = temp[id];
    else state.erase(id);
  }
  
  return result;
}

// NFuncCall

NFuncCall::NFuncCall(string id) : id(id) {
  arguments = new list<NExpression*>;
}

NFuncCall::NFuncCall(string id, list<NExpression*> *arguments) 
  : id(id), arguments(arguments) {}

void NFuncCall::print() {
  list<NExpression*>::iterator it = arguments->begin();
  
  cout << id << "(";
  while (it != arguments->end()) {
    (*it)->print();
    if (++it != arguments->end()) {
      cout << ", ";
    }
  }
  cout << ")";
}

void NFuncCall::printNode() {
  list<NExpression*>::iterator it;
    cout << treeIndent() << "NFuncCall " << id << endl;
  treeDepth += 1;
  for (it = arguments->begin(); it != arguments->end(); ++it) {
    (*it)->printNode();
  }
  treeDepth -= 1;
}

Value* NFuncCall::evaluate() {
  list<Value*> *values = new list<Value*>;
  list<NExpression*>::iterator it;
  for (it = arguments->begin(); it != arguments->end(); ++it) {
    values->push_back((*it)->evaluate());
  }
  
  return state[id]->value.func->call(values);
}

// NWhile

NWhile::NWhile(NExpression *cond, NBlock *body)
  : cond(cond), body(body) {}

void NWhile::print() {  cout << "while (";
  cond->print();
  cout << ")";
  body->printIndented();
  cout << "endl";
}

void NWhile::printNode() {
  cout << treeIndent() << "NWhile" << endl;
  treeDepth += 1;
  cond->printNode();
  body->printNode();
  treeDepth -= 1;
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
  cout << "do";
  body->printIndented();
  cout << " while (";
  cond->print();
  cout << ")" << endl;
}

void NDoWhile::printNode() {  cout << treeIndent() << "NDoWhile" << endl;
  treeDepth += 1;
  cond->printNode();
  body->printNode();
  treeDepth -= 1;
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
  cout << ")";
  body->printIndented();
}

void NFor::printNode() {  cout << treeIndent() << "NFor" << endl;
  treeDepth += 1;
  init->printNode();
  cond->printNode();
  incr->printNode();
  body->printNode();
  treeDepth -= 1;
}

Value* NFor::evaluate() {
  for (init->evaluate(); cond->evaluate()->isTrue(); incr->evaluate()) {
    body->evaluate();
  }

  return Value::fromVoid();
}

// NBranch

NBranch::NBranch(NExpression *cond, NBlock *pass, NBlock *fail)
  : cond(cond), pass(pass), fail(fail) {}

void NBranch::print() {
  cout << "if (";
  cond->print();
  cout << ") ";
  pass->printIndented();
  
  if (fail != NULL) {
    cout << " else {" << endl;
    fail->printIndented();
    cout << "}";
  }
}

void NBranch::printNode() {  cout << treeIndent() << "NBranch" << endl;
  treeDepth += 1;
  cond->printNode();
  pass->printNode();
  if (fail != NULL) fail->printNode();
  treeDepth -= 1;
}

Value* NBranch::evaluate() {
  bool test = cond->evaluate()->isTrue();
  if (test) pass->evaluate();
  else if (fail != NULL) fail->evaluate();
  
  return Value::fromVoid();
}

// NPrint

NPrint::NPrint(NExpression *expr) : expr(expr) {}

void NPrint::print() {  cout << "print ";
  expr->print();
}

void NPrint::printNode() {  cout << treeIndent() << "NPrint" << endl;
  treeDepth += 1;
  expr->printNode();
  treeDepth -= 1;
}

Value* NPrint::evaluate() {
  expr->evaluate()->print();
  cout << endl;
  return Value::fromVoid();}