#ifndef _SSFS_H_
#define _SSFS_H_

#include <fstream>
//#include <list>
#include <map>
#include <string>
#include <vector>
#include <pthread.h>
#include <semaphore.h>

using namespace std;
pthread_t tid, tid1;
pthread_attr_t attr;
sem_t full, empty;
pthread_mutex_t mu;

vector <vector<string>> buffer(1);

class ssfs{
public:
	ssfs(std::vector<std::string> args, int file_size, int block_size, int direct_blocks);
	map <string, int> inodeMap;
	map <string, int>::iterator it;
	int file_size, block_size, direct_blocks;
	void create(std::vector<std::string> args,SuperBlock sb);
	void import(std::vector<std::string> args,SuperBlock sb, bool*freeB);
	void cat(std::vector<std::string> args,SuperBlock sb);
	void del(std::vector<std::string> args,SuperBlock sb,bool *freeB);
	void write(std::vector<std::string> args,SuperBlock sb,bool *freeB);
	void read(std::vector<std::string> args,SuperBlock sb);
	void list(SuperBlock sb);
	void shut_down(std::vector<std::string> args);
	bool inArr[256];
	//bool freeB[s.numB];
	//std::map <std::string, int> inodeMap;
};

struct argies{
	SuperBlock s;
	bool *freeB;
	string filename;
	ssfs *fs;
	argies(){}
	argies(SuperBlock sb, bool *f, string fn, ssfs *sfs){
		s = sb;
		freeB = f;	
		filename = fn;
		fs = sfs;
	}
	argies(argies *argy) {
		s = argy->s;
		freeB = argy->freeB;
		filename = argy->filename;
		fs = argy->fs;
	}
};
#endif
