#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------------- MACROS and GLOBAL VARIABLES -------------------------------
/*
Using inverse notation from the project description
because of the usage.
*/
#define NO_ROUTE 4
#define PROVIDER 3
#define PEER 2
#define CUSTOMER 1
#define DESTINATION 0

#define BUFFSIZE 64
#define HOPSIZE 50
#define NETSIZE 65536

long N_PROVIDER = 0;
long N_PEER = 0;
long N_CUSTOMER = 0;
long N_UNUSABLE = 0;

// ------------------------------------------- Data Structures -------------------------------------
/*
 * Each path is caracterized by a path type (defined in the MACROS)
 * and a number of hops
 * Aditionally it is stored the id of the node associated with the 
 * path (prev_id)
*/
typedef struct path{
	int type;
	int hops;
	long prev_id;
} path;

/*
 * Each edge should contain an identifier for both
 * tail and head, and even the relationship from tail to head
*/
typedef struct edge{
	long tail;
	long head;
	int relationship;
} * Edge;

/*
 * Each node should contain an identifier
 * A pointer to the next node (in adjency list)
 * And a relationship identifier 
*/
typedef struct node{
	long id;
	int relationship;
	struct node * next;
} * link;

/*
 * The AS structure is the structure that will be stored as the 
 * first element on the adjency list. It represents the node, and the
 * path from the node to the target destination 
*/
typedef struct AS{
	path P;
	link next;
} AS;

/*
 * The graph should contain information about the
 * size of the network, and the relationships between
 * the nodes (adjency list) 
*/
typedef struct graph{
	long V;
	long E;
	long N_HOPS[HOPSIZE];
	AS list[NETSIZE];
} * Graph;

// ----------------------------------------------- Functions ---------------------------------------
/*
 * Create a new edge to be used in other functions
*/
Edge newE(long tail, long head, int relationship){
	Edge e = (Edge) malloc(sizeof(struct edge));

	e->tail = tail;
	e->head = head;
	e->relationship = relationship;

	return e;
}

/*
 * Create a new node to be used in graphInsertE
*/
link newNode(long id, int relationship, link next){
	link v = (link) malloc(sizeof(struct node));

	v->id = id;
	v->relationship = relationship;
	v->next = next;	

	return v;
}

/*
 * Insert a relationship into the graph
*/
void graphInsertE(Graph G, Edge e){
	if(G->list[e->tail].next == NULL)
		G->V++;
	G->list[e->tail].next = newNode(e->head, e->relationship, G->list[e->tail].next);
	G->E++;
	free(e);
	return;
}

