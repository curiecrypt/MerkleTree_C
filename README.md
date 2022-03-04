# MERKLE TREE

A Merkle tree is a data structure used in cryptography and computer science that provides efficient and secure verification of large-scale data.

Assume that there is a list of data with n elements, a Merkle tree constructed on this list consists of log n layers. The leaf nodes consist of the hashes of data and upper nodes (inner nodes) consist the hash of the concatenation of their children's hashes.

Root node of a Merkle tree allows verification of the data.

The path of a leaf node includes the hash values of the inner nodes that generate the hash of the root of the tree with the hash of that leaf node.

# Content

This is an implementation of a Merkle tree written in C language.
* **mtree.h** is a header file consisting Merkle tree functions.
* **mtree.c** is the C file that tests the functionality of Merkle tree implementation.

# Implementation

This project can be simply compiled using *gcc*.

# License

MerkleTree_C is licensed under the MIT License; see [`License`](LICENSE) for details.

* This project includes a third party module `fips202.c` which is licensed under public domain.
