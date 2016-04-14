CC=g++
#REMEMBER TO USE -o3 
LFLAGS=-Wall --std=c++14
CFLAGS=-Wall -c --std=c++14

des: fiestel.o
	$(CC) $(LFLAGS) fiestel.o -o des

fiestel.o: fiestel.cpp des.h constants.h
	$(CC) $(CFLAGS) fiestel.cpp
 
