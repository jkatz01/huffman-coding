#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    //01110110
    char *big = "01110110000000000000000000000000000000000000000000000000000000000000000001110111";
    int i = 0;
    char *ptr = malloc(sizeof(char));
    char c;
    //(big + i);
    while(i < strlen(big)) {
        if(i % 8 == 0) {
            memcpy(ptr, big + i, 8);
            c = strtol(ptr, 0, 2);
            printf("%s = %c = %d = 0x%.2X\n", ptr, c, c, c);
        }
        i++;
    }


    return 0;
}