#ifndef READER
#define READER

#include <stdlib.h>
#include <stdio.h>

struct Reader{
    size_t file_size;
    FILE* file_ptr;
    size_t* counters;
    const char* filename;
};

typedef struct Reader Reader;

Reader* Reader_init(const char*);
int Reader_free();
int Reader_count();
size_t get_file_size(const char*);

#endif