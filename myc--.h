#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <queue>
#include <tuple>

using namespace std;

class NFuncDecl;
class ValueArray;
class NString;

enum NodeType {
  N_MISC,
  N_ASSIGN,
  N_VARDECL,
  N_FUNCDECL,
};

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
      NString *s;
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
    static Value* fromString(string s);
    static Value* fromFunc(NFuncDecl *func);
    static Value* fromArray(ValueArray *array);
    static Value* fromVoid();
    
    bool isTrue();
    
    bool toBool();
    int toInt();
    float toFloat();
    char toChar();
    string toString();
    Value* callFunc();
    ValueArray* toArray();
    
    void print();
    
    static int compare(Value *l, Value *r);
};

extern map<string, Value*> state;

class ValueArray {
  public:
    ValueType valueType;
    int size;
    vector<Value*> values;
    
    ValueArray(ValueType valueType, int size);
    
    Value* getValue(int index);
    void setValue(int index, Value* v);
};

class NStatement; /* Forward Declaration */

typedef tuple<int, int> Edge;
typedef tuple<string, int> RDElt;
void printRDElt(RDElt e);
void printRDElts(list<RDElt> *es);
RDElt initRDElt(string id);

class CFG {
  private:
  static void findTerminalNodes(CFG* node, list<CFG*>* terminalNodes, list<CFG*>* visitedNodes);
  map<int, CFG*> createLabelNodeMap(list<int> *visitedNodes);
  list<tuple<int, int>> createLabelEdgeList(list<tuple<int, int>> *visitedNodes);

  protected:
  static int labelCount;

  public:
  NStatement* statement;
  list<CFG*> *edges;    
  CFG();
  map<int, CFG*> createLabelNodeMap();
  list<tuple<int, int>> createLabelEdgeList();
  static void findTerminalNodes(CFG* node, list<CFG*>* terminalNodes);
  static void printLabelEdgeMap(list<tuple<int, int>> edgeList);
  static void printLabelNodeMap(map<int, CFG*> labelNodeMap);
  static bool RDEltSetsEqual(list<RDElt>* listA, list<RDElt>* listB);
  
  list<int>* allLabels();
  list<string>* allIds();
  NStatement* labelledStatement(int label);
  list<int>* assignmentsToId(string id);
  list<int>* labelsTo(int label);
  list<int>* labelsFrom(int label);
  list<Edge>* edgesFrom(int label);
  list<RDElt>* killSet(int label);
  list<RDElt>* genSet(int label);
  void applyTransferMutation(list<RDElt>* input, int label);
  list<RDElt>* applyTransferFunction(list<RDElt>* input, int label);
  bool isInitStatement(int label);
  
  void printRDEntryEqn(int label);
  void printRDExitEqn(int label);
  
  int label;
  list<int> *finalLabels;
  list<tuple<int, int>> *extraEdges;
};

typedef map<int, list<RDElt>*> RDAnalysis;
void printRDAnalysis(RDAnalysis *analysis);

class Worklist {
  public:
    static tuple<RDAnalysis*, RDAnalysis*> solveRD(CFG *cfg);
};

class NExpression {
	public:
		virtual void print() = 0;
    virtual void printNode() = 0;
		virtual Value* evaluate() = 0;
};

class NNumber : public NExpression {
	public:
    float num;
    
		NNumber(float num);
		void print();
    void printNode();
		Value* evaluate();
};

class NBoolean : public NExpression { 
  public:
    bool b;
    
    NBoolean(bool b);
    void print();
    void printNode();
    Value* evaluate();
};

class NChar : public NExpression {
  public:
    char c;
    
    NChar(char c);
    void print();
    void printNode();
    Value* evaluate();
};

class NString : public NExpression {
  public:
    string s;
    
    NString(string s);
    void print();
    void printNode();
    Value* evaluate();
};

class NIdentifier : public NExpression {
  public:
    string id;
    
    NIdentifier(string id);
    void print();
    void printNode();
    Value* evaluate();
};

class NBinaryOp : public NExpression {
	public:
		NExpression *left;
		NExpression *right;
    int tag;

		NBinaryOp(NExpression *left, NExpression *right, int tag);	
    string opString();
    void print();
    void printNode();
    Value* evaluate();
};

class NUnaryOp : public NExpression {
  public:
    NExpression *expr;
    int tag;
    
