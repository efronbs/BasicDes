CC=g++ 
LFLAGS=-Wall --std=c++14
CFLAGS=-Wall -march=native -c -O3 --std=c++14

des: fiestel.o
	$(CC) $(LFLAGS) fiestel.o -o des

fiestel.o: fiestel.cpp des.h constants.h
	$(CC) $(CFLAGS) fiestel.cpp
 
