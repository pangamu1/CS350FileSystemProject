FLAGS = -Wall -std=c++11 -g

all: ssfs_mkdsk ssfs

ssfs_mkdsk:	mkdisk.o SuperBlock.o
	g++ $(FLAGS) mkdisk.o -o ssfs_mkdsk

mkdisk.o:	mkdisk.cpp Inode.h
	g++ $(FLAGS) -c mkdisk.cpp  -o mkdisk.o

SuperBlock.o: SuperBlock.cpp SuperBlock.h
	g++ $(FLAGS) -c SuperBlock.cpp  -o SuperBlock.o

ssfs: ssfs.o SuperBlock.o ssfs.h
	g++ $(FLAGS) ssfs.o -o ssfs

ssfs.o:	ssfs.cpp Inode.h
	g++ $(FLAGS) -c ssfs.cpp  -o ssfs.o

clean:
	rm -f *.o ssfs_mkdsk ssfs DISK
