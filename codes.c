#include <stdio.h>
#include <stdlib.h>

typedef struct letter {
    char letter;
    int frequency;
} Letter;

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

void print_array(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
    printf("%d", arr[i]);
    printf("\n");
}

void print_codes(Node *root, int codes[], int start) {
    if (root->left) {
        //exists
        codes[start] = 0;
        print_codes(root->left, codes, start + 1);
    }
    if (root->right) {
        //exists
        codes[start] = 1;
        print_codes(root->right, codes, start + 1);
    }

    if(leaf_node(root)) {
        printf("%c:", root->letter);
        print_array(codes, start);
    }
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

int main() {
    Letter letters[4];
    letters[0].letter = 'a';
    letters[0].frequency = 10;
    letters[1].letter = 'b';
    letters[1].frequency = 15;
    letters[2].letter = 'c';
    letters[2].frequency = 3;
    letters[3].letter = 'd';
    letters[3].frequency = 12;

    Node *root = build_huff_tree(letters, 4);
    int array[100];
    int start = 0;

    print_codes(root, array, start);

    char t[30] = "babcbdbabbbbbbbbba";
    char test[20] = "0111011001001110";

    read_binary_string(test, root, 16);

    return 0;
}