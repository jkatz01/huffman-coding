#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>

//function definitions:
void    generate_compressed_file(char *file_in, char *file_out, Code *data, Letter *letter_array);

int main() {
    //create letter frequency data and chart
    Letter *data;
    data = malloc(sizeof(Letter) * SIZE);
    create_letter_array(data);

    read_file("test1.txt", data);
    write_file("frequency.txt", data, SIZE);
    read_frequencies("frequency.txt");

    /*for (int b = 0; b < SIZE; b++) {
        printf("%c: %i\n", data[b].letter, data[b].frequency);
    }*/
    //build the huffman tree
    Node    *root = build_huff_tree(data, SIZE);
    int     array[100];
    int     start = 0;
    FILE    *fp;
    fp = fopen("codes.txt", "w+");
    print_codes(fp, root, array, start);
    fclose(fp);

    Code    *codes;
    codes = malloc(sizeof(Code) * SIZE);
    create_code_array(codes);
    read_codes(codes, "codes.txt");

    generate_compressed_file("test1.txt", "output.bin", codes, data);

    free(data);
    free(codes);
    return 0;
}

/* generate_compressed_file(filename_in, filename_out, Code *data)
*  calculate the total number of bits using the letter frequencies
*/
void generate_compressed_file(char *file_in, char *file_out, Code *data, Letter *letter_array) {
    FILE    *fp, *fw;
    size_t  infile_size;
    size_t  letters_size = SIZE;

    fp = fopen(file_in, "r");
    if (fp == NULL){
        printf("Error: file input cannot be opened\n");
        exit(1);
    }
    fw = fopen(file_out, "wb");
    if (fw == NULL){
        printf("Error: file output cannot be opened\n");
        exit(1);
    }

    // get file size
    fseek(fp, 0, SEEK_END);
    infile_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    printf("Hi\n");

    int bit_count = 0;
    int code_len;
    char read_c;
    char code_buf[32];
    size_t INPUT_SIZE = infile_size + 8; //8 for buffer
    char* input_str = (char *) calloc(INPUT_SIZE, sizeof(char));
    while ((read_c = fgetc(fp)) != EOF) {
        read_c = verify_char(read_c);
        if (read_c != 0) {
            if (bit_count >= (INPUT_SIZE/2)) {
                INPUT_SIZE = INPUT_SIZE * 2;
                input_str = (char *)realloc(input_str, INPUT_SIZE * sizeof(char));
                printf("\n\n\nNew input size: %zu\n\n\n", INPUT_SIZE);
            }
            code_len = strlen(get_code(data, read_c));
            strncat(input_str, get_code(data, read_c), code_len);
            if (code_len > 10) {
                printf("OH nonononononon\n");
            }
            bit_count += code_len;
        }
        else {
            //printf("\n");
        }
        
    }
    input_str[bit_count] = '\0';
    
    printf("code: %s\n", input_str);

    Bitstream out_buffer;
    // we're gonna know how much to alloc when we get the binary string
    // Allocate one uint32 for every 32 bits, + 1 for the remainder (if)
    out_buffer.data = (uint32_t *) malloc(((bit_count / 32) + 1) * sizeof(uint32_t));
    write_bitstream(input_str, &out_buffer);

    printf("Num bytes: %zu\n Remainder: %d\n", out_buffer.data_size, out_buffer.last_bit_offset);

    if (fwrite(&out_buffer, sizeof(Bitstream), 1, fw)!= 1) {
        printf("fwrite bitstream failed\n");
    }
    if (fwrite(&out_buffer.data[0], sizeof(uint32_t), out_buffer.data_size, fw) != out_buffer.data_size) {
        printf("fwrite bitstream data failed\n");
    }
    //write the letter frequencies for  the file
    if (fwrite(&letters_size, sizeof(int), 1, fw) != 1) {
        printf("fwrite letters size failed\n");
    }
    if (fwrite(letter_array, sizeof(Letter), letters_size, fw) != letters_size) {
        printf("fwrite letters array failed\n");
    }
    
    fclose(fp);
    fclose(fw);
}



