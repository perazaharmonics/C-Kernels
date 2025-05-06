/**
 * This interface file defines functions for a thread safe set of functions
 * that update and search a binary tree. In the prototypes below, the tree
 * variable is a structure that point to the root of the tree. Each element of
 * the tree holds a key-value pair. Another structure will need to be defined
 * for each element to include a mutx that protects that element so that only
 * one thread at a time can access it. The initialize(),ad(),and lookup()
 * functions are relatively simple to implement. The delete() function required
 * a more complex implementation. The delete() function will need to
 * traverse the tree to find the element to delete. Once the element is found,
 * the function will need to check if the element has children. If it does, the
 * function will need to find the minimum element in the right subtree and
 * replace the element to be deleted with the minimum element. If the element
 * does not have children, the function will need to simply remove the element
 * from the tree. The function will also need to update the parent of the
 * deleted element to point to the new element. The function will also need
 * to update the mutex of the parent element to point to the new element. The
 * function will also need to update the mutex of the deleted element to point
 * to the new element. 
 */

#include "thread_tree.h"


// A function to initialize the Red and Black Tree.
void Initialize (Tree* tree)
{                                       // ------------- Initialize --------------
  int status=0;                         // Status code.
  if (tree == NULL)                     // Have we allocated memory for the tree?
    return;                             // No, handle null pointer gracefully.
  tree->root=NULL;                      // Initialize the root of the tree.
  tree->size=0;                         // Initialize the # of nodes to zero.
  status=pthread_mutex_init(&tree->tmtx,NULL); // Initialize the tree's thread mutex.
  if (status!=0)                        // Did we fail to initialize the mutex?
    errExitEN(status,"pthread_mutex_init");// Yes, print error and exit.
}                                       // ------------- Initialize --------------

