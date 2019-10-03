#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>

using namespace std;

extern map<string, float> state;

enum TAG {
  OP_PLUS,
  OP_MINUS,
  OP_TIMES,
  OP_DIVIDE
};

class NExpression {
	public:
		virtual void print() = 0;
		virtual float evaluate() = 0;
};

class NNumber : public NExpression {
	public:
    float num;
    
		NNumber(float num);
		void print();
		float evaluate();
};

class NIdentifier : public NExpression {
  public:
    string id;
    
    NIdentifier(string id);
    void print();
    float evaluate();
};

class NBinaryOp : public NExpression {
	public:
		NExpression *left;
		NExpression *right;
    int tag;

		NBinaryOp(NExpression *left, NExpression *right, int tag);	
};

class NBinaryNumOp : public NBinaryOp {
  public: 
    NBinaryNumOp(NExpression *left, NExpression *right, int tag);
    void print();
    float evaluate();
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
	protected:
		string id;
		NExpression *expr;
	public:
		NAssign(string id, NExpression *expr);
		void print();
		void evaluate();
};