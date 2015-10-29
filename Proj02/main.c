#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------- MACROS ---------------------------------------
/*
Since the node ID are positive integers
it is safe to assume negative integers as MACROS
*/
#define PROVIDER -1
#define PEER -2
#define CUSTOMER -3

#define BUFFSIZE 64

// ------------------------------------------- Data Structures -------------------------------------
typedef struct node * link;
typedef struct edge * Edge;

struct edge{
	long tail;
	long head;
	int relationship;
};

/*
Each node should contain an identifier
A pointer to the next node (in adjency list)
And a relationship identifier 
*/
struct node{
	long id;
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
	link * adj;
}Graph;

// ----------------------------------------------- Functions ---------------------------------------
/*
Create a new edge to be used in other functions
*/
Edge newE(long tail, long head, int relationship){
	Edge e = (Edge) malloc(sizeof(struct edge));

	e->tail = tail;
	e->head = head;
	e->relationship = relationship;

	return e;
}

/*
Create a new node and return it
As this is meant to be used to create a node 'v' for 
the adjancy list of node 'u', the relationship notation
will be the relationship 'vu'.
So, if 'u' is a provider of 'v', the relationship shall be
<CUSTOMER>.
*/
link newNode(long id, int relationship, link next){
	link v = (link) malloc(sizeof(struct node));

	v->id = id;
	switch(relationship){
		// 'u' is provider of 'v'/'this', so:
		case(1):
			v->relationship = CUSTOMER;
			break;
		// 'u' and 'v'/'this' are peers, so:
		case(2):
			v->relationship = PEER;
			break;
		// 'u' is a costumer of 'v'/'this' so:
		case(3):
			v->relationship = PROVIDER;
			break;
		// There was something wrong
		default:
			printf("Some kind of error occurred with the relationships\n");
			v->relationship = 0;
			break;
	}
	v->next = next;

	return v;
}

/*
Insert a relationship into the graph
Assuming no verification has been done yet.

POR FAZEEEEEER

void graphInsertE(Graph G, Edge e){

	for(i = 0; i < G->V; i++)


	free(e);
}
*/

/*
Read the graph from a file and store it in a Graph * named G
*/
Graph * readGraph(char * filename){
	FILE *fp;
	Graph * G;
	char linha[BUFFSIZE];
	long head, tail;
	int relationship;

	// Opening the file with a 'read' flag
	if ((fp = fopen(filename, "r")) == NULL){
		printf("File not found\n");
		exit(1);
	}

	// Creating graph with no nodes
	G = (Graph *) malloc(sizeof(Graph));
	G->V = 0;
	G->E = 0;
	// Perguntar ao professor como saber dimensoes da lista de adjacencias
	// OU usar realloc (em 4~5 sites so 1 e que disse 'mal')
	G->adj = NULL;

	// Node addition from file input
	while(fgets(linha, BUFFSIZE, fp) != NULL){
		/*
		Link goes from tail to head
		If relationship == 1 => tail is provider of head
		If relationship == 2 => tail and head are peers
		If relationship == 3 => tail is customer of head
		*/
		if(sscanf(linha, "%li %li %d", &tail, &head, &relationship) == 3)
			graphInsertE(G, newE(tail, head, relationship));
		else
			printf("Invalid line format\n");
	}

	// Closing file
	fclose(fp);
	return G;
}

// Main
