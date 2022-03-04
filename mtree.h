/**
 * Merkle tree implementation for cryptographic applications.
 *
 * The main function build_tree() builds a Merkle tree of given text list.
 * The size of text list (LEAF_COUNT) must be the powers of 2.
 * The LEAF_COUNT (also NODE_COUNT and PATH_SIZE) can be updated from constant definitions.
 *
 *
 * FIPS202 SHAKE256 is used for hashing. 128-bit output is used in this version.
 * H_BYTES (hash bytes) and H_BITS (hash bits) can be updated as instructed in FIPS202.
 *
 *
 * Gamze Orhon Kilic.
 * 2021.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fips202.c"

//**** Constant definitions
#define H_BYTES 			16
#define H_BITS 				128
#define LEAF_COUNT		8
#define NODE_COUNT		((LEAF_COUNT)*2)-1
#define PATH_SIZE			(int)(log(LEAF_COUNT)/log(2))

//**** Merkle tree node and tree representations
struct NODE
{
  int node_id;
  unsigned char* hashval;
  int prev;
  int next;
  int parent;
};
typedef struct
{
  int leaf_count;
  int node_count;
  struct NODE* merkle_tree;
}TREE_t[0], *TREE;

//**** Helper function - convert_binary - ****//
// Converts an unsigned char array to binary array.
int convert_binary(unsigned char* hashval, int* binary)
{
	int i,j;
	int index =0;
	for(i=H_BYTES-1;i>=0;i--)
	{
		for(j = 0; j < 8; j++)
		{
			binary[H_BITS-index] = (hashval[i] >> j) & 1;
			index++;
		}
	}
	return 0;
}

//**** Helper function - get_concat_hash - ****//
// Computes the hash of the concatanation of given two texts.
int get_concat_hash(unsigned char* msg, unsigned char* msg1, unsigned char* md_value)
{
  int i;
	int* binary1 = (int*)malloc(sizeof(int)*H_BITS);
	int* binary2 = (int*)malloc(sizeof(int)*H_BITS);
	int* binary3 = (int*)malloc(sizeof(int)*H_BITS);

	//**** Convert binary ****//
	convert_binary(msg, binary1);
	convert_binary(msg1, binary2);

	//**** XOR binary ****//
	for(i = 0; i<H_BITS; i++)
		binary3[i] = binary1[i]^binary2[i];

	//**** Get hash of concatenated binaries ****//
	shake256(md_value, H_BYTES, (unsigned char*)binary3, H_BITS);

  return 0;
}

//**** Helper function - compare_hash - ****//
//Returns 1 if given two hashes are equal.
int compare_hash(unsigned char* hash1, unsigned char* hash2)
{
  int i;
  for(i=0;i<H_BYTES;i++)
  {
    if(hash1[i]!=hash2[i])
    {
      return 0;
    }
  }
  return 1;
}

