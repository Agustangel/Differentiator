#include <stdio.h>
#include <bintree.h>
#include "diff.h"
#include "diff_tree.h"

//TODO changed func Pow
//=========================================================================
node_t* differentiate(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    switch (node->type)
    {
    case NUM:
        return createNum(0);

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
            return Div(Sub(Mul(dL, cR), Mul(cL, dR)), Pow(cR, 2));

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
    return createNode(OP_ADD, left, right);
}

//-------------------------------------------------------------------

node_t* Sub(node_t* left, node_t* right)
{
    return createNode(OP_SUB, left, right);
}

//-------------------------------------------------------------------

node_t* Mul(node_t* left, node_t* right)
{
    return createNode(OP_MUL, left, right);
}

//-------------------------------------------------------------------

node_t* Div(node_t* left, node_t* right)
{
    return createNode(OP_DIV, left, right);
}
//-------------------------------------------------------------------

node_t* Pow(node_t* left, node_t* right)
{
    return createNode(OP_POW, left, right);
}
