// COMP2521 24T3 - Assignment 1
// Implementation of the Multiset ADT
// Written by Melina Salardini (z5393518)
// on 20/10/2024

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Mset.h"
#include "MsetStructs.h"

// define constants
#define INITIAL_HEIGHT 1
#define HEIGHT_OFFSET 1

// function prototypes
Mset MsetNew(void);
void MsetFree(Mset s);
static void treeFree(struct node *tree);
void MsetInsert(Mset s, int item);
static struct node *treeInsert(Mset s, struct node *tree, int item, int count);
static struct node *nodeNew(int element, int count);
void MsetInsertMany(Mset s, int item, int amount);
void MsetDelete(Mset s, int item);
static struct node *treeDelete(Mset s, struct node *tree, int item, int amount);
static struct node *handleTwoChildren(Mset s, struct node *tree);
static struct node *findMin(struct node *tree);
static struct node *handleOneOrNoChildren(struct node *tree);
void MsetDeleteMany(Mset s, int item, int amount);
static int treeGetCount(struct node *tree, int item);
int MsetGetCount(Mset s, int item);
void MsetPrint(Mset s, FILE *file);
static void printInOrder(struct node *tree, FILE *file, bool *isFirst);
Mset MsetUnion(Mset s1, Mset s2);
static void addUnionTree(Mset newMset, struct node *tree);
static struct node *findNode(struct node *tree, int elem);
Mset MsetIntersection(Mset s1, Mset s2);
static void addIntersectionTree(Mset newMset, struct node *t1, struct node *t2);
bool MsetIncluded(Mset s1, Mset s2);
static bool treeIncluded(struct node *t1, struct node *t2);
bool MsetEquals(Mset s1, Mset s2);
static bool treeEquals(struct node *t1, struct node *t2);
static int sort(const void *a, const void *b);
static void collectItems(struct node *tree, struct item allItems[], int *index);
int MsetMostCommon(Mset s, int k, struct item items[]);
static int max(int a, int b);
static int getBalance(struct node *node);
static int getHeight(struct node *tree);
static struct node *rotateLeft(struct node *x);
static struct node *rotateRight(struct node *y);
static struct node *balanceTree(struct node *tree);

////////////////////////////////////////////////////////////////////////
// Basic Operations

/**
 * Creates a new empty multiset.
 */
Mset MsetNew(void) {
  // allocating memory for the new multi set
  Mset newMset = malloc(sizeof(struct mset));
  if (newMset == NULL) {
    fprintf(stderr, "Error: Memory allocation failed for the new multi set");
    return NULL;
  }

  // initilise the struct fields
  newMset->tree = NULL;
  newMset->size = 0;
  newMset->total_count = 0;

  return newMset;
}

/**
 * Frees all memory allocated to the multiset.
 */
void MsetFree(Mset s) {
  if (s == NULL) {
    return;
  }

  // free every node in the tree
  treeFree(s->tree);
  // free the multiset itself
  free(s);
}

// a function to free all the nodes in a tree
static void treeFree(struct node *tree) {
  if (tree == NULL) {
    return;
  }

  treeFree(tree->right);
  treeFree(tree->left);
  free(tree);
}

/**
 * Inserts one of an item into the multiset. Does nothing if the item is
 * equal to UNDEFINED.
 */
void MsetInsert(Mset s, int item) {
  if (item == UNDEFINED) {
    return;
  }

  // insert the item into the tree
  if (s->tree != NULL) {
    s->tree = treeInsert(s, s->tree, item, 1);
  } else {
    s->tree = nodeNew(item, 1);
    // increase the size of the Mset and the total_count
    s->size++;
    s->total_count += 1;
  }
}

// a function to insert an item into a tree
static struct node *treeInsert(Mset s, struct node *tree, int item, int count) {
  if (tree == NULL) {
    // increase the size of the Mset and the total_count
    s->size++;
    s->total_count += count;

    return nodeNew(item, count);
  }

