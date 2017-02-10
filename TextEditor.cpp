#include<iostream>
#include<conio.h>

using namespace std;

#define BLOCKSIZE 256

// We use an AVL tree.
// Base index starts from 0 and goes to N - 1.

typedef int key_t;								/* Key of textLine object */
typedef char * line_t;							/* Sentence in the textLine object */

// Struct defined for the node to store text for a given line (will be the node for the AVL tree)
typedef struct textEditorNode {
	key_t					count;				/* Total nodes less than this count */
	line_t					textLine;			/* Text line */
	struct textEditorNode	*left;				/* Left child */
	struct textEditorNode	*right;				/* Right Child */
	int						height;				/* height of the node - to be used for balancing the tree */
} text_t;


text_t	*currentblock = NULL;					/* Block of new nodes. */
text_t	*free_list = NULL;					/* Nodes return to this list after deletion */
int		nodes_taken = 0;					/* Total number of lines in text. nodes_taken = N */
int		nodes_returned = 0;					/* Total number of nodes in free list / total nodes deleted */
int		size_left;								/* Nodes left in currentblock */

/*
Return a node from currentblock / free_list.
First checks in free list.
If no node is free, checks in currentblock
If currentblock is Empty, currentblock is initialzed to an array of size
BLOCKSIZE and the first node is returned.
*/
text_t *get_node(){
	text_t *tmp;

	nodes_taken += 1;

	if (free_list != NULL)
	{
		tmp = free_list;
		free_list = free_list->right;
	}
	else
	{
		if (currentblock == NULL || size_left == 0)
		{
			currentblock = (text_t *)malloc(BLOCKSIZE * sizeof(text_t));
			size_left = BLOCKSIZE;
		}
		tmp = currentblock++;
		size_left -= 1;
	}

	// Fresh node
	tmp->left = NULL;
	tmp->right = NULL;
	tmp->count = 1;
	tmp->height = 0;

	return(tmp);
}

/*
Adds the node to the free list
*/
void add_to_free_list(text_t *node){
	node->right = free_list;
	free_list = node;
	nodes_returned += 1;
}

/*
Creates an empty tree.
*/
text_t *create_text(){
	text_t *tmp_node;
	tmp_node = get_node();

	// Setting up the attributes for the new node
	tmp_node->left = NULL;
	tmp_node->count = 1;
	tmp_node->height = 0;

	return(tmp_node);
}

/*
Function to get the node at the specified index
*/
text_t* getNodeAtIndex(text_t *node, int index) {

	if (node == NULL || index < 0) {
		return NULL;
	}
	else {
		while (index > 0){
			if (node->count == index){
				// index of the root node matches with the index, hence we return the root node
				return node;
			}
			else if (node->count > index){
				// Move towards the left node as the required index lies below the current node index
				node = node->left;
			}
			else if (node->count < index){
				// Move towards the right node and subtract the index by [root->count] (as we are skipping these lines)
				index -= node->count;
				node = node->right;
			}
		}
	}
}

// Returns the number of lines
int length_text(text_t *txt){
	// return the number of lines below the given text node.
	return (txt->count + 1);
}

// Returns the line at index 'index'
char * getLine(text_t *txt, int index){
	// Do binary search.
	text_t *node = getNodeAtIndex(txt, index);

	if (node != NULL)
		return node->textLine;
	else
		return NULL;
}

/*
Function to replace the line text at the given index with the new line text
Returns a pointer to the previous line contents if the line at index exists, NULL otherwise
*/
char * set_line(text_t *txt, int index, char * new_line){
	// Get the node at the given index
	text_t *node = getNodeAtIndex(txt, index);

	char * prevLine = NULL;

	if (node != NULL) {
		// Store the prev line pointer to return
		prevLine = node->textLine;
		// Update the text of line for the given node
		node->textLine = new_line;
	}

	return prevLine;
}

/*
Function to get the height of the left child
*/
int getLeftHeight(text_t * node) {
	return (node->left != NULL) ? node->left->height + 1 : 0;
}

/*
Function to get the height of the right child
*/
int getRightHeight(text_t *node) {
	return (node->right != NULL) ? node->right->height + 1 : 0;
}