/*
Function to evaluate current node Path and set a flag for subsequent calls of findPath
-1 -> Older path is better
 1 -> New path is better - Broadcast
 0 -> New path is better - Send to customers only 
*/
int setPath(path * P, int type, int hops){
	// This first switch is meant to know from what type of node is the information coming
	switch(type){
		// This case only happens for the destination, and it's immediatly taken as the best case
		// This result is broadcasted, hence return 1
		case(DESTINATION):
			P->type = type;
			P->hops = hops;
			return 1;
			
		// This case, excluding DESTINATION, is the best, so its path type is always taken.
		// Afterwards, if the previous stored path was also a CUSTOMER path, it's needed to
		// check for the best number of hops.
		case(CUSTOMER):
			switch(P->type){
				// If the node has the same type of path, it must test for number of hops
				// If the path is better, it should broadcast it
				case(CUSTOMER):
					if(hops < P->hops){
						P->hops = hops;
						return 1;
					}

				// If the node is the DESTINATION or its current path is better, return -1
				case(DESTINATION):
					return -1;

				// Only nodes with current NO_ROUTE, PROVIDER and PEER paths get here	
				default:
					P->type = type;
					P->hops = hops;
					return 1;
			}
			
		// For this case, the same happens as with the previous case, but on a worst case: PEER.
		// If the previous stored case is better than PEER, there is no use in broadcasting the information
		// The only possible return types for this case are -1: path is worse or 0: path is better, send to 
		// customers only
		case(PEER):
			switch(P->type){		
				// If the node has the same type of path, it must test for number of hops
				// If the path is better, it should only send the path to CUSTOMERS (hence return 0)
				case(PEER):
					if(hops < P->hops){
						P->hops = hops;
						return 0;
					}

				// If the path type is already better, 
				// or same type and worse number of hops then do nothing (hence return -1)
				case(CUSTOMER):
				case(DESTINATION):
					return -1;
				
				// Only nodes with current NO_ROUTE and PROVIDER paths get here	
				default:
					P->type = type;
					P->hops = hops;
					return 0;	
			}
			
		// Really similar to the PEER case. Before counting the number of different paths, this used to be the same
		// if clause. Now it's not as easy to do so.	
		case(PROVIDER):
			switch(P->type){
				// If the node has the same type of path, it must test for number of hops
				// If the path is better, it should only send the path to CUSTOMERS (hence return 0)
				case(PROVIDER):
					if(hops < P->hops){
						P->hops = hops;
						return 0;					
					}

				// If the path type is already better, 
				// or same type and worse number of hops then do nothing (hence return -1)	
				case(PEER):
				case(CUSTOMER):
				case(DESTINATION):
					return -1;

				// Only nodes with current NO_ROUTE get here
				default:
					P->type = type;
					P->hops = hops;
					return 0;
			}

	}
	// No case should return here. If it returns, prints out a warning, and it doesn't broadcast, so that 
	// the error can not be propagated
	printf("An exception as occured with Path: type-%d hops-%d\n", P->type, P->hops);
	printf("and type-%d hops-%d\n", type, hops);
	return -1;
}
 
/*
 * Recursive algorithm that finds the type and number of hops for
 * a path to a given destination node
*/
void findPath(Graph G, int relationship, int n, long id, long prev_id){
	link aux;
	int broadcast = 0;

	// If the same neighbor is sending new information, it's because its
	// information is better now. 
	if(G->list[id].P.prev_id == prev_id && prev_id != -1) G->list[id].P.hops = n;
	
	broadcast = setPath(&(G->list[id].P), relationship, n);
	
	if (broadcast == -1) 
		return;
	else{
		G->list[id].P.prev_id = prev_id;
		n++;
		for(aux = G->list[id].next; aux != NULL; aux = aux->next)
			if(aux->id == prev_id)
				continue;
			else
				if(aux->relationship == CUSTOMER)
					findPath(G, PROVIDER, n, aux->id, id);
				else if((aux->relationship == PEER) && (broadcast == 1))
					findPath(G, PEER, n, aux->id, id);
				else if((aux->relationship == PROVIDER) && (broadcast == 1))
					findPath(G, CUSTOMER, n, aux->id, id);	
	}
	
}

/*
 * Read the graph from a file and store it in a Graph * named G
*/
Graph readGraph(char * filename){
	FILE *fp;
	Graph G;
	char linha[BUFFSIZE];
	long head, tail;
	long i;
	int relationship;

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
		G->list[i].next = NULL;
		G->list[i].P.hops = -1;
		G->list[i].P.prev_id = -1;
		G->list[i].P.type = NO_ROUTE;
	}
	for(i = 0; i < HOPSIZE; i++) G->N_HOPS[i] = 0;
	
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
	long i;
	for(i = 0; i < NETSIZE; i++)
		if(G->list[i].next != NULL)
			delLinks(G->list[i].next);
	free(G);
}

/*
 * This function prevents the usage of 'leftover values' from previous
 * calls to findPath 
*/
void memoryReset(Graph G){
	long i;
	for(i = 0; i < NETSIZE; i++){
		if(G->list[i].next != NULL){
			switch(G->list[i].P.type){
				case(CUSTOMER):
					N_CUSTOMER++;
					break;
				case(PEER):
					N_PEER++;
					break;
				case(PROVIDER):
					N_PROVIDER++;
					break;
				case(DESTINATION): break;
				default: N_UNUSABLE++;
			}
			G->N_HOPS[G->list[i].P.hops]++; 
			
			G->list[i].P.hops = -1;
			G->list[i].P.prev_id = -1;
			G->list[i].P.type = NO_ROUTE;
		}
	}
}

