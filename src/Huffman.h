#ifndef HUFFMAN
#define HUFFMAN

#include <stdlib.h>

#include "Reader.h"

typedef struct HNode HNode;
struct HNode{
    Pair* pair;
    HNode* q_next;
    HNode* t_left;
    HNode* t_right;
};
HNode* HNode_init(Pair*);
int HNode_free(HNode*);
int HNode_free_bfs(HNode*);
int HNode_dfs(HNode*, int, char*, char**);
int HNode_is_leaf(HNode*);

typedef struct HQueue HQueue;
struct HQueue{
    unsigned long n_elements;
    HNode* head;
    HNode* tail;
};
HQueue* HQueue_init();
int HQueue_free(HQueue*);
int HQueue_enqueue(HQueue*, HNode*);
HNode* HQueue_dequeue(HQueue*);
int HQueue_display_queue();
int HQueue_push(HQueue*, HNode*);


typedef struct Huffman Huffman;
struct Huffman{
    unsigned long n_elements;
    char** huffman_code;
    const char* filename;
    HQueue* byte_counters;
};
Huffman* Huffman_init(Reader*, const char*);
int Huffman_free(Huffman*);
int Huffman_compress(Huffman*, Reader*);
int Huffman_decompress(Huffman*, Reader*);

#endif