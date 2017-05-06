#ifndef _SSFS_H_
#define _SSFS_H_

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>


class ssfs{
public:
	ssfs(std::vector<std::string> args, int file_size, int block_size, int direct_blocks);
	map <string, int> inodeMap;
	map <string, int>::iterator it;
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

#endif
