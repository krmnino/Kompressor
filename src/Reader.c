#include <stdlib.h>
#include <stdio.h>

#include "Reader.h"

Pair* Pair_init(unsigned short bv, size_t bc){
    Pair* p = (Pair*)malloc(sizeof(Pair));
    p->byte_count = bc;
    p->byte_value = bv;

    return p;
}

int Pair_free(Pair* p){
    if(p == NULL){
        return -1;
    }

    free(p);

    return 0;
}

Reader* Reader_init(const char* fn){
    if(fn == NULL){
        return NULL;
    }

    // Set Reader's data member values
    Reader* r = (Reader*)malloc(sizeof(Reader));
    r->filename = fn;
    r->file_size = _get_file_size(fn);

    // Initialize byte counters
    r->counters = (Pair*)calloc(BLOCKS, sizeof(Pair));
    for(unsigned int i = 0; i < BLOCKS; i++){
        r->counters[i].byte_value = (unsigned short)i;
        r->counters[i].byte_count = 0;
    }

    return r;
}

int Reader_free(Reader* r){
    if(r == NULL){
        return -1;
    }

    // Free up counter array and Reader object
    free(r->counters);
    free(r);
    return 0;
}

int Reader_compress_count(Reader* r){
    if(r == NULL){
        return -1;
    }
    FILE* input_file;
    input_file = fopen(r->filename, "r");

    // Delcare byte-size buffers
    unsigned char buffer_l[1];
    unsigned char buffer_r[1];
    unsigned short idx;

    // Scan file byte by byte and count
    if(r->file_size % 2 != 0){
        for(size_t i = 0; i < r->file_size - 1; i += 2){
            // Read 2 bytes to create index value
            idx = 0;
            fread(buffer_l, sizeof(char), 1, input_file);
            fread(buffer_r, sizeof(char), 1, input_file);
            idx = (idx | (unsigned short)buffer_l[0]) << 8;
            idx = (idx | (unsigned short)buffer_r[0]);
            // Increase index count by one 
            r->counters[idx].byte_count++;
        }
        // Read last byte and pad remaining byte with zeros
        fread(buffer_l, sizeof(char), 1, input_file);
        idx = (idx | (unsigned short)buffer_l[0]) << 8;
        idx = (idx | 0x00);
        buffer_r[0] = 0x00;
        // Increase index count by one 
        r->counters[idx].byte_count++;
    }
    else{
        for(size_t i = 0; i < r->file_size - 1; i += 2){
            // Read 2 bytes to create index value
            idx = 0;
            fread(buffer_l, sizeof(char), 1, input_file);
            fread(buffer_r, sizeof(char), 1, input_file);
            idx = (idx | (unsigned short)buffer_l[0]) << 8;
            idx = (idx | (unsigned short)buffer_r[0]);
            // Increase index count by one 
            r->counters[idx].byte_count++;
        }
    }
    
    fclose(input_file);

    // Sort counters and determine how many byte pairs were written
    qsort(r->counters, BLOCKS, sizeof(Pair), _pair_compare);
    for(unsigned int i = 0; i < BLOCKS; i++){
        if(r->counters[i].byte_count != 0){
            r->pairs_written++;
        }
    }
    
    return 0;
}

int Reader_decompress_count(Reader* r){
    if(r == NULL){
        return -1;
    }

    // Open input file
    FILE* input_file;
    input_file = fopen(r->filename, "r");

    // Read pair count and last bit offset
    fread(&r->pairs_written, sizeof(unsigned int), 1, input_file);
    fread(&r->last_bit_offset, sizeof(unsigned char), 1, input_file);

    fclose(input_file);
    return 0;
}

int Reader_display_counters(Reader* r){
    if(r == NULL){
        return -1;
    }

    for(unsigned int i = 0; i < BLOCKS; i+=8){
        printf("%2x : %6d, "  , r->counters[i].byte_value    ,  r->counters[i].byte_count);
        printf("%2x : %6d, "  , r->counters[i + 1].byte_value,  r->counters[i + 1].byte_count);
        printf("%2x : %6d, "  , r->counters[i + 2].byte_value,  r->counters[i + 2].byte_count);
        printf("%2x : %6d, "  , r->counters[i + 3].byte_value,  r->counters[i + 3].byte_count);
        printf("%2x : %6d, "  , r->counters[i + 4].byte_value,  r->counters[i + 4].byte_count);
        printf("%2x : %6d, "  , r->counters[i + 5].byte_value,  r->counters[i + 5].byte_count);
        printf("%2x : %6d, "  , r->counters[i + 6].byte_value,  r->counters[i + 6].byte_count);
        printf("%2x : %6d\n"  , r->counters[i + 7].byte_value,  r->counters[i + 7].byte_count);
    }

    return 0;
}

int Reader_reset_count(Reader* r){
    if(r == NULL){
        return -1;
    }

    for(unsigned int i = 0; i < BLOCKS; i++){
        r->counters[i].byte_count = 0;
    }
    
    r->pairs_written = 0;
    return 0;
}

size_t _get_file_size(const char* fn){
    if(fn == NULL){
        return -1;
    }

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

    return 0;
}