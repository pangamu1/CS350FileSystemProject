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
#include <cstring>
#include <vector>
#include "buffer.h"
#include "ssfs.h"
//#include "Inode.cpp"

//const int DISKSIZE = 4096;
//const int BLOCKSIZE = 128;
//const int DIRECTBLOCKS = 12;

using namespace std;

ssfs::ssfs(std::vector<std::string> args, int file_size1, int block_size1, int direct_blocks1){
	
	file_size = file_size1;
	block_size = block_size1;
	direct_blocks = direct_blocks1;
}

void ssfs::list(SuperBlock sb){
	Inode node;
	//char name[32];
	char ch[sb.BS];
	//int size;
	FILE *fp;
	fp=fopen("DISK","rb+");
	for(int i =0; i<256; i++){
		//cout << inArr[i] << endl;
		if(!inArr[i]){
			//cout << "hi " <<i<<" "<<sb.inode<<endl;
			fseek(fp,(sb.BS)*(i+sb.inode),0);
			fread(ch,sizeof(ch),1,fp);
			memcpy(&node,&ch,sizeof(node));
			cout<<"File Name: "<<node.filename<<" Size: "<<node.file_size<<endl;
			
		}
	}
	//cout << "hello" << endl;
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
	FILE *fp;
	fp = fopen("DISK", "rb+");
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
	int indir[sb.BS/4];
	int startI=start_byte%sb.BS;
	int startB=start_byte / sb.BS;
	int endB=(start_byte+num_bytes)/sb.BS;
	int tempNum=num_bytes;
	int arr[endB-startB+1];
	//cout<<num_bytes/sb.BS<<endl;
	//cout<<endB<<" "<<startB<<endl;
	//start_byte = startB + (sb.BS*newnode.direct_pointer[startI]);
	//fseek(fp,start_byte,0);
	int count=0;
	for(int i=startB;i<12;i++){
		arr[count]=newnode.direct_pointer[i];
		count++;
	}
	if(newnode.indirect!=-1){
		//cout<<"IND "<<newnode.indirect<<endl;
		fseek(fp,sb.BS*newnode.indirect,0);
		fread(block_buff,sb.BS,1,fp);
		memcpy(&indir,&block_buff,sizeof(indir));
		//cout<<indir[0]<<endl;
		int count2=0;
		for(int i=count;i<(endB-startB+1);i++){
			arr[count]=indir[count2];
			count++;
			count2++;
		}
	}
	//iterate through inodes	
	for(int i=0;i<endB-startB;i++){
		//seek to datablock, hope my math is right
		//read block into buffer
		fseek(fp,(startI+(sb.BS*(arr[i]))),0);
		//cout<<<<endl;
		if (tempNum > sb.BS){
			fread(block_buff,sb.BS-startI,1,fp);			
			
			for (int j=0; j<sb.BS; j++){
				cout << block_buff[j];
			}
			cout << endl;
			tempNum -=(sb.BS-startB);
		}
		else{
			fread(block_buff,tempNum,1,fp);
			//cout<<num_bytes<<endl;
			for (int j=0; j<tempNum; j++){
				cout << block_buff[j];
			}
			cout << endl;
			tempNum -= sb.BS;
		}
		//cout<<"WHY"<<endl;
		startI=0;
		//prints each block. one per line.
		
		
	}
	fclose(fp);
}

//Cat and read are pretty much the same thing rip
//DID not account for double indirect pointers yet
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
	int indir[sb.BS/4];
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
	if(newnode.indirect!=-1){
		fseek(fp,sb.BS*newnode.indirect,0);
		fread(block_buff,sb.BS,1,fp);
		memcpy(&indir,&block_buff,sizeof(indir));
		//cout<<indir[0]<<endl;
		int num=newnode.file_size/sb.BS-12;
		for(int i=0;i<=num;i++){
			fseek(fp,sb.BS*indir[i],0);
			fread(block_buff,sb.BS,1,fp);
			for (int j=0; j<sb.BS; j++){
				cout << block_buff[j];
			}
			cout<<endl;
		}
		
	}
	fclose(fp);
}

