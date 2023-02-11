#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define COMMA 44
#define PERIOD 46
#define SIZE 39
#define CODESIZE 16

int main() {
    char buffer[100];
    FILE *fw;
    fw = fopen("output.bin", "rb");
    fread(buffer, sizeof(char), 100, fw);
    printf("%s", buffer);

    //read heap tree
    //call read_binary_string from codes.c

    return 0;
}