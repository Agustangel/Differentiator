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

        case OP_POW:
            return Mul(cR, Pow(cL, Sub(cR, createNum(1))));

        case OP_SIN:
            return Mul(Cos(cR), dR);

        case OP_COS:
            return Mul(Mul(Sin(cR), createNum(-1)), dR);
        
        default:
            break;
        }

    default:
        break;
    }
}

//=========================================================================

node_t* Add(node_t* left, node_t* right)
{
    CHECK(left  != NULL, ERR_DIFF_NULL_PTR);
    CHECK(right != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_ADD, left, right);
}

//-------------------------------------------------------------------

node_t* Sub(node_t* left, node_t* right)
{
    CHECK(left  != NULL, ERR_DIFF_NULL_PTR);
    CHECK(right != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_SUB, left, right);
}

//-------------------------------------------------------------------

node_t* Mul(node_t* left, node_t* right)
{
    CHECK(left  != NULL, ERR_DIFF_NULL_PTR);
    CHECK(right != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_MUL, left, right);
}

//-------------------------------------------------------------------

node_t* Div(node_t* left, node_t* right)
{
    CHECK(left  != NULL, ERR_DIFF_NULL_PTR);
    CHECK(right != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_DIV, left, right);
}
//-------------------------------------------------------------------

node_t* Pow(node_t* node, int power)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_POW, node, createNum(power));
}

//-------------------------------------------------------------------

node_t* Sin(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_SIN, NULL, node);
}

//-------------------------------------------------------------------

node_t* Cos(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_COS, NULL, node);
}
