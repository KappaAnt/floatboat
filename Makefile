CC = gcc
CFLAGS = -Wall -std=c18 -ggdb

test : testFloatx
	./testFloatx 16 4
	
clean :
	-rm testFloatx