#ifndef _INODE_H
#define _INODE_H

class Inode{
	public:
		char filename[32];
		int file_size;
		int direct_pointer[12];
		int indirect=-1;
		int double_indirect;		
		bool open=true;
};


#endif
