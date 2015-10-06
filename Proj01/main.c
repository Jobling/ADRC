#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITSIZE 8
#define BUFFSIZE 16

typedef struct node * link;
typedef struct node{
	int hop; 
	link left;
	link right;
} node;

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
	return root;
}

void DeletePrefix(link root, char * prefix);

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

void TwoTree(link root);
int AddressLookUp(char * prefix);

int main(int argc, char **argv){
	char filename[8];
	link root;
	char prefix[BITSIZE + 1];
	memset(prefix, '\0', BITSIZE + 1);
	
	if(argc == 1){
		printf("Missing file argument\n");
		exit(1);
	}
	
	sscanf(argv[1], "%s", filename);
	
	root = ReadTable(filename);
	PrintTable(root, prefix, 0);
	
	
exit(0);
}

