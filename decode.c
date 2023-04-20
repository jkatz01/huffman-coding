#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define COMMA 44
#define PERIOD 46
#define SIZE 39
#define CODESIZE 16
#define BIT_BUFFER_SIZE sizeof(bit_buffer) * 8
#define BIT_BUFFER_SIZE_1 BIT_BUFFER_SIZE - 1

typedef struct letter {
    char letter;
    int frequency;
} Letter;

typedef struct code {
    char letter;
    char *code;
} Code;

typedef struct node {
    char letter;
    int frequency;
    struct node *left;
    struct node *right;
} Node;

typedef struct minheap {
    int size;
    int capacity;
    Node **nodes;
} MinHeap;

int main() {
    char buffer[100];
    FILE *fr;
    fr = fopen("output.bin", "rb");
    long bits_num, bytes_num;
    fread(&bits_num, sizeof(long), 1, fr);
    fread(&bytes_num, sizeof(long), 1, fr);
    printf("bits: %ld, bytes: %ld\n", bits_num, bytes_num);

    char bits_buffer[bytes_num];
    fread(&bits_buffer, sizeof(char), bytes_num, fr);
    //fseek(fr, bytes_num, SEEK_CUR);
    //printf("hi");

    int letter_fr_size;
    fread(&letter_fr_size, sizeof(int), 1, fr);

    Letter *letter_frequencies;
    letter_frequencies = malloc(sizeof(Letter) * letter_fr_size);
    if (fread(letter_frequencies, sizeof(Letter), letter_fr_size, fr) != letter_fr_size) {
        printf("Something went wrong");
    }
    for (int i = 0; i < letter_fr_size; i++) {
        printf("Example Letter: %c freq: %d\n", letter_frequencies[i].letter, letter_frequencies[i].frequency);
    }

    // Everything is in memory, we can create the binary tree now
    


    return 0;
}