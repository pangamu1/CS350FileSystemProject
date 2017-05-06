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

void ssfs::read(vector<string> args, SuperBlock sb){
	cout << "READ FUNCTION" << endl;	
	//Get arguments
	string filename = args[1];
	int start_byte = atoi(args[2].c_str());
	int num_bytes = atoi(args[3].c_str());
	int inode_pos = inodeMap[filename];
	//cout << filename << "\t" << start_byte << "\t" << num_bytes << "\t" << inode_pos << endl;
	//open file
	FILE*fp = fopen("DISK", "rb+");
	//seek to where inode is, inode pos from inode map	
	fseek(fp,sb.BS*(inode_pos+sb.inode),0);
	//buffer for reading inode data	
	char ch4[sb.BS];	
	//read in inode data	
	fread(ch4,sb.BS,1,fp);
	//create new inode	
	Inode newnode;
	//copy data into inode variable	
	memcpy(&newnode,&ch4,sizeof(newnode));
	//checking if i got the inode	
	//cout << newnode.filename << "\t" << newnode.file_size << endl;
	//creating buffer for data block to be read into
	char block_buff[sb.BS];
	start_byte = start_byte + (sb.BS*newnode.direct_pointer[0]);
	fseek(fp,start_byte,0);
	//iterate through inodes	
	while(num_bytes > 0){
		//seek to datablock, hope my math is right
		//read block into buffer
		if (num_bytes > sb.BS){
			fread(block_buff,sb.BS,1,fp);			
			
			for (int j=0; j<sb.BS; j++){
				cout << block_buff[j];
			}
			cout << endl;
			num_bytes -=sb.BS;
		}
		else{
			fread(block_buff,num_bytes,1,fp);
			//cout<<num_bytes<<endl;
			for (int j=0; j<num_bytes; j++){
				cout << block_buff[j];
			}
			cout << endl;
			num_bytes -= sb.BS;
		}
		//prints each block. one per line.
		
		
	}
	fclose(fp);
}

//Cat and read are pretty much the same thing rip
//DID not account for indirect and double indirect pointers yet
void ssfs::cat(vector<string> args, SuperBlock sb){
	cout << "CAT FUNCTION" << endl;	
	//Get arguments
	string filename = args[1];
	int inode_pos = inodeMap[filename];
	//open file
	FILE*fp = fopen("DISK", "rb+");
	//seek to where inode is, inode pos from inode map	
	fseek(fp,sb.BS*(inode_pos+sb.inode),0);
	//buffer for reading inode data	
	char ch4[sb.BS];	
	//read in inode data	
	fread(ch4,sb.BS,1,fp);
	//create new inode	
	Inode newnode;
	//copy data into inode variable	
	memcpy(&newnode,&ch4,sizeof(newnode));
	//checking if i got the inode	
	//cout << newnode.filename << "\t" << newnode.file_size << endl;
	//creating buffer for data block to be read into
	char block_buff[sb.BS];
	//iterate through inodes	
	for(int i=0; i<12; i++){
		if (newnode.direct_pointer[i] != -1){
			//seek to datablock, hope my math is right
			fseek(fp,sb.BS*(newnode.direct_pointer[i]),0);
			//read block into buffer
			fread(block_buff,sb.BS,1,fp);
			//prints each block. one per line.
			for (int j=0; j<sb.BS; j++){
				cout << block_buff[j];
			}
			cout << endl;
		}
	}
	fclose(fp);
}

