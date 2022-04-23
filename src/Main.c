#include <stdlib.h>
#include <stdio.h>

#include "Reader.h"
#include "Huffman.h"

int main(int argc, char* argv[]){
    const char* input_fn = "../tests/test_data1.txt";
    //const char* input_fn = "../tests/test_data2.cl";
    //const char* input_fn = "../tests/test_data3.txt";
    //const char* input_fn = "../tests/test_data4.txt";
    //const char* input_fn = "../tests/test_data5.txt";
    //const char* input_fn = "../tests/test_data6.txt";
    //const char* input_fn = "../tests/test_data7.csv";
    //const char* input_fn = "../tests/test_data8.json";

    const char* output_fn = "output.kom";

    const char* decom_fn = "decom.txt";

    Reader* r1 = Reader_init(input_fn);
    Reader_compress_count(r1);
    Huffman* h1 = Huffman_init(r1, output_fn);
    Huffman_compress(h1, r1);
    Huffman_free(h1);
    Reader_free(r1);

    Reader* r2 = Reader_init(output_fn);
    Reader_decompress_count(r2);
    Huffman* h2 = Huffman_init(r2, decom_fn);
    Huffman_decompress(h2, r2);
    Huffman_free(h2);
    Reader_free(r2);

    return 0;
}