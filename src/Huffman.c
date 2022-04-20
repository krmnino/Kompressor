#include <stdlib.h>
#include <string.h>

#include "Reader.h"
#include "Huffman.h"

HNode* HNode_init(Pair* p){
    if(p == NULL){
        return NULL;
    }

    HNode* node = (HNode*)malloc(sizeof(HNode));
    node->pair = p;
    node->q_next = NULL;
    node->t_left = NULL;
    node->t_right = NULL;
    return node;
}

int HNode_free(HNode* node){
    if(node == NULL){
        return -1;
    }

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
        // Free Pair individually
        Pair_free(prev->pair);
        // When free previous node
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
    if(node == NULL){
        return -1;
    }

    // If current node is a leaf (contains two-byte value)
    if(node->t_left == NULL && node->t_right == NULL){
        // Clear remaining entries in code buffer beyond the current level index
        for(int i = level_count; i < BLOCKS; i++){
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
    if(queue == NULL){
        return -1;
    }
    if(node == NULL){
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
    if(queue == NULL){
        return -1;
    }
    if(node == NULL){
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

    return 0;
}

HNode* HQueue_dequeue(HQueue* queue){
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
    if(queue == NULL){
        return -1;
    }

    HNode* curr = queue->head;
    // Traverse queue and print each pair
    while(curr != NULL){
        printf("[%4x, %d]\n", curr->pair->byte_value, curr->pair->byte_count);
        curr = curr->q_next;
    }

    return 0;
}

Huffman* Huffman_init(Reader* r, const char* fn){
    if(r == NULL){
        return NULL;
    }
    if(fn == NULL){
        return NULL;
    }

    // Allocate Huffman object
    Huffman* h = (Huffman*)malloc(sizeof(Huffman));

    // Initialize Huffman object data members
    h->n_elements = r->pairs_written;
    h->filename = fn;
    h->byte_counters = HQueue_init();
    h->huffman_code = (char**)calloc(BLOCKS, sizeof(char*));
    
    return h;
}

int Huffman_free(Huffman* h){
    if(h == NULL){
        return -1;
    }

    // Free up huffman codes written in array
    for(unsigned int i = 0; i < BLOCKS; i++){
        if(h->huffman_code[i] != NULL){
            free(h->huffman_code[i]);
        }
    }

    // Free up HQueue, huffman code array, and Huffman obj
    HQueue_free(h->byte_counters);
    free(h->huffman_code);
    free(h);
    return 0;
}

int Huffman_compress(Huffman* h, Reader* r){
    if(h == NULL){
        return -1;
    }
    if(r == NULL){
        return -1;
    }

    // Declare local variables for queue fill and tree creation
    HQueue* queue;
    HNode* node;
    HNode* node_bc;
    HNode* n1;
    HNode* n2;
    Pair* p;
    Pair* p_bc;
    char* code_buffer;

    // Initialize queue and code buffer
    queue = HQueue_init();
    code_buffer = (char*)calloc(BLOCKS, sizeof(char));

    // Fill queues with Reader counter data
    for(unsigned int i = 0; i < BLOCKS && r->counters[i].byte_count != 0; i++) {
        p = Pair_init(r->counters[i].byte_value, r->counters[i].byte_count);
        p_bc = Pair_init(r->counters[i].byte_value, r->counters[i].byte_count);
        node = HNode_init(p);
        node_bc = HNode_init(p_bc);
        HQueue_enqueue(queue, node);
        HQueue_enqueue(h->byte_counters, node_bc);
    }

    // Build binary tree from queue
    while(1){
        // If there is one or less elements in queue, exit
        if(queue->n_elements <= 1){
            break;
        }
        // Get first 2 nodes in queue
        n1 = HQueue_dequeue(queue);
        n2 = HQueue_dequeue(queue);
        // Add byte count from both nodes
        p = Pair_init(0, n1->pair->byte_count + n2->pair->byte_count);
        node = HNode_init(p);
        // Make them left and right child nodes
        node->t_left = n1;
        node->t_right = n2;
        // Enqueue new parent node
        HQueue_enqueue(queue, node);
    }

    // Retrieve root and free queue object
    HNode* root = HQueue_dequeue(queue);
    HQueue_free(queue);

    // Perform depth search first to generate Huffman codes
    HNode_dfs(root, 0, code_buffer, h->huffman_code);

    // Free up buffer and binary tree from root
    free(code_buffer);
    HNode_free_bfs(root);

    // Delcare byte-size buffers and other variables
    size_t code_len;
    unsigned short word_buffer;
    unsigned short bit_offset;
    unsigned short idx;
    unsigned char byte_buffer;
    unsigned char byte_buffer_l[1];
    unsigned char byte_buffer_r[1];
    unsigned char last_bit_offset;
    FILE* input_file;
    FILE* output_file;

    // Open input and output files
    input_file = fopen(r->filename, "r");
    output_file = fopen(h->filename, "w");

    // Write pair count into file header
    fwrite(&r->pairs_written, sizeof(unsigned int), 1, output_file);

    // Write last two-byte offset after pair count
    byte_buffer = 0x00;
    fwrite(&byte_buffer, sizeof(unsigned char), 1, output_file);

    // Declare and init Nodes needed to write Pairs
    HNode* curr;
    curr = h->byte_counters->head;
    // Write two-byte values into file header
    while(curr != NULL){
        fwrite(&curr->pair->byte_value, sizeof(unsigned short), 1, output_file);
        curr = curr->q_next;
    }
    // Write two-byte value counts into file header
    curr = h->byte_counters->head;
    while(curr != NULL){
        fwrite(&curr->pair->byte_count, sizeof(unsigned int), 1, output_file);
        curr = curr->q_next;
    }

    // Scan file byte 
    if(r->file_size % 2 != 0){
        bit_offset = 0;
        word_buffer = 0;
        // Read through whole file except the last byte
        for(size_t i = 0; i < r->file_size - 1; i += 2){
            // Read 2 bytes to create index value
            idx = 0;
            fread(byte_buffer_l, sizeof(char), 1, input_file);
            fread(byte_buffer_r, sizeof(char), 1, input_file);
            idx = ((idx | byte_buffer_l[0]) << 8) | byte_buffer_r[0];
            code_len = strlen(h->huffman_code[idx]);
            for(size_t j = 0; j < code_len; j++){
                if(bit_offset >= 15){
                    // Get the corresponfding Huffman code bit
                    word_buffer = (word_buffer << 1) | (h->huffman_code[idx][j] & 0x0F);
                    fwrite(&word_buffer, sizeof(unsigned short), 1, output_file);
                    word_buffer = 0x00;
                    bit_offset = 0;
                }
                else{
                    // Get the corresponfding Huffman code bit
                    word_buffer = (word_buffer << 1) | (h->huffman_code[idx][j] & 0x0F);
                    bit_offset++;
                }
            }
        }
        // Read last byte and pad remaining byte with zeros
        fread(byte_buffer_l, sizeof(char), 1, input_file);
        idx = ((idx | byte_buffer_l[0]) << 8) | 0x00;
        code_len = strlen(h->huffman_code[idx]);
        for(size_t j = 0; j < code_len; j++){
            if(bit_offset >= 15){
                // Get the corresponfding Huffman code bit
                word_buffer = (word_buffer << 1) | (h->huffman_code[idx][j] & 0x0F);
                fwrite(&word_buffer, sizeof(unsigned short), 1, output_file);
                word_buffer = 0x00;
                bit_offset = 0;
            }
            else{
                // Get the corresponfding Huffman code bit
                word_buffer = (word_buffer << 1) | (h->huffman_code[idx][j] & 0x0F);
                bit_offset++;
            }
        }
        // If word buffer is not full, pad remaining bits with zeros by shifting left
        if(bit_offset != 15){
            word_buffer = word_buffer << (16 - bit_offset);
            last_bit_offset = 16 - bit_offset;
        }
        fwrite(&word_buffer, sizeof(unsigned short), 1, output_file);
        fseek(output_file, sizeof(unsigned int), SEEK_SET);    
        fwrite(&last_bit_offset, sizeof(unsigned char), 1, output_file);
    }
    else{
        bit_offset = 0;
        word_buffer = 0;
        // Read through whole file
        for(size_t i = 0; i < r->file_size - 1; i += 2){
            // Read 2 bytes to create index value
            idx = 0;
            fread(byte_buffer_l, sizeof(char), 1, input_file);
            fread(byte_buffer_r, sizeof(char), 1, input_file);
            idx = ((idx | byte_buffer_l[0]) << 8) | byte_buffer_r[0];
            code_len = strlen(h->huffman_code[idx]);
            for(unsigned int j = 0; j < code_len; j++){
                if(bit_offset >= 15){
                    // Get the corresponfding Huffman code bit
                    word_buffer = (word_buffer << 1) | (h->huffman_code[idx][j] & 0x0F);
                    fwrite(&word_buffer, sizeof(unsigned short), 1, output_file);
                    word_buffer = 0x00;
                    bit_offset = 0;
                }
                else{
                    // Get the corresponfding Huffman code bit
                    word_buffer = (word_buffer << 1) | (h->huffman_code[idx][j] & 0x0F);
                    bit_offset++;
                }
            }
        }
    }
    
    fclose(input_file);
    fclose(output_file);
    return 0;
}

int Huffman_decompress(Huffman* h, Reader* r){
    if(h == NULL){
        return -1;
    }
    if(r == NULL){
        return -1;
    }

    // Declare local variables
    HQueue* queue;
    size_t byte_count_offset;
    unsigned short word_buffer;
    unsigned int double_word_buffer;
    FILE* input_file;

    // Open input file
    input_file = fopen(r->filename, "r");

    // Initialize queue
    queue = HQueue_init();

    // Read byte values and byte counts from file
    for(unsigned int i = 0; i < r->pairs_written; i++){
        // Calculate byte offset for byte_value after the first 5 bytes in header + current value
        byte_count_offset = (sizeof(unsigned int) + sizeof(unsigned char));
        byte_count_offset = byte_count_offset + (sizeof(unsigned short) * i);
        fseek(input_file, byte_count_offset, SEEK_SET);
        // Read two-byte value from from file
        fread(&word_buffer, sizeof(unsigned short), 1, input_file);

        // Calculate byte offset for byte_value after (the first 5 bytes in header + byte_value * total_count) + current value
        byte_count_offset = (sizeof(unsigned int) + sizeof(unsigned char)) + (r->pairs_written * sizeof(unsigned short));
        byte_count_offset = byte_count_offset + (sizeof(unsigned int) * i);
        fseek(input_file, byte_count_offset, SEEK_SET);
        // Read two-byte value from from file
        fread(&double_word_buffer, sizeof(unsigned int), 1, input_file);

        printf("-> %x - %d\n", word_buffer, double_word_buffer);
    }

    // pair_count * (short_size + int_size) + int_size
    size_t data_section_offset = sizeof(unsigned int) + (r->pairs_written * (sizeof(unsigned short) + sizeof(unsigned int)));
    // (total_file_size - data_section_offset) / 2 (read 2 bytes per iteration)
    size_t data_section_size = (r->file_size - data_section_offset) / 2;

    if(data_section_size % 2 != 0){
        for(unsigned int i = 0; i < data_section_size - 1; i++){
            fread(&word_buffer, sizeof(unsigned short), 1, input_file);
            printf("-> %x\n", word_buffer);
        }
    }
    else{
        for(unsigned int i = 0; i < data_section_size; i++){
            fread(&word_buffer, sizeof(unsigned short), 1, input_file);
            printf("-> %x\n", word_buffer);
        }
    }

    fclose(input_file);
    return 0;
}