#include "huffman.h"
#include <stdio.h>

//function definitions
int read_binary_string(char letters[], Node *root, int size);

int main() {
    char buffer[100];
    FILE *fr;
    fr = fopen("output.bin", "rb");
    long bits_num, bytes_num;
    fread(&bits_num, sizeof(long), 1, fr);
    fread(&bytes_num, sizeof(long), 1, fr);
    printf("bits: %ld, bytes: %ld\n", bits_num, bytes_num);
    
    Bitstream in_buffer;
    char* bits_buffer = (char*)malloc(bytes_num);
    fread(&bits_buffer, sizeof(char), bytes_num, fr);
    int letter_fr_size;
    Letter *letter_frequencies;
    letter_frequencies = malloc(sizeof(Letter) * letter_fr_size);

    fread(&in_buffer, sizeof(Bitstream), 1, fr);
    fread(in_buffer.data, sizeof(uint32_t), in_buffer.data_size, fr);
    fread(&letter_fr_size, sizeof(int), 1, fr);
    if (fread(letter_frequencies, sizeof(Letter), letter_fr_size, fr) != letter_fr_size) {
        printf("Something went wrong");
    }
    for (int i = 0; i < letter_fr_size; i++) {
        printf("Example Letter: %c freq: %d\n", letter_frequencies[i].letter, letter_frequencies[i].frequency);
    }

    // Everything is in memory, we can create the binary tree now
    //build the huffman tree
    Node *root = build_huff_tree(letter_frequencies, SIZE);
    int array[100];
    int start = 0;
    FILE *fp;
    fp = fopen("decode_codes.txt", "w");
    print_codes(fp, root, array, start);
    fclose(fp);

    Code *codes;
    codes = malloc(sizeof(Code) * SIZE);
    create_code_array(codes);
    read_codes(codes, "decode_codes.txt");

    read_binary_string("TEMPORARY", root, bits_num);
    return 0;
}



int read_binary_string(char letters[], Node *root, int size) {
    //this function decodes a string using the tree basically
    char temp;
    char pr;
    Node *ptr = root;
    int i = 0;
    int status = 0; //if 0, then it finishes a previous 1, if 1 then new character
    while (i < size) {
        temp = letters[i];
        if (temp == '0') {
            //traverse left, left child must exist
            //reset ptr to root
            if (status == 1) {
                ptr = root;
                pr = ptr->left->letter;
            }
            pr = ptr->left->letter;
            printf("%c", pr);
            status = 1;
            i++;
        }
        if (temp == '1') {
            //traverse right, if it is a leaf node then we want to print the letter
            ///check for null
            if(ptr->right == NULL) {
                printf("error, an incorrect code has been given, no more 1's are possible\n");
                return 1;
            }
            ptr = ptr->right;
            //set status to 1 if a character just printed
            if(leaf_node(ptr)) {
                pr = ptr->letter;
                printf("%c", pr);
                status = 1;
            }
            else {
                status = 0;
            }
            //progress to next letter
            i++;
        }
        //printf("size of i: %d\n", i);
    }
    return 0;
}
