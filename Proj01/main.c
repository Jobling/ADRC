#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MACROS
#define BITSIZE 8
#define BUFFSIZE 16

#define ADD 0
#define DEL 1
#define PRINT 2
#define TWOTREE 3
#define LOOKUP 4
#define HELP 5
#define EXIT 6

// Data Structures
typedef struct node * link;
typedef struct node{
	int hop;
	link left;
	link right;
} node;

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
Function used to free used memory when exiting cleanly from the program.
*/
void MemoryCheck(link self){
	if(self->left != NULL){
		MemoryCheck(self->left);
		free(self->left);
	}
	if(self->right != NULL){
		MemoryCheck(self->right);
		free(self->right);
	}
	return;
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

// Tree Related Functions (Primary Functions)
/*
Function that relates a certain destination hop with a prefix.
To do that the program "travels along the tree" following the prefix and updates 
the value of the next hop when reaches the end of the prefix.
*/
void AddPrefix(link self, char * prefix, int hop){	
	switch (prefix[0]){
		case '\0':
			// All out of bits
			self->hop = hop;
			break;
		case '0':
			// This bit points left
			if (self->left == NULL){
				self->left = (link) malloc(sizeof(node));
				self->left->left = NULL;
				self->left->right = NULL;
				self->left->hop = 0;
			}
			AddPrefix(self->left, &prefix[1], hop);
			break;
		case '1':
			// This bit points right
			if(self->right == NULL){
				self->right = (link) malloc(sizeof(node));
				self->right->right = NULL;
				self->right->left = NULL;
				self->right->hop = 0;
			}
			AddPrefix(self->right, &prefix[1], hop);
			break;
		default:
			// Not interpreted as a bit
			// It should never happen because of previous verification
			printf("Unsupported prefix. Not binary.\n");
			exit(1);
	}
}
/*
Function that reads a file and returns a tree that's described on the file.
The file must have the following syntax:
[PREFIX] [NEXT-HOP]
*/
link ReadTable(char * filename){
	FILE *fp;
	link root;
	char *aux, linha[BUFFSIZE], prefix[BITSIZE + 1];
	int hop, n;
	
	// Opening the file with a 'read' flag
	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("File not found\n");
		exit(1);
	}
	
	// Reading the first line of the file
	aux = fgets(linha, BUFFSIZE, fp);
	if (aux == NULL){
		printf("Empty File\n");
	}
	
	// Creating tree with empty prefix (*). It's next-hop value will be 1
	root = (link) malloc(sizeof(node));
	root->hop = 1;
	root->left = NULL;
	root->right = NULL;
	
	// Node addition from file input
	while(aux != NULL){
		n = sscanf(linha, "%s %d", prefix, &hop);
		if(n == 2){
			if(isBinary(prefix)) AddPrefix(root, prefix, hop);
		}else printf("Invalid line format\n");
		aux = fgets(linha, BUFFSIZE, fp);
	}
	
	// Closing file
	fclose(fp);
	return root;
}
/*
Function used to "clean" a node.
This function does not free the memory used by the given node.
*/
void DeletePrefix(link self, char * prefix){
	switch (prefix[0]){
		case '\0':
			// All out of bits
			self->hop = 0;
			break;
		case '0':
			// This bit points left
			if (self->left == NULL)
				printf("The selected prefix doesn't exist.\n");
			else
				DeletePrefix(self->left, &prefix[1]);
			break;
		case '1':
			// This bit points right
			if(self->right == NULL)
				printf("The selected prefix doesn't exist.\n");
			else
				DeletePrefix(self->right, &prefix[1]);
			break;
		default:
			// Not interpreted as a bit
			// It should never happen because of previous verification
			printf("Unsupported prefix. Not binary.\n");
			exit(1);
	}
}
/*
Function used to print the expediction table stored in the tree on the terminal
*/
void PrintTable(link self, char * prefix, int level){
	// Index and hop printing
	if(level == 0) printf("%*c%4d\n", -BITSIZE, '*', self->hop);
	else if(self->hop != 0) printf("%*s%4d\n", -BITSIZE, prefix, self->hop);

	// Printing left child, if it exists
	if(self->left != NULL){
		prefix[level] = '0';
		level++;
		PrintTable(self->left, prefix, level);
		level--;
	}
	// Printing right child, if it exists
	if(self->right != NULL){
		prefix[level] = '1';
		level++;
		PrintTable(self->right, prefix, level);
		level--;
	}
	prefix[level] = '\0';
	return;
}
/*
Function used to turn the binary tree into a 2-Tree.
*/
void TwoTree(link self, int hop){
	// Verify if the node is "empty" or not
	// Set value to "pass down to child nodes"
	hop = (self->hop != 0) ? self->hop: hop;
	
	// If node is leaf node, it's value must be the value that was set beforehand
	if(self->right == NULL && self->left == NULL) self->hop = hop;
	// If the node has children, it's not a leaf
	// Pass on the value that was set beforehand
	else{
		// Since the node is not a leaf, it must be "cleaned", so that
		// only leaf nodes have values
		self->hop = 0;
		// Verifying left child
		// If it exists, "pass down the value that was set beforehand"
		if(self->left != NULL) TwoTree(self->left, hop);
		// If it doesn't exist, there is a right child, and this 
		// node must have two children (2-tree definition), so
		// we create a new child and set its value with the value 
		// that was set beforehand
		else{
				self->left = (link) malloc(sizeof(node));
				self->left->right = NULL;
				self->left->left = NULL;
				self->left->hop = hop;
		}
		// Verifying right child
		// If it exists, "pass down the value that was set beforehand"
		if(self->right != NULL) TwoTree(self->right, hop);
		// If it doesn't exist, there is a left child, and this 
		// node must have two children (2-tree definition), so
		// we create a new child and set its value with the value 
		// that was set beforehand
		else{
			self->right = (link) malloc(sizeof(node));
			self->right->right = NULL;
			self->right->left = NULL;
			self->right->hop = hop;
		}
	}
}
/*
Function used to know next-hop for given prefix
*/
int AddressLookUp(link self, char * prefix){
	// Integer used to store the value that's being returned
	int n;
	// If the prefix indicates the left node, and the left node
	// exists, we call the function on it
	if((prefix[0] == '0') && (self->left != NULL)) n = AddressLookUp(self->left, &prefix[1]); 
	else{
		// If the prefix indicates the right node, and the right node
		// exists, we call the function on it
		if((prefix[0] == '1') && (self->right != NULL))	n = AddressLookUp(self->right, &prefix[1]);
		// If the prefix ended or the are no more child nodes, this node is a leaf,
		// and we return whichever value is stored in it
		else return self->hop;	
	}
	// If the value returned is 0 (empty node) and we find a different value
	// "on our way up the tree", we adopt this value.
	if (n == 0) return self->hop;
	// Otherwise, we just "propagate" the value returned up the tree
	else return n;
}

// Main function
int main(int argc, char **argv){
	link root;
	char * aux;
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
		aux = fgets(linha, BUFFSIZE, stdin);
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

