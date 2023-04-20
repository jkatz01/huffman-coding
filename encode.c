/*
DEC 11 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

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

//function definitions:
//frequency
void read_file(char *filename, Letter *data);
void add_letter(Letter *data, char c);
void create_letter_array(Letter *data);
void write_file(char *filename, Letter *data, int size);
char verify_char(char c);
void read_frequencies(char *filename);
//codes
Node *new_node(char let, int freq);
MinHeap *new_heap(int cap);
void swap_nodes (Node **a, Node **b);
void heapify(MinHeap *hp, int index);
Node *extract_min(MinHeap *hp);
void insert(MinHeap *hp, Node *n);
int leaf_node (Node *root);
void build_heap(MinHeap *hp);
MinHeap *create_heap(Letter data[], int size);
Node *build_huff_tree(Letter data[], int size);
void print_codes(FILE *fp, Node *root, int codes[], int start);
//encoding
void generate_compressed_file(char *file_in, char *file_out, Code *data, long size, Letter *letter_array);
void create_code_array(Code *data);
void add_code(Code *data, char c, char *code);
void read_codes(Code *data, char *filename);
char *get_code(Code *data, char c);
//bit writing
void write_bit(uint32_t digit, FILE *output);
uint8_t print_bit(FILE *output);
void flush_remaining_bits();
long total_num_of_bits(Letter data[], Code *codes);

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
    Node *root = build_huff_tree(data, SIZE);
    int array[100];
    int start = 0;
    FILE *fp;
    fp = fopen("codes.txt", "w+");
    print_codes(fp, root, array, start);
    fclose(fp);

    Code *codes;
    codes = malloc(sizeof(Code) * SIZE);
    create_code_array(codes);
    read_codes(codes, "codes.txt");

    //get size
    long total_bits = total_num_of_bits(data, codes);
    //printf("Total number of bits: %ld\n", total_bits);
    
    generate_compressed_file("test1.txt", "output.bin", codes, total_bits, data);

    free(data);
    free(codes);
    return 0;
}

/* generate_compressed_file(filename_in, filename_out, Code *data)
*  calculate the total number of bits using the letter frequencies
*  calculate max code size
*  create a buffer uint32
*  read letter, add its code to the buffer using a custom putbit function
*   -> if the code is < 7 keep it in the buffer
*       -> add another letter code to the buffer
*       -> write the first 8 bits to file
*       -> shift buffer 8 to the left
*/
void generate_compressed_file(char *file_in, char *file_out, Code *data, long size, Letter *letter_array) {
    FILE *fp, *fw;
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

    char read_c;
    char temp_c;
    char line[64];
    int code_len, j, num;
    int bits_remainder = size % 8;
    int bit_count = 0;
    int letters_size = SIZE;
    //write number of bits to read at front of file
    fwrite(&size, sizeof(size), 1, fw);
    //write the total number of bytes to read
    long num_bytes = (size / 8) + bits_remainder;
    fwrite(&num_bytes, sizeof(num_bytes), 1, fw);

    printf("bits: %ld, bytes: %ld", size, num_bytes);

    while((read_c = fgetc(fp)) != EOF) {
        memset(line, '\0', 64);
        read_c = verify_char(read_c);
        if (read_c != 0) {
            code_len = strlen(get_code(data, read_c));
            strncat(line, get_code(data, read_c), code_len);
            //printf("code_len: %d line: %s\n", code_len, line);
            for (j = 0; j < code_len; j++) {
                temp_c = line[j];
                num = ((int) temp_c) - 48; //convert char to int
                write_bit(num, fw);
                bit_count++;
            }
            if (bit_count > (size - bits_remainder)) {
                //less than 8 bits remaining
                print_bit(fw);
            }
        }
        else {

        }
        
    }
    //write the letter frequencies for  the file
    if (fwrite(&letters_size, sizeof(int), 1, fw) != 1) {
        printf("fwrite failed\n");
    }
    if (fwrite(letter_array, sizeof(Letter), letters_size, fw) != letters_size) {
        printf("fwrite failed\n");
    }
    
    fclose(fp);
    fclose(fw);
}

static uint32_t bit_buffer = 0;
static int n_bits = 0;

