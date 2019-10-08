#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>

using namespace std;

enum Tag {
  OP_PLUS,
  OP_MINUS,
  OP_TIMES,
  OP_DIVIDE,
  OP_NOT
};

class Value {
  public:
    enum ValueType {
      BOOL = 0,
      FLOAT = 1,
      STRING = 2,
      CHAR = 3,
      VOID = 4
    };
    
    union ExprValue {
      bool b;
      float f;
      string *s;
      char c;
    };
  
    int valueType;
    ExprValue value;
    
    static Value* fromFloat(float f);
    static Value* fromBool(bool b);
    static Value* fromString(string *s);
    static Value* fromChar(char c);
    
    bool toBool();
    float toFloat();
    char toChar();
    string* toString();
    
    void print();
};

extern map<string, Value*> state;

class NExpression {
	public:
		virtual void print() = 0;
		virtual Value* evaluate() = 0;
};

class NNumber : public NExpression {
	public:
    float num;
    
		NNumber(float num);
		void print();
		Value* evaluate();
};

class NBoolean : public NExpression { 
  public:
    bool b;
    
    NBoolean(bool b);
    void print();
    Value* evaluate();
};

class NChar : public NExpression {
  public:
    char c;
    
    NChar(char c);
    void print();
    Value* evaluate();
};

class NString : public NExpression {
  public:
    string *s;
    
    NString(string *s);
    void print();
    Value* evaluate();
};

class NIdentifier : public NExpression {
  public:
    string id;
    
    NIdentifier(string id);
    void print();
    Value* evaluate();
};

class NBinaryOp : public NExpression {
	public:
		NExpression *left;
		NExpression *right;
    int tag;

		NBinaryOp(NExpression *left, NExpression *right, int tag);	
    void print();
    Value* evaluate();
};

class NUnaryOp : public NExpression {
  public:
    NExpression *expr;
    int tag;
    
    NUnaryOp(NExpression *expr, int tag);
    void print();
    Value* evaluate();
};

class NStatement {
	public:
		virtual void print() {}
		virtual void evaluate() = 0;
};

class NBlock {
  public:
    list<NStatement*> *statements;

    NBlock(NStatement *head);
    void print();
    void evaluate();
};

class NAssign : public NStatement {
	public:
    string id;
    NExpression *expr;
    
		NAssign(string id, NExpression *expr);
		void print();
		void evaluate();
};