    NUnaryOp(NExpression *expr, int tag);
    string opString();
    void print();
    void printNode();
    Value* evaluate();
};

class NIndex : public NExpression {
  public:
    NExpression *arrayExpr;
    NExpression *indexExpr;
    
    NIndex(NExpression *arrayExpr, NExpression *indexExpr);
    void print();
    void printNode();
    Value* evaluate();
};

class NStatement {
	public:
    virtual void print() = 0;
    virtual void printNode() = 0;
    virtual Value* evaluate() = 0;
    virtual bool isReturn();
    virtual CFG* makeCFG() = 0;
    virtual void printCFGNode() { print(); };
    virtual NodeType getNodeType() { return N_MISC; };
};

class NBlock {
  public:
    list<NStatement*> *statements;

    NBlock();
    NBlock(NStatement *head);
    void print();
    void printNode();
    void printIndented();
    Value* evaluate();
    CFG* makeCFG();
};

class NReturn : public NStatement {
  public:
    NExpression *expr;
    
    NReturn(NExpression *expr);
    void print();
    void printNode();
    Value* evaluate();
    bool isReturn();
    CFG* makeCFG();
};

class NAssign : public NStatement, public NExpression {
	public:
    const static NodeType nodeType = N_ASSIGN;
    string id;
    NExpression *expr;
    
		NAssign(string id, NExpression *expr);
		void print();
    void printNode();
    NodeType getNodeType();
		Value* evaluate();
    CFG* makeCFG();
    void printCFGNode();
};

class NIndexAssign : public NStatement, public NExpression {
  public:
    string id;
    NExpression *indexExpr;
    NExpression *expr;
    
    NIndexAssign(string id, NExpression *indexExpr, NExpression *expr);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
};

class NVarDecl : public NStatement {
  public:
    string id;
    ValueType type;
    NExpression *expr = NULL;
    
    NVarDecl(ValueType type, string id);
    NVarDecl(ValueType type, string id, NExpression *expr);
    NodeType getNodeType();
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
    void printCFGNode();
};

class NArrayDecl : public NStatement {
  public:
    string id;
    ValueType type;
    NExpression *sizeExpr;
      
    NArrayDecl(ValueType type, string id, NExpression* sizeExpr);
    
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
};

class NFuncDecl : public NStatement {
  public:
    ValueType returnType;
    string id;
    NBlock *body;
    list<NVarDecl*> *arguments;
    
    NFuncDecl(ValueType returnType, string id, NBlock *body);
    NFuncDecl(ValueType returnType, string id, NBlock *body, list<NVarDecl*> *arguments);
    NodeType getNodeType();
    void print();
    void printArguments();
    void printNode();
    void printType();
    void printCFGNode();
    Value* evaluate();
    Value* call(list<Value*> *args);
    CFG* makeCFG();
};

class NFuncCall : public NStatement, public NExpression {
  public:
    string id;
    list<NExpression*> *arguments;
    
    NFuncCall(string id);
    NFuncCall(string id, list<NExpression*> *arguments);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
};

class NWhile : public NStatement {
  public:
    NExpression *cond;
    NBlock *body;
    
    NWhile(NExpression *cond, NBlock *body);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
    void printCFGNode();
};

class NDoWhile : public NStatement {
  public:
    NExpression *cond;
    NBlock *body;
    
    NDoWhile(NExpression *cond, NBlock *body);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
    void printCFGNode();
};

class NFor : public NStatement {
  public:
    NStatement *init;
    NExpression *cond;
    NExpression *incr;
    NBlock *body;
    
    NFor(NStatement *init, NExpression *cond, NExpression *incr, NBlock *body);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
    void printCFGNode();
};

class NBranch : public NStatement {
  public:
    NExpression *cond;
    NBlock *pass;
    NBlock *fail;
    
    NBranch(NExpression *cond, NBlock *pass, NBlock *fail);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
    void printCFGNode();
};

class NPrint : public NStatement {
  public:
    list<NExpression*> *exprs;
    
    NPrint(list<NExpression*> *exprs);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
};

class NRead : public NStatement {
  public:
    ValueType type;
    string id;
    
    NRead(ValueType type, string id);
    void print();
    void printNode();
    Value* evaluate();
    CFG* makeCFG();
};

