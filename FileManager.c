#include <stdio.h>
#define MAX_SIZE 65536*1024
struct rootdir_entry{
	char name[20];
	unsigned short int size;
	unsigned short int index;
};
struct rootdir{
	unsigned short int free_blocks;
	unsigned short int trailing;
	struct rootdir_entry list_entry[170];
	unsigned char reserved[12];
};
unsigned char sector_data[1024];
unsigned short int sector_index[512];
char * consoleCommand;

int isEOF(struct rootdir_entry * entry){
	return ( * entry ).size == 0
				&& ( * entry ).index == 0
				&& ( * entry ).name[0] == 0;
};
int isDeleted(struct rootdir_entry * entry){
	return ( * entry ).index == 0  
				&& !( * entry ).size == 0
				&& !( * entry ).name[0] == 0 ;
};
void init(){
	//OPEN FILE "simul.fs"
	//IF EXISTS FILE DELETE IT
	//free_blocks = 0;
	//
}
void create(char name[] ){}
void read(char name[]){}
void delete(char name[]){}
void deleteFile(FILE * file){}
void list(){}
void main(int argc, const char* argv[]){
	
}
