#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

void make_disk_file(int numBlocks, int blockSize, string diskFileName);


int main(int argc, char** argv){
	int numBlks, blkSize;
	string filename;
	if (argc<3){
		fprintf(stderr, "ERROR: Not enough arguments\n");
		exit(0);
	}
	else if(argc>4){
		fprintf(stderr, "ERROR: Too many arguments\n");
		exit(0);	
	} 
	else if(argc==3){
		numBlks = atoi(argv[1]);
		blkSize = atoi(argv[2]);
		filename = "DISK";
	} 
	else{	
		numBlks = atoi(argv[1]);
		blkSize = atoi(argv[2]);
		filename = argv[3];
	}
	make_disk_file(numBlks, blkSize, filename);
	return 0;
}

void make_disk_file(int numBlocks, int blockSize, string diskFileName){
	int myfile = open(diskFileName.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
	if (myfile == -1){
		fprintf(stderr, "Opening file failed\n");
		exit(0);
	}	 	
	ftruncate(myfile, numBlocks*blockSize);
	close(myfile);
}
