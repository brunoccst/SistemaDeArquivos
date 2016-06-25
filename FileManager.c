#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 65535*1024

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


const char * init_ = "-init";
const char * create_ = "-create";
const char * read_ = "-read";
const char * del_ = "-del";
const char * ls_ = "-ls";

unsigned char sector_data[1024];
unsigned short int sector_index[512];
char consoleCommand[16];
char fileName[20];


FILE * simul;
struct rootdir_entry* findFile(){
	int i = 0;	
	for(;i<170;i++){
		printf("teste while::%s\n",ROOT.list_entry[i].name);
		if(strcmp(ROOT.list_entry[i].name, fileName) == 0 ){
			return &ROOT.list_entry[i];
		}
	}
	return NULL;
}

int isEOF(struct rootdir_entry * entry){
	return ( * entry ).size == 0
				&& ( * entry ).index == 0
				&& ( * entry ).name[0] == 0;
};
int isDeleted(struct rootdir_entry * entry){
	return ( * entry ).index == 0;
};

int filesize(FILE * fptr){
	fseek(fptr,0,SEEK_END);
	int size = ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	return size;
}
int hasEnoughSpace(int size){
	unsigned short int nextFreePosition =  ROOT.free_blocks;
	size += 1024;//PARA GRAVAR A LISTA DE INDEXES
	while(nextFreePosition && size>0){
		seekSector(&nextFreePosition,0);
		fread(nextFreePosition,sizeof(unsigned short int),1,simul);
		size -= 1024;
	}
	return size < 1;
}
void writeNextBlock(){
	seekSector(ROOT.free_blocks,0);
	loadNextFreeBlock();
	fwrite(sector_data,sizeof(sector_data),1,simul);
	memset(sector_data,0,sizeof(sector_data));
}
void seekSector(int bloco,int offset){
	int result = offset + 4096 + 1024*(bloco-1);
	//printf("resultado: %d\n",result);
	fseek(simul,result,SEEK_SET);
	printf("position: %d\n",ftell(simul));
	
}
void loadNextFreeBlock(){
	fread(&ROOT.free_blocks,sizeof(unsigned short int),1,simul);
}

void startRoot(){
	memset(&ROOT,0,sizeof(struct rootdir));
	ROOT.free_blocks = 1; // PRIMEIRO INDEX
}
void writeRoot(){
	fseek(simul,0,SEEK_SET);
	fwrite(&ROOT,sizeof(struct rootdir),1, simul);
}
void readRoot(){
	fseek(simul,0,SEEK_SET);
	fread(&ROOT,sizeof(struct rootdir),1, simul);
}
int createNewEntry(FILE * fptr){
	struct rootdir_entry * newEntry = ROOT.list_entry;
	int size = filesize(fptr);
	if(hasEnoughSpace(size)){
		int cont = 0;
		while(cont < 170 && !isDeleted(newEntry) ){
			cont++;
			newEntry++;
		}
		printf("%d\n",cont);
		if(cont == 170)
			return 0;
		printf("Criando entrada...");
		memset(newEntry,0,sizeof(struct rootdir_entry));
		( * newEntry ).size = size;
		( * newEntry ).index = ROOT.free_blocks;
		sprintf(( * newEntry ).name,"%s",fileName);
		seekSector(ROOT.free_blocks,0);
		loadNextFreeBlock();
		printf("Entrada criada!!!");	
		return 1;
	}
	return 0;
}
void copyFile(FILE * fptr){
	unsigned short int * indexes = sector_index;
	while(!feof(fptr)){
		if(ftell(fptr) < sizeof(sector_data))
			memset(sector_data,0,sizeof(sector_data));
		fread(sector_data,sizeof(sector_data),1,fptr);
		*indexes = ROOT.free_blocks;
		indexes++;
		writeNextBlock();
	}
}
void writeIndexes(){
	struct rootdir_entry * entry = findFile();
	seekSector(entry->index,sizeof(short int));
	fwrite(sector_index,sizeof(sector_index),1,simul);
	memset(sector_index,0,sizeof(sector_index));
	fwrite(sector_index,sizeof(sector_index),1,simul);
}
void loadIndexSector(unsigned short int index){
	seekSector(index,sizeof(short int));
	fread(sector_index,sizeof(sector_index),1,simul);
}
void deleteSector(unsigned short int index){
	seekSector(index,0);
	fwrite(&ROOT.free_blocks,sizeof(unsigned short int),1, simul);
	ROOT.free_blocks = index;
}