void ssfs::del(vector <string> args, SuperBlock sb,bool *freeB) {
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
			sb.freeIn++;
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
			freeB[blanknode.direct_pointer[j]]=true;
			sb.freeB++;
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
	fseek(fp,0,0);
	fwrite(&sb,sizeof(sb),1,fp);
	fclose(fp);
}

void ssfs::import(vector<string> args, SuperBlock sb, bool *freeB){
	//it = inodeMap.find(ssfs_filename);
	int inode_pos;
	string ssfs_filename = args[1];
	string unix_filename = args[2];
	bool file_not_exist = false;
	if (inodeMap.find(ssfs_filename) != inodeMap.end()){
		inode_pos = inodeMap[ssfs_filename];
	}
	else{
		vector<string> create_args(2);
		create_args[0] = "CREATE";
		create_args[1] = unix_filename;
		cout << "FILE DOES NOT EXIST. CREATING NEW FILE" << unix_filename << endl;
		create(create_args, sb);
	}
	ifstream unix_fs(unix_filename);
	string line;
	vector<string> write_args(5);
	write_args[0] = "WRITE";
	if (file_not_exist) write_args[1] = (unix_filename); 
	else write_args[1] = (ssfs_filename);
	int counter = 0;
	while(getline(unix_fs, line)){
		for(int i=0; i<line.length(); i++){
			write_args[2] = line[i];
			
			write_args[3] = to_string(counter);
			counter++;
			write_args[4] ="1";
			//cout<<write_args[1]<<write_args[2]<<write_args[3]<<write_args[4]<<endl;
			write(write_args, sb, freeB);
		}
	}

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
	//cout<<inodeMap[filename]<<endl;
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
	//cout<<in1.filename<<endl;
	if(start>in1.file_size){
		perror("Start Byte is too big");
		exit(0);
	}
	//cout<<"indirect"<<in1.indirect<<endl;
	int indir[sb.BS/4];
	int directS=start/sb.BS;
	int directE=(start+num)/sb.BS;
	int numb=num;
	int start2;
	int blocks[(directE-directS)+1];
	int bc=0;
	if(directE>=12){
		for (int i=directS;i<12;i++){
			if(in1.direct_pointer[i]==-1){
					for(int x=0;x<sb.numB;x++){
						if(freeB[x]==true){
							freeB[x]=false;
							sb.freeB--;
							//cout<<"X="<<x<<endl;
							//fseek(fp,sb.BS*(x),0);
							in1.direct_pointer[i]=x;
							x=sb.numB;
					}else{
						if(x==sb.numB-1){
							cerr<<"No More free data blocks"<<endl;
							return;
						}
					}
				}
			}
			blocks[bc]=in1.direct_pointer[i];
			bc++;
		}
		if(in1.indirect==0){
			for(int x=0;x<sb.numB;x++){
				if(freeB[x]==true){
					freeB[x]=false;
					sb.freeB--;
					//cout<<"X="<<x<<endl;
					//fseek(fp,sb.BS*(x),0);
					in1.indirect=x;
					x=sb.numB;
				}else{
						if(x==sb.numB-1){
							cerr<<"No more free data blocks"<<endl;
							return;
						}
					}
			//cout<<"Indirect1 :"<<in1.indirect<<endl;
			}
		
			int bc2=0;
			for(int i=12;i<=directE;i++){
				for(int x=0;x<sb.numB;x++){
					if(freeB[x]==true){
						freeB[x]=false;
						indir[bc2]=x;
						blocks[bc]=x;
						bc++;
						bc2++;
						x=sb.numB;
					}else{
						if(x==sb.numB-1){
							cerr<<"No more free data blocks"<<endl;
							return;
						}
					}
				}
			}
		}else{
			char temp[sb.BS];
			//int indi[directE-12];
			//cout<<"HERE: "<<in1.indirect<<endl;
			fseek(fp,sb.BS*(in1.indirect),0);
			fread(temp,sizeof(temp),1,fp);
			memcpy(&indir,&temp,sizeof(indir));
			for(int i=0;i<(sb.BS/4);i++){
				if(indir[i]!=0){
					blocks[bc]=indir[i];
					bc++;
				}
			}
			for(int i=bc;i<=(directE-directS);i++){
				for(int x=0;x<sb.numB;x++){
					if(freeB[x]==true){
						freeB[x]=false;
						blocks[bc]=x;
						indir[bc-12]=x;
						bc++;
						//bc2++;
						x=sb.numB;
					}else{
						if(x==sb.numB-1){
							cerr<<"No more free data blocks"<<endl;
							return;
						}
					}
				}

			}

		}
		//cout<<"HERE: "<<in1.indirect<<endl;
		fseek(fp,sb.BS*(in1.indirect),0);
		//cout<<"Ind: "<<in1.indirect<<endl;
		//cout<<"HELLO: "<<indir[5]<<endl;
		fwrite(&indir,sizeof(indir),1,fp);
		for(int i=0;i<=(directE-directS);i++){
			if(i==0){
				start2=start%sb.BS;
				fseek(fp,(start2+sb.BS*(blocks[i])),0);
				if(numb>=sb.BS-start2){
						//sb.freeB--;
						for(int y=0;y<sb.BS-start2;y++){
							fwrite(&c,1,1,fp);
						}
						numb-=(sb.BS-start2);
						}
				else{
					//sb.freeB--;
					for(int y=0;y<numb;y++){
						fwrite(&c,1,1,fp);
					}
				}
				
			}
			else{
					fseek(fp,sb.BS*(blocks[i]),0);
					if(numb>=sb.BS){
						//sb.freeB--;
						for(int y=0;y<sb.BS;y++){
							fwrite(&c,1,1,fp);
						}
						numb-=sb.BS;
						}
					else{
						//sb.freeB--;
						for(int y=0;y<numb;y++){
							fwrite(&c,1,1,fp);
						}
					}
				}
			
		}

	}
		else if(in1.file_size==0){
		
				for(int i=0;i<=directE;i++){
					for(int x=0;x<sb.numB;x++){
						if(freeB[x]==true){
							freeB[x]=false;
							//cout<<"X="<<x<<endl;
							fseek(fp,sb.BS*(x),0);
							in1.direct_pointer[i]=x;
							x=sb.numB;
							if(numb>sb.BS){
								sb.freeB--;
								//cout<<"WRITING HERE"<<endl;
								for(int y=0;y<sb.BS;y++){
									fwrite(&c,sizeof(c),1,fp);
								}
							numb-=sb.BS;
							
							}
							else{
								//cout<<"WRITING HERE 2 "<<c<<" num :"<<numb<<endl;			
								sb.freeB--;
								for(int y=0;y<numb;y++){
									fwrite(&c,sizeof(c),1,fp);
								}
							}

						}else{
						if(x==sb.numB-1){
							cerr<<"no more free data blocks"<<endl;
							return;
						}
					}
					}

				}

			}
		else if(in1.direct_pointer[directE]!=-1){
			for(int x=directS;x<=directE;x++){
				if(x==directS){
					//fseek(fp,sb.BS*(in1.direct_pointer[x]),0);
					//cout<<"HI"<<endl;
					start2=start%sb.BS;
					fseek(fp,start2+sb.BS*(in1.direct_pointer[x]),0);
					if(numb>=sb.BS-start2){
						//sb.freeB--;
						for(int y=0;y<sb.BS-start2;y++){
							fwrite(&c,1,1,fp);
						}
						numb-=(sb.BS-start2);
						}
					else{
						//sb.freeB--;
						for(int y=0;y<numb;y++){
							fwrite(&c,1,1,fp);
						}
					}
				
				}else{
					fseek(fp,sb.BS*(in1.direct_pointer[x]),0);
					if(numb>=sb.BS){
						sb.freeB--;
						for(int y=0;y<sb.BS;y++){
							fwrite(&c,1,1,fp);
						}
						numb-=sb.BS;
						}
					else{
						sb.freeB--;
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
					fseek(fp,start2+(sb.BS*(in1.direct_pointer[x])),0);
					if(numb>=sb.BS-start2){
						//sb.freeB--;
						for(int y=0;y<sb.BS-start2;y++){
							fwrite(&c,1,1,fp);
						}
						numb-=(sb.BS-start2);
						}
					else{
						//sb.freeB--;
						for(int y=0;y<numb;y++){
							fwrite(&c,1,1,fp);
						}
					}

				}
				else if(in1.direct_pointer[x]!=-1){
					fseek(fp,sb.BS*(in1.direct_pointer[x]),0);
					if(numb>=sb.BS){
						//sb.freeB--;
						for(int y=0;y<sb.BS;y++){
							fwrite(&c,1,1,fp);
						}
						numb-=sb.BS;
						}
					else{
						//sb.freeB--;
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
								sb.freeB--;
								for(int y=0;y<sb.BS;y++){
									fwrite(&c,1,1,fp);
								}
								numb-=sb.BS;
							}
							else{
								sb.freeB--;
								for(int y=0;y<numb;y++){
									fwrite(&c,1,numb,fp);
								}
							}

						}else{
						if(x==sb.numB-1){
							cerr<<"no more free data blocks"<<endl;
							return;
						}
					}
					}
					}

				}
			}
		//}
		if((start+num)>in1.file_size){
			in1.file_size=num+start;
		}
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
		//cout<<"Indirect2: "<<in1.indirect<<endl;
		fseek(fp,sb.BS*(sb.inode+in),0);
		fwrite(&in1,sizeof(in1),1,fp);
		fseek(fp,0,0);
		fwrite(&sb,sizeof(sb),1,fp);
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
			i=256;
			if(strcmp(file_name.c_str(), tempinode.filename) == 0){
				cerr << "not a unique file" << endl;
				return;
			}
		}/*else{
			if(i==255){
				cerr << "No more file slots left" << endl;
				exit(0);
			}

		}*/
	}
		for(int i =0; i <256; i++){
				//find empty inode
			if(inArr[i] == true){
				inode_num = i;
				inArr[i] = false;
				//it=inodeMap.find(i);
				//inodeMap.erase(inodeMap.find(i));
				inodeMap[file_name]=i;
				sb.freeIn-=1;
				//sb.inode++;
				//fseek(file_name,(s->BS)*(s->offset+i), 0);
				break;
			}else{
			if(i==255){
				cerr << "No more file slots left" << endl;
				return;
				}
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
	//cout<<"Hi"<<endl;
	//cout<<newnode.filename<<endl;
	//set file length MAYBE

	//update inode which flips bit at that index

	//fseek(fp, sb.off +inode_num *sb.BS, SEEK_SET);
	fseek(fp,sb.BS*(inode_num+sb.inode),0);
	fwrite(&newnode,sizeof(newnode),1,fp);
	//cout<<"HELLO: "<<newnode.filename<<endl;
	fclose(fp);
	fp=fopen("DISK","rb+");
	Inode ret;
	fseek(fp, (sb.inode +inode_num) *sb.BS, 0);
	//cout<<"FUCK "<<sb.inode<<endl;
	char ch2[sb.BS];
	fread(ch2, sb.BS, 1, fp);
	memcpy(&ret,&ch2,sizeof(ret));
	//cout << ret.filename << endl;
	//cout << newnode->filename <<endl;
	//cout << fread(newnode,sizeof(Inode),1,fp) <<endl;
	fseek(fp,0,0);
	fwrite(&sb,sizeof(sb),1,fp);
	fclose(fp);
	//cout << "hi " <<endl;
	//free(tempinode);
	//exit(1);
	
}

void *producer(void *param){
	string *ar = (string *)param;
	//cout << "here" << endl;
	ifstream file(*ar);
	string line, token;
	vector<string> args;
	//cout << "prod1" << endl;
	while(1){
		while(getline(file, line, '\n')){
			args.clear();
			istringstream ss(line);
			while(ss>>token) args.push_back(token);
			sem_wait(&full);
			pthread_mutex_lock(&mu);
			buffer.push_back(args);
			cout << "Produced" << endl;
			pthread_mutex_unlock(&mu);
			sem_post(&empty);
		}
		pthread_exit(0);
		//delete ar;
	}	
}

void *consumer(void *param){
	argies *ar = new argies;
	ar = (argies *)param;
	ssfs *tempFS = ar->fs;
	vector<string> conArg;
	while(1){
		cout << "Entering" << endl;
		sem_wait(&empty);
		pthread_mutex_lock(&mu);
		conArg = buffer.at(buffer.size()-1);
		buffer.pop_back();
		//cout << conArg[0] << endl;
		if(conArg[0] == "CREATE"){
			if (conArg.size() < 2) {
				cout << "Incorrect number of arguments. " << endl;
				//continue;
			}
			else tempFS->create(conArg,ar->s);
		}
		else if(conArg[0] == "IMPORT"){
			if (conArg.size() < 3) {
				cout << "Incorrect number of arguments. " << endl;
				//continue;
			}
			else tempFS->import(conArg, ar->s, ar->freeB);
		}
		else if(conArg[0] == "CAT"){
			if (conArg.size() < 2) {
				cout << "Incorrect number of arguments. " << endl;
				//continue;
			}
			else tempFS->cat(conArg,ar->s);
		}
		else if(conArg[0] == "DELETE"){
			if (conArg.size() < 2) {
				cout << "Incorrect number of arguments. " << endl;
				//continue;
			}
			else tempFS->del(conArg,ar->s, ar->freeB);
		}
		else if(conArg[0] == "WRITE"){
			if (conArg.size() < 5) {
				cout << "Incorrect number of arguments. " << endl;
			//	continue;
			}
			else tempFS->write(conArg,ar->s,ar->freeB);
		}
		else if(conArg[0] == "READ"){
			if (conArg.size() < 4) {
				cout << "Incorrect number of arguments. " << endl;
		//		continue;
			}
			else tempFS->read(conArg, ar->s);
		}
		else if(conArg[0] == "LIST"){
			tempFS->list(ar->s);			
		}
		else if(conArg[0] == "SHUTDOWN"){
			//not updated yet			
		}
		cout << "consumed" << endl;
		pthread_mutex_unlock(&mu);
		cout << "hersdsdse" << endl;
		sem_post(&full);
		cout << 2 << endl;
	}	
	pthread_exit(0);
	delete ar;
}

/*void ssfs::disk_controller(SuperBlock s, bool *freeB){
	while(1){
		//Disk Controller executes command in command vector
		
		break;
	}
	
	
}*/

int main(int argc, char * argv[]){
	FILE *fd;
	fd=fopen("DISK","rb+");
	char ch[128]="\0";
	fread(ch,128,1,fd);
	SuperBlock s;
	memcpy(&s,&ch,sizeof(SuperBlock));
	/*cout<<s.freeB<<endl;
	cout<<s.off<<endl;
	cout<<s.inode<<endl;
	cout<<s.freeBA<<endl;*/
	vector<string> args;
	string cmd;

	string token;

	//if(argc > 1){
		ssfs *fs = new ssfs(args, s.numB, s.BS, 12);
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
	//cout<<s.BS<<endl;
	if(s.BS<256){
		fread(ch1,s.BS,1,fd);
		memcpy(&temp,&ch1,sizeof(ch1));
		fseek(fd,2*s.BS,0);
		fread(ch1,s.BS,1,fd);
		memcpy(&temp2,&ch1,sizeof(ch1));
		for(int i=0;i<256;i++){
			if(i<128) fs->inArr[i]=temp[i];
			else fs->inArr[i]=temp2[i-128];
		}
	}else{
		fread(ch1,s.BS,1,fd);
		//cout<<ch1[0]<<endl;
		memcpy(&(fs->inArr),&ch1,sizeof(ch1));
		//cout<<fs->inArr[10]<<endl;
	}
	//cout<<fs->inArr[100]<<endl;
	fseek(fd,((s.BS)*(s.freeBA)),0);
	int count=0;
	int countB=s.freeBA;
	//cout<<"Free BA:"<<s.freeBA<<endl;
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
	//cout<<freeB[136]<<endl;
	
	char in[s.BS];
	for(int i=0;i<256;i++){
		if(!fs->inArr[i]){
			//cout<<"HERE NOW: "<<s.inode<<endl; 
			fseek(fd,(s.BS)*(s.inode+i),0);
			fread(in,s.BS,1,fd);
			Inode n;
			memcpy(&n,&in,sizeof(in));
			//cout<<n.filename<<endl;
			fs->inodeMap[n.filename]=i;
			//cout<<"hi "<<n.filename<<endl;
		}
	}
	//cout<<"HERE :"<<fs->inodeMap["temp2.txt"]<<endl;
	string cmd_file (argv[1]);
	argies a(s, freeB, cmd_file, fs);
	sem_init(&full, 0, 1);
	sem_init(&empty, 0, 0);
	
	pthread_create(&tid,NULL,producer, (void *)&cmd_file);
	pthread_create(&tid1,NULL,consumer,(void *)&a);
	
	int check = pthread_join(tid, NULL);
	cout << check << endl;
	pthread_join(tid1, NULL);
	cout << "endMahdi" << endl;
	

	/*ifstream cmd_file2(argv[2]);
	cout << "here bitch" << endl;
	//thread(bind(fs->opthread_parser, cmd_file, s, freeB));
	for (int l=0; l < (fs->buffer).size(); l++){
		cout << fs->buffer[l] << " ";
	}

	//

*/
	fclose(fd);
	return 0;
	/*while(getline(cmd_file, cmd,'\n')||true){
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
			//cout << "hiii" <<endl;
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
			fs->import(args,s,freeB);
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
			fs->del(args,s,freeB);
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
			fseek(fd,s.BS,0);
			if(s.BS>=256){
				bool b[s.BS];
				for(int i=0;i<256;i++) b[i]=fs->inArr[i];
				//for(int i=0;i<256;i++) b[i]=true;
				fwrite(&b,sizeof(b),1,fd);
			}else{
				bool b[128];
				for(int i=0;i<128;i++) b[i]=fs->inArr[i];
				fwrite(&b,sizeof(s.BS),1,fd);
				fseek(fd,2*s.BS,0);
				for(int i=0;i<128;i++) b[i]=fs->inArr[i+128];
				fwrite(&b,sizeof(s.BS),1,fd);
				
				
			}
			int start=0;
			bool temp1[s.BS];
			//cout<<"BLOCK SIZE: "<<s.BS<<endl;
			if(s.BS==128) {start=3;}
			else {start = 2;}
			//cout<<start<<endl;
			fseek(fd,start*s.BS,0);
			int counter=0;
			for(int i=0;i<(s.numB/s.BS);i++){
				for(int x=0;x<s.BS;x++){
					temp1[x]=freeB[counter];
					counter++;
				}
				fwrite(&temp1,sizeof(temp1),1,fd);
				//i+=s.BS;

			}
			fclose(fd);
			exit(0);

		}
	}*/


	
}
