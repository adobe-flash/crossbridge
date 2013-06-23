// Node.h

class Node {
friend class Tree;
private:
  int use;
protected:
  Node() {use = 1;}

  virtual float nodeValue() {return 0.0;}

public:
  virtual ~Node(void) {}
};

// Tree.h

class Tree {
private:
  Node* nodePtr;

public:
  Tree(float n);

  Tree(char* n);

  Tree(char*, Tree t);

  Tree(Tree left, char*, Tree right);

  Tree(const Tree& t);

  ~Tree(void);

  float value();

  float operator () (float x, float y, float z);
  
  void operator =(const Tree& t);
};

// RealNode.h

class RealNode: public Node {
friend class Tree;
private:
  float n;

  char* symbol;

  RealNode(float k, char* ch=0);

  ~RealNode(void) {delete symbol; }

  float nodeValue();
};

// UnaryNode.h

class UnaryNode: public Node {
friend class Tree;
private:
  char* op;

  Tree *opnd;

  UnaryNode(char* a, Tree b);

  ~UnaryNode(void) {delete op; delete opnd; }

  float nodeValue();
};

// BinaryNode.h

class BinaryNode: public Node {
friend class Tree;
private:
  char* op;

  Tree* left;

  Tree* right;

  BinaryNode(char* a, Tree b, Tree c);

  ~BinaryNode(void) {delete op; delete left; delete right; }

  float nodeValue();
};

// globals.h

float values0, values1, values2;
