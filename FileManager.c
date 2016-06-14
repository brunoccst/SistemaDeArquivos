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


FILE * simul;

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

int filesize(FILE * fptr){
	fseek(fptr,0,SEEK_END);
	int size = ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	return size;
}
int hasEnoughSpace(int size){
	int nextFreePosition = ROOT.free_blocks;
	while(nextFreePosition){
		seekBlock(nextFreePosition);
		fread(&nextFreePosition,sizeof(int),1,simul);
		size -= 1024;
	}
	return size < 1;
}
void writeNextBlock(){
	seekBlock(ROOT.free_blocks);
	fread(&ROOT.free_blocks,sizeof(unsigned short int),1,simul);
	fread(&ROOT.free_blocks,sizeof(unsigned short int),1,simul);
	//TODO

}
void seekBlock(int bloco){
	fseek(simul,4096 + 1024*(bloco-1),SEEK_SET);
}

void startRoot(){
	memset(&ROOT,0,sizeof(struct rootdir));
	ROOT.free_blocks = 1; // PRIMEIRO INDEX
}
void writeRoot(){
	fwrite(&ROOT,sizeof(struct rootdir),1, simul);
}
void readRoot(){
	fread(&ROOT,sizeof(struct rootdir),1, simul);
}

void init()
{
	//printf("'teste record ---> ROOT.free_blocks = %u\n'",ROOT.free_blocks);
	startRoot();
	remove("simulfs");
	//printf("'teste record ---> ROOT.free_blocks = %u\n'",ROOT.free_blocks);
	if ( !( simul = fopen( "simul.fs", "wb" ))){
	    printf( "File could not be opened.\n" );
	}
	else
	{
	    printf("Initializing 'simul.fs'\n");
		writeRoot();
		int cont = 1;
		memset(sector_data,0,1024);
		while(cont <= 65536){
			fwrite(&cont,sizeof(int),1,simul);
			fwrite(sector_data,1024,1, simul);
			cont++;
		}
	    printf("'simul.fs' initialized.\n");	
		fclose(simul);
		//printf("'teste record ---> ROOT.free_blocks = %u'\n",ROOT.free_blocks);
	}
}
int createNewEntry(FILE * fptr){
	struct rootdir_entry newEntry;
	memset(&newEntry,0, sizeof(struct rootdir_entry));//CRIA NOVA ENTRADA
	hasEnoughSpace(filesize(fptr));
	return 0;
}

void create(char name[] ){
	FILE *toCreate;
	if ( !( simul = fopen( "simul.fs", "wb+" ))){
	    printf( "File could not be opened.\n" );
	}
	else
	{
		readRoot(simul);
		memset(sector_data,0,1024);
		toCreate = fopen(name,"rb");
		if(createNewEntry(toCreate))
		{
			printf("Nao há espaço livre!!!");
			return;
		}
		while(!feof(toCreate))
		{
			fread(sector_data,sizeof(sector_data),1,toCreate);
			writeNextBlock(simul);
		}	
		fclose(simul);	
		fclose(toCreate);
	}


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
