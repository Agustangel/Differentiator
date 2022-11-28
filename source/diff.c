#include <stdio.h>
#include "diff.h"
#include "diff_tree.h"
#include "debug.h"



//=========================================================================
node_t* differentiate(node_t* node)
{
    CHECK(node != NULL, NULL);

    switch (node->type)
    {
    case NUM:
        return createNum(0);

    case VAR:
        return createNum(1);

    case OP:
        switch (node->data.opValue)
        {
        case OP_ADD:
            return Add(dL, dR);

        case OP_SUB:
            return Sub(dL, dR);

        case OP_MUL:
            return Add(Mul(dL, cR), Mul(cL, dR));

        case OP_DIV:
            return Div(Sub(Mul(dL, cR), Mul(cL, dR)), Pow(cR, createNum(2)));

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
    CHECK(left  != NULL, NULL);
    CHECK(right != NULL, NULL);

    return createNode(OP_ADD, left, right);
}

//-------------------------------------------------------------------

node_t* Sub(node_t* left, node_t* right)
{
    CHECK(left  != NULL, NULL);
    CHECK(right != NULL, NULL);

    return createNode(OP_SUB, left, right);
}

//-------------------------------------------------------------------

node_t* Mul(node_t* left, node_t* right)
{
    CHECK(left  != NULL, NULL);
    CHECK(right != NULL, NULL);

    return createNode(OP_MUL, left, right);
}

//-------------------------------------------------------------------

node_t* Div(node_t* left, node_t* right)
{
    CHECK(left  != NULL, NULL);
    CHECK(right != NULL, NULL);

    return createNode(OP_DIV, left, right);
}
//-------------------------------------------------------------------

node_t* Pow(node_t* node, node_t* power)
{
    CHECK(node != NULL, NULL);

    return createNode(OP_POW, node, power);
}

//-------------------------------------------------------------------

node_t* Sin(node_t* node)
{
    CHECK(node != NULL, NULL);

    return createNode(OP_SIN, NULL, node);
}

//-------------------------------------------------------------------

node_t* Cos(node_t* node)
{
    CHECK(node != NULL, NULL);

    return createNode(OP_COS, NULL, node);
}

//-------------------------------------------------------------------

node_t* Exp(node_t* node)
{
    CHECK(node != NULL, NULL);

    return createNode(OP_EXP, NULL, node);
}

//=========================================================================

int dump(tree_t* tree)
{
    FILE* outfile = fopen("texdump.tex", "a");
    CHECK(outfile != NULL, ERR_DIFF_NULL_PTR);

    fprintf(outfile, "$$");
    dumpLaTeX(outfile, tree->root);
    fprintf(outfile, "$$\n");

    return DIFF_SUCCESS;
}

//=========================================================================

void dumpLaTeX(FILE* file, const node_t* node)
{
    CHECK(file != NULL, ;);
    CHECK(node != NULL, ;);

    switch (node->type)
    {
        case NUM:
            fprintf(file, "%lg", node->data.dblValue);
            return;

        case VAR:
            fprintf(file, "%c", *node->data.varValue);
            return;

        case OP:
            if ((node->left == NULL) || (node->right == NULL))
            {
                fprintf(file, "\\text{error}");
                return;    
            }

            fprintf(file, "(");
            switch(node->data.opValue)
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

//=========================================================================

int printExpression(node_t* node, int type)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    if(type == ALL)
    {
        printf("(");
    }
    if(node->left != NULL)
    {
        printAllExpression(node->left);
    }

    printf("%s", node->data);
    if(type == ALL)
    {
        printf(")");
    }

    if(node->right != NULL)
    {
        printAllExpression(node->right);
    }      

    return DIFF_SUCCESS;
}

//=========================================================================

node_t* makeAST(char* str)
{
    CHECK(str != NULL, NULL);

    char* s = str;
    node_t* root = getG(s);

    return root;
}

//-------------------------------------------------------------------

node_t* getG(char* s)
{
    node_t* val = getE(s);
    CHECK(*s == '\0', NULL);

    node_t* val;
}

//-------------------------------------------------------------------

node_t* getN(char* s)
{
    int val = 0;
    char* sOld = s;

    while((*s >= '0') && (*s <= '9'))
    {
        val = (val * 10) + (*s - '0');
        ++s;
    }
    CHECK(s > sOld, NULL);

    return createNum(val);
}

//-------------------------------------------------------------------

node_t* getE(char* s)
{
    node_t* val = getT(s);

    while((*s == '+') || (*s == '-'))
    {
        char op = *s;
        ++s;

        node_t* val_2 = getT(s);
        if(op == '+')
        {
            val = Add(val, val_2);
        }
        else
        {
            val = Sub(val, val_2);
        }
    }

    return val;
}

//-------------------------------------------------------------------

node_t* getT(char* s)
{
    node_t* val = getP(s);

    while((*s == '*') || (*s == '/'))
    {
        char op = *s;
        ++s;

        node_t* val_2 = getP(s);
        if(op == '*')
        {
            val = Mul(val, val_2);
        }
        else
        {
            val = Div(val, val_2);
        }
    }

    return val;    
}

//-------------------------------------------------------------------

node_t* getP(char* s)
{
    node_t* val = 0;
    if(*s == '(')
    {
        ++s;
        val = getE(s);
        CHECK(*s == ')', NULL);
        ++s;
    }
    else
    {
        val = getN(s);
    }

    return val;
}

//-------------------------------------------------------------------
