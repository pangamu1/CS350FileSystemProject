#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "ssfs.h"
#include "buffer.h"
#include "Inode.h"
#include "SuperBlock.h"

using namespace std;

const int DISKSIZE = 10000000;
const int BLOCKSIZE = 1024;
const int DIRECTBLOCKS = 100;

void Ssfs::create(vector<string> args){
	//do global checks
	Buffer buf;
	SuperBlock sb;
	string file_name = args[1];
	int inode_num;
	Inode *tempinode;
	//open file disk
	FILE*fp = fopen(diskfilename.c_str(), "rb+");
	//go to beginning of inode list... go to beginning of file and move a block
	fseek(fp,sb.BS, SEEK_SET); //seekc_cur move one block size from where you are right now
	//set inode temp to buffer
	tempinode = (*inode)buf;
	//iteratire through inode map to see if there is a file
	for(int i =0; i <256; i++){
		if(strcmp(file_name, tempinode->filename) == 0){
			cerr << "not a unique file" << endl;
		}
		else{
				//find empty inode
			if(inArr[i] == true){
				inode_num = i;
				//fseek(file_name,(s->BS)*(s->offset+i), 0);
				break;
			}
		}
	}

	Inode newnode;
	memset(&newnode,-1, sizeof(Inode));
	strcpy(newnode.filename, filename.c_str());
	//set file length MAYBE

	//update inode which flips bit at that index

	fseek(fp, sb.off +inode_num *sb.BS, SEEK_SET);
	fwrite(*newnode,sizeof(Inode),1,fp);
	fclose(fp);

}
int main(int argc, char *argv[]){
	if(argc > 1){
		Ssfs *fs = new Ssfs(args, DISKSIZE, BLOCKSIZE, DIRECTBLOCKS);
	}
	string cmd;
	vector<string> args;
	string token;

	cout << "sh> ";

	while(getline(cin, cmd)){
		args.clear();
		istringstream ss(cmd);
		while(ss > token){
			args.push_back(token);
		}
		if(args.size() == 0){
			cout << "sh> ";
			continue;
		}
		if(args[0] == "CREATE"){
			fs->create(args);
		}
		else if(args[0] == "IMPORT"){
			fs->import(args);
		}
		else if(args[0] == "CAT"){
			fs->cat(args);
		}
	}
	return 0;
}
