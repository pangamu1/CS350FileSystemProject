#ifndef _SSFS_H_
#define _SSFS_H_

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>

class Ssfs{
public:
	Ssfs(std::vector<std::string> args, int file_size, int block_size, int direct_blocks);
	void create(std::vector<std::string> args);
	void import(std::vector<std::string> args);
	void cat(std::vector<std::string> args);
	void del(std::vector<std::string> args);
	void wrtie(std::vector<std::string> args);
	void read(std::vector<std::string> args);
	void list(std::vector<std::string> args);
	void shut_down(std::vector<std::string> args);
};

#endif