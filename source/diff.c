#include <stdio.h>
#include <bintree.h>
#include "diff.h"


//=========================================================================
node_t* differentiate(const tree_t* tree, node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    switch (node->type)
    {
    case NUM:
        insert_node(tree, node, LEFT, 0);
        insert_node(tree, node, RIGHT, 0);
        break;
    case VAR:
        /* code */
        break;

    case OP:
        switch (node->opValue)
        {
        case OP_ADD:
            break;

        case OP_SUB:
            break;

        case OP_MUL:
            break;

        case OP_DIV:
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}
