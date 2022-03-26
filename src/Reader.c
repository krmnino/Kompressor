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
    int ret_code;
    ret_code = fclose(r->file_ptr);
    if(ret_code != 0){
        return -1;
    }
    free(r->counters);
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

