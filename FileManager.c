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

const char * init_ = "-init";
const char * create_ = "-create";
const char * read_ = "-read";
const char * del_ = "-del";
const char * ls_ = "-ls";

unsigned char sector_data[1024];
unsigned short int sector_index[512];
char* consoleCommand;
char* fileName;

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

void init()
{
	FILE *fptr;
	if ( !( fptr = fopen( "simul.fs", "wb" ))){
	    printf( "File could not be opened.\n" );
	}
	else
	{
	    printf("Initializing 'simul.fs'");
	    char * buff = (char*)malloc(MAX_SIZE);
	    memset(buff, 0, MAX_SIZE);
	    fwrite (buff , sizeof(char), MAX_SIZE, fptr);
	    printf("'simul.fs' initialized.");
	}
}

void create(char name[] ){}
void read(char name[]){}
void delete(char name[]){}
void deleteFile(FILE * file){}
void list(){}

void main(int argc, const char* argv[]){

	consoleCommand = (char*) argv[1];
	fileName = (char*) argv[2];

	if (strcmp(consoleCommand, init_) == 0)
	{
		init();
	}
	else if (strcmp(consoleCommand, create_) == 0)
	{
		create(fileName);
	}
	else if (strcmp(consoleCommand, read_) == 0)
	{
		read(fileName);
	}
	else if (strcmp(consoleCommand, del_) == 0)
	{
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
