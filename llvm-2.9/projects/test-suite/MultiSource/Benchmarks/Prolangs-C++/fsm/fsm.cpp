// Obtained from "C++ Programming Style" by Tom Cargill,
// Addison Wesley Publishing Company, Inc.  1992 Edition
// Chapter 8, Listing 8.7

#include <string.h>
#include <limits.h>
const int range = CHAR_MAX + 1; // 0x7F + 1;

class state {
public:
  state *transition[range];
  state();
};

class triple {
public:
  int from, to;
  char input;
};

class fsm {
  state *graph;
  state *current;
public:
  void reset();
  void advance(char);
  int end_state();
  int doom_state();
  fsm(triple*);
  virtual ~fsm();
};

state::state() {
  for (int i = 0; i < range; ++i)
    transition[i] = 0;
}

fsm::fsm(triple *p) {
  int max_node = 0;
  for (triple *e = p; e->from; ++e) {
    if (e->from > max_node)
      max_node = e->from;
    if (e->to > max_node)
      max_node = e->to;
  }
  graph = new state[max_node+1]; // was new state[max_node+1]
  for (triple *e=p; e->from; ++e)
    graph[e->from].transition[(int)e->input] = &graph[e->to];
  current = 0;
}

fsm::~fsm() {
  delete [] graph;
}

void fsm::reset() {
  current = &graph[1];
}

void fsm::advance(char x) {
  if (current)
    current = current->transition[(int)x];
}

int fsm::end_state() {
  return current==&graph[0];
}

int fsm::doom_state() {
  return current==0;
}

class sample : public fsm {
  static triple edges[];
public:
  sample();
};

triple sample::edges[3] = {{1,2,'A'}, {1, 3, 'B'}};

sample::sample() : fsm(edges) {
}

#include <stdio.h>

int main() {
  char input_string[80];
  printf("Enter input expression: ");
  scanf("%s", input_string);
  sample m;
  m.reset();
  int index = 0;
  m.advance(input_string[index++]);
  while (!m.end_state() && !m.doom_state()) {
    m.advance(input_string[index++]);
  }
  if (m.end_state())
    printf("\nValid input expression");
  else
    printf("\nInvalie input expression");
  return 0;
}
