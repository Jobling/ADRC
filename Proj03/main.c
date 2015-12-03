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
	int tail;
	int head;
} * Edge;

/*
 * This structure holds information about the capacities of a node
*/
typedef struct neighbor{
	int id;
	int c;
	struct neighbor * next;
} * link;

/*
 * Each node should contain an identifier
 * A pointer to the next node (in adjency list)
 * And a relationship identifier 
*/
typedef struct NODE{
	int pred;
	link n;
} node;

/*
 * The graph should contain information about the
 * size of the network, and the relationships between
 * the nodes (adjency list) 
 * if(i < NETSIZE) it represents an out node (+)
 * else it represents an inward node (-)
*/
typedef struct graph{
	int V;
	int E;
	int max_flow;
	node list[2 * NETSIZE];
} * Graph;

// ----------------------------------------------- Functions ---------------------------------------
/*
 * Create a new edge to be used in other functions
*/
Edge newE(int tail, int head){
	Edge e = (Edge) malloc(sizeof(struct edge));

	e->tail = tail;
	e->head = head;
	
	return e;
}

/*
 * Create a new node to be used in graphInsertE
*/
link newNode(int id, int c, link next){
	link v = (link) malloc(sizeof(struct neighbor));
	v->id = id;
	v->c = c;
	v->next = next;	

	return v;
}

/*
 * Insert a relationship into the graph
*/
void graphInsertE(Graph G, Edge e){
	if(G->list[e->tail].n == NULL){
		G->list[e->tail + NETSIZE].n = newNode(e->tail, 1, G->list[e->tail + NETSIZE].n);	// Apontar para tail (1)
		G->list[e->tail].n = newNode(e->tail + NETSIZE, 0, G->list[e->tail].n);				// Apontar para tail + NETSIZE (0)
		G->V += 2;
		G->E += 2;
	}
	if(G->list[e->head].n == NULL){
		G->list[e->head + NETSIZE].n = newNode(e->head, 1, G->list[e->head + NETSIZE].n);	// Apontar para head (1)
		G->list[e->head].n = newNode(e->head + NETSIZE, 0, G->list[e->head].n);				// Apontar para head + NETSIZE (0)
		G->V += 2;
		G->E += 2;
	}
	G->list[e->tail].n = newNode(e->head + NETSIZE, 1, G->list[e->tail].n);				// Apontar para head + NETSIZE (1)
	G->list[e->head].n = newNode(e->tail + NETSIZE, 1, G->list[e->head].n);				// Apontar para tail + NETSIZE (1)
	G->list[e->tail + NETSIZE].n = newNode(e->head, 0, G->list[e->tail + NETSIZE].n);		// Aponta para head (0)
	G->list[e->head + NETSIZE].n = newNode(e->tail, 0, G->list[e->head + NETSIZE].n);		// Aponta para tail (0)
	G->E += 4;
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
	G->max_flow = 0;
	for(i = 0; i < 2*NETSIZE; i++){
		G->list[i].pred = -1;
		G->list[i].n = NULL;
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
	for(i = 0; i < 2*NETSIZE; i++)
		if(G->list[i].n != NULL)
			delLinks(G->list[i].n);
	free(G);
}

/*
 * Between iterations 
*/
void reset(Graph G){
	int i;
	link aux;
	for(i = 0; i < 2 * NETSIZE; i++)
		for(aux = G->list[i].n; aux != NULL; aux = aux->next)
			if(i < NETSIZE)
				if(aux->id == i + NETSIZE) aux->c = 0;
				else aux->c = 1;
			else
				if(aux->id == i - NETSIZE) aux->c = 1;
				else aux->c = 0;
}

/*
 * Max flow algorithm
 * On main call EdmondsKarp(G, source, destination + NETSIZE)
*/
int EdmondsKarp(Graph G, int source, int destination){
	int q[2 * NETSIZE];
	int flow = 0;
	int i, j;
	link aux;
	
	if(G->list[source].n == NULL) return 0;
	for(aux = G->list[source].n; aux != NULL; aux = aux->next)
		if(aux->id == destination)
			return 0;
	
	G->list[source].pred = -5;

	while(1){
		// Restarting
		memset(q, -1, 2 * NETSIZE * sizeof(int));
		i = 0;
		j = 1;
		aux = NULL;
		
		q[i] = source;
		// BFS
		while(i < j){
			for(aux = G->list[q[i]].n; aux != NULL; aux = aux->next){
				if(aux->c && G->list[aux->id].pred == -1){
					q[j] = aux->id;
					G->list[q[j]].pred = q[i];
					j++;
				} 
			}
			if(G->list[destination].pred != -1) break;
			i++;
		}
		// No more paths left
		if(G->list[destination].pred == -1) break;
		
		// Invert all edges
		for(i = destination; i != source; i = G->list[i].pred){
			for(aux = G->list[i].n; aux != NULL; aux = aux->next){
				if(aux->id == G->list[i].pred){
					aux->c = 1;
					break;
				}
			}
			for(aux = G->list[G->list[i].pred].n; aux != NULL; aux = aux->next){
				if(aux->id == i){
					aux->c = 0;
					break;
				}
			}
		}
		for(i = 0; i <= j; i++) G->list[q[i]].pred = -1;
		flow++;
	}
	for(i = 0; i <= j; i++) G->list[q[i]].pred = -1;
	return flow;
}

// ----------------------------------------------- Main --------------------------------------------
int main(int argc, char **argv){
	Graph G;
	char filename[BUFFSIZE];
	int source, destination;
	int nodes;

	// Obtaining filename from arguments
	switch(argc){
		case(4):
			sscanf(argv[2], "%d", &source);
			sscanf(argv[3], "%d", &destination);
		case(2):
			sscanf(argv[1], "%s", filename);
			break;
		default:
			printf("Wrong number of arguments.\n");
			exit(1);
	}
	

	// Reading Graph
	G = readGraph(filename);

	printf("-------------------------------------------------------\n");
	printf("There are %d nodes and %d edges!\n", G->V, G->E);
	printf("-------------------------------------------------------\n");
	
	if(argc == 4){
		printf("To separate %d from %d --> Remove %d nodes\n", source, destination, EdmondsKarp(G, source, destination + NETSIZE));
	}else{
		for(source = 0; source < NETSIZE - 1; source++){
			for(destination = source + 1; destination < NETSIZE; destination++){
				if(source != destination){
					if((nodes = EdmondsKarp(G, source, destination + NETSIZE)) != 0){
						printf("To separate %d from %d --> Remove %d nodes\n", source, destination, nodes);
						reset(G);
					}
				}
			}
		}
	}
	
	// Free Memory allocated previously
	memoryCheck(G);
	exit(0);
}