void init()
{
	if ( !( simul = fopen( "simul.fs", "w" )))
	{
	    printf( "ON INIT simul.fs could not be opened.\n" );
	}
	else
	{
		startRoot();
		printf("Initializing 'simul.fs'\n");
		writeRoot();
		unsigned short int cont = 1;
		int z= 0;
		//printf("%d\n",&cont);
		memset(sector_data,0,1024);
		while(cont++){
			sector_data[0] = cont & 0x00FF;
			sector_data[1] = (cont & 0xFF00)>>8;
			fwrite(sector_data,sizeof(sector_data),1,simul);
			//printf("pocição que esta:%d\n",ftell(simul));
			z += sizeof(sector_data);
		}	
		//printf("%d\n",--cont);
		printf("Finished initializing 'simul.fs'\n");
		fclose(simul);
	}
}
void create(){
	FILE *toCreate;
	memset(sector_data,0,1024);
	
	if ( !( toCreate = fopen( fileName, "r" )))
	{
	    printf( "ON CREATE file could not be opened.\n" );
	}
	else
	{
		if(!createNewEntry(toCreate))
		{
			printf("Nao há espaço livre!!!\n");
			return;
		}
		printf("Criando arquivo...\n");
		copyFile(toCreate);
		writeIndexes();
		printf("Arquivo criado!!!\n");
		fclose(toCreate);
	}
	writeRoot();
}
void read(){
	FILE *toCreate;
	struct rootdir_entry * entry = findFile();
	if(entry == NULL || isDeleted(entry)){
		printf("Arquivo nao encontrado!!!\n");
		return;
	}

	if ( !( toCreate = fopen( fileName, "wb+" )))
	{
	    printf( "ON READ file could not be opened.\n" );
	}
	else{
		loadIndexSector((* entry ).index);
		unsigned short int * indexes = sector_index;
		int size = (* entry ).size;
		while((*indexes) != 0)
		{
			seekSector(*indexes,sizeof(short int));
			fread(sector_data,sizeof(sector_data),1,simul);
			fwrite(sector_data,size < 1024 ? size : sizeof(sector_data),1,toCreate);
			size -= sizeof(sector_data);
		}
		fclose(toCreate);
	}
	
}
void delete(){
	struct rootdir_entry * entry = findFile();
	if(entry == NULL || isDeleted(entry)){
		printf("Arquivo nao encontrado!!!\n");
		return;
	}
	loadIndexSector((*entry).index);
	unsigned short int * indexes = sector_index;
	while((*indexes) != 0)
	{
		deleteSector(*indexes);
	}
	writeRoot();
}
void list(){
	int cont = 0;
	struct rootdir_entry * newEntry = ROOT.list_entry;
	printf("-------------Arquivos-------------\n");
	while(cont < 170){
		cont++;
		if(isDeleted(newEntry))
			continue;
		printf("%s\n", ( * newEntry ).name);
		newEntry++;
	}

	printf("---------------FIM----------------\n");
}

void main(int argc, const char* argv[]){
	memset(fileName,0,20);
	memset(consoleCommand,0,10);
	sprintf(consoleCommand,"%s",(char*) argv[1]);
	if (strcmp(consoleCommand, init_) == 0)
	{
		init();
	}
	else if ( !( simul = fopen( "simul.fs", "r+" )))
	{
	    printf( "File could not be opened.\n" );
	}
	else
	{	
		readRoot();
		fseek(simul,0,SEEK_SET);
		if (strcmp(consoleCommand, create_) == 0)
		{
			sprintf(fileName,"%s",(char*) argv[2]);
			create();
		}
		else if (strcmp(consoleCommand, read_) == 0)
		{
			sprintf(fileName,"%s",(char*) argv[2]);
			read();
		}
		else if (strcmp(consoleCommand, del_) == 0)
		{
			sprintf(fileName,"%s",(char*) argv[2]);
			delete();
		}
		else if (strcmp(consoleCommand, ls_) == 0)
		{
			list();
		}
		else
		{
			printf("Command not valid.\n");
		}
			fclose(simul);
	}
}
