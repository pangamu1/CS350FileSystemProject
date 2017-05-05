#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "SuperBlock.cpp"
#include "Inode.h"
#include <map>
//#include <string>
#include <cstring>
#include <vector>
#include "buffer.h"
#include "ssfs.h"
//#include "Inode.cpp"

const int DISKSIZE = 4096;
const int BLOCKSIZE = 128;
const int DIRECTBLOCKS = 12;

using namespace std;

ssfs::ssfs(std::vector<std::string> args, int file_size, int block_size, int direct_blocks){
	
	file_size = 0;
	block_size = BLOCKSIZE;
	direct_blocks = DIRECTBLOCKS;
}

void ssfs::list(SuperBlock sb){
	Inode node;
	char name[32];
	char ch[sb.BS];
	int size;
	FILE *fp;
	fp=fopen("DISK","rb+");
	for(int i =0; i<256; i++){
		//cout << inArr[i] << endl;
		if(!inArr[i]){
			//cout << "hi " <<endl;
			fseek(fp,(sb.BS)*(i+sb.inode),0);
			fread(ch,sizeof(ch),1,fp);
			memcpy(&node,&ch,sizeof(node));
			cout<<"File Name: "<<node.filename<<" Size: "<<node.file_size<<endl;
			
		}
	}
	cout << "hello" << endl;
}

void write(std::vector<std::string> args){
	string filename=argv[1];
	char c=argv[2];
	int start=atoi(argv[3]);
	int num=atoi(argv[4]);
	


}

void ssfs::create(vector<string> args,SuperBlock sb){
	//do global checks
	//buffer *buf;
	//SuperBlock sb;
	string file_name = args[1];
	int inode_num;
	Inode tempinode;
	//open file disk
	FILE*fp = fopen("DISK", "rb+");
	//go to beginning of inode list... go to beginning of file and move a block
	fseek(fp,sb.BS, SEEK_SET); //seekc_cur move one block size from where you are right now
	//memcpy(tempinode, buf, sb.BS);
	//set inode temp to buffer
	//tempinode = buf;
	//iterate through inode map to see if there is a file
	for(int i =0; i <256; i++){
		if(strcmp(file_name.c_str(), tempinode.filename) == 0){
			cerr << "not a unique file" << endl;
		}
		else{
				//find empty inode
			if(inArr[i] == true){
				inode_num = i;
				inArr[i] = false;
				//it=inodeMap.find(i);
				//inodeMap.erase(inodeMap.find(i));
				inodeMap[tempinode.filename]=i;
				//sb.inode++;
				//fseek(file_name,(s->BS)*(s->offset+i), 0);
				break;
			}
		}
	}
	char ch4[sb.BS];
	fseek(fp,sb.BS*(inode_num+sb.inode),0);
	fread(ch4,sb.BS,1,fp);
	Inode newnode;
	memcpy(&newnode,&ch4,sizeof(newnode));
	//memset(newnode,-1, sizeof(Inode));
	strcpy(newnode.filename, file_name.c_str());
	//newnode->filename=file_name;
	newnode.file_size = 0;
	cout<<"Hi"<<endl;
	cout<<newnode.filename<<endl;
	//set file length MAYBE

	//update inode which flips bit at that index

	//fseek(fp, sb.off +inode_num *sb.BS, SEEK_SET);
	fseek(fp,sb.BS*(inode_num+sb.inode),0);
	fwrite(&newnode,sizeof(newnode),1,fp);
	fclose(fp);
	fp=fopen("DISK","rb+");
	Inode ret;
	fseek(fp, (sb.inode +inode_num) *sb.BS, 0);
	char ch2[sb.BS];
	fread(ch2, sb.BS, 1, fp);
	memcpy(&ret,&ch2,sizeof(ret));
	cout << ret.filename << endl;
	//cout << newnode->filename <<endl;
	//cout << fread(newnode,sizeof(Inode),1,fp) <<endl;
	fclose(fp);
	cout << "hi " <<endl;
	//free(tempinode);
	//exit(1);
	
}


int main(int argc, char * argv[]){
	FILE *fd;
	fd=fopen("DISK","rb+");
	char ch[128]="\0";
	fread(ch,128,1,fd);
	SuperBlock s;
	memcpy(&s,&ch,sizeof(SuperBlock));

	vector<string> args;
	string cmd;

	string token;

	//if(argc > 1){
		ssfs *fs = new ssfs(args, DISKSIZE, BLOCKSIZE, DIRECTBLOCKS);
	//}


	/*cout<<s.BS<<endl;
	cout<<s.freeB<<endl;
	cout<<s.off<<endl;
	cout<<s.inode<<endl;
	cout<<s.freeBA<<endl;*/
	fseek(fd,s.BS,0);
	char ch1[s.BS]="\0";
	//cout<<s.BS<<endl;
	fs->inArr[256]={true};
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
			if(1<128) fs->inArr[i]=temp[i];
			else fs->inArr[i]=temp2[i-128];
		}
	}else{
		fread(ch1,s.BS,1,fd);
		//cout<<ch1[0]<<endl;
		memcpy(&(fs->inArr),&ch1,sizeof(ch1));
	}
	cout<<fs->inArr[100]<<endl;
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
	
	char in[s.BS];
	for(int i=0;i<256;i++){
		if(!fs->inArr[i]){
			fseek(fd,(s.BS)*(s.inode+i),0);
			fread(in,s.BS,1,fd);
			Inode n;
			memcpy(&n,&in,sizeof(in));
			fs->inodeMap[n.filename]=i;
			//cout<<"hi"<<endl;
		}
	}

	ifstream cmd_file ("input.txt");

	while(getline(cmd_file, cmd,'\n')){
		args.clear();
		istringstream ss(cmd);
		while(ss >> token){
			cout << "sh> ";
			args.push_back(token);
		}
		//args.push_back(cmd);
		if(args.size() == 0){
			//cout << "sh> ";
			continue;
		}
		if(args[0] == "CREATE"){
			fs->create(args,s);
			cout << "hiii" <<endl;
			if (args.size() < 2) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "IMPORT"){
			//fs->import(args);
			if (args.size() < 3) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "CAT"){
			//fs->cat(args);
			if (args.size() < 2) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "DELETE"){
			if (args.size() < 2) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
			
		}
		else if(args[0] == "WRITE"){
			if (args.size() < 5) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "READ"){
			if (args.size() < 4) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "LIST"){
			fs->list(s);

			
		}
		else if(args[0] == "SHUTDOWN"){
			
		}
	}
/*
	while(getline(cmd_file, cmd)){
		args.clear();
		istringstream ss(cmd);
		args.push_back(cmd);
		cout << args[0] <<endl;
		
	}*/

	fclose(fd);
}
