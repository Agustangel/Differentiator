#include <stdio.h>
#include <stdlib.h>

#include "diff_tree.h"
#include "debug.h"


//=========================================================================
int treeCtor(tree_t* tree)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);

    tree->root = NULL;
    tree->size = 0;

    return TREE_SUCCESS;
}

//=========================================================================

node_t* createNode(int val, node_t* left, node_t* right)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = OP;
    node->data.opValue = val;

    node->left  = left;
    node->right = right;

    return node; 
}

//=========================================================================

int treeDtor(tree_t* tree)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);
    //int verify = tree_verify(tree);
    //CHECK(verify == TREE_SUCCESS, verify);

    treeNodeDtor(tree->root);
    tree->root = NULL;
    tree->size = SIZE_MAX;

    return TREE_SUCCESS;
}

//=========================================================================

void treeNodeDtor(node_t* node)
{
    if (node == NULL)
    {
        return;
    }

    treeNodeDtor(node->right);
    treeNodeDtor(node->left);
    free(node);
}
