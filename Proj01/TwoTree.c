/*
Function used to turn the binary tree into a 2-Tree.
*/
void TwoTree(link self, int hop){
	// Verify if the node is "empty" or not
	// Set value to "pass down to child nodes"
	hop = (self->hop != 0) ? self->hop: hop;
	
	// If node is leaf node, it's value must be the value that was set beforehand
	if(self->right == NULL && self->left == NULL) self->hop = hop;
	// If the node has children, it's not a leaf
	// Pass on the value that was set beforehand
	else{
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
