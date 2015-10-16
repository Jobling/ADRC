#include "tree.h"

/*
Function used to free used memory when exiting cleanly from the program.
*/
void MemoryCheck(link self){
	if(self->left != NULL){
		MemoryCheck(self->left);
		free(self->left);
	}
	if(self->right != NULL){
		MemoryCheck(self->right);
		free(self->right);
	}
	return;
}
// Tree Related Functions (Primary Functions)
/*
Function that relates a certain destination hop with a prefix.
To do that the program "travels along the tree" following the prefix and updates 
the value of the next hop when reaches the end of the prefix.
*/
void AddPrefix(link self, char * prefix, int hop){	
	switch (prefix[0]){
		case '\0':
			// All out of bits
			self->hop = hop;
			break;
		case '0':
			// This bit points left
			if (self->left == NULL){
				self->left = (link) malloc(sizeof(node));
				self->left->left = NULL;
				self->left->right = NULL;
				self->left->hop = 0;
			}
			AddPrefix(self->left, &prefix[1], hop);
			break;
		case '1':
			// This bit points right
			if(self->right == NULL){
				self->right = (link) malloc(sizeof(node));
				self->right->right = NULL;
				self->right->left = NULL;
				self->right->hop = 0;
			}
			AddPrefix(self->right, &prefix[1], hop);
			break;
		default:
			// Not interpreted as a bit
			// It should never happen because of previous verification
			printf("Unsupported prefix. Not binary.\n");
			exit(1);
	}
}
/*
Function that reads a file and returns a tree that's described on the file.
The file must have the following syntax:
[PREFIX] [NEXT-HOP]
*/
link ReadTable(char * filename){
	FILE *fp;
	link root;
	char *aux, linha[BUFFSIZE], prefix[BITSIZE + 1];
	int hop, n;
	
	// Opening the file with a 'read' flag
	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("File not found\n");
		exit(1);
	}
	
	// Reading the first line of the file
	aux = fgets(linha, BUFFSIZE, fp);
	if (aux == NULL){
		printf("Empty File\n");
	}
	
	// Creating tree with empty prefix (*). It's next-hop value will be 1
	root = (link) malloc(sizeof(node));
	root->hop = 1;
	root->left = NULL;
	root->right = NULL;
	
	// Node addition from file input
	while(aux != NULL){
		n = sscanf(linha, "%s %d", prefix, &hop);
		prefix[BITSIZE] = '\0';
		if(n == 2){
			if(isBinary(prefix)) AddPrefix(root, prefix, hop);
		}else printf("Invalid line format\n");
		aux = fgets(linha, BUFFSIZE, fp);
	}
	
	// Closing file
	fclose(fp);
	return root;
}
/*
Function used to "clean" a node.
This function does free the memory used by the given node, if the node is a leaf
*/
int DeletePrefix(link self, char * prefix){
	switch (prefix[0]){
		case '\0':
			// All out of bits
			self->hop = 0;
			break;
		case '0':
			// This bit points left
			if (self->left == NULL)
				printf("The selected prefix doesn't exist.\n");
			else
				if(DeletePrefix(self->left, &prefix[1])){
					free(self->left);
					self->left = NULL;
				}
			break;
		case '1':
			// This bit points right
			if(self->right == NULL)
				printf("The selected prefix doesn't exist.\n");
			else
				if(DeletePrefix(self->right, &prefix[1])){
					free(self->right);
					self->right = NULL;
				}
			break;
		default:
			// Not interpreted as a bit
			// It should never happen because of previous verification
			printf("Unsupported prefix. Not binary.\n");
			exit(1);
	}
	// If the node is a leaf node, its memory can be freed, so
	// a flag (int) is sent back.
	if ((self->left == NULL && self->right == NULL) && !self->hop) return 1;
	else return 0;
}
/*
Function used to print the expediction table stored in the tree on the terminal
*/
void PrintTable(link self, char * prefix, int level){
	// Index and hop printing
	if(level == 0) printf("%*c%4d\n", -BITSIZE, '*', self->hop);
	else if(self->hop != 0) printf("%*s%4d\n", -BITSIZE, prefix, self->hop);

	// Printing left child, if it exists
	if(self->left != NULL){
		prefix[level] = '0';
		level++;
		PrintTable(self->left, prefix, level);
		level--;
	}
	// Printing right child, if it exists
	if(self->right != NULL){
		prefix[level] = '1';
		level++;
		PrintTable(self->right, prefix, level);
		level--;
	}
	prefix[level] = '\0';
	return;
}
/*
Function used to turn the binary tree into a 2-Tree.
*/
void TwoTree(link self, int hop){
	// Knowing that if a node is a leaf, it must
	// have a next-hop (due to DeletePrefix implementation)
	// We can assume this function only needs to affect 
	// parent nodes, meaning, nodes with one or more child nodes
	if(self->right != NULL || self->left != NULL){
		// Verify if the node is "empty" or not
		// Set value to "pass down to child nodes"
		hop = (self->hop != 0) ? self->hop: hop;
		// Since the node is not a leaf, it must be "cleaned", so that
		// only leaf nodes have values
		self->hop = 0;
		// Verifying left child
		// If it exists, "pass down the value that was set beforehand"
		if(self->left != NULL) TwoTree(self->left, hop);
		// If it doesn't exist, there is a right child, and this 
		// node must have two children (2-tree definition), so
		// we create a new child and set its value with the value 
		// that was set beforehand
		else{
				self->left = (link) malloc(sizeof(node));
				self->left->right = NULL;
				self->left->left = NULL;
				self->left->hop = hop;
		}
		// Verifying right child
		// If it exists, "pass down the value that was set beforehand"
		if(self->right != NULL) TwoTree(self->right, hop);
		// If it doesn't exist, there is a left child, and this 
		// node must have two children (2-tree definition), so
		// we create a new child and set its value with the value 
		// that was set beforehand
		else{
			self->right = (link) malloc(sizeof(node));
			self->right->right = NULL;
			self->right->left = NULL;
			self->right->hop = hop;
		}
	}
}
/*
Function used to know next-hop for given prefix
*/
int AddressLookUp(link self, char * prefix){
	// Integer used to store the value that's being returned
	int n;
	// If the prefix indicates the left node, and the left node
	// exists, we call the function on it
	if((prefix[0] == '0') && (self->left != NULL)) n = AddressLookUp(self->left, &prefix[1]); 
	else{
		// If the prefix indicates the right node, and the right node
		// exists, we call the function on it
		if((prefix[0] == '1') && (self->right != NULL))	n = AddressLookUp(self->right, &prefix[1]);
		// If the prefix ended or the are no more child nodes, this node is a leaf,
		// and we return whichever value is stored in it
		else return self->hop;	
	}
	// If the value returned is 0 (empty node) and we find a different value
	// "on our way up the tree", we adopt this value.
	if (n == 0) return self->hop;
	// Otherwise, we just "propagate" the value returned up the tree
	else return n;
}

