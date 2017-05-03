#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "ssfs.h"

using namespace std;

const int DISKSIZE = 10000000;
const int BLOCKSIZE = 1024;
const int DIRECTBLOCKS = 100;

void Ssfs::create(vector<string> args){
	//do global checks
	if(free_list == 0){
		return;
	}

	//check if file already exists

	//allocate new inode : which group?

	//pick inode

	//update global inode numbers
}

void Ssfs::import(vector<string> args){

}

void Ssfs::cat(vector<string> args){
	if(args.size() < 2){
		cerr << args[0] << ": missing operand" <<endl;
	}
	else{
		for(int i = 1; i<args.size(); i++){
			Buffer buf;

			buf.cmd = 0;
			buf.filename = args[1];
			
		}
	}
}

int main(int argc, char *argv[]){
sfs(std::vector<std::string> args, int file_size, int block_size, int direct_blocks);
	if(argc > 1){
		Ssfs *fs = new Ssfs(argv[1], DISKSIZE, BLOCKSIZE, DIRECTBLOCKS);
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