#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define COMMA 44
#define PERIOD 46
#define SIZE 39

typedef struct letter {
    char letter;
    int frequency;
} Letter;
void read_file(char *filename, Letter *data);
void add_letter(Letter *data, char c);
void create_letter_array(Letter *data);
void write_file(char *filename, Letter *data, int size);
char verify_char(char c);
void read_frequencies(char *filename);


int main() {
    Letter *data;
    data = malloc(sizeof(Letter) * SIZE);
    create_letter_array(data);

    read_file("test.txt", data);
    write_file("output.txt", data, SIZE);
    read_frequencies("output.txt");

    free(data);
    return 0.;
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
        printf("%c:%d", line[0], atoi(ptr));
    }
    
    fclose(fp);
}