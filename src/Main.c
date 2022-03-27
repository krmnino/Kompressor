#include <stdlib.h>
#include <stdio.h>

#include "Reader.h"

int main(int argc, char* argv[]){
    const char* fn = "../tests/test_data1.txt";
    //const char* fn = "../tests/test_data2.cl";

    Reader* r = Reader_init(fn);

    Reader_count(r);

    Reader_sort_count(r);

    Reader_display_counters(r);

    Reader_reset_count(r);

    Reader_free(r);
    return 0;
}