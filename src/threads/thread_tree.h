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
#ifndef THREAD_TREE_H
#define THREAD_TREE_H
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include "tlpi_hdr.h"

// Define the color of the tree node (red or black).
typedef enum
{
    RED,                                // Red color.
    BLACK                               // Black color.
} NodeColor;                            // Color of each node.

// Define the structure for each element in the tree
typedef struct TreeNode
{
    char* key;                          // The key of the node.
    int value;                          // Value of the element.
    int height;                         // Height of the element.
    NodeColor color;                    // Color of the element.
    pthread_mutex_t nmtx;               // Mutex to protect the element.
    struct TreeNode *left;              // Pointer to the left child.
    struct TreeNode *right;             // Pointer to the right child.
    struct TreeNode *parent;            // Pointer to the root of the tree.
} TreeNode;                             // TreeNode structure.
// Define the structure for the tree
typedef struct Tree
{
    TreeNode *root;                     // Pointer to the root of the tree.
    pthread_mutex_t tmtx;               // Mutex to protect the tree.
    int size;                           // Number of nodes in the tree.             
} Tree;                                 // Tree structure.

// A function to initialize the Red-Black tree.
void Initialize(Tree* tree);
// A function to destroy the tree.
void DestroyTree(Tree* tree);
// A function to delete a subtree.
void DestroySubtree(TreeNode* node);
// A function to insert or add a node.
void InsertNode(Tree* tree, char* key, int value);
// A function to delete a node from the tree.
void DeleteNode(Tree* tree, char* key);
// A function to search the Red-Black Tree.
bool Lookup(Tree* tree,char* key, int* value);
// A function to balance the tree.
//void BalanceTree(Tree* tree, TreeNode* node);
// A function to rotate a node left.
void LeftRotate(Tree* tree,TreeNode* node);
// A function to rotate a node right.
void RightRotate(Tree* tree,TreeNode* node);
// A function to balance after insertion.
void FixInsert(Tree* tree,TreeNode* node);
// A function to balance after deletion.
void FixDelete(Tree* tree,TreeNode* node);
// A function to find the minimum node in the tree.
TreeNode* FindMin(TreeNode* node);
// A function to find the maximum node in a tree.
TreeNode* FindMax(TreeNode* node);
// A function to replace one subtree with another.
void ReplaceNode(Tree* tree, TreeNode* p, TreeNode* q);

#endif 
