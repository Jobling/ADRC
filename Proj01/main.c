#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITSIZE 8
#define BUFFSIZE 16

#define ADD 0
#define DEL 1
#define PRINT 2
#define TWOTREE 3
#define LOOKUP 4
#define HELP 5
#define EXIT 6

// Definição de estruturas de dados
typedef struct node * link;
typedef struct node{
	int hop; 
	link left;
	link right;
} node;

// Funções auxiliares
void PrintInterface(){
	printf("\n");
	printf("-------------------------------------------------------\n");
	printf("[COMMAND]\t\t- [ARGUMENTS]\n");
	printf("-------------------------------------------------------\n");
	printf("[ADD]\t\t- [Prefix (binary)] [Hop (int)]\n");
	printf("[DEL]\t\t- [Prefix (binary)]\n");
	printf("[PRINT]\t- [NULL]\n");
	printf("[TWOTREE]\t\t- [NULL]\n");
	printf("[LOOKUP]\t- [Prefix (binary)]\n");
	printf("[HELP]\n");
	printf("[EXIT]\n");
	printf("-------------------------------------------------------\n");
}
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
void MemoryCheck(link self);

// Funções relacionadas com a árvore
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
				self->left->hop = 0;
			}
			prefix++;
			AddPrefix(self->left, prefix, hop);
			break;
		case '1':
			// This bit points right
			if(self->right == NULL){
				self->right = (link) malloc(sizeof(node));
				self->right->hop = 0;
			}
			prefix++;
			AddPrefix(self->right, prefix, hop);
			break;
		default:
			// Not interpreted as a bit
			printf("Unsupported prefix. Not binary.\n");
			// There should be a memory verification here.
			exit(1);
	}
}
link ReadTable(char * filename){
	FILE *fp;
	link root;
	char *aux, linha[BUFFSIZE], prefix[BITSIZE + 1];
	int hop, n;
	
	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("File not found\n");
		exit(1);
	}
	
	aux = fgets(linha, BUFFSIZE, fp);
	if (aux == NULL){
		printf("Failed to read the file\n");
		exit(1);
	}
	
	// Creating tree with empty prefix (*). It's next-hop value will be 1
	root = (link) malloc(sizeof(node));
	root->hop = 1;
	root->left = NULL;
	root->right = NULL;
	
	while(aux != NULL){
		n = sscanf(linha, "%s %d", prefix, &hop);
		// Here we assume the prefix already comes in a binary form.
		// Ex: 11010110\0 (8 bit)		
		if(n != 2){
			printf("Invalid file format\n");
			// There should be a memory verification here.
			exit(1);
		}
		AddPrefix(root, prefix, hop);
		aux = fgets(linha, BUFFSIZE, fp);
	}
	
	fclose(fp);
	return root;
}
void DeletePrefix(link self, char * prefix){
	switch (prefix[0]){
		case '\0':
			// All out of bits
			self->hop = 0;
			break;
		case '0':
			// This bit points left
			if (self->left == NULL){
				printf("The selected prefix doesn't exist.\n");
			}else{				
				prefix++;
				DeletePrefix(self->left, prefix);
			}
			break;
		case '1':
			// This bit points right
			if(self->right == NULL){
				printf("The selected prefix doesn't exist.\n");
			}else{
				prefix++;
				DeletePrefix(self->right, prefix);
			}
			break;
		default:
			// Not interpreted as a bit
			printf("Unsupported prefix. Not binary.\n");
			// There should be a memory verification here.
			exit(1);
	}
}
void PrintTable(link self, char prefix[BITSIZE + 1], int level){
	if(level == 0) printf("%*c%4d\n", -BITSIZE, '*', self->hop);
	else 
		if(self->hop != 0) printf("%*s%4d\n", -BITSIZE, prefix, self->hop);

	if(self->left != NULL){
		prefix[level] = '0';
		level++;
		PrintTable(self->left, prefix, level);
		level--;
	}
	if(self->right != NULL){
		prefix[level] = '1';
		level++;
		PrintTable(self->right, prefix, level);
		level--;
	}
	
	prefix[level] = '\0';
	return;
}

// Ver se se faz depois
void TwoTree(link root);
int AddressLookUp(link self, char * prefix){
	int n;
	
	if((prefix[0] == '0') && (self->left != NULL)){
		prefix++;
		n = AddressLookUp(self->left, prefix); 
	}else{
		if((prefix[0] == '1') && (self->right != NULL)){
			prefix++;
			n = AddressLookUp(self->right, prefix);
		}else{
			return self->hop;	
		}
	}
	if (n == 0) return self->hop;
}

int main(int argc, char **argv){
	link root;
	char * aux;
	char filename[BUFFSIZE];
	char prefix[BITSIZE + 1];
	char command[BUFFSIZE];
	char linha[BUFFSIZE];
	int n = 0, hop = 0;
	
	// Leitura do ficheiro
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
		switch(getCMD(command)){
			case ADD:
				if(n != 3){
					printf("Invalid number of arguments for command ADD\n");
					continue;
				}
				AddPrefix(root, prefix, hop);
				break;
			case DEL:
				if(n != 2){
					printf("Invalid number of arguments for command DEL\n");
					continue;
				}
				DeletePrefix(root, prefix)
				break;
			case PRINT:
				PrintTable(root, prefix, 0);
				break;	
			case TWOTREE:
				break;	
			case LOOKUP:
				if(n != 2){
					printf("Invalid number of arguments for command LOOKUP\n");
				}
				printf("The next hop for %s is %d.", prefix, AddressLookUp(root, prefix));
				break;
			case HELP:
				PrintInterface();
				break;
			case EXIT:
				// There should be memory verification here
				exit(0);
				break;	
			default:
				break;
		}
	}
}

