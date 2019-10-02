#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include "myc--.h"

using namespace std;map<string, float> state;

NBinaryOp::NBinaryOp(NExpression *left, NExpression *right)   : left(left), right(right) {}

NNumber::NNumber(float num) : num(num) {}void NNumber::print() { cout << num; }float NNumber::evaluate() { return num; }NAssign::NAssign(string id, NExpression *expr)   : id(id), expr(expr) {}void NAssign::print() {  cout << id << " = ";  expr->print();}void NAssign::evaluate() {  float result = expr->evaluate();  state[id] = result;}