  if (item > tree->elem) {
    tree->right = treeInsert(s, tree->right, item, count);
  } else if (item < tree->elem) {
    tree->left = treeInsert(s, tree->left, item, count);
  } else {
    tree->count = tree->count + count;
    // increase the total_count
    s->total_count += count;
    return tree;
  }

  return balanceTree(tree);
}

// a function to create a new node and initilize different fields of it
static struct node *nodeNew(int element, int count) {
  struct node *new = malloc(sizeof(struct node));
  if (new == NULL) {
    return NULL;
  }

  new->elem = element;
  new->count = count;
  new->height = INITIAL_HEIGHT;
  new->right = NULL;
  new->left = NULL;

  return new;
}

// function to balance the tree with rotation if needed
static struct node *balanceTree(struct node *tree) {
  // update the height of this node
  tree->height =
      HEIGHT_OFFSET + max(getHeight(tree->left), getHeight(tree->right));

  // get the balance factor to check if balancing is needed
  int balance = getBalance(tree);

  // perform rotations based on balance factor
  // Left Left Case
  if (balance > 1 && getBalance(tree->left) >= 0) {
    return rotateRight(tree);
  }

  // Left Right Case
  if (balance > 1 && getBalance(tree->left) < 0) {
    tree->left = rotateLeft(tree->left);
    return rotateRight(tree);
  }

  // Right Right Case
  if (balance < -1 && getBalance(tree->right) <= 0) {
    return rotateLeft(tree);
  }

  // Right Left Case
  if (balance < -1 && getBalance(tree->right) > 0) {
    tree->right = rotateRight(tree->right);
    return rotateLeft(tree);
  }

  return tree;
}

// function to get the height of the tree
static int getHeight(struct node *tree) {
  if (tree == NULL) {
    return -1;
  }

  return tree->height;
}

// function get the balance factor of the tree
static int getBalance(struct node *node) {
  if (node == NULL) {
    return 0;
  }

  return getHeight(node->left) - getHeight(node->right);
}

// function to get the max of the two values
static int max(int a, int b) { return (a > b) ? a : b; }

// function to right rotate a subtree
static struct node *rotateRight(struct node *y) {
  struct node *x = y->left;
  struct node *T2 = x->right;

  // perform rotation
  x->right = y;
  y->left = T2;

  // update the height
  y->height = max(getHeight(y->left), getHeight(y->right)) + HEIGHT_OFFSET;
  x->height = max(getHeight(x->left), getHeight(x->right)) + HEIGHT_OFFSET;

  return x;
}

// function to left rotate a subtree
static struct node *rotateLeft(struct node *x) {
  struct node *y = x->right;
  struct node *T2 = y->left;

  // perform rotation
  y->left = x;
  x->right = T2;

  // update height
  x->height = max(getHeight(x->left), getHeight(x->right)) + HEIGHT_OFFSET;
  y->height = max(getHeight(y->left), getHeight(y->right)) + HEIGHT_OFFSET;

  return y;
}

/**
 * Inserts the given amount of an item into the multiset. Does nothing
 * if the item is equal to UNDEFINED or the given amount is 0 or less.
 */
void MsetInsertMany(Mset s, int item, int amount) {
  if (item == UNDEFINED) {
    return;
  }
  // if amount is 0 or less do nothing
  if (amount <= 0) {
    return;
  }

  if (s->tree != NULL) {
    s->tree = treeInsert(s, s->tree, item, amount);
  } else {
    s->tree = nodeNew(item, amount);
    // increase the size of the Mset and the total_count
    s->size++;
    s->total_count += amount;
  }
}

/**
 * Deletes one of an item from the multiset.
 */
void MsetDelete(Mset s, int item) {
  if (s == NULL) {
    return;
  }
  s->tree = treeDelete(s, s->tree, item, 1);
}

