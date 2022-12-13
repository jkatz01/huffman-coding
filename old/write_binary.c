#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 3

typedef struct plop {
    char letter;
    int frequency;
} Plop;

bool write_data(char *filename, Plop *data, int total);
Plop *read_data(char *filename, int *total);

int main() {
    Plop *meal;
    meal = malloc(sizeof(Plop) * SIZE);

    meal[0].letter = 's';
    meal[0].frequency = 5;
    meal[1].letter = 'd';
    meal[1].frequency = 15;
    meal[2].letter = 'u';
    meal[2].frequency = 2;

    if (write_data("test.bin", meal, SIZE)) {
        printf("Wrote data\n");
    }
    else {
        printf("Error writing file\n");
        return 1;
    }
    free(meal);

    int total = 0;
    Plop *file_data;
    file_data = read_data("test.bin", &total);
    if (file_data == NULL) {
        printf("Error reading from file \n");
        return 1;
    }
    printf("\ndata read \n");
    for (int i = 0; i < total; i++) {
        printf("Letter %c: %d\n", file_data[i].letter, file_data[i].frequency);
    }
    free(file_data);
    return 0;
}

bool write_data(char *filename, Plop *data, int total) {
    FILE *fp;
    fp = fopen(filename, "wb"); //write binary

    if (fp == NULL) return false;

    if (fwrite(&total, sizeof(int), 1, fp) != 1) { //write total a number of 1 times to file 
        return false;
    }

    if (fwrite(data, sizeof(Plop), total, fp) != total) {
        return false;
    }

    if (fclose(fp) == EOF) {
        return false;
    }

    return true;
}

Plop *read_data(char *filename, int *total) {
    FILE *fp;
    fp = fopen(filename, "rb");

    if (fp == NULL) return NULL;

    if (fread(total, sizeof(int), 1, fp) != 1) {
        return NULL;
    }
    Plop *data = malloc(sizeof(Plop) * *total);

    if (fread(data, sizeof(Plop), *total, fp) != *total) {
        free(data);
        return NULL;
    }
    if (fclose(fp) == EOF) {
        free(data);
       return NULL;
    }

    return data;

}