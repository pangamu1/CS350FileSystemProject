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
	void create(std::vector<std::string> args);
	void import(std::vector<std::string> args);
	void cat(std::vector<std::string> args);
	void del(std::vector<std::string> args);
	void wrtie(std::vector<std::string> args);
	void read(std::vector<std::string> args);
	void list();
	void shut_down(std::vector<std::string> args);
	bool inArr[256];
	//std::map <std::string, int> inodeMap;
};

#endif
