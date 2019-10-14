#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include <vector>

using namespace std;

class NFuncDecl;
class ValueArray;

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
  OP_NOT,
};

enum ValueType {
  BOOL,
  INT,
  FLOAT,
  CHAR,
  STRING,
  FUNC,
  ARRAY,
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
      NFuncDecl *func;
      ValueArray *array;
    };
  
    int valueType;
    ExprValue value;
    
    static Value* fromBool(bool b);
    static Value* fromInt(int i);
    static Value* fromFloat(float f);
    static Value* fromChar(char c);
    static Value* fromString(string *s);
    static Value* fromFunc(NFuncDecl *func);
    static Value* fromArray(ValueArray *array);
    static Value* fromVoid();
    
    bool isTrue();
    
    bool toBool();
    int toInt();
    float toFloat();
    char toChar();
    string* toString();
    Value* callFunc();
    ValueArray* toArray();
    
    void print();
    
    static int compare(Value *l, Value *r);
};

extern map<string, Value*> state;

class ValueArray {
  public:
    int valueType;
    int size;
    vector<Value*> values;
    
    ValueArray(int valueType, int size);
    
    Value* getValue(int index);
    void setValue(int index, Value* v);
};

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

class NIndex : public NExpression {
  public:
    NExpression *arrayExpr;
    NExpression *indexExpr;
    
    NIndex(NExpression *arrayExpr, NExpression *indexExpr);
    void print();
    Value* evaluate();
};

class NStatement {
	public:
    bool isReturn;
    
    virtual void print() {}
    virtual Value* evaluate() = 0;
};

class NBlock {
  public:
    list<NStatement*> *statements;

    NBlock(NStatement *head);
    void print(int indent);
    void print();
    Value* evaluate();
};

class NReturn : public NStatement {
  public:
    NExpression *expr;
    
    NReturn(NExpression *expr);
    void print();
    Value* evaluate();
};

class NAssign : public NStatement, public NExpression {
	public:
    string id;
    NExpression *expr;
    
		NAssign(string id, NExpression *expr);
		void print();
		Value* evaluate();
};

class NIndexAssign : public NStatement, public NExpression {
  public:
    string id;
    NExpression *indexExpr;
    NExpression *expr;
    
    NIndexAssign(string id, NExpression *indexExpr, NExpression *expr);
    void print();
    Value* evaluate();
};

class NVarDecl : public NStatement {
  public:
    string id;
    ValueType type;
    NExpression *expr = NULL;
    
    NVarDecl(ValueType type, string id);
    NVarDecl(ValueType type, string id, NExpression *expr);
    void print();
    Value* evaluate();
};

class NArrayDecl : public NStatement {
  public:
    string id;
    ValueType type;
    NExpression *sizeExpr;
      
    NArrayDecl(ValueType type, string id, NExpression* sizeExpr);
    
    void print();
    Value* evaluate();
};

class NFuncDecl : public NStatement {
  public:
    ValueType returnType;
    string id;
    NBlock *body;
    list<NVarDecl*> *arguments;
    
    NFuncDecl(ValueType returnType, string id, NBlock *body, list<NVarDecl*> *arguments);
    void print();
    void printArguments();
    Value* evaluate();
    Value* call(list<Value*> *args);
};

class NFuncCall : public NStatement, public NExpression {
  public:
    string id;
    list<NExpression*> *arguments;
    
    NFuncCall(string id, list<NExpression*> *arguments);
    void print();
    Value* evaluate();
};

class NWhile : public NStatement {
  public:
    NExpression *cond;
    NBlock *body;
    
    NWhile(NExpression *cond, NBlock *body);
    void print();
    Value* evaluate();
};

class NDoWhile : public NStatement {
  public:
    NExpression *cond;
    NBlock *body;
    
    NDoWhile(NExpression *cond, NBlock *body);
    void print();
    Value* evaluate();
};

class NFor : public NStatement {
  public:
    NStatement *init;
    NExpression *cond;
    NExpression *incr;
    NBlock *body;
    
    NFor(NStatement *init, NExpression *cond, NExpression *incr, NBlock *body);
    void print();
    Value* evaluate();
};
