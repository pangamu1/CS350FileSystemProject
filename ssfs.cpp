#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "SuperBlock.cpp"
#include "Inode.h"
#include <map>

using namespace std;

int main(){
	FILE *fd;
	fd=fopen("DISK","rb");
	char ch[256];
	fread(ch,128,1,fd);
	SuperBlock *s;
	s=(SuperBlock *) ch;
	fseek(fd,s->BS,0);
	fread(ch,128,1,fd);
	bool inArr[256];
	bool freeB[s->numB];
	if(s->BS<256){
		for(int i=0;i<s->BS;i++){
			inArr[i]=(bool) ch[i];
		}
		for(int i=s->BS;i<256;i++){
			inArr[i]=(bool) ch[i];
		}
	}else{
		for(int i=0;i<s->BS;i++){
			inArr[i]=(bool) ch[i];
		}
	}
	fseek(fd,s->inode,0);
	int count=0;
	int countB=s->inode;
	while(count<s->numB){
		fread(ch,s->BS,1,fd);
		for(int i=0;i<s->BS;i++){
			freeB[count]=(bool)ch[i];
			count++;
		}
		countB++;
		fseek(fd,(s->BS)*countB,0);
	}
	map <string, int> inodeMap;
	char in[s->BS];
	for(int i=0;i<256;i++){
		if(!inArr[i]){
			fseek(fd,(s->BS)*(s->off+i),0);
			fread(in,s->BS,1,fd);
			Inode *n;
			n=(Inode*) in;
			inodeMap[n->filename]=i;
		}
	}
	
}
