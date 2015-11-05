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
#define NETSIZE 65536

long N_PROVIDER = 0;
long N_PEER = 0;
long N_CUSTOMER = 0;
long N_UNUSABLE = 0;

// ------------------------------------------- Data Structures -------------------------------------
typedef struct node * link;
typedef struct edge * Edge;

/*
Each path is caracterized by a path type (defined in the MACROS)
and a number of hops
Eventually it will be stored in a matrix, or it will hold the id of
the destination
*/
typedef struct{
	int type;
	int hops;
} path;

/*
Each edge should contain an identifier for both
tail and head, and even the relationship from tail to head
*/
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

typedef struct{
	path P;
	link next;
} AS;

/*
The graph should contain information about the
size of the network, and the relationships between
the nodes (adjency list) 
*/
typedef struct{
	long V;
	long E;
	AS list[NETSIZE];
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
*/
link newNode(long id, int relationship, link next){
	link v = (link) malloc(sizeof(struct node));

	v->id = id;
	v->relationship = relationship;
	v->next = next;	

	return v;
}

/*
Insert a relationship into the graph
Assuming no verification has been done yet.
*/
void graphInsertE(Graph * G, Edge e){
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
	switch(type){
		case(DESTINATION):
			P->type = type;
			P->hops = hops;
			return 1;
			
		case(CUSTOMER):
			switch(P->type){
				case(PROVIDER):
					N_PROVIDER--;
					break;
				case(PEER):
					N_PEER--;
					break;
				case(CUSTOMER):
					if(hops < P->hops){
						P->hops = hops;
						return 1;
					}
				case(DESTINATION):
					return -1;
			}
			// Only nodes with current NO_ROUTE, PROVIDER and PEER paths get here
			P->type = type;
			P->hops = hops;
			N_CUSTOMER++;
			return 1;
			
		case(PEER):
			switch(P->type){
				case(PROVIDER):
					N_PROVIDER--;
					break;
				case(PEER):
					if(hops < P->hops){
						P->hops = hops;
						return 0;
					}
				case(CUSTOMER):
				case(DESTINATION):
					return -1;
			}
			// Only nodes with current NO_ROUTE and PROVIDER
			P->type = type;
			P->hops = hops;
			N_PEER++;
			return 0;
			
		case(PROVIDER):
			switch(P->type){
				case(PROVIDER):
					if(hops < P->hops){
						P->hops = hops;
						return 0;					
					}else return -1;
				case(PEER):
				case(CUSTOMER):
				case(DESTINATION):
					return -1;
			}
			// Only nodes with current NO_ROUTE
			P->type = type;
			P->hops = hops;
			N_PROVIDER++;
			return 0;
	}
	return 2;
}
 
/*
Recursive algorithm that finds the type of path to a given node
Eventualy we shall pass a 'hop' value as argument
*/
void findPath(Graph * G, long id, int relationship, int n){
	link aux;
	int broadcast = 0;

	broadcast = setPath(&(G->list[id].P), relationship, n);
	if (broadcast == -1) return;
	if (broadcast == 2){ 
		printf("Something wrong with %li %d %d\n", id, relationship, n);
		exit(0);
	}
	n++;
	for(aux = G->list[id].next; aux != NULL; aux = aux->next)
		if(aux->relationship == CUSTOMER)
			findPath(G, aux->id, PROVIDER, n);
		else if((aux->relationship == PEER) && (broadcast == 1))
			findPath(G, aux->id, PEER, n);
		else if((aux->relationship == PROVIDER) && (broadcast == 1))
			findPath(G, aux->id, CUSTOMER, n);

}

/*
Read the graph from a file and store it in a Graph * named G
*/
Graph * readGraph(char * filename){
	FILE *fp;
	Graph * G;
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
	G = (Graph *) malloc(sizeof(Graph));
	G->V = 0;
	G->E = 0;
	for(i = 0; i < NETSIZE; i++){
		G->list[i].next = NULL;
		G->list[i].P.hops = -1;
		G->list[i].P.type = NO_ROUTE;
	}
	
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
Auxiliary function used when deleting all the entries for the adjancy list
*/
void delLinks(link l){
	if(l->next != NULL){
		delLinks(l->next);
		free(l);
		l = NULL;
	}
}

/*
Function used to free used memory when exiting cleanly from the program.
*/
void memoryCheck(Graph * G){
	long i;
	for(i = 0; i < NETSIZE; i++)
		if(G->list[i].next != NULL)
			delLinks(G->list[i].next);
	free(G);
}

/*
This function prevents the usage of 'leftover values' from previous
calls to findPath 
*/
void memoryReset(Graph * G){
	long i;
	for(i = 0; i < NETSIZE; i++){
		if(G->list[i].next != NULL){
			G->list[i].P.hops = -1;
			G->list[i].P.type = NO_ROUTE;
		}
	}
}

/*
Function used to print the list of the path_types
*/
void printResult(Graph * G){
	long i;
	printf("Node\t\t\tPath (Type, Hops)\n");
	for(i = 0; i < NETSIZE; i++)
		if(G->list[i].next != NULL)
			switch(G->list[i].P.type){
				case(DESTINATION):
					printf("%-5li (DESTINATION, %2d)\n", i, G->list[i].P.hops);
					break;
				case(CUSTOMER):
					printf("%-5li (CUSTOMER,    %2d)\n", i, G->list[i].P.hops);
					break;
				case(PEER):
					printf("%-5li (PEER,        %2d)\n", i, G->list[i].P.hops);
					break;
				case(PROVIDER):
					printf("%-5li (PROVIDER,    %2d)\n", i, G->list[i].P.hops);
					break;
				case(NO_ROUTE):
					printf("%-5li (UNUSABLE,    %2d)\n", i, G->list[i].P.hops);
					break;
				default:
					printf("Something wrong happened with the path type resolution\n");
					break;
			}
}


// ----------------------------------------------- Main --------------------------------------------
int main(int argc, char **argv){
	Graph * G;
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
	
	// Doing some magic here
	if(argc == 3){
		if(sscanf(argv[2], "%li", &destination) != 1){
			printf("Destination must be a long integer!\n");
			memoryCheck(G);
			exit(0);
		}
		findPath(G, destination, DESTINATION, 0);
		printResult(G);
	}else{
		for(i = 0; i < NETSIZE; i++){
			if(G->list[i].next != NULL){
				findPath(G, i, DESTINATION, 0);
				memoryReset(G);
			}
		}
		N_UNUSABLE = (G->V * (G->V - 1)) - (N_PROVIDER + N_PEER + N_CUSTOMER);
	}
	
	printf("Found %li Provider paths, %li Peer paths, %li Customer paths and %li Unusable Paths\n", N_PROVIDER, N_PEER, N_CUSTOMER, N_UNUSABLE);
	memoryCheck(G);
	exit(0);
}
