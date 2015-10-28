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

// Helper Functions

// Node Functions

// Statistics Functions

// Main
