#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include "myc--.h"

using namespace std;

NBinaryOp::NBinaryOp(NExpression *left, NExpression *right, int tag) 
  
NBinaryNumOp::NBinaryNumOp(NExpression *left, NExpression *right, int tag)
  : NBinaryOp(left, right, tag) {}
  
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

float NBinaryNumOp::evaluate() {
  float r = right->evaluate();
  
  switch (tag) {
    case OP_PLUS: return l + r;
    case OP_MINUS: return l - r;
    case OP_TIMES: return l * r;
    case OP_DIVIDE: return l / r;
  }
}
  

NNumber::NNumber(float num) : num(num) {}