// A function to destroy the tree and free memory allocated for it.
void DestroyTree (Tree* p)
{                                       // ------------ DestroyTree -------------
  int status=0;                         // Status code.
    if (p->root!=NULL)                  // Do we have the root populated ?
    DestroySubtree(p->root);           // Yes, it's safe to destroy it.
  status=pthread_mutex_destroy(&p->tmtx);// Destroy the tree's mutex.
  if (status!=0)                        // Did we fail to destroy the mutex?
    errExitEN(status,"pthread_mutex_destroy");// Yes, print error and exit.
  free((void*) p);                      // Free the tree's memory.
  p=NULL;                               // Set the pointer to NULL.
}                                       // ------------ DestroyTree -------------
// A function to destroy the subtree rooted at the given node.
void DestroySubtree (TreeNode* p)
{                                       // ---------- DestroySubtree ------------
  int status=0;                         // Status code.
    if (p->left!=NULL)                  // Do we have a left child?
    DestroySubtree(p->left);            // Yes, destroy the left subtree.
  if (p->right!=NULL)                   // Do we have a right child?
    DestroySubtree(p->right);           // Yes, destroy the right subtree.
  status=pthread_mutex_destroy(&p->nmtx);// Destroy the node's mutex.
  if (status!=0)                        // Did we fail to destroy the mutex?
    errExitEN(status,"pthread_mutex_destroy");// Yes, print error and exit.
  free((void*) p);                      // Free the node's memory.
  p=NULL;                               // Set the pointer to NULL.
}                                       // ---------- DestroySubtree ------------
// A function to insert a new node given a key and value.
void InsertNode (                       // Insert a new node into the tree.
    Tree* tree,                         // The tree to insert into.
    char* key,                          // The key of the node.
    int value)                          // The value of the node.
{                                       // ------------- InsertNode -------------
  int status=0;                         // Status code.
  status=pthread_mutex_lock(&tree->tmtx);// Lock the tree's mutex.
  if (status!=0)                        // Did we fail to lock the mutex?
    return;                             // Yes, so return.
  TreeNode* q=NULL;                     // Pointer to the new node.
  q=(TreeNode*) malloc(sizeof(TreeNode)); // Allocate memory for the new node.
  if (q==NULL)                          // Did we fail to allocate memory?
    errExit("malloc");                  // Yes, print error and exit.
  q->key=key;                           // Set the key of the new node.
  q->value=value;                       // Set the value of the new node.
  q->color=RED;                         // Set the color of the new node to red.
  q->left=q->right=q->parent=NULL;      // This node has no relationship yet.
  status=pthread_mutex_init(&q->nmtx,NULL);// Initialize the node's mutex.
  if (status!=0)                        // Did we fail to initialize the mutex?
    errExitEN(status,"pthread_mutex_init");// Yes, print error and exit.
  if (tree->root==NULL)                 // Is this the first element in the tree?
  {                                     // Yes
    tree->root=q;                       // So this new node will be the root node.
    tree->root->parent=NULL;            // The root has no parent it the great ancestor.
    tree->root->color=BLACK;            // The root is always black.
    tree->size=1;                       // The size of the tree is 1.
  }                                     // Done with if first element.
  else                                  // Else it is not the first element.
  {                                     // So we will have to find where it goes.
    TreeNode* p=tree->root;             // Current node is the root of the tree (starting point).
    TreeNode* o=NULL;                   // Currently the parent is NULL.
    while (p!=NULL)                     // While we haven't found an empty insertion point.
    {                                   // Start searching for the insertion point.
      o=p;                              // The parent is now the current node.
      if (strcmp(key,p->key)<0)         // Is the new key less than the current key?
        p=p->left;                      // Yes, so go left.
      else                              // Else the new key is greater than the current key.
        p=p->right;                     // So switch with the right child.
    }                                   // Done searching for the insertion point.
    q->parent=o;                        // The parent of the new node is the current node.
    if (strcmp(key,o->key)<0)           // Is the new key less than the current key?
      o->left=q;                        // Yes, so insert as a left child.
    else if (strcmp(key,o->key)>0)      // Else the new key is greater than the current key.
      o->right=q;                       // So insert as a right child.
    else                                // Else the new key is equal to the current key.
    {                                   // So let's replace the old node with the new node.
        ReplaceNode(tree,o,q);          // Replace the old node with the new node.
    }                                   // Done with else not first element in the tree.
    if (q->parent->color==RED)          // Is the parent of the new node red?
      FixInsert(tree,q);                // Yes, so fix the tree to maintain the Red-Black properties.
    else                                // Else the parent of the new node is black.
      q->color=RED;                     // So set the color of the new node to red.
//    FixInsert(tree,q);                  // Fix the tree to maintain the Red-Black properties.
  }                                     // Done with else not first element in the tree.
  tree->size++;                         // We inserted a new element.
  status=pthread_mutex_unlock(&tree->tmtx);// Unlock the tree's mutex.
  if (status!=0)                        // Did we fail to unlock the mutex?
    errExitEN(status,"pthread_mutex_unlock");// Yes, print error and exit.          
}                                       // ------------- InsertNode -------------
// A function to find the minimum node in the tree.
TreeNode* FindMin (TreeNode* p)
{                                       // ------------ FindMin ----------------
  while (p->left!=NULL)                 // While there are still nodes to the left
  {                                     // Then we are not at the minimum node.
    p=p->left;                          // Go left.
  }                                     // Done getting the minimum.
  return p;                             // Return the minimum node.
}                                       // ------------ FindMin ----------------
// A function to find the maximum node in the tree.
TreeNode* FindMax (TreeNode* p)
{                                       // ------------ FindMax ----------------
  while (p->right!=NULL)                // While there are still nodes to the right
  {                                     // Then we are not at the minimum.
    p=p->right;                         // Go right.
  }                                     // Done getting the maximum.
  return p;                             // Return the maximum node.
}                                       // ------------ FindMax ----------------
// A function to replace one subtree with another.
// Note: caller is responsible for freeing the memory of 'p'.
void ReplaceNode (
  Tree* tree,                           // The tree to replace in.
  TreeNode* p,                          // The node to be replaced (oldNode)
  TreeNode* q)                          // The node to replace with (newNode)
{                                       // ------------ ReplaceNode -------------
  if (p->parent==NULL)                  // Is the old node the root?
    tree->root=q;                       // Yes, so set the new node as the root.
  else if (p==p->parent->left)          // Is the old node the left child?
    p->parent->left=q;                  // Yes, so set the new node as the left child.
  else                                  // Else, the old node must be the right child.
    p->parent->right=q;                 // So set the new node as the right child.
  if (q!=NULL)                          // Is the new node not NULL?
    q->parent=p->parent;                // Yes, so the new node's parent is the old node's parent.

}                                       // ------------ ReplaceNode -------------
// A function to delete a node from the tree.
void DeleteNode (
    Tree* tree,                         // The tree to delete from.
    char* key)                          // The key of the node to delete.
{                                       // ------------- DeleteNode -------------
  int status=pthread_mutex_lock(&tree->tmtx);// Lock the tree's mutex.
  if (status!=0)                        // Did we fail to lock the mutex?
    return;                             // Yes, so return.
  TreeNode* p=tree->root;               // Start at the root of the tree.
  while (p!=NULL && strcmp(key,p->key)!=0)// While we haven't found the node to delete.
  {                                     // Search for the node be deleted.
    if (strcmp(key,p->key)<0)           // Is the key less than the current key?
    {                                   // Yes, so we go left.
      p=p->left;                        // Go left.
    }                                   // Done working with left subtree.
    else                                // Else the key is greater than the current key.
    {                                   // So it must be in the right subtree.
      p=p->right;                       // Go right.
    }                                   // Done searching right subtree.
  }                                     // Done searching for the node to delete.
  if (p==NULL)                          // Did we find the node to delete?
  {                                     // No, so we will..
    status=pthread_mutex_unlock(&tree->tmtx);// Unlock the tree's mutex.
    if (status!=0)                      // Did we fail to unlock the mutex?
      errExitEN(status,"pthread_mutex_unlock");// Yes, print error and exit.
    return;                             // Return, no key was found.
  }                                     // Done checking if no node to delete.
  TreeNode* y=p;                        // The node to delete.
  TreeNode* x=NULL;                     // The node to replace the deleted node.
  NodeColor clr=y->color;               // The color of the node to delete.
  if (p->left==NULL)                    // Does the node to delete have a left child?
  {                                     // No, so we will
    x=p->right;                         // Set the node to replace with the right child.
    if (x!=NULL)                        // Is the right child Null?
      ReplaceNode(tree,p,x);            // No, replace the node with the right child.
  }                                     // Done with NULL left subtree ptr.
  else if (p->right==NULL)              // Does the node to delete have a right child?
  {                                     // No, so we will...
    x=p->left;                          // Set the node to replace with the left child.
    if (x!=NULL)                        // Is the left child NULL?
      ReplaceNode(tree,p,x);            // No, replace the node with the left child.
  }                                     // Done with NULL right subtree.
  else                                  // Else the node to delete has two children.
  {                                     // So we will...
    y=FindMin(p->right);                // Find the min node in the right subtree.
    clr=y->color;                       // Save the color of the node to delete.
    x=y->right;                         // Set the node to replace as the right child.
    if (y->parent==p)                   // Is the parent of the min node the node to delete?
      if (x!=NULL)                      // Is the replacement node NULL?
        x->parent=y;                    // No, so set the parent of the replacement node.
    else                                // Else the parent of the node to delete is not the min node.
    {                                   // So we will..
      ReplaceNode(tree,y,y->right);     // Replace the min node with its right child.
      y->right=p->right;                // Set the right child of the min node to the right child of the node to delete.
      y->right->parent=y;               // Set the parent of the right child to the min node.
    }                                   // Done with else parent of min node not the node to delete.
    ReplaceNode(tree,p,y);              // Replace the node to delete with the min node.
    y->left=p->left;                    // Update y's left pointer.
    y->left->parent=y;                  // Update y's left child's parent.
    y->color=p->color;                  // Update y's color.
  }                                     // Done with else two children.
  status=pthread_mutex_destroy(&p->nmtx);// Destroy the node's mutex.
  if (status!=0)                        // Did we fail to destroy the mutex?
    errExitEN(status,"pthread_mutex_destroy");// Yes, print error and exit.
  free(p);                              // Free the node.
  p=NULL;                               // Set the pointer to NULL.
  if (clr==BLACK)                       // Is the original color BLACK?
    FixDelete(tree,x);                  // Yes, so fix the tree.
  tree->size--;                         // Decrement the size of the tree, we just deleted a node.
  status=pthread_mutex_unlock(&tree->tmtx);// Unlock the tree's mutex.
  if (status!=0)                        // Did we fail to unlock the mutex?
    errExitEN(status,"pthread_mutex_unlock");// Yes, print error and exit.
}                                       // ------------- DeleteNode -------------
// A function to fix the tree after deletion.
void FixDelete (
    Tree* tree,                         // The tree to fix
    TreeNode* p)                        // The node of the tree that needs fixing.
{                                       // ------------- FixDelete --------------
  while (p != tree->root && (p == NULL || p->color == BLACK))
  {
    TreeNode* w = NULL;                 // The sibling of the node to fix.
    if (p->parent != NULL) {            // Ensure p->parent is not NULL.
      if (p == p->parent->left) {       // Is the node the left child of its parent?
        w = p->parent->right;           // No, the sibling is the right child.
      } else {                          // Otherwise, it is the left child.
        w = p->parent->left;            // Yes, set sibling as left child.
      }                                 // Done setting sibling as left child.                                
    }                                   // Done with existence of parent.
    if (w == NULL) {                    // Is the sibling NULL?
      break;                            // Yes, so break from the while loop.                     
    }                                   // Done checking for leaf brother.
    if (w->color == RED) {              // Is the sibling red?
      w->color = BLACK;                 // Set the sibling to black.
      p->parent->color = RED;           // Set the parent to red.
      // ------------------------------ //
      // Now we have to rotate based on the position of p
      // ------------------------------ //
      if (p == p->parent->left) {       // Is p it's parent's left uncle?
        LeftRotate(tree, p->parent);    // Yes, rotate the tree to the left.
        w = p->parent->right;           // Update the sibling.
      } else {                          // Else, it is its right uncle.                
        RightRotate(tree, p->parent);   // Rotate the tree to the right.
        w = p->parent->left;            // Update the sibling.
      }                                 // Done with right uncle.
    }                                   // Done with p-based rotation.
    // -------------------------------- //
    // Now we have to enfore color rules of the tree.
    // Are the left and right child of the sibling NULL and BLACK?
    // -------------------------------- //
    if ((w->left == NULL || w->left->color == BLACK) &&
        (w->right == NULL || w->right->color == BLACK)) 
    {
      w->color = RED;                   // Yes, set the sibling to red.
      p = p->parent;                    // Move p up the tree.
    } else {                            // Else they are not one of those.
      if (p == p->parent->left) {       // Is p the left child?
        // ---------------------------- //
        // Is w's right child NULL or BLACK?
        // ---------------------------- //
        if (w->right == NULL || w->right->color == BLACK) {
          if (w->left != NULL) {        // .. and left child not NULL?
            w->left->color = BLACK;     // Yes, set the left child of the sibling to black.
          }                             // Done setting left child of the sibling to black.
          w->color = RED;               // Set the sibling to red.
          RightRotate(tree, w);         // Rotate the tree to the right.
          w = p->parent->right;         // Update the sibling.
        }                               // Done handling if right child NULL or BLACK.
        w->color = p->parent->color;    // Set the sibling's color to the parent's color.
        p->parent->color = BLACK;       // Set the parent to black.
        if (w->right != NULL) {         // Is the right child NULL?
          w->right->color = BLACK;      // Set the right child of the sibling to black.
        }                               // Done checking if right child NULL?   
        LeftRotate(tree, p->parent);    // Rotate the tree to the left.
        p = tree->root;                 // Set p to the root.
      } else {                          // If p is the right child.
        // ---------------------------- //
        // Is the left child NULL or BLACK?
        // ---------------------------- //
        if (w->left == NULL || w->left->color == BLACK) {
          if (w->right != NULL) {       // Is the right child NULL?
            w->right->color = BLACK;    // No, set the right child of the sibling to black.
          }                             // Done checking if right child not a leaf.
          w->color = RED;               // Set the sibling to red.
          LeftRotate(tree, w);          // Rotate the tree to the left.
          w = p->parent->left;          // Update the sibling.
        }                               // Done handling leafy left child or black.
        w->color = p->parent->color;    // Set the sibling's color to the parent's color.
        p->parent->color = BLACK;       // Set the parent to black.
        if (w->left != NULL) {          // Is the left child NULL?
          w->left->color = BLACK;       // No, set the left child of the sibling to black.
        }                               // Done checking if left child not a leaf.
        RightRotate(tree, p->parent);   // Rotate the tree to the right.
        p = tree->root;                 // Set p to the root.
      }                                 // Done checking if p is the right child.
    }                                   // Done checking if left or right child NULL or BLACK.
  }                                     // Done with while p is not root and p is NULL or BLACK
  if (p != NULL) {                      // Is p NULL?
    p->color = BLACK;                   // No, set the node to black.
  }                                     // Done checking if p is NULL.
}                                       // ------------- FixDelete --------------

