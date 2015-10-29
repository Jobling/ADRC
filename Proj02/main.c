#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MACROS
/*
Since the node ID are positive integers
it is safe to assume negative integers as MACROS
*/
#define PROVIDER -1
#define PEER -2
#define CUSTOMER -3

#define BUFFSIZE 64

// Data Structures
typedef struct node * link;

/*
Each node should contain an identifier
A pointer to the next node (in adjency list)
And a relationship identifier 
*/
struct node{
	int id;
	int relationship;
	link next;
};
/* 
The graph should contain information about the
size of the network, and the relationships between
the nodes (adjency list) 
*/
typedef struct{
	int V;
	int E;
	list * adj;
} * Graph;

/*
Create a new node and return it
As this is meant to be used to create a node 'v' for 
the adjancy list of node 'u', the relationship notation
will be the relationship 'vu'.
So, if 'u' is a provider of 'v', the relationship shall be
<CUSTOMER>.
*/
link newNode(int id, int relationship, link next){
	
}
/*
Read the graph from a file and store it in a Graph * named G
*/
Graph readGraph(char * filename){
	FILE *fp;
	Graph G;
	char linha[BUFFSIZE];
	char * aux;
	int head, tail, relationship;
	
	// Opening the file with a 'read' flag
	if ((fp = fopen(filename, "r")) == NULL){
		printf("File not found\n");
		exit(1);
	}
	
	// Creating graph with no nodes
	G = (Graph) malloc(sizeof(*Graph));
	G->V = 0;
	G->E = 0;
	// Perguntar ao professor como saber dimensoes da lista de adjacencias
	// OU usar realloc (em 4~5 sites so 1 e que disse 'mal')
	G->adj = NULL;
	
	// Node addition from file input
	while(fgets(linha, BUFFSIZE, fp) != NULL){
		n = sscanf(linha, "%d %d %d", &tail, &head, &relationship);
		/*
		Link goes from tail to head
		If relationship == 1 => tail is provider of head
		If relationship == 2 => tail and head are peers
		If relationship == 3 => tail is customer of head
		*/
		if(n != 3) printf("Invalid line format\n");
		else 
	}
	
	// Closing file
	fclose(fp);
	return G;
}

// Main