void ssfs::del(vector <string> args, SuperBlock sb) {
	string file_name = args[1];
	//char ch[sb.BS] = "\0";
	FILE *fp;
	fp = fopen("DISK", "rb+");
	Inode tempinode;
	fseek(fp, sb.BS, SEEK_SET);
	int i = 0;
	char ch[sb.BS];
	for (i; i < 256; i++) {
		fseek(fp, sb.BS*(i+sb.inode), 0);
		fread(ch, sb.BS, 1, fp);
		memcpy(&tempinode, &ch, sizeof(tempinode));
		if (strcmp(file_name.c_str(), tempinode.filename) == 0) {
			inArr[i] = true;
			inodeMap.erase(file_name);
			break;
		}
	}
	fseek(fp,sb.BS*(i+sb.inode),0);
	fread(ch,sb.BS,1,fp);
	Inode blanknode;
	memcpy(&blanknode,&ch,sizeof(blanknode));
	//memset(newnode,-1, sizeof(Inode));
	strcpy(blanknode.filename, "                    ");
	//cout << blanknode.filename << endl;
	blanknode.file_size = 0;
	char cha='\0';
	for (int j=0;j<12;j++){
		if(blanknode.direct_pointer[j]!=-1){
			fseek(fp,sb.BS*blanknode.direct_pointer[j],0);
			for(int z=0;z<sb.BS;z++){
				fwrite(&cha,1,1,fp);
			}
			blanknode.direct_pointer[j]=-1;
		}
	}
	fseek(fp,sb.BS*(i+sb.inode),0);
	fwrite(&blanknode,sb.BS,1,fp);
	//cout<<"Hi"<<endl;
	//cout<<newnode.filename<<endl;
	//set file length MAYBE

	//update inode which flips bit at that index

	//fseek(fp, sb.off +inode_num *sb.BS, SEEK_SET);
	fclose(fp);
}

void ssfs::write(std::vector<std::string> args,SuperBlock sb,bool *freeB){
	FILE *fp;
	Inode in1;
	char ch[sb.BS];
	string filename=args[1];
	char c=args[2][0];
	int start=atoi(args[3].c_str());
	int num=atoi(args[4].c_str());
	int in;
	it=inodeMap.find(filename);
	cout<<inodeMap[filename]<<endl;
	//cout<<inodeMap.end()<<endl;
	if(inodeMap.find(filename)!=inodeMap.end()){
		in =inodeMap[filename];
	}else{
		perror("Filename does not exist");
		exit(0);
	}
	fp=fopen("DISK","rb+");
	fseek(fp,sb.BS*(sb.inode+in),0);
	fread(ch,sb.BS,1,fp);
	memcpy(&in1,&ch,sizeof(in1));
	//cout<<"HI"<<endl;
	cout<<in1.filename<<endl;
	if(start>in1.file_size){
		perror("Start Byte is too big");
		exit(0);
	}
	int directS=start/sb.BS;
	int directE=(start+num)/sb.BS;
	int numb=num;
	int start2;
	if(in1.file_size==0){
		if(directE<12){
			for(int i=0;i<=directE;i++){
				for(int x=0;x<sb.numB;x++){
					if(freeB[x]==true){
						freeB[x]=false;
						//cout<<"X="<<x<<endl;
						fseek(fp,sb.BS*(x),0);
						in1.direct_pointer[i]=x;
						x=sb.numB;
						if(numb>sb.BS){
							//cout<<"WRITING HERE"<<endl;
							for(int y=0;y<sb.BS;y++){
								fwrite(&c,sizeof(c),1,fp);
							}
						numb-=sb.BS;
							
						}
						else{
							//cout<<"WRITING HERE 2 "<<c<<" num :"<<numb<<endl;
							for(int y=0;y<numb;y++){
								fwrite(&c,sizeof(c),1,fp);
							}
						}

					}
				}

			}

		}
	}
	else if(in1.direct_pointer[directE]!=-1){
		for(int x=directS;x<=directE;x++){
			if(x==directS){
				//fseek(fp,sb.BS*(in1.direct_pointer[x]),0);
				cout<<"HI"<<endl;
				start2=start%sb.BS;
				fseek(fp,start2+sb.BS*(in1.direct_pointer[x]),0);
				if(numb>=sb.BS-start2){
					for(int y=0;y<sb.BS-start2;y++){
						fwrite(&c,1,1,fp);
					}
					numb-=(sb.BS-start2);
					}
				else{
					for(int y=0;y<numb;y++){
						fwrite(&c,1,1,fp);
					}
				}
				
			}else{
				fseek(fp,sb.BS*(in1.direct_pointer[x]),0);
				if(numb>=sb.BS){
					for(int y=0;y<sb.BS;y++){
						fwrite(&c,1,1,fp);
					}
					numb-=sb.BS;
					}
				else{
					for(int y=0;y<numb;y++){
						fwrite(&c,1,1,fp);
					}
				}
			}
			
		}
	}
	else{
		for(int x=directS;x<=directE;x++){
			if(in1.direct_pointer[x]!=-1&&x==directS){
				//fseek(fp,sb.BS*(in1.direct_pointer[x]),0);
				start2=start%sb.BS;
				fseek(fp,start2+sb.BS*(in1.direct_pointer[x]),0);
				if(numb>=sb.BS-start2){
					for(int y=0;y<sb.BS-start2;y++){
						fwrite(&c,1,1,fp);
					}
					numb-=(sb.BS-start2);
					}
				else{
					for(int y=0;y<numb;y++){
						fwrite(&c,1,1,fp);
					}
				}

			}
			else if(in1.direct_pointer[x]!=-1){
				fseek(fp,sb.BS*(in1.direct_pointer[x]),0);
				if(numb>=sb.BS){
					for(int y=0;y<sb.BS;y++){
						fwrite(&c,1,1,fp);
					}
					numb-=sb.BS;
					}
				else{
					for(int y=0;y<numb;y++){
						fwrite(&c,1,1,fp);
					}
				}
			}
			else{
				for(int i=0;i<sb.numB;i++){
					if(freeB[i]==true){
						freeB[i]=false;
						fseek(fp,sb.BS*(i),0);
						in1.direct_pointer[x]=i;
						i=sb.numB;
						if(numb>sb.BS){
							for(int y=0;y<sb.BS;y++){
								fwrite(&c,1,1,fp);
							}
							numb-=sb.BS;
						}
						else{
							for(int y=0;y<numb;y++){
								fwrite(&c,1,numb,fp);
							}
						}

					}
				}
				}

			}
		}
		in1.file_size=num;
		//cout<<in1.direct_pointer[0]<<endl;
		fseek(fp,in1.direct_pointer[0]*(sb.BS),0);
		char ch1;
		//for(int i=0;i<100;i++){
		//fread(&ch1,1,1,fp);
		//cout<<ch1<<endl;
		//}
		//string s;
		//s=ch1;
		//memcpy(&s,&ch,sizeof(ch));
		//cout<<s<<endl;
		fseek(fp,sb.BS*(sb.inode+in),0);
		fwrite(&in1,sizeof(in1),1,fp);
		fclose(fp);
		//return freeB;
	}
	



