#include <stdlib.h>
#include <stdio.h>

#include "Reader.h"
#include "Huffman.h"

int main(int argc, char* argv[]){
    const char* fn = "../tests/test_data1.txt";
    //const char* fn = "../tests/test_data2.cl";
    //const char* fn = "../tests/test_data3.txt";
    //const char* fn = "../tests/test_data4.txt";
    //const char* fn = "../tests/test_data5.txt";
    //const char* fn = "../tests/test_data6.txt";

    Reader* r = Reader_init(fn);

    Reader_compress_count(r);
  
    //Reader_display_counters(r);

    Huffman* h = Huffman_init(r, "output.kom");

    Huffman_compress(h, r);
//
    //Reader_reset_count(r);
//
    Reader_free(r);
    Huffman_free(h);
    
    return 0;
}