/*
Function used to turn the binary tree into a 2-Tree.
*/
void TwoTree(link self, int hop){
	// Verify if the node is "empty"
	if(self->hop == 0){
		// If the node has no children it is a leaf
		// Update next-hop with value that "came down from the tree"
		if(self->right == NULL && self->left == NULL){
			self->hop = hop;
		// If the node has children, it's not a leaf
		// Pass on the value that "came down from the tree"
		}else{
			// Verifying left child
			// If it exists, "pass down value"
			if(self->left != NULL) TwoTree(self->left, hop);
			// If it doesn't exist, there is a right child, and this 
			// node must have two children (2-tree definition), so
			// we create a new child and set its value with the value
			// that "came down the tree"
			else{
				self->left = (link) malloc(sizeof(node));
				self->left->right = NULL;
				self->left->left = NULL;
				self->left->hop = hop;
			}
			// Verifying right child
			// If it exists, "pass down value"
			if(self->right != NULL) TwoTree(self->right, hop);
			// If it doesn't exist, there is a left child, and this 
			// node must have two children (2-tree definition), so
			// we create a new child and set its value with the value
			// that "came down the tree"
			else{
				self->right = (link) malloc(sizeof(node));
				self->right->right = NULL;
				self->right->left = NULL;
				self->right->hop = hop;
			}
		}
	// If the node has a value	
	}else{
		// If the node has any children
		if(self->right != NULL || self->left != NULL){
			// Verifying left child
			// If it exists, pass down own value
			if(self->left != NULL) TwoTree(self->left, self->hop);
			// If it doesn't exist, there is a right child, and this 
			// node must have two children (2-tree definition), so
			// we create a new child and set its value with own value
			else{
				self->left = (link) malloc(sizeof(node));
				self->left->right = NULL;
				self->left->left = NULL;
				self->left->hop = self->hop;
			}
			// Verifying right child
			// If it exists, pass down own value
			if(self->right != NULL) TwoTree(self->right, self->hop);
			// If it doesn't exist, there is a left child, and this 
			// node must have two children (2-tree definition), so
			// we create a new child and set its value with own value
			else{
				self->right = (link) malloc(sizeof(node));
				self->right->right = NULL;
				self->right->left = NULL;
				self->right->hop = self->hop;
			}
			// After passing down its own value, this node (that had a value)
			// must be "cleaned", so that only leaf nodes have values
			self->hop = 0;
		}
	}
}