// A function to fix tree after insertion
void FixInsert (
    Tree* tree,                         // The tree to fix
    TreeNode* p)                        // The node to fix
{                                       // ------------- FixInsert -------------
  // While the node is not the root and the color of the node is red.
    // Then this means that we need to fix the tree.
    while (p!=tree->root && p->parent->color==RED)
    {                                   // Yes, so we will
      if (p->parent==p->parent->parent->left)// Is the parent of the node the left child?
      {                                 // Yes.
        TreeNode* y=p->parent->parent->right;// The uncle of the node to fix.
        if (y!=NULL && y->color==RED)   // Is the uncle red?
        {                               // Yes, so we will
          p->parent->color=BLACK;       // Set the parent to black.
          y->color=BLACK;               // Set the uncle to black.
          p->parent->parent->color=RED; // Set the grandparent to red.
          p=p->parent->parent;           // Set the node to the grandparent.
        }                               // Done with if uncle red.
        else                            // Else the uncle is not red.
        {                               // So we will..
          if (p==p->parent->right)     // Is the node the right child?
          {                             // Yes, so we will..
            p=p->parent;                // Set the node to the parent.
            LeftRotate(tree,p);         // Rotate the tree to the left.
          }                             // Done with if right child.
          p->parent->color=BLACK;       // Set the parent to black.
          p->parent->parent->color=RED; // Set the grandparent to red.
          RightRotate(tree,p->parent->parent);// Rotate the tree to the right.
        }                               // Done with else uncle not red.
      }                                 // Done with if parent left child.
      else                              // Else it is a right child.
      {                                 // Yes, it is.
        TreeNode* y=p->parent->parent->left;// The uncle of the node to fix.
        if (y!=NULL && y->color==RED)   // Is the uncle red?
        {                               // Yes, so we will
          p->parent->color=BLACK;       // Set the parent to black.
          y->color=BLACK;               // Set the uncle to black.
          p->parent->parent->color=RED;  // Set the grandparent to red.
          p=p->parent->parent;           // Set the node to the grandparent.
        }                               // Done with if uncle red.
        else                            // Else the uncle is not red.
        {                               // So we will..
          if (p==p->parent->left)      // Is the node the left child?
          {                             // Yes, so we will..
            p=p->parent;                // Set the node to the parent.
            RightRotate(tree,p);        // Rotate the tree to the right.
          }                             // Done with if left child.
          p->parent->color=BLACK;       // Set the parent to black.
          p->parent->parent->color=RED;  // Set the grandparent to red.
          LeftRotate(tree,p->parent->parent);// Rotate the tree to the left.
        }                               // Done with else uncle not red.
      }                                 // Done with else right child.
    }                                   // Done with while loop.
  tree->root->color=BLACK;              // Set the root to black.
}                                      // ------------- FixInsert -------------

