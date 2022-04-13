#ifndef READER
#define READER

#include <stdlib.h>
#include <stdio.h>

#define BLOCKS 0x10000

typedef struct Pair Pair;
struct Pair{
    unsigned int byte_count;
    unsigned short byte_value;
};
Pair* Pair_init(unsigned short, size_t);
int Pair_free(Pair*);

typedef struct Reader Reader;
struct Reader{
    size_t file_size;
    FILE* file_ptr;
    Pair* counters;
    const char* filename;
    int pairs_written;
};
Reader* Reader_init(const char*);
int Reader_free(Reader*);
int Reader_compress_count(Reader*);
int Reader_decompress_count(Reader*);
int Reader_display_counters(Reader*);
int Reader_reset_count(Reader*);
size_t _get_file_size(const char*);
int _pair_compare(const void*,const void*);

#endif