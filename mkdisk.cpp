#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "SuperBlock.cpp"
#include "Inode.h"

using namespace std;

void make_disk_file(int numBlocks, int blockSize, string diskFileName);
void initiateDisk(int numBlocks, int blockSize, string diskFileName);
void check();


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
	initiateDisk(numBlks,blkSize,filename);
	check();
	return 0;
}

void make_disk_file(int numBlocks, int blockSize, string diskFileName){
	//int myfile = open(diskFileName.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
	int myfile = open(diskFileName.c_str(), O_RDWR | O_CREAT, 0600);
	if (myfile == -1){
		fprintf(stderr, "Opening file failed\n");
		exit(0);
	}	 	
	if (ftruncate(myfile, numBlocks*blockSize) == -1){
		fprintf(stderr, "Truncate failed");
		close(myfile);
		exit(0);
	}
	
	close(myfile);
}

void initiateDisk(int numBlocks, int blockSize, string diskFileName){
	FILE *fd;
	int off;
	fd=fopen(diskFileName.c_str(),"wb");
	perror("FIle null");	
	//fseek(fd,0,0);
	SuperBlock s(numBlocks,blockSize,numBlocks,256);
	fwrite(&s, sizeof(s),1,fd);
	bool b[256];
	for(int i=0;i<256;i++) b[i]=true;
	fseek(fd,s.BS,0);
	if(sizeof(b)<blockSize){
		fwrite(&b,sizeof(b),1,fd);
		fseek(fd,3*s.BS,0);
		off=3;
	}else{
		for(int i=0;i<blockSize;i++){
			fwrite(&b[i],1,1,fd);
		}
		for(int i=blockSize;i<sizeof(b);i++){
			fwrite(&b[i],1,1,fd);
		}
		fseek(fd,4*s.BS,0);
		off=4;
	}
	bool free[numBlocks];
	for(int i=0;i<numBlocks;i++) free[i]=true;
	free[1]=false;
	int count=0;
	int countB=0;
	while(count<sizeof(free)){
		for(int i=countB*blockSize;i<(countB+1)*blockSize;i++){
			fwrite(&free[i],1,1,fd);
			count++;
		}
			countB++;
	}
	off+=countB;
	fseek(fd,(off)*s.BS,0);
	for(int i=0;i<256;i++){
		Inode *in=new Inode();
		fwrite(&in,sizeof(in),1,fd);
		off++;
	}
	fclose(fd);

}

void check(){
	FILE *fd;
	fd=fopen("DISK","rb");
	char ch[256];
	fread(ch,128,1,fd);
	SuperBlock *s;
	s=(SuperBlock *) ch;
	cout<<s->numB<<endl;
}
