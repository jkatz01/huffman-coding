/*
Yehonatan Katz
DEC 11 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define COMMA 44
#define PERIOD 46
#define SIZE 39
#define CODESIZE 16

typedef struct letter {
    char letter;
    int frequency;
} Letter;

typedef struct code {
    char letter;
    char code[CODESIZE];
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
void generate_compressed_file(char *file_in, char *file_out, Code *data);
void create_code_array(Code *data);
void add_code(Code *data, char c, char *code);
void read_codes(Code *data, char *filename);

int main() {
    //create letter frequency data and chart
    Letter *data;
    data = malloc(sizeof(Letter) * SIZE);
    create_letter_array(data);

    read_file("test1.txt", data);
    write_file("frequency.txt", data, SIZE);
    read_frequencies("frequency.txt");

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
    
    generate_compressed_file("test1.txt", "output.bin", codes);
    free(data);
    return 0;
}

void generate_compressed_file(char *file_in, char *file_out, Code *data) {
    FILE *fp;
    fp = fopen(file_in, "r");
    printf("reading file: ");

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(file_size);
    memset(buffer, 0, file_size);

    char c;
    char temp;
    int i = 0;
    while((c = fgetc(fp)) != EOF) {
        temp = verify_char(c);
        if (temp != 0) {
            //add to buffer
            buffer[i] = temp;
            i++;
        }
    }
    printf("buffer size: %d, buffer: %s\n", file_size, buffer);
    //FILE *fw;
    //fw = fopen(file_out, "wb");
    free(buffer);
    fclose(fp);
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
    memcpy(data[i].code, code, CODESIZE);
}

void read_codes(Code *data, char *filename) {
    FILE *fp;
    fp = fopen(filename, "r");

    char line[32];
    char *ptr;
    while (fgets(line, sizeof(line), fp) != NULL) {
        ptr = &line[2];
        add_code(data, line[0], ptr);
    }
    fclose(fp);
}