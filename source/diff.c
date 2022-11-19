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
            return Mul(Pow(cL, Sub(cR, createNum(1))), cR);

        case OP_SIN:
            return Mul(Cos(cR), dR);

        case OP_COS:
            return Mul(Mul(Sin(cR), createNum(-1)), dR);
        
        case OP_EXP:
            return Mul(Exp(cR), dR);

        case OP_LN:
            return Mul(Div(createNum(1), cR), dR);

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

//-------------------------------------------------------------------

node_t* Exp(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    return createNode(OP_EXP, NULL, node);
}

//=========================================================================

void dumpLaTeX(FILE* file, const node_t* node)
{
    CHECK(file != NULL, ;);
    CHECK(node != NULL, ;);

    switch (node->data.type)
    {
        case NUM:
            fprintf(file, "%lg", node->data.dblValue);
            return;

        case VAR:
            fprintf(file, "%c", node->data.varValue);
            return;

        case OP:
            if ((node->left == NULL) || (node->right == NULL))
            {
                fprintf(file, "\\text{error}");
                return;    
            }

            fprintf(file, "(");
            switch (node->data.opcode)
            {
                case OP_ERROR:
                    fprintf(file, " \\text{error} ");
                    break;

                case OP_ADD:
                    dumpLaTeX(file, node->left);
                    fprintf(file, " + ");
                    dumpLaTeX(file, node->right);
                    break;

                case OP_SUB:
                    dumpLaTeX(file, node->left);
                    fprintf(file, " - ");
                    dumpLaTeX(file, node->right);
                    break;

                case OP_MUL:
                    dumpLaTeX(file, node->left);
                    fprintf(file, " \\cdot ");
                    dumpLaTeX(file, node->right);
                    break;

                case OP_DIV:
                    fprintf(file, " \\frac{ ");
                    dumpLaTeX(file, node->left);
                    fprintf(file, "}{");
                    dumpLaTeX(file, node->right);
                    fprintf(file, "} ");
                    break;

                default:
                    fprintf(file, "\\text{error}");
            }
            fprintf(file, ")");
            return;
        
        default:
            fprintf(file, "\\text{error}");
            return;
    }
}
