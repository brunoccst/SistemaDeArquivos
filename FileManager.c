#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

struct rootdir ROOT;


const char * init_ = "-init\0";
const char * create_ = "-create\0";
const char * read_ = "-read\0";
const char * del_ = "-del\0";
const char * ls_ = "-ls\0";

unsigned char sector_data[1024];
unsigned short int sector_index[512];
char consoleCommand[10];
char fileName[20];

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



void startRoot(){
	memset(&ROOT,0,sizeof(struct rootdir));
	ROOT.free_blocks = 4096; // PRIMEIRO INDEX
}
void writeRoot(FILE * file){
	fwrite(&ROOT,sizeof(struct rootdir),1, file);
}
void readRoot(FILE * file){
	fread(&ROOT,sizeof(struct rootdir),1, file);
}

void init()
{
	//printf("'teste record ---> ROOT.free_blocks = %u\n'",ROOT.free_blocks);
	startRoot();
	remove("simulfs");
	//printf("'teste record ---> ROOT.free_blocks = %u\n'",ROOT.free_blocks);
	FILE *fptr;
	if ( !( fptr = fopen( "simul.fs", "wb+" ))){
	    printf( "File could not be opened.\n" );
	}
	else
	{
	    printf("Initializing 'simul.fs'\n");
		writeRoot(fptr);
		fseek(fptr,0,SEEK_SET);
	    printf("'simul.fs' initialized.\n");
		ROOT.free_blocks = 0;
		readRoot(fptr);
		//printf("'teste record ---> ROOT.free_blocks = %u'\n",ROOT.free_blocks);
	}
}

void create(char name[] ){


}
void read(char name[]){}
void delete(char name[]){}
void deleteFile(FILE * file){}
void list(){}

void main(int argc, const char* argv[]){
	memset(&fileName,0,20);
	sprintf(consoleCommand,"%s\0",(char*) argv[1]);
	if (strcmp(consoleCommand, init_) == 0)
	{
		init();
	}
	else if (strcmp(consoleCommand, create_) == 0)
	{
		sprintf(fileName,"%s\0",(char*) argv[2]);
		create(fileName);
	}
	else if (strcmp(consoleCommand, read_) == 0)
	{
		sprintf(fileName,"%s\0",(char*) argv[2]);
		read(fileName);
	}
	else if (strcmp(consoleCommand, del_) == 0)
	{
		sprintf(fileName,"%s\0",(char*) argv[2]);
		delete(fileName);
	}
	else if (strcmp(consoleCommand, ls_) == 0)
	{
		list();
	}
	else
	{
		printf("Command not valid.\n");
	}
}
