#include <stdlib.h>
#include <stdio.h>

#include "Reader.h"

Reader* Reader_init(const char* fn){
    Reader* r = (Reader*)malloc(sizeof(Reader));
    r->filename = fn;
    r->file_ptr = fopen(fn, "rb");
    r->counters = (size_t*)malloc(0xFF * sizeof(size_t));
    r->file_size = get_file_size(fn);
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
    char buffer[1];
    unsigned byte_index;
    for(int i = 0; i < r->file_size; i ++){
        fread(buffer, sizeof(char), 1, r->file_ptr);
        byte_index = (unsigned int)buffer[0];
        if(byte_index < 0 || byte_index >= 0xFF){
            return -1;
        }
        r->counters[byte_index]++;
    }
    return 0;
}

int Reader_display_counters(Reader* r){
    if(r == NULL){
        return -1;
    }
    for(int i = 0; i < 0xFF; i++){
        printf("%3x : %ld\n", i, r->counters[i]);
    }
    return 0;
}

size_t get_file_size(const char* fn){
    FILE* f = fopen(fn, "rb");
    if(f == NULL){
        return -1;
    }
    fseek(f, 0, SEEK_END);
    long ret = ftell(f);
    fclose(f);
    return ret;
}

