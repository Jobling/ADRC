#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------- MACROS ---------------------------------------
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
	long id;
	path P;
	link next;
} AS;

/*
The graph should contain information about the
size of the network, and the relationships between
the nodes (adjency list) 
*/
typedef struct{
	int V;
	int E;
	AS ** list;
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

AS * newAS(long id){
	AS * a = (AS *) malloc(sizeof(AS));
	
	a->id = id;
	a->P.type = NO_ROUTE;
	a->P.hops = -1;
	a->next = NULL;
	
	return a;
}

/*
Insert a relationship into the graph
Assuming no verification has been done yet.
*/
void graphInsertE(Graph * G, Edge e){
	int i;
	for(i = 0; i < G->V; i++){
		// If there is a tail node in the list, we just add the head node
		if(G->list[i]->id == e->tail){
			G->list[i]->next = newNode(e->head, e->relationship, G->list[i]->next);
			G->E++;
			free(e);
			return;
		}
	}
	
	// There is no tail node
	// Creating the tail node
	G->V++;
	if((G->list = (AS **) realloc(G->list, G->V * sizeof(AS *))) == NULL){
			printf("Dynamic memory allocation error (realloc)\n");
			exit(1);
	}
	// Adding tail node
	G->list[G->V - 1] = newAS(e->tail);
	// Adding head node
	G->list[G->V - 1]->next = newNode(e->head, e->relationship, G->list[G->V - 1]->next);
	G->E++;

	free(e);
}

/*
Recursive algorithm that finds the type of path to a given node
Eventualy we shall pass a 'hop' value as argument
*/
void findPath(Graph * G, long id, int relationship, int n){
	int i;
	link aux;
	AS * l = NULL;
	
	// Getting targeted node's neighbors
	for(i = 0; i < G->V; i++)
		if(G->list[i]->id == id)
			l = G->list[i];
		
	if(l == NULL){
		printf("There was some kind of error while finding path type\n");
		return;
	}
	if((relationship == DESTINATION) || (relationship == CUSTOMER)){
		l->P.type = relationship;
		if((l->P.hops == -1) || (l->P.hops < n)) l->P.hops = n;
		n++;
		for(aux = l->next; aux != NULL; aux = aux->next)
			switch(aux->relationship){
				case(CUSTOMER):
					findPath(G, aux->id, PROVIDER, n);
					break;
				case(PEER):
					findPath(G, aux->id, PEER, n);
					break;
				case(PROVIDER):
					findPath(G, aux->id, CUSTOMER, n);
					break;
				default:
					printf("Some kind of error occurred with the relationships while finding path type [recursive]\n");
					break;
			}
	}else{
		if(l->P.type > relationship){ 
			l->P.type = relationship;
			l->P.hops = n;
		}else if((l->P.type == relationship) && (l->P.hops > n))
			l->P.hops = n;
		n++;
		for(aux = l->next; aux != NULL; aux = aux->next)
			if(aux->relationship == CUSTOMER)
				findPath(G, aux->id, PROVIDER, n);	
	}
}

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
	// A memória para a lista será alocada dinamicamente, posteriormente
	G->list = NULL;

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
	int i;
	for(i = 0; i < G->V; i++){ 
		delLinks(G->list[i]->next);
		free(G->list[i]);
	}
	free(G->list);
	free(G);
}

/*
Function used to print the list of the path_types
*/
void printResult(Graph * G){
	int i;
	printf("Node\t\t\tPath (Type, Hops)\n");
	for(i = 0; i < G->V; i++)
		switch(G->list[i]->P.type){
			case(DESTINATION):
				printf("%-5li (DESTINATION, %2d)\n", G->list[i]->id, G->list[i]->P.hops);
				break;
			case(CUSTOMER):
				printf("%-5li (CUSTOMER,    %2d)\n", G->list[i]->id, G->list[i]->P.hops);
				break;
			case(PEER):
				printf("%-5li (PEER,        %2d)\n", G->list[i]->id, G->list[i]->P.hops);
				break;
			case(PROVIDER):
				printf("%-5li (PROVIDER,    %2d)\n", G->list[i]->id, G->list[i]->P.hops);
				break;
			case(NO_ROUTE):
				printf("%-5li (UNUSABLE,    %2d)\n", G->list[i]->id, G->list[i]->P.hops);
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

	// Obtaining filename from arguments
	if(argc != 3){
		printf("Wrong number of arguments.\n");
		exit(1);
	}
	sscanf(argv[1], "%s", filename);
	if(sscanf(argv[2], "%li", &destination) != 1){
		printf("Destination must be a long integer!\n");
		exit(0);
	}

	// Reading Graph
	printf("Reading file\n");
	G = readGraph(filename);
	
	printf("The file %s was loaded successfully to the Graph\n", filename);
	printf("There are %d nodes and %d edges!\n", G->V, G->E);
	
	// Doing some magic here
	findPath(G, destination, DESTINATION, 0);
	printResult(G);
	
	memoryCheck(G);
	exit(0);
}