// A function to Rotate the tree Left
void LeftRotate (
  Tree* tree,                           // The tree to rotate.
  TreeNode* p)                          // The node to rotate.
{                                       // ------------- LeftRotate -------------
    TreeNode* y=p->right;               // The right child of the node to rotate.
    p->right=y->left;                   // Set the right child of the node to the left child of the right child.
    if (y->left!=NULL)                  // Is the left child NULL?
        y->left->parent=p;              // No, set the parent of the left child to the node.
    y->parent=p->parent;                // Set the parent of the right child to the parent of the node.
    if (p->parent==NULL)                // Is the parent NULL?
        tree->root=y;                   // Yes, so set the root to the right child.
    else if (p==p->parent->left)        // Is the node the left child?
        p->parent->left=y;              // Yes, so set the left child of the parent to the right child.
    else                                // Else it is a right child.
        p->parent->right=y;             // So set the right child of the parent to the right child.
    y->left=p;                          // Set the left child of the right child to the node.
    p->parent=y;                        // Set the parent of the node to the right child.
    y->color=p->color;                  // Set the color of the right child to the color of the node.
    p->color=RED;                       // Set the color of the node to red.
    p->right=y->left;                   // Set the right child of the node to the left child of the right child.
    if (y->left!=NULL)                  // Is the left child NULL?
        y->left->parent=p;              // No, set the parent of the left child to the node.
    y->left=p;                          // Set the left child of the right child to the node.
    p->parent=y;                        // Set the parent of the node to the right child.
}                                       // ------------- LeftRotate -------------
// A function to Rotate the tree right.
void RightRotate (
  Tree* tree,                           // The tree to rotate
  TreeNode* p)                          // The node of the tree to rotate.
{                                       // ------------- RightRotate -------------
    TreeNode* y=p->left;                // The left child of the node to rotate.
    p->left=y->right;                   // Set the left child of the node to the right child of the left child.
    if (y->right!=NULL)                 // Is the right child NULL?
        y->right->parent=p;             // No, set the parent of the right child to the node.
    y->parent=p->parent;                // Set the parent of the left child to the parent of the node.
    if (p->parent==NULL)                // Is the parent NULL?
        tree->root=y;                   // Yes, so set the root to the left child.
    else if (p==p->parent->right)       // Is the node the right child?
        p->parent->right=y;             // Yes, so set the right child of the parent to the left child.
    else                                // Else it is a left child.
        p->parent->left=y;              // So set the left child of the parent to the left child.
    y->right=p;                         // Set the right child of the left child to the node.
    p->parent=y;                        // Set the parent of the node to the left child.
    y->color=p->color;                  // Set the color of the left child to the color of the node.
    p->color=RED;                       // Set the color of the node to red.
    p->left=y->right;                   // Set the left child of the node to the right child of the left child.
    if (y->right!=NULL)                 // Is the right child NULL?
        y->right->parent=p;             // No, set the parent of the right child to the node.
    y->right=p;                         // Set the right child of the left child to the node.
    p->parent=y;                        // Set the parent of the node to be equal to y. 
}                                       // ------------- RightRotate -------------

