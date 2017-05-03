#include <string>


class SuperBlock{
	public:
		int numB;
		int BS;
		int freeB;
		int freeIn;
		int off;
		int inode;
		int freeBA;
		SuperBlock(int numB1,int BS1,int freeB1, int freeIn1);
		/*SuperBlock(int numB1,int BS1,int freeB1, int freeIn1){
		numB=numB1;
		BS=BS1;
		freeB=freeB1;
		freeIn=freeIn1;

	}*/
};

