#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
 * tail and head
*/
typedef struct edge{
	int tail;
	int head;
} * Edge;

/*
 * This structure holds information about the capacities of an edge
*/
typedef struct neighbor{
	int id;
	int c;
	struct neighbor * next;
} * link;

/*
 * Each node contains:
 * A list of adjacent nodes
 * And a index for the final predecessor (used when finding which 
 * nodes are needed to separate the graph)
*/
typedef struct NODE{
	int pred;
	link n;
} node;

/*
 * The graph should contain information about the
 * size of the network, the nodes (adjency list) 
 * and statistical information about the graph (flows)
 * if(i < NETSIZE) it represents an out node (+)
 * else it represents an inward node (-)
*/
typedef struct graph{
	int V;
	int E;
	int n_flow;
	int flow[NETSIZE];
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
 * Insert an edge into the graph
 * Produce a 'residual' graph preparation with vertex splitting
*/
void graphInsertE(Graph G, Edge e){
	if(G->list[e->tail].n == NULL){
		G->list[e->tail + NETSIZE].n = newNode(e->tail, 1, G->list[e->tail + NETSIZE].n);	// Apontar para tail (1)
		G->list[e->tail].n = newNode(e->tail + NETSIZE, 0, G->list[e->tail].n);				// Apontar para tail + NETSIZE (0)
		G->V++;
	}
	
	if(G->list[e->head].n == NULL){
		G->list[e->head + NETSIZE].n = newNode(e->head, 1, G->list[e->head + NETSIZE].n);	// Apontar para head (1)
		G->list[e->head].n = newNode(e->head + NETSIZE, 0, G->list[e->head].n);				// Apontar para head + NETSIZE (0)
		G->V++;
	}
	
	G->list[e->tail].n = newNode(e->head + NETSIZE, 1, G->list[e->tail].n);					// Apontar para head + NETSIZE (1)
	G->list[e->head].n = newNode(e->tail + NETSIZE, 1, G->list[e->head].n);					// Apontar para tail + NETSIZE (1)
	G->list[e->tail + NETSIZE].n = newNode(e->head, 0, G->list[e->tail + NETSIZE].n);		// Aponta para head (0)
	G->list[e->head + NETSIZE].n = newNode(e->tail, 0, G->list[e->head + NETSIZE].n);		// Aponta para tail (0)
	G->E++;
	free(e);
	return;
}

/*
 * Read the graph from a file and store it in a Graph named G
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
	G->n_flow = 0;
	for(i = 0; i < NETSIZE; i++){
		G->flow[i] = 0;
		
		G->list[i].pred = -1;
		G->list[i].n = NULL;
		
		G->list[i + NETSIZE].pred = -1;
		G->list[i + NETSIZE].n = NULL;
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
 * Function to be used after independent paths are found
 * to regain original graph capacities  
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
 * Print statistics
*/
void printResult(Graph G){
	int i = 0;
	int V_TOTAL = G->V * (G->V - 1)/2;
	
	for(i = 1; i < NETSIZE; i++)
		if(G->flow[i] != 0)
			printf("There are %-3d pairs of nodes separated by %-3d nodes [%-3.3f%%]\n", G->flow[i], i, (G->flow[i]*100.0/V_TOTAL));
	printf("There are %-3d pairs of nodes that are not separable [%-3.3f%%]\n", G->flow[0], (G->flow[0]*100.0/V_TOTAL));
	printf("-------------------------------------------------------\n");
}

// ------------------------------------------- Algorithms ------------------------------------------
/*
 * Max flow algorithm
 * On main() call EdmondsKarp(G, source, destination + NETSIZE)
 * Returns:
 * 	1 -> There is at least one independent path
 *  0 -> There is no path
*/
int EdmondsKarp(Graph G, int source, int destination){
	int q[2 * NETSIZE];		// BFS queue vector
	int p[2 * NETSIZE];		// Path auxiliary predecessor vector
	int flow = 0;
	int i, j;
	link aux;
	
	// Check if source and destination are 'in the graph'
	if(G->list[source].n == NULL || G->list[destination].n == NULL) return 0;
	
	// If the source and the destination are neighbors, they are
	// inseperable, and so there are no independent paths between them
	for(aux = G->list[source].n; aux != NULL; aux = aux->next)
		if(aux->id == destination){
			G->flow[0]++;
			return 0;
		}

	while(1){
		// Restarting
		memset(q, -1, 2 * NETSIZE * sizeof(int));
		memset(p, -1, 2 * NETSIZE * sizeof(int));
		q[0] = source;
		
		// ----------------------- BFS --------------------------------
		// For every node in the queue
		for(i = 0, j = 1, aux = NULL; i < j; i++){
			// For every neighbor of the current node
			for(aux = G->list[q[i]].n; aux != NULL; aux = aux->next){
				// If the neighbor wasn't visited (no pred), add to queue
				if(aux->c && p[aux->id] == -1 && aux->id != source){
					q[j] = aux->id;
					p[q[j]] = q[i];
					j++;
				} 
			}
			// If path to destination is found halfway BFS break
			if(p[destination] != -1) break;
		}
		// -------------------- END OF BFS ----------------------------
		
		// If after BFS search, the destination has no predecessor,
		// there are no more paths to the destination, on the residual
		// graph, so break from while(1)
		if(p[destination] == -1) break;
		
		// If a path is found, the capacity of its edges must be inverted
		// on the residual graph. So, for every node, starting on the
		// destination and ending on the source:
		for(i = destination; i != source; i = G->list[i].pred){
			// For every neighbor of the node
			for(aux = G->list[i].n; aux != NULL; aux = aux->next)
				// If the neighbor is the node's predecessor for this path
				// the capacity must be inverted (full capacity)
				if(aux->id == p[i]){
					aux->c = 1;
					break;
				}
			// For every neighbor of the node's predecessor	
			for(aux = G->list[p[i]].n; aux != NULL; aux = aux->next)
				// If the predecessor's neighbor is the node being 
				// 'targeted' the capacity must be inverted (no capacity)
				if(aux->id == i){
					aux->c = 0;
					break;
				}
			// For the sake of finding the nodes that might be removed 
			// to separate the graph, the predecessor values for each 
			// discovered path must be stored	
			G->list[i].pred = p[i];
		}
		// Since the capacities are all unitary, the flow is incremented
		flow++;
	}
	// Before exiting this function, the graph is left as it was read from
	// the file with the call of function reset(G)
	reset(G);
	
	// For the statistical component of the assignement, the total number 
	// of independent paths (alas, the flow) must be stored
	G->flow[flow]++;
	
	// Update flow
	G->n_flow = flow;
	
	// Return (flow != 0) [only 0 when there is no path]
	return (flow != 0);
}

/*
 * Function that gives an example of nodes to separate Graph 
 * and prints result on console
*/
void findNodes(Graph G, int source, int destination){
	int q[2 * NETSIZE];		// BFS queue vector
	int p[2 * NETSIZE];		// Path auxiliary predecessor vector
	int i, j, k, l;
	int * nodes;			// Vector used to store possible removable nodes
	link aux;
	
	// Vector containing nodes to be removed
	nodes = (int *) malloc(G->n_flow * sizeof(int));
	for(k = 0; k < G->n_flow; k++) nodes[k] = -1;
	
	memset(q, -1, 2 * NETSIZE * sizeof(int));
	memset(p, -1, 2 * NETSIZE * sizeof(int));
	
	// BFS
	for(q[0] = source, i = 0, j = 1, aux = NULL; i < j; i++){
		for(aux = G->list[q[i]].n; aux != NULL; aux = aux->next){
			if(aux->c && p[aux->id] == -1 && aux->id != source && aux->id != destination){
				if(G->list[aux->id].pred != -1){
					// Running through discovered nodes
					for(k = 0; k < G->n_flow; k++) 
						if(nodes[k] == -1 || nodes[k] == aux->id){
							nodes[k] = aux->id;
							break;
						}
					if(k == G->n_flow){
						for(l = G->list[aux->id].pred; l != source; l = G->list[l].pred){
							for(k = 0; k < G->n_flow; k++)
								if(nodes[k] == l){
									nodes[k] = aux->id;
									break;
								}
						}
					}
				}else{
					q[j] = aux->id;
					p[q[j]] = q[i];
					j++;
				}
			} 
		}
	}
	printf("To separate %-3d from %-3d remove nodes (%d paths):\n", source, destination, G->n_flow);
	for(k = 0; k < G->n_flow; k++) printf("--> %d\n", nodes[k] % NETSIZE);
	printf("-------------------------------------------------------\n");
	
	// Reset the paths so that no residual paths may produce  
	// errors on other iterations 
	for(i = 0; i < 2 * NETSIZE; i++) G->list[i].pred = -1;
	free(nodes); 
	
}

// ----------------------------------------------- Main --------------------------------------------
int main(int argc, char **argv){
	Graph G;
	char filename[BUFFSIZE];
	int source, destination;
	double elapsed_time;
	clock_t start, end;
	
	int VERBOSE = 0;

	// Obtaining filename from arguments
	switch(argc){
		case(4):
			if((sscanf(argv[2], "%d", &source) != 1) || (sscanf(argv[3], "%d", &destination) != 1)){
				printf("Inputs must be integers.\n");
				exit(1);
			}
		case(3):
			VERBOSE = 1;
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
	printf("There are %d nodes and %d edges on the 'residual' network!\n", 2*G->V, 4*G->E + 2*G->V);
	printf("-------------------------------------------------------\n");
	
	if(argc == 4){
		if(EdmondsKarp(G, source, destination + NETSIZE))
			findNodes(G, source, destination);
	}else{
		start = clock();
		for(source = 0; source < NETSIZE; source++)
			for(destination = source + 1; destination < NETSIZE; destination++)
				if(source != destination)
					if(EdmondsKarp(G, source, destination + NETSIZE) && VERBOSE)
						findNodes(G, source, destination);
				
		end = clock();
		elapsed_time = (double)(end - start)/CLOCKS_PER_SEC;
		printResult(G);
		printf("Algorithm takes %f seconds\n", elapsed_time);
		printf("-------------------------------------------------------\n");
	}
	
	// Free Memory allocated previously
	memoryCheck(G);
	exit(0);
}

