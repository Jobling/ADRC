#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>
/*
CLOCK USAGE
clock_t start, end;
int elapsed_time;

start = clock();
-- ALGORITHM
end = clock();

elapsed_time = (double)(end - start)/CLOCKS_PER_SEC;
*/

// ------------------------------------- MACROS and GLOBAL VARIABLES -------------------------------
/*
Using inverse notation from the project description
because of the usage.
*/
#define BUFFSIZE 64
#define NETSIZE 100

// ------------------------------------------- Data Structures -------------------------------------
/*
 * Each edge should contain an identifier for both
 * tail and head, and even the relationship from tail to head
*/
typedef struct edge{
	long tail;
	long head;
} * Edge;

/*
 * Each node should contain an identifier
 * A pointer to the next node (in adjency list)
 * And a relationship identifier 
*/
typedef struct node{
	long id;
	link next;
} * link;

/*
 * The graph should contain information about the
 * size of the network, and the relationships between
 * the nodes (adjency list) 
*/
typedef struct graph{
	int V;
	int E;
	link in[NETSIZE];
	link out[NETSIZE];
} * Graph;

// ----------------------------------------------- Functions ---------------------------------------
/*
 * Create a new edge to be used in other functions
*/
Edge newE(long tail, long head){
	Edge e = (Edge) malloc(sizeof(struct edge));

	e->tail = tail;
	e->head = head;
	
	return e;
}

/*
 * Create a new node to be used in graphInsertE
*/
link newNode(long id, link next){
	link v = (link) malloc(sizeof(struct node));

	v->id = id;
	v->next = next;	

	return v;
}

/*
 * Insert a relationship into the graph
*/
void graphInsertE(Graph G, Edge e){
	if(G->out[e->tail].next == NULL) G->V++;
	if(G->out[e->head].next == NULL) G->V++;

	G->out[e->tail].next = newNode(e->head, G->out[e->tail].next);
	G->out[e->head].next = newNode(e->tail, G->out[e->head].next);
	G->E += 2;
	free(e);
	return;
}

/*
 * Read the graph from a file and store it in a Graph * named G
*/
Graph readGraph(char * filename){
	FILE *fp;
	Graph G;
	char linha[BUFFSIZE];
	int head, tail, i;

	// Opening the file with a 'read' flag
	if ((fp = fopen(filename, "r")) == NULL){
		printf("File not found\n");
		exit(1);
	}

	// Creating graph with no nodes
	G = (Graph) malloc(sizeof(struct graph));
	G->V = 0;
	G->E = 0;
	for(i = 0; i < NETSIZE; i++){
		G->in[i].id = i;
		G->in[i].next = newNode(i, NULL);
		G->E++;

		G->out[i].id = i;
		G->out[i].next = NULL;
	}

	// Node addition from file input
	while(fgets(linha, BUFFSIZE, fp) != NULL){
		if(sscanf(linha, "%d %d", &tail, &head) == 2)
			graphInsertE(G, newE(tail, head));
		else
			printf("Invalid line format\n");
	}

	// Closing file
	fclose(fp);
	return G;
}

/*
 * Auxiliary function used when deleting all the entries for the adjancy list
*/
void delLinks(link l){
	if(l->next != NULL){
		delLinks(l->next);
		free(l);
		l = NULL;
	}
}

/*
 * Function used to free used memory when exiting cleanly from the program.
*/
void memoryCheck(Graph G){
	int i;
	for(i = 0; i < NETSIZE; i++)
		if(G->list[i].next != NULL)
			delLinks(G->list[i].next);
	free(G);
}

// ----------------------------------------------- Main --------------------------------------------
int main(int argc, char **argv){
	Graph G;
	char filename[BUFFSIZE];
	int i;

	// Obtaining filename from arguments
	if(argc != 2){
		printf("Wrong number of arguments.\n");
		exit(1);
	}
	sscanf(argv[1], "%s", filename);

	// Reading Graph
	G = readGraph(filename);

	printf("-------------------------------------------------------\n");
	printf("There are %d nodes and %d edges!\n", G->V, G->E);
	printf("-------------------------------------------------------\n");

	// Free Memory allocated previously
	memoryCheck(G);
	exit(0);
}

