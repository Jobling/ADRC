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

node ReadTable(char * filename){
	FILE *fp;
	char *aux, *linha, *prefix;
	int hop, n;
	
	fp = fopen(filename);
	if (fp == NULL){
		printf("File not found\n");
		exit(1);
	}
	aux = fgets(linha, BUFFSIZE, fp);
	if (aux == NULL){
		printf("Failed to read the file\n");
		exit(1);
	}
	// Create tree, with empty prefix (*)
	while(aux != NULL){
		n = sscanf(linha, "%s %d", prefix, &hop);
		// Here we assume the prefix already comes in a binary form.
		// Ex: 11010110\0 (8 bit)		
		if(n != 2){
			printf("Invalid file format\n");
			exit(1);
		}
		// AddPrefix()?
		aux = fgets(linha, BUFFSIZE, fp);
	}
}

void AddPrefix(link self, char * prefix, int hop){	
	switch (prefix[0]){
		case '\n':
			// All out of bits
			self->hop = hop;
			break;
		case '0':
			// This bit points left
			if (self->left == NULL){
				self->left = (link) malloc(sizeof(node));
			}
			AddPrefix(self->left, prefix++, hop);
			break;
		case '1':
			// This bit points right
			if(self->right == NULL){
				self->right = (link) malloc(sizeof(node));
			}
			AddPrefix(self->right, prefix++, hop);
			break;
		default:
			// Not interpreted as a bit
			printf("Unsupported prefix. Not binary.\n");
			// There should be a memory verification here.
			exit(1);
			break;
	}
}

void DeletePrefix(link root, char prefix);
void PrintTable(link root);
void TwoTree(link root);
int AddressLookUp(char prefix);

int main(int argc, char **argv){
}