// A function to look up a key in the tree.
bool Lookup (
    Tree* tree,                         // The tree to look in
    char* key,                          // The key to look up.
    int* value)                         // The value to return.
{                                       // ------------- Lookup ----------------
  int status=0;                         // Status code.
  TreeNode* p=NULL;                     // Pointer to the node to look up.
  status=pthread_mutex_lock(&tree->tmtx);// Lock the tree's mutex.
  if (status!=0)                        // Did we fail to lock the mutex?
    return false;                       // Yes so return
  p=tree->root;                         // Start at the root of the tree.
  while (p!=NULL && strcmp(key,p->key)!=0)// While we haven't found the node to look up.
  {                                     // Search for the node be looked up.
    if (strcmp(key,p->key)<0)           // Is the key less than the current key?
      p=p->left;                        // Yes, so we go left.
    else                                // Else the key is greater than the current key.
      p=p->right;                       // So we go right.
  }                                     // Done searching for the node to look up.
  if (p==NULL)                          // Did we find the node to look up?
  {                                     // No, so we will..
    status=pthread_mutex_unlock(&tree->tmtx);// Unlock the tree's mutex.
    if (status!=0)                      // Did we fail to unlock the mutex?
      errExitEN(status,"pthread_mutex_unlock");// Yes, print error and exit.
    return false;                       // Return false, no key was found.
  }                                     // Done checking if no node to look up.
  *value=p->value;                      // Set the value to be returned to the value of the node found.
  status=pthread_mutex_unlock(&tree->tmtx);// Unlock the tree's mutex.
  if (status!=0)                        // Did we fail to unlock the mutex?
    errExitEN(status,"pthread_mutex_unlock");// Yes, print error and exit.          
  return true;                          // Return true, key was found. 
}                                       // ------------- Lookup ----------------