void ssfs::create(vector<string> args,SuperBlock sb){
	//do global checks
	//buffer *buf;
	//SuperBlock sb;
	char ch[sb.BS];
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
		if(inArr[i]==false){
			fseek(fp,sb.BS*(i+sb.inode),0);
			fread(ch,sb.BS,1,fp);
			memcpy(&tempinode,&ch,sizeof(tempinode));
		
			if(strcmp(file_name.c_str(), tempinode.filename) == 0){
				cerr << "not a unique file" << endl;
				exit(0);
			}
		}
	}
		for(int i =0; i <256; i++){
				//find empty inode
			if(inArr[i] == true){
				inode_num = i;
				inArr[i] = false;
				//it=inodeMap.find(i);
				//inodeMap.erase(inodeMap.find(i));
				inodeMap[file_name]=i;
				//sb.inode++;
				//fseek(file_name,(s->BS)*(s->offset+i), 0);
				break;
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
	for (int i=0; i<12; i++){
		newnode.direct_pointer[i] = -1;
		//cout << newnode.direct_pointer[i] << " ";
	}
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
	bool *freeB=new bool[s.numB];
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
	cout<<"Free BA:"<<s.freeBA<<endl;
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
	cout<<freeB[136]<<endl;
	
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
			//cout << "sh> ";
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
			fs->cat(args,s);
		}
		else if(args[0] == "DELETE"){
			if (args.size() < 2) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
			fs->del(args,s);
		}
		else if(args[0] == "WRITE"){
			if (args.size() < 5) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
			else fs->write(args,s,freeB);
		}
		else if(args[0] == "READ"){
			if (args.size() < 4) {
				cout << "Incorrect number of arguments. " << endl;
				continue;
			}
			fs->read(args, s);
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