/*
 * Function used to print the list of the path_types
*/
void printResult(Graph G, long destination){
	long i;
	printf("Node\t\tPath (Type, Hops)\n");
	for(i = 0; i < NETSIZE; i++)
		if(G->list[i].next != NULL)
			switch(G->list[i].P.type){
				case(DESTINATION):
					printf("%-5li\t\t(DESTINATION, %2d)\n", i, G->list[i].P.hops);
					break;
				case(CUSTOMER):
					printf("%-5li\t\t(CUSTOMER,    %2d)\n", i, G->list[i].P.hops);
					N_CUSTOMER++;
					break;
				case(PEER):
					printf("%-5li\t\t(PEER,        %2d)\n", i, G->list[i].P.hops);
					N_PEER++;
					break;
				case(PROVIDER):
					printf("%-5li\t\t(PROVIDER,    %2d)\n", i, G->list[i].P.hops);
					N_PROVIDER++;
					break;
				case(NO_ROUTE):
					printf("%-5li\t\t(UNUSABLE,    %2d)\n", i, G->list[i].P.hops);
					N_UNUSABLE++;
					break;
				default:
					printf("Something wrong happened with the path type resolution\n");
					break;
			}
	printf("---------------\n");
	printf("Found %li Provider paths, %li Peer paths, %li Customer paths and %li Unusable Paths to %li\n", 
					N_PROVIDER, N_PEER, N_CUSTOMER, N_UNUSABLE, destination);
}

/*
 * Function used to print statistics from a given graph
*/
void printStat(Graph G){
	int i;
	long total = (G->V * (G->V - 1));
	
	printf("In %li paths there are:\n", (G->V * (G->V - 1)));
	printf("Provider Paths:\t %-5li [%-3.1f\%%]\n", N_PROVIDER, (N_PROVIDER * 100.0)/total);
	printf("Peer Paths:\t %-5li [%-3.1f\%%]\n", N_PEER, (N_PEER * 100.0)/total);
	printf("Customer Paths:\t %-5li [%-3.1f\%%]\n", N_CUSTOMER, (N_CUSTOMER * 100.0)/total);
	printf("Unusable Paths:\t %-5li [%-3.1f\%%]\n", N_UNUSABLE, (N_UNUSABLE * 100.0)/total);
	printf("--------------------------------------------\n");
	for(i = 0; i < HOPSIZE; i++)
		if(G->N_HOPS[i] != 0)
			printf("There are %-5li [%-3.1f\%%] nodes distanced by %d hops\n", G->N_HOPS[i], (G->N_HOPS[i] * 100.0)/total, i);
}

// ----------------------------------------------- Main --------------------------------------------
int main(int argc, char **argv){
	Graph G;
	char filename[BUFFSIZE];
	long destination;
	long i;

	// Obtaining filename from arguments
	if(argc < 2 || argc > 3){
		printf("Wrong number of arguments.\n");
		exit(1);
	}
	sscanf(argv[1], "%s", filename);

	// Reading Graph
	printf("Reading file\n");
	G = readGraph(filename);
	
	printf("The file %s was loaded successfully to the Graph\n", filename);
	printf("There are %li nodes and %li edges!\n", G->V, G->E);
	
	// In case there is a second argument, it is the target destination
	if(argc == 3){
		if(sscanf(argv[2], "%li", &destination) != 1){
			printf("Destination must be a integer!\n");
			memoryCheck(G);
			exit(0);
		}
		findPath(G, DESTINATION, 0, destination, -1);
		printResult(G, destination);
	// Otherwise, the algorithm is run for every possible destination,
	// and the network statistics are generated (and printed)
	}else{
		for(i = 0; i < NETSIZE; i++){
			if(G->list[i].next != NULL){
				findPath(G, DESTINATION, 0, i, -1);
				memoryReset(G);
			}
		}
		printStat(G);
	}
	
	// Free Memory allocated previously
	memoryCheck(G);
	exit(0);
}

