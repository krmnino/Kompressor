#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "Reader.h"
#include "Huffman.h"

int main(int argc, char* argv[]) {
    if(argc != 4 && argc != 5){
        printf("USAGE: ./kompressor [INPUT_FILE] [-c/-d] [-ow] [OUTPUT_FILE]\n");
        return -1;
    }

    int ret;

    // Validate option flags
    if(strcmp(argv[2], "-c") != 0 && strcmp(argv[2], "-d") != 0){
        printf("ERROR: Invalid option. -c = compress. -d = decompress\n");
        printf("USAGE: ./kompressor [INPUT_FILE] [-c/-d] [-ow] [OUTPUT_FILE]\n");
        return -1;
    }

    // Check if input file exists
    ret = access(argv[1], F_OK);
    if(ret != 0){
        printf("ERROR: Input file cannot be found.\n");
        return -1;
    }

    // If program parameter count is 4
    if(argc == 4){
        // Check if output file exists
        ret = access(argv[3], F_OK);
        if(ret == 0){
            printf("ERROR: Output file exists but overwrite option -ow is disabled.\n");
            printf("USAGE: ./kompressor [INPUT_FILE] [-c/-d] [-ow] [OUTPUT_FILE]\n");
            return -1;
        }

        // If -c option is passed, then compress
        if(strcmp(argv[2], "-c") == 0){
            Reader* r1 = Reader_init(argv[1]);
            Reader_compress_count(r1);
            Huffman* h1 = Huffman_init(r1, argv[3]);
            Huffman_compress(h1, r1);
            Huffman_free(h1);
            Reader_free(r1);
        }
        // Else if -d option is passed, then decompress
        else{
            Reader* r2 = Reader_init(argv[1]);
            Reader_decompress_count(r2);
            Huffman* h2 = Huffman_init(r2, argv[3]);
            Huffman_decompress(h2, r2);
            Huffman_free(h2);
            Reader_free(r2);
        }
    }
    // Else if program parameter count is 5
    else{
        // Validate option flags
        if(strcmp(argv[3], "-ow") != 0){
            printf("ERROR: Invalid option. -ow = allow output file overwrite\n");
            printf("USAGE: ./kompressor [INPUT_FILE] [-c/-d] [-ow] [OUTPUT_FILE]\n");
            return -1;
        }

        // If -c option is passed, then compress
        if(strcmp(argv[2], "-c") == 0){
            Reader* r1 = Reader_init(argv[1]);
            Reader_compress_count(r1);
            Huffman* h1 = Huffman_init(r1, argv[4]);
            Huffman_compress(h1, r1);
            Huffman_free(h1);
            Reader_free(r1);
        }
        // Else if -d option is passed, then decompress
        else{
            Reader* r2 = Reader_init(argv[1]);
            Reader_decompress_count(r2);
            Huffman* h2 = Huffman_init(r2, argv[4]);
            Huffman_decompress(h2, r2);
            Huffman_free(h2);
            Reader_free(r2);
        }
    }
    return 0;
}