void write_bit(uint32_t digit, FILE *output) {
    //returns 1 if 8 bits are ready to be written, 0 if they arent ready
    bit_buffer |= digit << (BIT_BUFFER_SIZE_1 - n_bits); // OR the digit into the right size of bit_buffer, shift left by 32 - number_of_bits_written
    n_bits++;
    uint8_t num;
    //printf("bit buffer: %u\n", bit_buffer);
    if (n_bits == 8) {
        num = print_bit(output);
    }
    else if (n_bits == BIT_BUFFER_SIZE_1) flush_remaining_bits(); // protection from overflow
}

// this function reads the 8 first bits of buffer and returns them
// it also CLEARS the 8 bits in the buffer by shifting it left
uint8_t print_bit(FILE *output) {
    uint8_t encoded_number = 0;
    uint32_t buffer_copy = bit_buffer;
    encoded_number |= buffer_copy >> 24; // use OR to write the first 8 bits
    n_bits -= 8;
    bit_buffer = bit_buffer << 8;
    //printf("number to write: %u ", encoded_number);
    //printf("char equivalent: %c\n", (char)encoded_number);
    if (fwrite(&encoded_number, sizeof(encoded_number), 1, output) != 1) {
        printf("fwrite failed\n");
    }
    return encoded_number;
}


void flush_remaining_bits() {
    if (n_bits > 0) {
        putchar(bit_buffer);
        bit_buffer = 0;
        n_bits = 0;
    }
}

long total_num_of_bits(Letter data[], Code *codes) {
    long sum = 0;
    char c;
    int bits_of_letter;
    for (int i = 0; i < SIZE; i++) {
        c = data[i].letter;
        bits_of_letter = data[i].frequency * strlen(get_code(codes, c));
        sum += (long) bits_of_letter;
    }
    return sum;
}


void read_file(char *filename, Letter *data) {
    FILE *fp;
    fp = fopen(filename, "r");

    char c;
    char temp;
    while((c = fgetc(fp)) != EOF) {
        temp = verify_char(c);
        if (temp != 0) {
            add_letter(data, temp);
        }
    }
    fclose(fp);
}

void add_letter(Letter *data, char c) {
    int i = 0;
    while(i < SIZE) {
        if(data[i].letter == c) {
            break;
        }
        i++;
    }
    if(i > SIZE) {
        printf("Error: incorrect character passed\n");
        return;
    }
    data[i].frequency++;
}

void create_letter_array(Letter *data) {
    char letters[SIZE+1] = "abcdefghijklmnopqrstuvwxyz0123456789 ,.";
    for (int i = 0; i < SIZE; i++) {
        data[i].letter = letters[i];
        data[i].frequency = 0;
    }
}

void write_file(char *filename, Letter *data, int size) {
    FILE *fp;
    fp = fopen(filename, "w");

    for(int i = 0; i < size; i++) {
        fprintf(fp, "%c:%d\n", data[i].letter, data[i].frequency);
    }

    fclose(fp);
}

char verify_char(char c) {
    //returns correct character or nullif it needs to be ignored and not printed
    char temp;
    if (isalpha(c) != 0) {
        temp = tolower(c);
    }
    else if (isdigit(c) != 0) {
        temp = c;
    }
    else if (isspace(c) != 0) {
        temp = 32;
    }
    else if (c == COMMA || c == PERIOD) {
        temp = c;
    }
    else {
        temp = 0;
    }
    return temp;
}

void read_frequencies(char *filename) {
    FILE *fp;
    fp = fopen(filename, "r");

    char line[32];
    char *ptr;
    while (fgets(line, 32, fp) != NULL) {
        ptr = &line[2];
        //printf("%c:%d", line[0], atoi(ptr));
    }
    
    fclose(fp);
}

Node *new_node(char let, int freq) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->letter = let;
    n->frequency = freq;
    n->left = NULL;
    n->right = NULL;
    return n;
}

MinHeap *new_heap(int cap) {
    MinHeap *hp = (MinHeap*)malloc(sizeof(MinHeap));
    hp->capacity = cap;
    hp->size = 0;
    hp->nodes = (Node**)malloc(hp->capacity * sizeof(Node*));
    return hp;
}

