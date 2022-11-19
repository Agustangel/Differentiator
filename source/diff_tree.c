#include <stdint.h>
#include "diff_tree.h"


int tree_ctor(tree_t* tree)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);

    tree->root = NULL;
    tree->size = 0;

    return TREE_SUCCESS;
}
