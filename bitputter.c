#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void putbit(uint8_t digit, uint32_t *bit_buffer, int *n_bits);
void flush_remaining_bits(uint32_t bit_buffer, int n_bits);
void write_8_bits(uint32_t bit_buffer, int n_bits);
void shift_8_left(uint32_t bit_buffer);

int main() {
    uint32_t character_buffer;
    int number_bits;

    putbit(1, &character_buffer, &number_bits);

}

void putbit(uint8_t digit, uint32_t *bit_buffer, int *n_bits) {
    *bit_buffer |= digit << (sizeof(uint32_t) - *n_bits); // OR the digit into the right size of bit_buffer, shift left by 32 - number_of_bits_written
    *n_bits++;
    if (*n_bits == sizeof(uint32_t)) flush_remaining_bits(bit_buffer, n_bits);

}