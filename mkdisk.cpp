#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "SuperBlock.cpp"
#include "Inode.h"
#include <cstring>

using namespace std;

void make_disk_file(int numBlocks, int blockSize, string diskFileName);
void initiateDisk(int numBlocks, int blockSize, string diskFileName);
void check();
void updateSuper(int num,int num2);


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
	//check();
	//updateSuper(off,-1);
	//check();
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
	int off,off1;
	fd=fopen(diskFileName.c_str(),"rb+");
	//fd=open(diskFileName.c_str(), O_RDWR | O_CREAT, 0600);
	perror("FIle null");	
	//fseek(fd,0,0);
	SuperBlock s(numBlocks,blockSize,numBlocks,256);
	fwrite(&s, sizeof(s),1,fd);
	//cout<<s.freeB<<endl;
	
	//for(int i=0;i<256;i++) b[i]=1;
	fseek(fd,s.BS,0);
	if(blockSize>=256){
		bool b[256];
		for(int i=0;i<256;i++) b[i]=true;
		fwrite(&b,sizeof(b),1,fd);
		fseek(fd,2*s.BS,0);
		off=2;
	}else{
		bool b[128]={true};
		for(int i=0;i<128;i++) b[i]=true;
		fwrite(&b,sizeof(b),1,fd);
		fseek(fd,2*s.BS,0);
		fwrite(&b,sizeof(b),1,fd);
		fseek(fd,3*s.BS,0);
		off=3;
	}
	//check();
	bool free[blockSize];
	int countB=off;
	for(int i=0;i<blockSize;i++) free[i]=false;
	int start=off+(numBlocks/blockSize)+256;
	cout<<"START="<<start<<endl;
	int count=0;
	if(start<blockSize){
		for(int i=start;i<blockSize;i++) free[i]=true;
		fwrite(&free,sizeof(free),1,fd);
		countB++;
	}
	while(start>blockSize){
		fwrite(&free,sizeof(free),1,fd);
		countB++;
		start-=blockSize;
		cout<<"START="<<start<<endl;
		count+=blockSize;
	}
	for(int i=start;i<blockSize;i++)free[i]=true;
	fwrite(&free,sizeof(free),1,fd);
	countB++;
	count+=blockSize;
	//fseek(fd,countB*s.BS,0);
	
	for(int i=0;i<blockSize;i++) free[i]=true;
	while(count<numBlocks){
		fwrite(&free,sizeof(free),1,fd);
		count+=blockSize;
		countB++;
		fseek(fd,countB*s.BS,0);
	}
	off1=countB;
	fseek(fd,(off1)*s.BS,0);
	for(int i=0;i<256;i++){
		Inode *in=new Inode();
		fwrite(&in,sizeof(in),1,fd);
		off1++;
		fseek(fd,(off1)*s.BS,0);
	}
	fclose(fd);
	//check();
	cout<<"OFF1: "<<off1<<endl;
	updateSuper(off1,off);
	check();
	//check();
}

void updateSuper(int num,int num2){
	FILE *fd;
	//cout<<num<<num2<<endl;
	fd=fopen("DISK","rb");
	char ch[256]="\0";
	fread(ch,128,1,fd);
	SuperBlock s;
	memcpy(&s,&ch,sizeof(SuperBlock));
	//if(num!=-1){
		s.freeB=s.freeB-num;
		cout<<"Free B :"<<s.freeB<<endl;
		s.off=num;
	//}
	//if(num2!=-1){
		s.freeBA=num2;
		s.inode=s.off-256;
	//}
	fclose(fd);
	fd=fopen("DISK","rb+");
	fwrite(&s, sizeof(s),1,fd);
	fclose(fd);

}
void check(){
	FILE *fd;
	fd=fopen("DISK","rb");
	char ch[256]="\0";
	fread(ch,128,1,fd);
	fclose(fd);
	SuperBlock s;	
	memcpy(&s,&ch,sizeof(SuperBlock));
	cout<<s.freeB<<endl;
	cout<<s.off<<endl;
	cout<<s.inode<<endl;
	cout<<s.freeBA<<endl;
	//fclose(fd);
}
