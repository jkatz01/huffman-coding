CC=gcc
CFLAGS = -o

encode: encode.c
	$(CC) encode.c $(CFLAGS) encode

decode: decode.c
	$(CC) decode.c $(CFLAGS) decode

clean:
	-del -fR *.exe codes.txt decode_codes.txt frequency.txt output.bin