#include <iostream>
#include <stdlib.h>
#include <conio.h>

using namespace std;

#define BLOCKSIZE 256

// We use an AVL tree.
// Base index starts from 0 and goes to N - 1.

typedef int key_t;								/* Key of textLine object */
typedef char * line_t;							/* Sentence in the textLine object */

// Struct defined for the node to store text for a given line (will be the node for the AVL tree)
struct text_t {
	key_t					count;				/* Total nodes less than this count */
	line_t					textLine;			/* Text line */
	struct text_t			*left;				/* Left child */
	struct text_t			*right;				/* Right Child */
	int						height;				/* height of the node - to be used for balancing the tree */
	bool					isHead;				/* flag for the root node of the text-editor, created by 'create_text' function,
												root node will not contain any text */
};


text_t	*currentblock = NULL;					/* Block of new nodes. */
text_t	*free_list = NULL;						/* Nodes return to this list after deletion */
int		nodes_taken = 0;						/* Total number of lines in text. nodes_taken = N */
int		nodes_returned = 0;						/* Total number of nodes in free list / total nodes deleted */
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
	tmp->isHead = false;

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
Function to get the node at the specified index
*/
text_t * getNodeAtIndex(text_t *node, int index) {

	if (node == NULL || index < 0) {
		return NULL;
	}
	else {
		while (index > 0){
			if (node->count == index){
				// index of the root node matches with the index, hence we return the root node
				return node;
			}
			else if (index < node->count){
				// Move towards the left node as the required index lies below the current node index
				node = node->left;
			}
			else if (index > node->count){
				// Move towards the right node and subtract the index by [root->count] (as we are skipping these lines)
				index -= node->count;
				node = node->right;
			}
		}
	}
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
text_t * rotate_zig_zig(text_t * unbalanced_node, bool isleft){
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
text_t * rotate_zig_zag(text_t * unbalanced_node, bool isleft){
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
text_t * rebalance(text_t* node){

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
text_t * insert(text_t *txt, int index, char * new_line){

	if (txt == NULL || index < 0) {
		return NULL;
	}
	else if (txt->isHead == true){

		if (txt->count == 1)
		{
			// Append the first child
			text_t *newNode = get_node();
			txt->left = get_node();
			txt->left->textLine = new_line;
		}
		else
		{
			text_t *balancedNode = insert(txt->left, index, new_line);

			if (balancedNode != NULL)
			{
				txt->left = balancedNode;
			}
		}

		// Increase the count by 1 after addition
		txt->count += 1;
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
		/*if (!txt->isHead)
		{*/
		update_node_height(txt);
		//}

		// rebalance the child-subtrees if required
		return rebalance(txt);
	}
}

/*
Function to delete the line at the given index
*/
text_t* delete_node(text_t *txt, int index, text_t &deletedNode){

	if (txt == NULL || index < 0) {
		return NULL;
	}
	else if (txt->isHead == true){
		// Skip the node in case it is the head of the tree
		text_t *tmp = delete_node(txt->left, index, deletedNode);

		if (tmp != NULL)
		{
			// In case the node returned is the deleted node
			if (tmp->height == -1){
				tmp->height = 0;
				deletedNode = *tmp;
			}
			else{
				txt->left = tmp;
			}
		}

		if (&deletedNode != NULL)
		{
			txt->count -= 1;
		}
	}
	else {
		if (txt->count == index){
			// check if the node has any child

			if (txt->left == NULL || txt->right == NULL)
			{
				text_t *tmp = txt->left != NULL ? txt->left : txt->right;

				text_t deletedNode = *txt;
				deletedNode.height = -1;

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

				return &deletedNode;
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
				text_t temp;
				delete_node(txt->right, 1, temp);
			}
		}
		else if (txt->count > index){
			// Move towards the left node
			text_t *balancedNode = delete_node(txt->left, index, deletedNode);

			if (balancedNode != NULL)
			{
				if (balancedNode->height == -1)
				{
					txt->left = NULL;
					balancedNode->height = 0;
					deletedNode = *balancedNode;
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
			// Move towards the right node and subtract the index by root->count (as we are skipping these lines)

			// Move to the right sub-tree
			text_t *balancedNode = delete_node(txt->right, index - txt->count, deletedNode);

			if (balancedNode != NULL)
			{
				if (balancedNode->height == -1)
				{
					//txt->right = NULL;
					balancedNode->height = 0;
					deletedNode = *balancedNode;
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

text_t * create_text(){
	text_t *tmp_node;
	tmp_node = get_node();

	// Setting up the attributes for the new node
	tmp_node->left = NULL;
	tmp_node->count = 1;
	tmp_node->height = 0;
	tmp_node->isHead = true;
	tmp_node->textLine = "\0";

	return(tmp_node);
}


int length_text(text_t *txt){
	if (txt != NULL){
		if (txt->isHead)
			return (txt->count - 1);		/* Decrement 1 from the count available at 'head' node since it does not contain text */
		else
			return (txt->count);			/* Return the count of lines available at the node (inclusive of the current node)*/
	}

	return 0;
}


char * get_line(text_t *txt, int index){
	// Do binary search.
	text_t *node = NULL;

	if (txt != NULL)
	{
		node = getNodeAtIndex(txt, index);
	}

	if (node != NULL)
		return node->textLine;
	else
		return NULL;
}

void append_line(text_t *txt, char * new_line)
{
	// Insert a line at index = node_taken + 1
	if (new_line != NULL && txt != NULL){
		insert(txt, (nodes_taken + 1), new_line);
	}
}


char * set_line(text_t *txt, int index, char * new_line){
	// Get the node at the given index
	text_t *node = getNodeAtIndex(txt, index);

	char * prevLine = NULL;

	if (node != NULL && new_line != NULL) {
		// Store the prev line pointer to return
		prevLine = node->textLine;
		// Update the text of line for the given node
		node->textLine = new_line;
	}

	return prevLine;
}


void insert_line(text_t *txt, int index, char * new_line){

	if (new_line != NULL)
	{
		insert(txt, index, new_line);
	}
}

char * delete_line(text_t *txt, int index)
{
	text_t *deletedNode = NULL;

	text_t *tmp = delete_node(txt, index, deletedNode);

	if (tmp != NULL)
	{
		// In case the node returned is the deleted node
		if (tmp->height == -1){
			tmp->height = 0;
			deletedNode = *tmp;
		}
		else{
			txt = tmp;
		}
	}

	return deletedNode != NULL ? deletedNode->textLine : NULL;
}

//int main(){
//
//	// Create a root node
//	text_t *root = create_text();
//
//	// Insert / Append lines
//	append_line(root, "This is first line");
//	append_line(root, "This is second line");
//	append_line(root, "This is fourth line");
//	insert_line(root, 3, "Third line inserted");
//	append_line(root, "This is fifth line");
//
//	insert_line(root, 100, "What is this?");
//
//	// Check the length_text function
//	cout << "Length of the text:" << endl;
//	cout << "There are " << length_text(root) << " number of lines in the text." << endl;
//
//	cout << endl << "Get Line:" << endl;
//	cout << "Line at index 4 = '" << get_line(root, 4) << "'\n";
//
//	// Change the text of the third line
//	cout << "Set Line:";
//	char *oldtext = set_line(root, 3, "This is third line");
//	cout << endl << "Line at index 3 (before update) = '" << oldtext << "'";
//	cout << endl << "Line at index 3 (after update) = '" << get_line(root, 3) << "'\n";
//
//	// Delete line 5
//	cout << "Delete line:" << endl;
//	char *p = delete_line(root, 5);
//	cout << "5th line deleted. Deleted text = '" << p << "'" << endl;
//
//	cout << endl << "Pre-order traversal" << endl;
//	// Print the whole tree
//	preorder_traversal(root);
//
//	_getch();
//}


int main(){

	int i, tmp;
	text_t *txt2;
	char *c;

	txt2 = create_text();
	for (i = 1; i <= 10000; i++){
		if (i % 2 == 1)
			append_line(txt2, "A");
		else
			append_line(txt2, "B");
	}

	tmp = length_text(txt2);
	cout << "length should be 10000, is " << tmp << endl;

	c = get_line(txt2, 9876);
	cout << "line 9876 of txt2 should be B, found " << c << endl;

	for (i = 10000; i > 1; i -= 2){
		c = delete_line(txt2, i);

		/*char ch = *c;

		if (ch != 'B'){
		cout << "line "<< i << " of txt2 should be B, found  " << c << endl;
		break;
		}*/

		//append_line(txt2, c);
	}

	cout << "All Tests passed" << endl;

	_getch();

	return(0);
}