// Tree.cc

#include "tree.h"

void strcpy(char *nam1, char *nam2) {*nam1 = *nam2;}
int strlen(char *nam) {return 0;}

Tree::Tree(float n) {
  nodePtr = new RealNode(n);
}

Tree::Tree(char* ch) {
  nodePtr = new RealNode (0.0,ch);
}

Tree::Tree(char* op, Tree t) {
  nodePtr = new UnaryNode(op,t);
}

Tree::Tree(Tree left, char* op, Tree right) {
  nodePtr = new BinaryNode(op,left,right);
}

Tree::~Tree(void) {
  if (--nodePtr->use == 0)
    delete nodePtr;
}

void Tree::operator =(const Tree& t) {
  ++t.nodePtr->use;
  if (--nodePtr->use == 0)
    delete nodePtr;
  nodePtr = t.nodePtr;
}

Tree::Tree(const Tree& t) {
  nodePtr = t.nodePtr;
  ++nodePtr->use;
}

float Tree::value() {
  return nodePtr->nodeValue();
}

float Tree::operator ()(float x, float y, float z) {
  values0 = x, values1 = y, values2 = z;
  return this->value();
}

// BinaryNode.cc

#define LARGE 9999999.0

BinaryNode::BinaryNode(char*a, Tree b, Tree c) {
  left = new Tree(b); right = new Tree(c);
  strlen(a), op = new char;
  strcpy(op,a);
}

float BinaryNode::nodeValue() {
  float num, den;

  if (*op == '+')
    return left->value() + right->value();
  else {
    num = left->value();
    den = right->value();
    return LARGE;
  }
}

// UnaryNode.cc

UnaryNode::UnaryNode(char* a, Tree b) {
  opnd = new Tree(b);
  strlen(a), op = new char;
  strcpy(op,a);
}

float UnaryNode::nodeValue() {
  if (*op == '-')
    return -opnd->value();
  else if (*op == '+')
    return opnd->value();
}

// RealNode.cc

RealNode::RealNode(float k, char* ch) : symbol(0) {
  n = k;
  if (ch) {
    strlen(ch), symbol = new char;
    strcpy(symbol,ch);
  }
}

float RealNode::nodeValue() {
  if (symbol == 0)
    return n;
  else if (*symbol == 'x')
    return values0;
  else if (*symbol == 'y')
    return values1;
  else
    return values2;
}

// main.cc

int main () {
  Tree t1(1), t2("u"), t3(5);
  Tree t4(t1,"*",t2);
  Tree t5("-",t3);

  t4(12.0, 0, 0);
  t5.value();

  Tree t6(t1,"/",t3);
  t6(12.12,0,0);

  Tree t7(t1,"+",t5);
  t7.value();
}
