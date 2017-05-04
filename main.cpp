#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "ssfs.h"
#include "buffer.h"

using namespace std;

const int DISKSIZE = 10000000;
const int BLOCKSIZE = 1024;
const int DIRECTBLOCKS = 100;

void Ssfs::create(vector<string> args){
	cout << "yup" << endl;
	//do global checks
	//if(free_list == 0){
	//	return;
	//}

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
			buffer buf;
			buf.cmd = 0;
			buf.filename = args[1];
			
		}
	}
}

int main(int argc, char *argv[]){
	/*Ssfs(vector<string> args, int file_size, int block_size, int direct_blocks);
	if(argc > 1){
		Ssfs *fs = new Ssfs(argv[1], DISKSIZE, BLOCKSIZE, DIRECTBLOCKS);
	}*/
	string cmd;
	vector<string> args;
	string token;

	cout << "sh> ";

	while(getline(cin, cmd)){
		args.clear();
		istringstream ss(cmd);
		while(ss >> token){
			args.push_back(token);
		}
		if(args.size() == 0){
			cout << "sh> ";
			continue;
		}
		if(args[0] == "CREATE"){
			//fs->create(args);
			if (args.size() != 2) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "IMPORT"){
			//fs->import(args);
			if (args.size() != 3) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "CAT"){
			//fs->cat(args);
			if (args.size() != 2) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "DELETE"){
			if (args.size() != 2) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
			
		}
		else if(args[0] == "WRITE"){
			if (args.size() != 5) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "READ"){
			if (args.size() != 4) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
		}
		else if(args[0] == "LIST"){
			
		}
		else if(args[0] == "SHUTDOWN"){
			
		}
	}
	return 0;
}
