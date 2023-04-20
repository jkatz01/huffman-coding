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
void create_code_array(Code *data);
void add_code(Code *data, char c, char *code);
void read_codes(Code *data, char *filename);
char *get_code(Code *data, char c);
int read_binary_string(char letters[], Node *root, int size);

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


    /* just testing stuff
    printf("Example code: %s", codes[5].code);
    read_binary_string(bits_buffer, root, bits_num);
    uint8_t test_c = bits_buffer[120];
    printf("%c %u \n", bits_buffer[120]);
    char test_str[9] = "";
    toBinary(test_c, test_str);
    puts(test_str);
    */

    char binary_bits_string[bytes_num * 8];
    char temp_buffer[33];
    char temp_b_str[9];
    for (int j = 0; j < bytes_num; j++) {
        itoa(bits_buffer[j], temp_buffer, 2);
        memcpy(temp_b_str, &temp_buffer[24], 8); //we need this because itoa returns a 32 bit integer but we want 8 bit
        strncat(binary_bits_string, temp_b_str, 8);
    } 
    
    printf("%.32s", binary_bits_string);
    
    read_binary_string(binary_bits_string, root, bits_num);
    return 0;
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