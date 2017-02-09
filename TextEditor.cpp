#include<iostream>

using namespace std;

#define BLOCKSIZE 256

// We use an AVL tree.
// Base index starts from 0 and goes to N - 1.
// Based on Model-1.

// Key of textLine object
typedef int key_t;
// Sentence in the textLine object
typedef char * line_t;

typedef struct t_t {
  // Total nodes less than this count
  key_t      count;
  // Left child
  struct t_t  *left;
  // Right Child
  struct t_t  *right;
  // Text line
  line_t textLine;
  // height of node
  int height;
} text_t;

// Block of new nodes.
text_t *currentblock = NULL;
// Nodes left in currentblock
int    size_left;
// Nodes return to this list after deletion.
text_t *free_list = NULL;
// Total number of lines in text. nodes_taken = N
int nodes_taken = 0;
// Total number of nodes in free list / total nodes deleted.
int nodes_returned = 0;

// Return a node from currentblock / free_list.
// First checks in free list.
// If no node is free, checks in currentblock
// If currentblock is Empty, currentblock is initialzed to an array of size
// BLOCKSIZE and the first node is returned.
text_t *get_node(){
  text_t *tmp;
  nodes_taken += 1;
  if( free_list != NULL ){
    tmp = free_list;
    free_list = free_list -> right;
  }
  else{
    if( currentblock == NULL || size_left == 0){
      currentblock = (text_t *) malloc( BLOCKSIZE * sizeof(text_t) );
      size_left = BLOCKSIZE;
    }
    tmp = currentblock++;
    size_left -= 1;
  }
  // Fresh node
  tmp->left = NULL;
  tmp->right = NULL;
  tmp->count = 0;
  tmp->height = 0;
  return( tmp );
}

// Adds the node to the free list
void add_to_free_list(text_t *node){
  node->right = free_list;
  // This node is cut from tree.
  node->height = 0;
  free_list = node;
  nodes_returned +=1;
}

// Creates an empty tree.
text_t *create_text(){
  return(get_node());
}

// Returns the number of lines.
int length_text(text_t *txt){
  return nodes_taken;
}

// text_t
text_t * getNodeAtIndex(text_t * root, int index){
  // Performs binary search using iteration
  if(root == NULL) return NULL;
  while(index >= 0){
    if(index < root->count){
      root = root->left;
    } else if(root->count == index){
      return root;
    }
    else{
      root = root->right;
      index -= root->count;
    }
  }
  return NULL;
}

// Returns the line at index 'index'
char * getLine(text_t *txt, int index){
  // Do binary search.
  text_t * node = getNodeAtIndex(txt, index);
  if(node != NULL) return node->textLine;
  else return NULL;
}

// Returns at 0->predecessor and 1->node
// text_t ** getPredecessorAndNode(text_t * root, int index){
//   // Use iteration
//   text_t * nodes[2];
//   nodes[1] = getNodeAtIndex(root, index);
//   if(nodes[1] != NULL){
//     if(nodes[1]->left != NULL){
//       nodes[0] = getNodeAtIndex(nodes[1], nodes[1]->count - 1);
//     }else{
//       nodes[0] = NULL;
//     }
//   }
//   return nodes;
// }

char * set_line( text_t *txt, int index, char * new_line){
  text_t *node = getNodeAtIndex(txt, index);
  char * toReturn = NULL;
  if(node != NULL){
    // Found node with given index
    char * toReturn = node->textLine;
    node->textLine = new_line;
  }
  return toReturn;
}

void rotate_zig_zig(text_t * unbalanced_node, text_t * parent_unbalanced_node){
  if(parent_unbalanced_node->left == unbalanced_node){
    // Do a Right rotation

  }else{
    // Do a Left rotation

  }
}

void rotate_zig_zag(text_t * unbalanced_node, text_t * parent_unbalanced_node){
  if(parent_unbalanced_node->left == unbalanced_node){
    // Do a Left-Right rotation

  }else{
    // Do a Right-Left rotation
  }
}

void insert_line( text_t *txt, int index, char * new_line){
  text_t * node = getNodeAtIndex(txt, index);
  if(node != NULL){
    // Append in the middle
    text_t * tmp = get_node();
    
  }else{
    // Append at end

  }
}

void append_line( text_t *txt, char * new_line){
  insert_line(txt, nodes_taken, new_line);
}

char * delete_line( text_t *txt, int index){
  return NULL;
}

int main(){
  cout<<"Hi"<<endl;
}
