#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>

using namespace std;

enum Tag {
  OP_LEQ,
  OP_GEQ,
  OP_EQ,
  OP_NEQ,
  OP_AND,
  OP_OR,
  OP_LT,
  OP_GT,
  OP_PLUS,
  OP_MINUS,
  OP_TIMES,
  OP_DIVIDE,
  OP_MODULO,
  OP_NOT
};

enum ValueType {
  BOOL,
  INT,
  FLOAT,
  CHAR,
  STRING,
  VOID,
};

class Type {
  public:
    static string toString(ValueType valueType);
};

class Value {
  public:
    union ExprValue {
      bool b;
      int i;
      float f;
      string *s;
      char c;
    };
  
    int valueType;
    ExprValue value;
    
    static Value* fromBool(bool b);
    static Value* fromInt(int i);
    static Value* fromFloat(float f);
    static Value* fromChar(char c);
    static Value* fromString(string *s);
    static Value* fromVoid();
    
    bool toBool();
    int toInt();
    float toFloat();
    char toChar();
    string* toString();
    
    void print();
    
    static int compare(Value *l, Value *r);
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

class NVarDecl : public NStatement {
  public:
    string id;
    ValueType type;
    NExpression *expr = NULL;
    
    NVarDecl(ValueType type, string id);
    NVarDecl(ValueType type, string id, NExpression *expr);
    void print();
    void evaluate();
};