//**** Main function - build_tree - ****//
// Constructs the Merkle tree for given list of unsigned char arrays
int build_tree(TREE mtree, unsigned char** hlist, int length)
{
	int i,j;
  int node_count = mtree->node_count;
  int leaf_count = mtree->leaf_count;

	//**** Init tree starts ****//
  for (i = 0; i < node_count; i++)
  {
    mtree->merkle_tree[i].hashval = (unsigned char*)malloc(sizeof(unsigned char)*H_BYTES);
    mtree->merkle_tree[i].node_id = i;
    mtree->merkle_tree[i].parent = -1;
    mtree->merkle_tree[i].prev = -1;
    mtree->merkle_tree[i].next = -1;
  }
	//**** Init tree ends ****//

	//**** Fill leaves starts ****//
  for (i = 0; i < leaf_count; i++)
	{
		//The prev and next of the first and the last nodes of a layer are -1
    if (i==0)
    {
      mtree->merkle_tree[i].prev = -1;
      mtree->merkle_tree[i].next = mtree->merkle_tree[i+1].node_id;
    }
    else if(i==leaf_count-1)
    {
      mtree->merkle_tree[i].prev  = mtree->merkle_tree[i-1].node_id;
      mtree->merkle_tree[i].next = -1;
    }
    else
    {
      mtree->merkle_tree[i].prev  = mtree->merkle_tree[i-1].node_id;
      mtree->merkle_tree[i].next = mtree->merkle_tree[i+1].node_id;
    }
		//Get hash of the leaf node
		shake256(mtree->merkle_tree[i].hashval, H_BYTES, hlist[i],  length);
  }
	//**** Fill leaves ends ****//

	//**** Variables for filling the inner nodes ****//
  j=0;
  int layer_node_count = leaf_count/2; //Stores the node count of current layer
  int sub_len = leaf_count; //Stores the node count of child layer
  int in = leaf_count; //Stores the index of current node

	//**** Fill inner nodes starts ****//
  while (in < node_count)
  {
    while(j<sub_len-1)
    {
      if(in!=node_count-1) //If the current node is not the root
      {
				//The prev and next of the first and the last nodes of a layer are -1
        if (in==sub_len)
        {
          mtree->merkle_tree[in].prev = -1;
          mtree->merkle_tree[in].next = mtree->merkle_tree[in+1].node_id;
        }
        else if(in==sub_len+layer_node_count-1)
        {
          mtree->merkle_tree[in].prev  = mtree->merkle_tree[in-1].node_id;
          mtree->merkle_tree[in].next = -1;
        }
        else
        {
          mtree->merkle_tree[in].prev  = mtree->merkle_tree[in-1].node_id;
          mtree->merkle_tree[in].next = mtree->merkle_tree[in+1].node_id;
        }
      }

			//**** Update the parent of child nodes of current node ****//
      mtree->merkle_tree[j].parent = in;
      mtree->merkle_tree[j+1].parent = in;

			//**** Get concatenated hash of child nodes and store it in hashval of current node ****//
      get_concat_hash(mtree->merkle_tree[j].hashval, mtree->merkle_tree[j+1].hashval, mtree->merkle_tree[in].hashval);

			//**** Update the indicies ****//
      j=j+2;
      in = in+1;
    }
		//**** Go to next layer ****//
    sub_len = leaf_count+sub_len/2;
    layer_node_count = layer_node_count/2;
  }
	//**** Fill inner nodes ends ****//
  return 0;
}

//**** Merkle tree function - print_tree - ****//
//Prints the hash, node id, previous, next and parent nodes of each node in the given Merkle tree.
int print_tree(TREE mtree)
{
  int i,j;
  for (i = 0; i < mtree->node_count; i++) {
    for (j = 0; j < H_BYTES; j++)
      printf("%02X", mtree->merkle_tree[i].hashval[j]);

    printf(" %d\t%d\t%d\t%d\n****\n",
        mtree->merkle_tree[i].node_id,
        mtree->merkle_tree[i].prev,
        mtree->merkle_tree[i].next,
        mtree->merkle_tree[i].parent);
    }
    printf("************\n");
  return 0;
}

//**** Merkle tree function - find_path - ****//
//Finds the path of given node to the root of given merkle tree.
int find_path(TREE mtree, int node_id, unsigned char** path)
{
  int parent = mtree->merkle_tree[node_id].parent;
  int sibling;
  int i=0;
  int t;
  while(parent!=-1)//Stop if root
  {
    path[i] = (unsigned char*)malloc(H_BYTES*sizeof(unsigned char));

    if (node_id%2 ==1)
      sibling = mtree->merkle_tree[node_id].prev;
    else
      sibling = mtree->merkle_tree[node_id].next;

    node_id =parent;
    parent = mtree->merkle_tree[node_id].parent;
    for(t=0; t<H_BYTES; t++)
      path[i][t] = mtree->merkle_tree[sibling].hashval[t];
    i++;
  }
  return 0;
}

//**** Merkle tree function - find_root_from_path - ****//
//Finds the root of a Merkle tree with given node hash and the path
//Note that this function does not use the information of Merkle tree.
int find_root_from_path(unsigned char** path,unsigned char* node_hash, unsigned char* root, int len)
{
  unsigned char* md_values[len];
  int i;
  for (i = 0; i < len; i++)
    md_values[i] = (unsigned char*)malloc(H_BYTES*sizeof(unsigned char));

	get_concat_hash(path[0],node_hash,md_values[0]);

  for (i = 1; i < len; i++)
		get_concat_hash(path[i],md_values[i-1],md_values[i]);

  for (i = 0; i < H_BYTES; i++)
    root[i] = md_values[len-1][i];

  return 0;
}

//**** Merkle tree function - find_nodeid - ****//
//Returns the node id of given text list element
int find_nodeid(TREE mtree, unsigned char* text_list_element, int length)
{
  int node_id=-1;
	unsigned char hash[H_BYTES];
	shake256(hash, H_BYTES, text_list_element, length);
  int i;
  for(i=0; i<mtree->leaf_count;i++)
  {
    if(compare_hash(hash, mtree->merkle_tree[i].hashval))
    {
      node_id = mtree->merkle_tree[i].node_id;
      break;
    }
  }
  return node_id;
}
