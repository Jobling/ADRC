#ifndef __TREE_H_
#define __TREE_H_

#include "helper.h"

// Data Structures
typedef struct node * link;
typedef struct node{
	int hop;
	link left;
	link right;
} node;

void MemoryCheck(link self);
void AddPrefix(link self, char * prefix, int hop);
link ReadTable(char * filename);
int DeletePrefix(link self, char * prefix);
void PrintTable(link self, char * prefix, int level);
void TwoTree(link self, int hop);
int AddressLookUp(link self, char * prefix);

#endif