// a function to find the value in the tree and delete it
static struct node *treeDelete(Mset s, struct node *tree, int item,
                               int amount) {
  if (tree == NULL) {
    return NULL;
  }

  if (item < tree->elem) {
    tree->left = treeDelete(s, tree->left, item, amount);
  } else if (item > tree->elem) {
    tree->right = treeDelete(s, tree->right, item, amount);
  } else {
    // decrease the total_count and handle when deleting more than count
    if (amount > tree->count) {
      s->total_count -= tree->count;
    } else {
      s->total_count -= amount;
    }

    tree->count -= amount;
    // delete the whole node if the count is 0 or less
    if (tree->count <= 0) {
      // decreasing the size of the Mset
      s->size--;

      // if the node has one or no child
      if (tree->left == NULL || tree->right == NULL) {
        return handleOneOrNoChildren(tree);
      }
      // if the node has both right and left child
      return handleTwoChildren(s, tree);
    }
  }

  return balanceTree(tree);
}

// function to handle deleting the node that has one or no children
static struct node *handleOneOrNoChildren(struct node *tree) {
  // if there was no children
  if (tree->right == NULL && tree->left == NULL) {
    free(tree);
    return NULL;
  }

  // if there is one child
  struct node *temp = NULL;
  if (tree->left == NULL) {
    temp = tree->right;
  } else {
    temp = tree->left;
  }
  free(tree);
  return temp;
}

// function to find the min value node in the subtree
static struct node *findMin(struct node *tree) {
  while (tree->left != NULL) {
    tree = tree->left;
  }
  return tree;
}

// function to handle deleting a node when it has both right and left children
static struct node *handleTwoChildren(Mset s, struct node *tree) {
  // find the in-order successor which is the min node in the right subtree
  struct node *minNode = findMin(tree->right);

  // replace the current node fileds with the successor
  tree->elem = minNode->elem;
  tree->count = minNode->count;

  // Delete the in-order successor from the right subtree
  tree->right = treeDelete(s, tree->right, minNode->elem, minNode->count);

  return tree;
}

/**
 * Deletes the given amount of an item from the multiset.
 */
void MsetDeleteMany(Mset s, int item, int amount) {
  if (s == NULL) {
    return;
  }
  s->tree = treeDelete(s, s->tree, item, amount);
}

/**
 * Returns the number of distinct elements in the multiset.
 */
int MsetSize(Mset s) { return s->size; }

/**
 * Returns the sum of counts of all elements in the multiset.
 */
int MsetTotalCount(Mset s) { return s->total_count; }

/**
 * Returns the count of an item in the multiset, or 0 if it doesn't
 * occur in the multiset.
 */
int MsetGetCount(Mset s, int item) {
  if (s == NULL) {
    return 0;
  }

  return treeGetCount(s->tree, item);
}

// return the count of an item in a tree, or 0 if it doesnt exists
static int treeGetCount(struct node *tree, int item) {
  if (tree == NULL) {
    return 0;
  }

  if (item > tree->elem) {
    return treeGetCount(tree->right, item);
  } else if (item < tree->elem) {
    return treeGetCount(tree->left, item);
  } else {
    return tree->count;
  }
}

/**
 * Prints the multiset to a file.
 * The elements of the multiset should be printed in ascending order
 * inside a pair of curly braces, with elements separated by a comma
 * and space. Each element should be printed inside a pair of
 * parentheses with its count, separated by a comma and space.
 */
void MsetPrint(Mset s, FILE *file) {
  fprintf(file, "{");

  if (s->tree != NULL) {
    bool isFirst = true;
    printInOrder(s->tree, file, &isFirst);
  }

  fprintf(file, "}");
}

// function to print the nodes in the tree in order
// 1) left subtree   2) root   3) right subtree
static void printInOrder(struct node *tree, FILE *file, bool *isFirst) {
  if (tree == NULL) {
    return;
  }
  // print the left subtree
  printInOrder(tree->left, file, isFirst);

  // print the current node

  // print a ',' before if its not the root
  if (!(*isFirst)) {
    fprintf(file, ", ");
  }
  fprintf(file, "(%d, %d)", tree->elem, tree->count);
  // turn the isFirst flag to false when reaching this point
  *isFirst = false;

  // print the right subtree
  printInOrder(tree->right, file, isFirst);
}

