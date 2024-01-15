#include "huffman.h"

//function definitions:
void generate_compressed_file(char *file_in, char *file_out, Code *data, long size, Letter *letter_array);
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


