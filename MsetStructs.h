// COMP2521 24T3 - Assignment 1
// definition of structs used in the Multiset ADT
// Written by Melina Salardini (z5393518)
// on 20/10/2024

#ifndef MSET_STRUCTS_H
#define MSET_STRUCTS_H

// IMPORTANT: Only structs should be placed in this file.
//            All other code should be placed in Mset.c.

// DO NOT MODIFY THE NAME OF THIS STRUCT
struct mset {
  struct node *tree; // DO NOT MODIFY/REMOVE THIS FIELD
  int size;
  int total_count;
};

// DO NOT MODIFY THE NAME OF THIS STRUCT
struct node {
  int elem;           // DO NOT MODIFY/REMOVE THIS FIELD
  int count;          // DO NOT MODIFY/REMOVE THIS FIELD
  struct node *left;  // DO NOT MODIFY/REMOVE THIS FIELD
  struct node *right; // DO NOT MODIFY/REMOVE THIS FIELD

  int height;
};

// You may define more structs here if needed

////////////////////////////////////////////////////////////////////////
// Cursors

struct cursor {
  // You may add more fields here if needed
};

////////////////////////////////////////////////////////////////////////

#endif