////////////////////////////////////////////////////////////////////////
// Advanced Operations

/**
 * Returns a new multiset representing the union of the two given
 * multisets.
 */
Mset MsetUnion(Mset s1, Mset s2) {
  Mset newMset = MsetNew();

  // add s1 and s2 nodes to the newMset
  addUnionTree(newMset, s1->tree);
  addUnionTree(newMset, s2->tree);

  return newMset;
}

// a function to add a whole tree to a multiset if the multiset doesnt contain
// the new element
// and update the count of the node to match the max of the two nodes if
// the new element exists
static void addUnionTree(Mset newMset, struct node *tree) {
  if (tree == NULL) {
    return;
  }
  // recursively process the left subtree
  addUnionTree(newMset, tree->left);

  struct node *node = findNode(newMset->tree, tree->elem);
  // if the node already exists in the newMset change the count the max of them
  if (node != NULL) {
    if (node->count < tree->count) {
      // update the total count of the newMset
      newMset->total_count = newMset->total_count - node->count + tree->count;

      node->count = tree->count;
    }
  } else {
    // insert the new node that is not already in the newMset
    MsetInsertMany(newMset, tree->elem, tree->count);
  }

  // recursively process the right subtree
  addUnionTree(newMset, tree->right);
}

// a function to find a node with a spesific element in a tree
static struct node *findNode(struct node *tree, int elem) {
  if (tree == NULL) {
    return NULL;
  }

  if (elem < tree->elem) {
    return findNode(tree->left, elem);
  } else if (elem > tree->elem) {
    return findNode(tree->right, elem);
  } else {
    // found the element
    return tree;
  }
}

/**
 * Returns a new multiset representing the intersection of the two
 * given multisets.
 */
Mset MsetIntersection(Mset s1, Mset s2) {
  Mset newMset = MsetNew();

  // add the intersection of the s1 and s2 trees
  addIntersectionTree(newMset, s1->tree, s2->tree);

  return newMset;
}

// a function to add the intersection of two trees to the new multiset
static void addIntersectionTree(Mset newMset, struct node *t1,
                                struct node *t2) {
  if (t1 == NULL || t2 == NULL) {
    return;
  }
  // recursively add the left subtree of t1 if eligable
  addIntersectionTree(newMset, t1->left, t2);

  struct node *t2Node = findNode(t2, t1->elem);
  // add the node to the newMset if we found the node succesfully
  if (t2Node != NULL) {
    // add the min count to the newMset
    if (t2Node->count > t1->count) {
      MsetInsertMany(newMset, t1->elem, t1->count);
    } else {
      MsetInsertMany(newMset, t2Node->elem, t2Node->count);
    }
  }

  // recursevly add the right subtree of t1 if eligable
  addIntersectionTree(newMset, t1->right, t2);
}

/**
 * Returns true if the multiset s1 is included in the multiset s2, and
 * false otherwise.
 */
bool MsetIncluded(Mset s1, Mset s2) { return treeIncluded(s1->tree, s2->tree); }

// a function that return true if t2 contains every node in the t1 and return
// false otherwise
static bool treeIncluded(struct node *t1, struct node *t2) {
  if (t1 == NULL && t2 == NULL) {
    return true;
  }
  if (t1 == NULL && t2 != NULL) {
    return true;
  }
  if (t1 != NULL && t2 == NULL) {
    return false;
  }

  struct node *node = findNode(t2, t1->elem);
  // if the node exists in t2 but the count is not the same return false
  if (node != NULL) {
    if (node->count < t1->count) {
      return false;
    }
  } else {
    return false;
  }

  return treeIncluded(t1->right, t2) && treeIncluded(t1->left, t2);
}

/**
 * Returns true if the two given multisets are equal, and false
 * otherwise.
 */
