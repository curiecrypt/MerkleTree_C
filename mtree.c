/**
 * Merkle tree implementation for cryptographic applications.
 *
 * The function fillTextList() generates random texts to test the implementation.
 * One can fill the leaves with user input, by reading from a file or by statically assigning.
 * The size of a text is set to 256B in constant definition. It can be updated.
 *
 * The function printHEX() is a helper function to print the elements in hexadecimal.
 *
 * Gamze Orhon Kilic.
 * 2021.
 **/

#include <time.h>
#include <stdlib.h>
#include "mtree.h"

//**** Constant definition
#define TEXT_BYTES	256

//**** Helper function - fillTextList - ****//
//Fills randomly the text list.
int fillTextList(unsigned char** text_list)
{
	int i,j;
	for(i=0;i<LEAF_COUNT;i++)
		for(j=0;j<TEXT_BYTES;j++)
			text_list[i][j]=(unsigned char) rand();

	return 0;
}

//**** Helper function - printHEX - ****//
//Prints the given unsigned char array in hexadecimal format.
int printHEX(unsigned char* hex, int length)
{
	int i;
	for(i=0;i<length;i++)
		printf("%02X", hex[i]);
	printf("\n");
	return 0;
}

int main() {

	srand(time(NULL));

	int i,node_index, node_id;
	unsigned char* text_list[LEAF_COUNT], *path[PATH_SIZE], *node_hash, *root_hash;
	TREE mtree;

	//**** Memory allocation starts. ****//
	node_hash = (unsigned char*)malloc(sizeof(unsigned char)*H_BYTES);
	root_hash = (unsigned char*)malloc(sizeof(unsigned char)*H_BYTES);

	for(i = 0; i<PATH_SIZE; i++)
		path[i] = (unsigned char*)malloc(sizeof(unsigned char)*H_BYTES);

	for(i=0;i<LEAF_COUNT;i++)
		text_list[i] = (unsigned char*)malloc(sizeof(unsigned char)*TEXT_BYTES);

	mtree = (TREE)malloc(sizeof(TREE));
	mtree->leaf_count = LEAF_COUNT;
	mtree->node_count = NODE_COUNT;
	mtree->merkle_tree = (struct NODE*)malloc(sizeof(struct NODE)*NODE_COUNT);
	//**** Memory allocation ends. ****//

	//**** Random text generation starts. ****//
	fillTextList(text_list);
	/* 	//Print the text list.
	for(i=0;i<LEAF_COUNT;i++)
		printHEX(text_list[i],TEXT_BYTES);
	*/
	//**** Random text generation ends. ****//


	//**** Build tree starts. ****//
	build_tree(mtree, &*text_list, TEXT_BYTES);
	//**** Build tree ends. ****//


	//**** Find path starts. ****//
	node_index = rand()%LEAF_COUNT;
	node_id = find_nodeid(mtree, text_list[node_index], TEXT_BYTES);
	find_path(mtree, node_id, &*path);
	//**** Find path ends. ****//

	//**** Find root from path starts. ****//
	node_hash = mtree->merkle_tree[node_id].hashval;
	root_hash = mtree->merkle_tree[NODE_COUNT-1].hashval;
	find_root_from_path(path,node_hash,root_hash,PATH_SIZE);
	//**** Find root from path ends. ****//



	//**** Print results starts. ****//
	printf("\n******************************************************************************\n");
	printf("****A merkle tree with %d leaves and %d nodes**********************************\n", LEAF_COUNT, NODE_COUNT);
	printf("******************************************************************************\n\n");
	print_tree(mtree);
	printf("******************************************************************************\n\n");

	printf("****The root hash of the tree*************************************************\n");
	printHEX(mtree->merkle_tree[NODE_COUNT-1].hashval,H_BYTES);
	printf("******************************************************************************\n\n");

	printf("****The path of the node with node id %d***************************************\n",node_id);
	for(i = 0; i<PATH_SIZE; i++){
		printHEX(path[i],H_BYTES);
		printf("****\n");
	}
	printf("******************************************************************************\n\n");

	printf("****The root hash found with path of the node with node id %d******************\n", node_id);
	printHEX(root_hash,H_BYTES);
	printf("******************************************************************************\n\n");
	//**** Print results ends. ****//

	return 0;
}
