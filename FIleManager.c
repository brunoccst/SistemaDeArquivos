#include <stdio.h>
struct​ rootdir_entry{
	char ​name[20];
	unsigned short int s​ize;
	unsigned short int​ index;
};
struct rootdir{
	unsigned short int ​free_blocks
	unsigned short int​ trailing;
	struct ​rootdir_entry list_entry[170];
	unsigned char r​eserved[12];
};
unsigned char​ sector_data[1024];
unsigned short int​ sector_index[512];
char * consoleCommand;


int isEOF(struct rootdir_entry * entry){
	return ( * entry ).size == 0
				&& ( * entry ).index == 0
				&& ( * entry ).name[0] == 0;
}
int isDeleted(struct rootdir_entry * entry){
	return ( * entry ).index == 0  
				&& !( * entry ).size == 0
				&& !( * entry ).name[0] == 0 ;
}
void menu(){
	printf("~	");
  	scanf("%s", consoleCommand);
	printf("~	%s",consoleCommand);
}
void main(int argc, const char* argv[]){
	menu();
}
