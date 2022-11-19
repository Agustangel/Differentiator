#include <stdio.h>
#include <bintree.h>
#include "diff.h"
#include "diff_tree.h"


//=========================================================================
node_t* differentiate(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    switch (node->type)
    {
    case NUM:
        return createNode(NUM, 0, NULL, NULL);

    case VAR:
        return createNum(1);

    case OP:
        switch (node->opValue)
        {
        case OP_ADD:
            return Add(dL, dR);

        case OP_SUB:
            return Sub(dL, dR);

        case OP_MUL:
            return Add(Mul(dL, cR), Mul(cL, dR));

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

//=========================================================================

node_t* Add(node_t* left, node_t* right)
{
    return createNode(ADD, left, right);
}

//-------------------------------------------------------------------

node_t* Sub(node_t* left, node_t* right)
{
    return createNode(SUB, left, right);
}

//-------------------------------------------------------------------

node_t* Mul(node_t* left, node_t* right)
{
    return createNode(MUL, left, right);
}

//-------------------------------------------------------------------

node_t* Div(node_t* left, node_t* right)
{
    return createNode(DIV, left, right);
}
