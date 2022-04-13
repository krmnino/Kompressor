#include <stdlib.h>
#include <stdio.h>

#include "Reader.h"
#include "Huffman.h"

int main(int argc, char* argv[]){
    //const char* fn = "../tests/test_data1.txt";
    //const char* fn = "../tests/test_data2.cl";
    //const char* fn = "../tests/test_data3.txt";
    //const char* fn = "../tests/test_data4.txt";
    //const char* fn = "../tests/test_data5.txt";
    //const char* fn = "../tests/test_data6.txt";
    //const char* fn = "../tests/test_data7.csv";
    const char* fn = "../tests/test_data8.json";

    Reader* r = Reader_init(fn);
    Reader* r2 = Reader_init("output.kom");

    Reader_compress_count(r);
  
    Huffman* h = Huffman_init(r, "output.kom");

    Huffman_compress(h, r);


    Huffman_free(h);
    Reader_free(r);

    Reader_decompress_count(r2);
    Reader_free(r2);

    return 0;
}