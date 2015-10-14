#include "tree.h"

// Main function
int main(int argc, char **argv){
	link root;
	char filename[BUFFSIZE];
	char prefix[BITSIZE + 1];
	char command[BUFFSIZE];
	char linha[BUFFSIZE];
	int n = 0, hop = 0;
	
	// Obtaining filename from arguments
	memset(prefix, '\0', BITSIZE + 1);
	if(argc != 2){
		printf("Wrong number of arguments.\n");
		exit(1);
	}
	sscanf(argv[1], "%s", filename);
	
	// Tree Operations
	root = ReadTable(filename);
	printf("The file %s was loaded successfully to the tree.\n", filename);
	
	PrintInterface();
	
	// Principal Code
	while(1){
		fgets(linha, BUFFSIZE, stdin);
		n = sscanf(linha, "%s %s %d", command, prefix, &hop);
		// With the MACROS, this switch speaks for itself
		switch(getCMD(command)){
			case ADD:
				if(n != 3){
					printf("Invalid number of arguments for command ADD\n");
					continue;
				}
				if(!isBinary(prefix)) continue;
				AddPrefix(root, prefix, hop);
				break;
			case DEL:
				if(n != 2){
					printf("Invalid number of arguments for command DEL\n");
					continue;
				}
				if(!isBinary(prefix)) continue;
				DeletePrefix(root, prefix);
				break;
			case PRINT:
				PrintTable(root, prefix, 0);
				break;
			case TWOTREE:
				TwoTree(root, -1);
				break;
			case LOOKUP:
				if(n != 2){
					printf("Invalid number of arguments for command LOOKUP\n");
				}
				if(!isBinary(prefix)) continue;
				printf("The next hop for %s is %d.\n", prefix, AddressLookUp(root, prefix));
				break;
			case HELP:
				PrintInterface();
				break;
			case EXIT:
				MemoryCheck(root);
				free(root);
				exit(0);
				break;
			default:
				printf("Not a valid command\n");
				break;
		}
		printf("----------\n");
		// This memset exists to enforce an empty prefix, whatever the input
		// on the console might be
		memset(prefix, '\0', BITSIZE + 1);
	}
}