void swap_nodes (Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(MinHeap *hp, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < hp->size && hp->nodes[left]->frequency < hp->nodes[smallest]->frequency) {
        smallest = left;
    }
    if (right < hp->size && hp->nodes[right]->frequency < hp->nodes[smallest]->frequency) {
        smallest = right;
    }
    if(smallest != index) {
        swap_nodes(&hp->nodes[smallest], &hp->nodes[index]);
        heapify(hp, smallest);
    }
}

Node *extract_min(MinHeap *hp) {
    Node *temp = hp->nodes[0];
    hp->nodes[0] = hp->nodes[hp->size - 1];

    hp->size -= 1;
    heapify(hp, 0);

    return temp;
}

void insert(MinHeap *hp, Node *n) {
    hp->size += 1;
    int i = hp->size - 1;

    while (i && n->frequency < hp->nodes[(i - 1) / 2]->frequency) {
        hp->nodes[i] = hp->nodes[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    hp->nodes[i] = n;
}

int leaf_node (Node *root) {
    //a leaf node has no left/right child
    //returns 0 for true, 1 for false
    int i = 0;
    if (root->left == NULL) i = 1;
    if (root->right == NULL) i = 1;
    return i;
}

void build_heap(MinHeap *hp) {
    //this function turns the array hp->nodes into a proper heap
    //by using heapify
    int n = hp->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i) {
        heapify(hp, i);
    }
}

MinHeap *create_heap(Letter data[], int size) {
    //this function creates a new heap with all the 
    //letters given in
    MinHeap *hp = new_heap(size);
    for(int i = 0; i < size; ++i) {
        hp->nodes[i] = new_node(data[i].letter, data[i].frequency);
    }
    hp->size = size;
    build_heap(hp);

    return hp;
}

Node *build_huff_tree(Letter data[], int size) {
    //we need to create a heap and populate it 
    MinHeap *hp = create_heap(data, size);
    //we need these nodes to create the huffman tree from the heap
    Node *left, *right, *parent;

    //extract (and delete) two minimum letters from the heap
    //and make them left/right of a new internal node
    //add this new internal node to the min heap
    while(hp->size != 1) {
        left = extract_min(hp);
        right = extract_min(hp);

        parent = new_node('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        insert(hp, parent);
    }

    //this node returned is the root node
    return extract_min(hp);
}

void print_codes(FILE *fp, Node *root, int codes[], int start) {
    if (root->left) {
        //exists
        codes[start] = 0;
        print_codes(fp, root->left, codes, start + 1);
    }
    if (root->right) {
        //exists
        codes[start] = 1;
        print_codes(fp, root->right, codes, start + 1);
    }

    if(leaf_node(root)) {
        fprintf(fp, "%c:", root->letter);
        //print_array(codes, start);
        int i;
        for (i = 0; i < start; ++i)
        fprintf(fp, "%d", codes[i]);
        fprintf(fp, "\n");
    }
}

void create_code_array(Code *data) {
    char letters[SIZE+1] = "abcdefghijklmnopqrstuvwxyz0123456789 ,.";
    for (int i = 0; i < SIZE; i++) {
        data[i].letter = letters[i];
    }
}

void add_code(Code *data, char c, char *code) {
    int i = 0;
    while(i < SIZE) {
        if(data[i].letter == c) {
            break;
        }
        i++;
    }
    if(i > SIZE) {
        printf("Error: incorrect character passed\n");
        return;
    }
    data[i].code = malloc(strlen(code));
    code[strlen(code)-1] = '\0';
    memcpy(data[i].code, code, strlen(code));
}

void read_codes(Code *data, char *filename) {
    FILE *fp;
    fp = fopen(filename, "r");

    char line[32];
    char *ptr;
    while (fgets(line, sizeof(line), fp) != NULL) {
        ptr = &line[2];
        add_code(data, line[0], ptr);
        //printf("Added char: %c with code: %s\n", line[0], ptr);
    }
    printf("\n\n");
    fclose(fp);
}

char *get_code(Code *data, char c) {
    int i = 0;
    while(i < SIZE) {
        if(data[i].letter == c) {
            break;
        }
        i++;
    }
    if(i > SIZE) {
        printf("Error: incorrect character passed\n");
        return "fail: letter doesnt exist\n";
    }
    char *temp = data[i].code;
    //printf("\n--- char: %c --- code: %s --- strlen: %d\n", c, temp, strlen(temp));
    return temp;
}