/*
Function to increment the node height of the given node
*/
void update_node_height(text_t *node){
	int leftHeight = getLeftHeight(node);
	int rightHeight = getRightHeight(node);

	node->height = (leftHeight > rightHeight) ? leftHeight : rightHeight;
}

/*
Function to check the balance of the given node
*/
bool isNodeBalanced(text_t* node){
	bool balance = true;

	if (node != NULL) {
		int leftHeight = getLeftHeight(node);
		int rightHeight = getRightHeight(node);

		if (abs(leftHeight - rightHeight) > 1)
			balance = false;
	}

	return balance;
}

/*
Function to balance the tree with zig-zig pattern
*/
text_t* rotate_zig_zig(text_t * unbalanced_node, bool isleft){
	text_t* tmp = NULL;

	if (isleft) {
		// Do a right rotation
		tmp = unbalanced_node->left;

		unbalanced_node->left = unbalanced_node->left->right;
		tmp->right = unbalanced_node;

		// Adjust the count associated with the nodes
		unbalanced_node->count -= tmp->count;
	}
	else {
		// Do a left rotation
		tmp = unbalanced_node->right;

		unbalanced_node->right = unbalanced_node->right->left;
		tmp->left = unbalanced_node;

		// Adjust the count associated with the nodes
		tmp->count += unbalanced_node->count;
	}

	update_node_height(unbalanced_node);
	update_node_height(tmp);

	return tmp;
}

/*
Function to balance the tree with zig-zag pattern
*/
text_t* rotate_zig_zag(text_t * unbalanced_node, bool isleft){
	text_t *tmp = NULL;

	if (isleft){
		tmp = unbalanced_node->left->right;

		unbalanced_node->left->right = tmp->left;
		tmp->left = unbalanced_node->left;
		unbalanced_node->left = tmp->right;
		tmp->right = unbalanced_node;

		// Adjust the count associated with the nodes
		tmp->count += tmp->left->count;
		unbalanced_node->count -= tmp->count;
	}
	else
	{
		tmp = unbalanced_node->right->left;

		unbalanced_node->right->left = tmp->right;
		tmp->right = unbalanced_node->right;
		unbalanced_node->right = tmp->left;
		tmp->left = unbalanced_node;

		// Adjust the count associated with the nodes
		tmp->right->count -= tmp->count;
		tmp->count += unbalanced_node->count;
	}

	update_node_height(tmp);
	update_node_height(tmp->left);
	update_node_height(tmp->right);

	return tmp;
}

/*
Function to check if the child sub-trees at a given node are balanced
Balances the child nodes in case they are not
*/
text_t* rebalance(text_t* node){

	// Checking the balance of the left child
	if (!isNodeBalanced(node)){

		if (getLeftHeight(node) > getRightHeight(node))
		{
			if (getLeftHeight(node->left) > getRightHeight(node->left)){
				// rotate zig-zig left to rebalance the left-child of the node
				return rotate_zig_zig(node, true);
			}
			else {
				// rotate zig-zig left to rebalance the left-child of the node
				return rotate_zig_zag(node, true);
			}
		}
		else {
			if (getLeftHeight(node->right) > getRightHeight(node->right)){
				// rotate zig-zag left to rebalance the left-child of the node
				return rotate_zig_zag(node, false);
			}
			else {
				// rotate zig-zig left to rebalance the left-child of the node
				return rotate_zig_zig(node, false);
			}
		}
	}

	return NULL;
}

/*
Function to insert the line at the given index
*/
text_t* insert(text_t *txt, int index, char * new_line){

	if (txt == NULL || index < 0) {
		return NULL;
	}
	else {
		if (txt->count == index){
			// need to insert the new_line as the left child of txt
			text_t *newNode = get_node();
			newNode->textLine = new_line;

			// Increase the count of the left node by 1 after addition
			txt->count += 1;

			if (txt->left == NULL) {
				txt->left = newNode;
				txt->left->textLine = new_line;
			}
			else {
				// Move to the righmost child of the left child
				txt = txt->left;

				while (txt->right != NULL) {
					txt = txt->right;
				}

				txt->right = newNode;
			}
		}
		else if (txt->count > index){
			// Move towards the left node
			text_t *balancedNode = insert(txt->left, index, new_line);

			if (balancedNode != NULL)
			{
				txt->left = balancedNode;
			}

			// Increase the count of the left node by 1 after addition
			txt->count += 1;
		}
		else if (txt->count < index){
			// Move towards the right node and subtract the index by [root->count + 1] (as we are skipping these lines)
			if (txt->right == NULL){
				// Append the node
				txt->right = get_node();
				txt->right->textLine = new_line;
			}
			else
			{
				// Move to the right sub-tree
				text_t *balancedNode = insert(txt->right, index - txt->count, new_line);

				if (balancedNode != NULL)
				{
					txt->right = balancedNode;
				}
			}
		}

		// Adjust the height of the current node
		update_node_height(txt);

		// rebalance the child-subtrees if required
		return rebalance(txt);
	}
}

