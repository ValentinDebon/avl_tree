/*
	avl_tree.c
	Copyright (c) 2019, Valentin Debon

	This file is part of the avl_tree repository
	subject the BSD 3-Clause License, see LICENSE
*/
#include "avl_tree.h"

#include <stdlib.h>

#define avl_tree_node_balance(node) ((node) == NULL ? 0 : \
	(avl_tree_node_height((node)->right) - avl_tree_node_height((node)->left)))
#define max(a, b) ((a) > (b) ? (a) : (b))

/** Node in the avl_tree */
struct avl_tree_node {
	avl_tree_element_t *element;

	struct avl_tree_node *left, *right;
	avl_tree_height_t height;
};

static struct avl_tree_node *
avl_tree_node_rotate_right(struct avl_tree_node *node) {
	struct avl_tree_node *node2 = node->left;

	node->left = node2->right;
	node2->right = node;

	node->height = max(avl_tree_node_height(node->left),
		avl_tree_node_height(node->right)) + 1;
	node2->height = max(avl_tree_node_height(node2->left),
		node->height) + 1;

	return node2;
}

static struct avl_tree_node *
avl_tree_node_rotate_left(struct avl_tree_node *node) {
	struct avl_tree_node *node2 = node->right;

	node->right = node2->left;
	node2->left = node;

	node->height = max(avl_tree_node_height(node->left),
		avl_tree_node_height(node->right)) + 1;
	node2->height = max(avl_tree_node_height(node2->right),
		node->height) + 1;

	return node2;
}

static struct avl_tree_node *
avl_tree_node_rotate_left_right(struct avl_tree_node *node) {

	node->left = avl_tree_node_rotate_left(node->left);

	return avl_tree_node_rotate_right(node);
}

static struct avl_tree_node *
avl_tree_node_rotate_right_left(struct avl_tree_node *node) {

	node->right = avl_tree_node_rotate_right(node->right);

	return avl_tree_node_rotate_left(node);
}

static struct avl_tree_node *
avl_tree_node_insert(struct avl_tree_node *root,
	struct avl_tree_node *node,
	avl_tree_hash_t (*hash_field)(const avl_tree_element_t *)) {

	if(root == NULL) {

		return node;
	} else {

		if(hash_field(node->element) < hash_field(root->element)) {

			root->left = avl_tree_node_insert(root->left, node, hash_field);

			if(avl_tree_node_balance(root) == -2) {

				if(hash_field(node->element) < hash_field(root->left->element)) {
					root = avl_tree_node_rotate_right(root);
				} else {
					root = avl_tree_node_rotate_left_right(root);
				}
			}

		} else if(hash_field(node->element) > hash_field(root->element)) {

			root->right = avl_tree_node_insert(root->right, node, hash_field);

			if(avl_tree_node_balance(root) == 2) {

				if(hash_field(node->element) > hash_field(root->right->element)) {
					root = avl_tree_node_rotate_left(root);
				} else {
					root = avl_tree_node_rotate_right_left(root);
				}
			}
		} else {
			/* Tried to reinsert an element, we do nothing */
		}

		root->height = max(avl_tree_node_height(root->left), avl_tree_node_height(root->right)) + 1;

		return root;
	}
}

/*
 * Returns the root of the avl_tree
 * and the removed node in removed,
 * left unchanged if the node dosn't exist
 */
static struct avl_tree_node *
avl_tree_node_remove(struct avl_tree_node *root,
	avl_tree_hash_t hash,
	struct avl_tree_node **removed,
	avl_tree_hash_t (*hash_field)(const avl_tree_element_t *)) {

	if(root != NULL) {
		if(hash < hash_field(root->element)) {
			root->left = avl_tree_node_remove(root->left, hash, removed, hash_field);
		} else if(hash > hash_field(root->element)) {
			root->right = avl_tree_node_remove(root->right, hash, removed, hash_field);
		} else {
			*removed = root;

			if(root->left != NULL
				&& root->right != NULL) {
				struct avl_tree_node *successor = root->right;

				while(successor->left != NULL) {
					successor = successor->left;
				}

				root->right = avl_tree_node_remove(root->right,
					hash_field(successor->element), &successor, hash_field);

				successor->left = root->left;
				successor->right = root->right;
				root = successor;
			} else if(root->left == NULL) {
				root = root->right;
			} else {
				root = root->left;
			}

			(*removed)->left = NULL;
			(*removed)->right = NULL;
			(*removed)->height = 1;
		}
	}

	if(root != NULL) {
		root->height = max(avl_tree_node_height(root->left),
			avl_tree_node_height(root->right)) + 1;

		if(avl_tree_node_balance(root) > 1) {
			if(avl_tree_node_balance(root->right) >= 0) {
				root = avl_tree_node_rotate_left(root);
			} else {
				root = avl_tree_node_rotate_right_left(root);
			}
		} else if(avl_tree_node_balance(root) < -1) {
			if(avl_tree_node_balance(root->left) <= 0) {
				root = avl_tree_node_rotate_right(root);
			} else {
				root = avl_tree_node_rotate_left_right(root);
			}
		}
	}

	return root;
}

struct avl_tree_node *
avl_tree_node_create(avl_tree_element_t *element) {
	struct avl_tree_node *node = malloc(sizeof(*node));

	node->element = element;
	node->left = NULL;
	node->right = NULL;
	node->height = 1;

	return node;
}

void
avl_tree_node_destroy(struct avl_tree_node *node) {

	if(node != NULL) {

		avl_tree_node_destroy(node->left);
		avl_tree_node_destroy(node->right);

		free(node);
	}
}

avl_tree_element_t *
avl_tree_node_element(struct avl_tree_node *node) {

	return node->element;
}

struct avl_tree_node *
avl_tree_node_left(struct avl_tree_node *node) {

	return node->left;
}

struct avl_tree_node *
avl_tree_node_right(struct avl_tree_node *node) {

	return node->right;
}

avl_tree_height_t
avl_tree_node_height(struct avl_tree_node *node) {

	return node == NULL ? 0 : node->height;
}

void
avl_tree_init(struct avl_tree *tree,
	avl_tree_hash_t (*hash_field)(const avl_tree_element_t *)) {

	tree->root = NULL;
	tree->hash_field = hash_field;
}

void
avl_tree_deinit(struct avl_tree *tree) {

	avl_tree_node_destroy(tree->root);
}

void
avl_tree_insert(struct avl_tree *tree,
	struct avl_tree_node *node) {

	tree->root = avl_tree_node_insert(tree->root, node,
		tree->hash_field);
}

struct avl_tree_node *
avl_tree_remove(struct avl_tree *tree,
	avl_tree_hash_t hash) {
	struct avl_tree_node *removed = NULL;

	tree->root = avl_tree_node_remove(tree->root, hash,
		&removed, tree->hash_field);

	return removed;
}

struct avl_tree_node *
avl_tree_find(struct avl_tree *tree,
	avl_tree_hash_t hash) {
	struct avl_tree_node *current = tree->root;

	while(current != NULL
		&& tree->hash_field(current->element) != hash) {

		if(hash < tree->hash_field(current->element)) {
			current = current->left;
		} else { /* tree->hash_field(current->element) < hash */
			current = current->right;
		}
	}

	return current;
}

