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
#include <cstring>

using namespace std;

int main(){
	FILE *fd;
	fd=fopen("DISK","rb");
	char ch[128]="\0";
	fread(ch,128,1,fd);
	SuperBlock s;
	memcpy(&s,&ch,sizeof(SuperBlock));
	/*cout<<s.BS<<endl;
	cout<<s.freeB<<endl;
	cout<<s.off<<endl;
	cout<<s.inode<<endl;
	cout<<s.freeBA<<endl;*/
	fseek(fd,s.BS,0);
	char ch1[s.BS]="\0";
	//cout<<s.BS<<endl;
	bool inArr[256]={true};
	//cout<<s.BS<<endl;
	bool temp[128];
	bool temp2[128];
	bool freeB[s.numB];
	cout<<s.BS<<endl;
	if(s.BS<256){
		fread(ch1,s.BS,1,fd);
		memcpy(&temp,&ch1,sizeof(ch1));
		fseek(fd,2*s.BS,0);
		fread(ch1,s.BS,1,fd);
		memcpy(&temp2,&ch1,sizeof(ch1));
		for(int i=0;i<256;i++){
			if(1<128) inArr[i]=temp[i];
			else inArr[i]=temp2[i-128];
		}
	}else{
		fread(ch1,s.BS,1,fd);
		//cout<<ch1[0]<<endl;
		memcpy(&inArr,&ch1,sizeof(ch1));
	}
	cout<<inArr[100]<<endl;
	fseek(fd,((s.BS)*(s.freeBA)),0);
	int count=0;
	int countB=s.freeBA;
	bool temp3[s.BS];
	while(count<s.numB){
		fread(ch1,s.BS,1,fd);
		memcpy(&temp3,&ch1,sizeof(ch1));
		for (int i=0;i<s.BS;i++) {
			freeB[count]=temp3[i];
			count++;
		}
		countB++;
		fseek(fd,(s.BS)*countB,0);
	}
	cout<<freeB[0]<<endl;
	map <string, int> inodeMap;
	char in[s.BS];
	for(int i=0;i<256;i++){
		if(!inArr[i]){
			fseek(fd,(s.BS)*(s.inode+i),0);
			fread(in,s.BS,1,fd);
			Inode n;
			memcpy(&n,&in,sizeof(in));
			inodeMap[n.filename]=i;
			//cout<<"hi"<<endl;
		}
	}
	fclose(fd);
}
