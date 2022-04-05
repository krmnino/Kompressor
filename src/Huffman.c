#include <stdlib.h>
#include <string.h>

#include "Reader.h"
#include "Huffman.h"

HNode* HNode_init(Pair* p){
    HNode* node = (HNode*)malloc(sizeof(HNode));
    node->pair = p;
    node->q_next = NULL;
    node->t_left = NULL;
    node->t_right = NULL;
    return node;
}

int HNode_free(HNode* node){
    // Pair data member must be free'd before calling this function
    free(node);
    return 0;
}

int HNode_free_bfs(HNode* node){
    if(node == NULL){
        return -1;
    }
    // Initialize queue and HNode pointers
    HNode* prev = NULL;
    HNode* curr = node;
    HQueue* queue = HQueue_init();
    // Perform breadth search first, push back child nodes and free parents
    while(curr != NULL){
        if(curr->t_left != NULL){
            HQueue_push(queue, curr->t_left);
        }
        if(curr->t_right != NULL){
            HQueue_push(queue, curr->t_right);
        }
        prev = curr;
        curr = curr->q_next;
        free(prev->pair);
        HNode_free(prev);
    }
    // Free queue
    free(queue);
    return 0;
}

HQueue* HQueue_init(){
    HQueue* queue = (HQueue*)malloc(sizeof(HQueue));
    queue->head = NULL;
    queue->n_elements = 0;
    return queue;
}

int HQueue_free(HQueue* queue){
    // Check if node is null (invalid input)
    if(queue == NULL){
        return -1;
    }
    // Traverse queue by moving to the next node and free'ing the previous one
    HNode* prev = NULL;
    HNode* curr = queue->head;
    while(curr != NULL){
        prev = curr;
        curr = curr->q_next;
        free(prev->pair);
        HNode_free(prev);
    }
    // Free queue
    free(queue);
    return 0;
}

int HNode_dfs(HNode* node, int level_count, char* code_buffer, char** huffman_code){
    // Check if node is null (invalid input)
    if(node == NULL){
        return -1;
    }
    // If current node is a leaf (contains two-byte value)
    if(node->t_left == NULL && node->t_right == NULL){
        // Clear remaining entries in code buffer beyond the current level index
        for(int i = level_count + 1; i < BLOCKS; i++){
            code_buffer[i] = 0x00;
        }
        // Allocate string of appropriate size in huffman code array
        huffman_code[node->pair->byte_value] = (char*)malloc(level_count * sizeof(char));
        // Copy code buffer value to huffman code array
        strcpy(huffman_code[node->pair->byte_value], code_buffer);
        return 0;
    }
    // If left child is not null
    if(node->t_left != NULL){
        // Add 0 to code buffer
        code_buffer[level_count] = '0';
        // Recursive call to HNode_dfs with increased level count
        HNode_dfs(node->t_left, level_count + 1, code_buffer, huffman_code);
    }
    // If right child is not null
    if(node->t_right != NULL){
        // Add 1 to code buffer
        code_buffer[level_count] = '1';
        // Recursive call to HNode_dfs with increased level count
        HNode_dfs(node->t_right, level_count + 1, code_buffer, huffman_code);
    }
    return 0;
}

int HQueue_push(HQueue* queue, HNode* node){
    if(queue == NULL || node == NULL){
        return -1;
    }
    // If queue is empty; then, set the head and tail
    if(queue->n_elements == 0){
        queue->head = node;
        queue->tail = node;
        queue->n_elements++;
        return 0;
    }
    // Otherwise, update the tail 
    queue->tail->q_next = node;
    queue->tail = node;
    queue->n_elements++;
    return 0;
}

int HQueue_enqueue(HQueue* queue, HNode* node){
    // Check is queue pointer and node pointer is valid
    if(queue == NULL || node == NULL){
        return -1;
    }
    // If queue is empty, node must be head and tail of queue
    if(queue->n_elements == 0){
        queue->head = node;
        queue->tail = node;
        queue->n_elements++;
        return 0;
    }
    HNode* prev = NULL;
    HNode* curr = queue->head;
    while(1){
        // If current node byte_count is greater than new node and current node
        // is the queue's head, then new node must be the head.
        if(curr->pair->byte_count > node->pair->byte_count && curr == queue->head){
            node->q_next = queue->head;
            queue->head = node;
            queue->n_elements++;
            return 0;
        }
        // If current node byte_count is greater than new node, then add before
        // the current node.
        else if(curr->pair->byte_count > node->pair->byte_count){
            node->q_next = prev->q_next;
            prev->q_next = node;
            queue->n_elements++;
            return 0;
        }
        // If loop reaches the last node of the queue, means that new node has
        // a greater byte_count value than the tail node. Add to the end.
        else if(curr->q_next == NULL){
            curr->q_next = node;
            queue->tail = node;
            queue->n_elements++;
            return 0;
        }
        prev = curr;
        curr = curr->q_next;
    }
}

HNode* HQueue_dequeue(HQueue* queue){
    // Check if queue pointer is valid
    if(queue == NULL){
        return NULL;
    }
    // Retrieve node from front of queue
    HNode* node = queue->head;
    queue->head = queue->head->q_next;
    node->q_next = NULL;
    queue->n_elements--;
    return node; 
}

int HQueue_display_queue(HQueue* queue){
    // Check if queue pointer is valid
    if(queue == NULL){
        return -1;
    }
    HNode* curr = queue->head;
    // Traverse queue and print each pair
    while(curr != NULL){
        printf("[%4x, %ld]\n", curr->pair->byte_value, curr->pair->byte_count);
        curr = curr->q_next;
    }
    return 0;
}

Huffman* Huffman_init(Reader* r){
    if(r == NULL){
        return NULL;
    }
    Huffman* h = (Huffman*)malloc(sizeof(Huffman));
    h->n_elements = r->pairs_written;

    // Initialize queue
    HQueue* queue = HQueue_init();

    // Fill queue with Reader counter data
    Pair* p;
    HNode* node;
    for(unsigned int i = 0; i < BLOCKS && r->counters[i].byte_count != 0; i++) {
        p = Pair_init(r->counters[i].byte_value, r->counters[i].byte_count);
        node = HNode_init(p);
        HQueue_enqueue(queue, node);
    }

    HNode* n1;
    HNode* n2;
    while(1){
        if(queue->n_elements <= 1){
            break;
        }
        n1 = HQueue_dequeue(queue);
        n2 = HQueue_dequeue(queue);
        p = Pair_init(0, n1->pair->byte_count + n2->pair->byte_count);
        node = HNode_init(p);
        node->t_left = n1;
        node->t_right = n2;
        HQueue_enqueue(queue, node);
    }

    // Retrieve root and free queue object
    HNode* root = HQueue_dequeue(queue);
    HQueue_free(queue);

    // Perform depth search first to generate Huffman codes
    h->huffman_code = (char**)malloc(BLOCKS * sizeof(char*));
    char* code_buffer = (char*)malloc(BLOCKS * sizeof(char));
    HNode_dfs(root, 0, code_buffer, h->huffman_code);
    for(unsigned int i = 0; i < BLOCKS; i++){
        if(h->huffman_code[i] != NULL){
            printf("%4x, %s\n", i, h->huffman_code[i]);
        }
    } 
    free(code_buffer);
    HNode_free_bfs(root);
    
    return h;
}

int Huffman_free(Huffman* h){
    free(h->huffman_code);
    free(h);
    return 0;
}