FLAGS = -Wall -std=c++11 -g

all: ssfs_mkdsk

ssfs_mkdsk:	mkdisk.o
	g++ $(FLAGS) mkdisk.o -o ssfs_mkdsk

mkdisk.o:	mkdisk.cpp
	g++ $(FLAGS) -c mkdisk.cpp -o mkdisk.o

clean:
	rm -f *.o ssfs_mkdsk
