/*
	avl_tree.h
	Copyright (c) 2019, Valentin Debon

	This file is part of the avl_tree repository
	subject the BSD 3-Clause License, see LICENSE
*/
#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdint.h>

/*
 * avl_tree.h is a generic AVL Tree
 * which contains an anonymous pointer,
 * for logn access according to
 * a field determined by a function
 */

/** Alias to what it holds, just to avoid void * for code clarity */
typedef void avl_tree_element_t;
typedef uint64_t avl_tree_hash_t;
typedef long avl_tree_height_t;

/** The avl_tree, with hash field accessor */
struct avl_tree {
	struct avl_tree_node *root;
	avl_tree_hash_t (*hash_field)(const avl_tree_element_t *);
};

/**
 * Creates a new node to hold element
 * @param element Element hold by the node
 * @return Newly allocated node, must be avl_tree_node_destroy()'d
 */
struct avl_tree_node *
avl_tree_node_create(avl_tree_element_t *element);

/**
 * Destroys a previously avl_tree_node_create()'d node
 * @param node Node to destroy
 */
void
avl_tree_node_destroy(struct avl_tree_node *node);

/**
 * Fetch element of node
 * @param node A valid node
 * @return Node's element
 */
avl_tree_element_t *
avl_tree_node_element(struct avl_tree_node *node);

/**
 * Access left child of node
 * @param node A valid node
 * @return Node's left child or NULL
 */
struct avl_tree_node *
avl_tree_node_left(struct avl_tree_node *node);

/**
 * Access right child of node
 * @param node A valid node
 * @return Node's right child or NULL
 */
struct avl_tree_node *
avl_tree_node_right(struct avl_tree_node *node);

/**
 * Access height of tree at node
 * @param node A valid node, or NULL
 * @return Height's of tree at node, or 0 if NULL
 */
avl_tree_height_t
avl_tree_node_height(struct avl_tree_node *node);

/**
 * Initialize avl_tree structure
 * @param avl_tree Pointer to the structure to initialize
 * @param hash_field "Getter" for the hash used to index each element
 */
void
avl_tree_init(struct avl_tree *tree,
	avl_tree_hash_t (*hash_field)(const avl_tree_element_t *));

/**
 * Frees informations of a avl_tree
 * @param avl_tree Tree to deinitialize
 */
void
avl_tree_deinit(struct avl_tree *tree);

/**
 * Insert a node in the avl_tree
 * @param avl_tree Tree in which to insert
 * @param node Node to insert
 */
void
avl_tree_insert(struct avl_tree *tree,
	struct avl_tree_node *node);

/**
 * Remove a node from the avl_tree
 * @param avl_tree Tree to remove from
 * @param hash Hash of the (maybe) designated node
 * @return NULL if not found, or the node else, removed from avl_tree
 */
struct avl_tree_node *
avl_tree_remove(struct avl_tree *tree,
	avl_tree_hash_t hash);

/**
 * Find a node by its hash
 * @param avl_tree Tree to find in
 * @param hash Hash of the (maybe) designated node
 * @return NULL if not found, or the node else
 */
struct avl_tree_node *
avl_tree_find(struct avl_tree *tree,
	avl_tree_hash_t hash);

/* AVL_TREE_H */
#endif
