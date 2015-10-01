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
		if(n != 2){
			printf("Invalid file format\n");
			exit(1);
		}
		// Update tree
		aux = fgets(linha, BUFFSIZE, fp);
	}
}

void AddPrefix(link root, int prefix, int hop);
void DeletePrefix(link root, int prefix);
void PrintTable(link root);
void TwoTree(link root);
int AddressLookUp(int address);

int main(int argc, char **argv){
}
