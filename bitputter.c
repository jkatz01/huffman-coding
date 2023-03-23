#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BIT_BUFFER_SIZE sizeof(bit_buffer) * 8
#define BIT_BUFFER_SIZE_1 BIT_BUFFER_SIZE - 1

void write_bit(uint32_t digit);
uint8_t print_bit();
void flush_remaining_bits();

int main() {
    char sentence[100] = "0110000101100";
    char second_sen[100] = "010";
    char temp_c;
    uint8_t num;
    int i = 0;
    while ((temp_c = sentence[i]) != '\0') {
        num = ((int) temp_c) - 48; //convert char to uint8
        write_bit(num);
        i++;
    }
    i = 0;
}

static uint32_t bit_buffer = 0;
static int n_bits = 0;

void write_bit(uint32_t digit) {
    //returns 1 if 8 bits are ready to be written, 0 if they arent ready
    bit_buffer |= digit << (BIT_BUFFER_SIZE_1 - n_bits); // OR the digit into the right size of bit_buffer, shift left by 32 - number_of_bits_written
    n_bits++;
    uint8_t num;
    printf("bit buffer: %u\n", bit_buffer);
    if (n_bits == 8) {
        num = print_bit();
    }
    else if (n_bits == BIT_BUFFER_SIZE_1) flush_remaining_bits(); // protection from overflow
}

// this function reads the 8 first bits of buffer and returns them
// it also CLEARS the 8 bits in the buffer by shifting it left
uint8_t print_bit() {
    uint8_t encoded_number = 0;
    uint32_t buffer_copy = bit_buffer;
    encoded_number |= buffer_copy >> 24; // use OR to write the first 8 bits
    n_bits -= 8;
    bit_buffer = bit_buffer << 8;
    printf("number to write: %u ", encoded_number);
    printf("char equivalent: %c\n", (char)encoded_number);
    return encoded_number;
}


void flush_remaining_bits() {
    if (n_bits > 0) {
        putchar(bit_buffer);
        bit_buffer = 0;
        n_bits = 0;
    }
}