/*
Function to delete the line at the given index
*/
text_t* delete_node(text_t *txt, int index){

	if (txt == NULL || index < 0) {
		return NULL;
	}
	else {
		if (txt->count == index){
			// check if the node has any child

			if (txt->left == NULL || txt->right == NULL)
			{
				text_t *tmp = txt->left != NULL ? txt->left : txt->right;

				if (tmp == NULL) {
					tmp = txt;
				}
				else {
					txt->textLine = tmp->textLine;
					txt->left = tmp->left;
					txt->right = tmp->right;
					txt->count = tmp->count;
					txt->height = tmp->height;
				}

				// Delete temp node
				add_to_free_list(tmp);

				tmp->height = -1;

				return tmp;
			}
			else
			{
				// Find the inorder successor of the node
				text_t *successor = txt->right;

				while (successor->left != NULL)
					successor = successor->left;

				// Copy the contents of the successor to the current node
				txt->textLine = successor->textLine;

				// delete the successor
				delete_node(txt->right, 1);
			}
		}
		else if (txt->count > index){
			// Move towards the left node
			text_t *balancedNode = delete_node(txt->left, index);

			if (balancedNode != NULL)
			{
				if (balancedNode->height == -1)
				{
					txt->left = NULL;
					balancedNode->height = 0;
				}
				else
				{
					txt->left = balancedNode;
				}
			}

			// Increase the count of the left node by 1 after addition
			txt->count -= 1;
		}
		else if (txt->count < index){
			// Move towards the right node and subtract the index by [root->count + 1] (as we are skipping these lines)

			// Move to the right sub-tree
			text_t *balancedNode = delete_node(txt->right, index - txt->count);

			if (balancedNode != NULL)
			{
				if (balancedNode->height == -1)
				{
					txt->right = NULL;
					balancedNode->height = 0;
				}
				else
				{
					txt->right = balancedNode;
				}
			}
		}

		// Adjust the height of the current node
		update_node_height(txt);

		// rebalance the child-subtrees if required
		return rebalance(txt);
	}
}


void insert_line(text_t *txt, int index, char * new_line){
	text_t *tmp = insert(txt, index, new_line);

	if (tmp != NULL)
	{
		txt = tmp;
	}
}

char * delete_line(text_t *txt, int index){
	//text_t *ptr = getNodeAtIndex(txt, index);

	text_t *tmp = delete_node(txt, index);

	if (tmp != NULL)
	{
		if (tmp->height == -1){
			//ptr = tmp;
			tmp->height = 0;
		}
		else{
			txt = tmp;
		}
	}

	return NULL;
}

void append_line(text_t *txt, char * new_line){
	// Insert a line at index = node_taken + 1
	text_t *tmp = insert(txt, (nodes_taken + 1), new_line);

	if (tmp != NULL)
	{
		txt = tmp;
	}
}

void preorder_traversal(text_t* root){
	if (root == NULL)
		return;

	preorder_traversal(root->left);
	cout << root->textLine << endl;
	preorder_traversal(root->right);
}


int main(){

	text_t *root = create_text();

	root->textLine = "This is the first line";

	insert_line(root, 2, "This is second line");

	text_t *tmp = insert(root, 3, "This is third line");
	if (tmp != NULL)
	{
		root = tmp;
	}

	insert_line(root, 4, "This is fourth line");

	insert_line(root, 5, "This is fifth line");

	char *p = delete_line(root, 5);

	//cout << "previous line = " << p << endl;

	preorder_traversal(root);

	_getch();
}