bool MsetEquals(Mset s1, Mset s2) {
  // if the size or even the total_count of the Msets
  // are not the same return false
  if (s1->size != s2->size || s1->total_count != s2->total_count) {
    return false;
  }

  return treeEquals(s1->tree, s2->tree);
}

// a function that returns true if the two trees are euqual and
// returns false otherwise
static bool treeEquals(struct node *t1, struct node *t2) {
  if (t1 == NULL) {
    return true;
  }

  struct node *node = findNode(t2, t1->elem);
  if (node != NULL) {
    if (node->count != t1->count) {
      return false;
    }
  } else {
    return false;
  }

  return treeEquals(t1->right, t2) && treeEquals(t1->left, t2);
}

/**
 * Stores the k most common elements in the multiset into the given
 * items array in decreasing order of count and returns the number of
 * elements stored. Elements with the same count should be stored in
 * increasing order. Assumes that the items array has size k.
 */
int MsetMostCommon(Mset s, int k, struct item items[]) {
  if (k <= 0 || s == NULL) {
    return 0;
  }

  // collect all the elements of the tree in the allItems array
  struct item *allItems = malloc(s->size * sizeof(struct item));
  if (allItems == NULL) {
    fprintf(stderr, "Memory allocation failed");
    return 0;
  }

  // collect all the items into the allItems array
  int index = 0;
  collectItems(s->tree, allItems, &index);

  // sort the items in the array base on their count
  qsort(allItems, s->size, sizeof(struct item), sort);

  // collect the kth elements
  int i;
  for (i = 0; i < k && i < s->size; i++) {
    items[i] = allItems[i];
  }

  free(allItems);
  return i;
}

// a function to collect all the items in a tree into a array
// this will be sorted in a decending order base on the 'elem' field
static void collectItems(struct node *tree, struct item allItems[],
                         int *index) {
  if (tree == NULL) {
    return;
  }
  // traverse the left subtree
  collectItems(tree->left, allItems, index);

  // store the current node
  allItems[*index].elem = tree->elem;
  allItems[*index].count = tree->count;
  (*index)++;

  // traverse the right subtree
  collectItems(tree->right, allItems, index);
}

// function to sort the struct item array base on their count first and
// second base on their elem
static int sort(const void *a, const void *b) {
  struct item *itemA = (struct item *)a;
  struct item *itemB = (struct item *)b;

  // first sort by 'count' in decending order
  if (itemA->count != itemB->count) {
    return itemB->count - itemA->count;
  } else {
    // sort by 'elem' in ascending order
    return itemA->elem - itemB->elem;
  }
}

////////////////////////////////////////////////////////////////////////
// Cursor Operations

/**
 * Creates a new cursor positioned at the start of the multiset.
 * (see spec for explanation of start and end)
 */
MsetCursor MsetCursorNew(Mset s) {
  // TODO
  return NULL;
}

/**
 * Frees all memory allocated to the given cursor.
 */
void MsetCursorFree(MsetCursor cur) {
  // TODO
}

/**
 * Returns the element at the cursor's position and its count, or
 * {UNDEFINED, 0} if the cursor is positioned at the start or end of
 * the multiset.
 */
struct item MsetCursorGet(MsetCursor cur) {
  // TODO
  return (struct item){UNDEFINED, 0};
}

/**
 * Moves the cursor to the next greatest element, or to the end of the
 * multiset if there is no next greatest element. Does not move the
 * cursor if it is already at the end. Returns false if the cursor is at
 * the end after this operation, and true otherwise.
 */
bool MsetCursorNext(MsetCursor cur) {
  // TODO
  return false;
}

/**
 * Moves the cursor to the next smallest element, or to the start of the
 * multiset if there is no next smallest element. Does not move the
 * cursor if it is already at the start. Returns false if the cursor is
 * at the start after this operation, and true otherwise.
 */
bool MsetCursorPrev(MsetCursor cur) {
  // TODO
  return false;
}

////////////////////////////////////////////////////////////////////////
