#ifndef TRIE_H
#define TRIE_H


#define TRIEWIDTH ('z'-'a'+1)

/* Typedefs: */
typedef struct trie_s {
  struct trie_s *next[TRIEWIDTH];
  int number;
} * trie;

typedef char * string;

/* Prototypes: */
char index2char (int i);
int char2index (char c);
trie trie_init (void);
trie trie_insert (trie t, string s);
int trie_lookup (trie t, string s);
void trie_scan (trie t, void f (int, char *));


#endif
