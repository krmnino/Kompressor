#include <stdlib.h>
#include <stdio.h>

#include "Reader.h"

Pair* Pair_init(size_t bc, unsigned char bv){
    Pair* p = (Pair*)malloc(sizeof(Pair));
    p->byte_count = bc;
    p->byte_value = bv;
    return p;
}

int Pair_free(Pair* p){
    free(p);
    return 0;
}

Reader* Reader_init(const char* fn){
    if(fn == NULL){
        return NULL;
    }
    Reader* r = (Reader*)malloc(sizeof(Reader));
    r->filename = fn;
    r->file_ptr = fopen(fn, "rb");
    r->counters = (Pair*)calloc(BYTES_SIZE, sizeof(Pair));
    r->file_size = _get_file_size(fn);
    for(int i = 0; i < BYTES_SIZE; i++){
        r->counters[i].byte_value = bytes[i];
    }
    return r;
}

int Reader_free(Reader* r){
    if(r == NULL){
        return -1;
    }
    int ret_code = fclose(r->file_ptr);
    if(ret_code != 0){
        return -1;
    }
    free(r->counters);
    free(r);
    return 0;
}

int Reader_count(Reader* r){
    if(r == NULL){
        return -1;
    }
    unsigned char buffer[1];
    unsigned byte_index;
    for(int i = 0; i < r->file_size; i ++){
        fread(buffer, sizeof(char), 1, r->file_ptr);
        byte_index = (unsigned int)buffer[0];
        if(byte_index < 0 || byte_index >= 0xFF){
            return -1;
        }
        r->counters[byte_index].byte_count++;
    }
    rewind(r->file_ptr);
    return 0;
}

int Reader_display_counters(Reader* r){
    if(r == NULL){
        return -1;
    }
    for(int i = 0; i < BYTES_SIZE; i++){
        printf("%2x : %ld\n", r->counters[i].byte_value, r->counters[i].byte_count);
    }
    return 0;
}

int Reader_sort_count(Reader* r){
    if(r == NULL){
        return -1;
    }
    qsort(r->counters, BYTES_SIZE, sizeof(Pair), _pair_compare);
    for(int i = 0; i < BYTES_SIZE; i++){
        if(r->counters[i].byte_count != 0){
            r->pairs_written++;
        }
    }
    return 0;
}

int Reader_reset_count(Reader* r){
    if(r == NULL){
        return -1;
    }
    for(int i = 0; i < BYTES_SIZE; i++){
        r->counters[i].byte_value = bytes[i];
        r->counters[i].byte_count = 0;
    }
    r->pairs_written = 0;
    return 0;
}

size_t _get_file_size(const char* fn){
    FILE* f = fopen(fn, "rb");
    if(f == NULL){
        return -1;
    }
    fseek(f, 0, SEEK_END);
    long ret = ftell(f);
    fclose(f);
    return ret;
}

int _pair_compare(const void* p1, const void* p2){
    Pair* p1_ = (Pair*)p1;
    Pair* p2_ = (Pair*)p2;
    if(p1_->byte_count > p2_->byte_count){
        return -1;
    }
    else if(p1_->byte_count < p2_->byte_count){
        return 1;
    }
    else{
        return 0;
    }
}