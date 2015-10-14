#include "helper.h"

// Auxiliary Functions
/* 
Function used to print a user interface for a better understanding of available
commands, and respective arguments, that can be applied to the tree.
*/
void PrintInterface(){
	printf("-------------------------------------------------------\n");
	printf("[COMMAND]\t- [ARGUMENTS]\n");
	printf("-------------------------------------------------------\n");
	printf("[ADD]\t\t- [Prefix (binary)] [Hop (int)]\n");
	printf("[DEL]\t\t- [Prefix (binary)]\n");
	printf("[PRINT]\t\t- [NULL]\n");
	printf("[TWOTREE]\t- [NULL]\n");
	printf("[LOOKUP]\t- [Prefix (binary)]\n");
	printf("[HELP]\t\t- [NULL]\n");
	printf("[EXIT]\t\t- [NULL]\n");
	printf("-------------------------------------------------------\n");
}
/* 
Function that selects an action from the input command meant to simplify
comprehension of the switch routine in the main function.
*/
int getCMD(char * command){
	if(!strcasecmp(command, "ADD")) return ADD;
	if(!strcasecmp(command, "DEL")) return DEL;
	if(!strcasecmp(command, "PRINT")) return PRINT;
	if(!strcasecmp(command, "TWOTREE")) return TWOTREE;
	if(!strcasecmp(command, "LOOKUP")) return LOOKUP;
	if(!strcasecmp(command, "HELP")) return HELP;
	if(!strcasecmp(command, "EXIT")) return EXIT;
	return -1;
}
/*
Function used for string verification.
For many functions of this program it's needed to verify if the prefix only contains
1's and 0's (binary prefix). That's what this function does.
*/
int isBinary(char * prefix){
	int i;
	for (i = 0; prefix[i] != '\0'; i++){
		if(prefix[i] != '0' && prefix[i] != '1'){
			printf("%s is not binary!\n", prefix);
			return 0;
		}
	}
	return 1;
}

