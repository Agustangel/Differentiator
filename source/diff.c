#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "diff.h"
#include "diff_tree.h"
#include "debug.h"


char* s = NULL;
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
        switch(node->data.opValue)
        {
        case OP_ADD:
            return Add(dL, dR);

        case OP_SUB:
            return Sub(dL, dR);

        case OP_MUL:
            return Add(Mul(dL, copyNode(cR)), Mul(copyNode(cL), dR));

        case OP_DIV:
            return Div(Sub(Mul(dL, copyNode(cR)), Mul(copyNode(cL), dR)), Pow(copyNode(cR), createNum(2)));

        case OP_POW:
            return Mul(Mul(Pow(cL, Sub(copyNode(cR), createNum(1))), copyNode(cR)), dL);

        case OP_SIN:
            return Mul(Cos(copyNode(cR)), dR);

        case OP_COS:
            return Mul(Mul(Sin(copyNode(cR)), createNum(-1)), dR);
        
        case OP_EXP:
            return Mul(Exp(copyNode(cR)), dR);

        case OP_LN:
            return Mul(Div(createNum(1), copyNode(cR)), dR);

        default:
            break;
        }

    default:
        break;
    }
}

//=========================================================================

void convolveConst(node_t* node)
{
    CHECK(node != NULL, ;);
    
    if((node->type == OP) && (isNum(node->left)) && (isNum(node->right)))
    {
        node->type = NUM;

        double val = 0;
        switch(node->data.opValue)
        {
        case OP_ADD:
            val = getVal(node->left) + getVal(node->right);
            node->data.dblValue = val;
            break;

        case OP_SUB:
            val = getVal(node->left) - getVal(node->right);
            node->data.dblValue = val;
            break;

        case OP_MUL:
            val = getVal(node->left) * getVal(node->right);
            node->data.dblValue = val;
            break;

        case OP_DIV:
            val = getVal(node->left) / getVal(node->right);
            node->data.dblValue = val;
            break;
        }
        node->data.opValue = 0;

        treeNodeDtor(node->left);
        treeNodeDtor(node->right);
        node->left = NULL;
        node->right = NULL;

        return;
    }

    if(node->left != NULL)
    {
        convolveConst(node->left);
    }
    if(node->right != NULL)
    {
        convolveConst(node->right);
    }
}

//=========================================================================

void convolveNeutral(node_t* node)
{
    CHECK(node != NULL, ;);

    if(node->left != NULL)
    {
        convolveNeutral(node->left);
    }
    if(node->right != NULL)
    {
        convolveNeutral(node->right);
    }

    if(isOP(OP_MUL) && (isZERO(node->left) || isZERO(node->right)))
    {
        node->type = NUM;
        node->data.dblValue = 0;

        treeNodeDtor(node->left);
        treeNodeDtor(node->right);
        node->left = NULL;
        node->right = NULL;
    }
    if(isOP(OP_POW) && (isZERO(node->left) || isZERO(node->right)))
    {
        node->type = NUM;
        node->data.dblValue = 1;

        treeNodeDtor(node->left);
        treeNodeDtor(node->right);
        node->left = NULL;
        node->right = NULL;
    }

    if((isOP(OP_MUL) || isOP(OP_DIV) || isOP(OP_POW)) && isONE(node->left))
    {
        treeNodeDtor(node->left);
        node->left = NULL;
        node = node->right;
    }
    if((isOP(OP_MUL) || isOP(OP_DIV) || isOP(OP_POW)) && isONE(node->right))
    {
        treeNodeDtor(node->right);
        node->right = NULL;
        node = node->left;
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
    CHECK(tree != NULL, ERR_TREE_NULL_PTR);

    FILE* outfile = fopen("texdump.tex", "a");
    CHECK(outfile != NULL, ERR_DIFF_NULL_PTR);

    fprintf(outfile, "\\documentclass{article}\n"
                 "\n"
                 "\\title{formula}\n"
                 "\\author{Julia}\n"
                 "\n"
                 "\\begin{document}\n"
                 "\\maketitle\n"
                 "\n");
    fprintf(outfile, "$$");
    dumpLaTeX(outfile, tree->root);
    fprintf(outfile, "$$\n");
    fprintf(outfile, "\\end{document}\n");

    return DIFF_SUCCESS;
}

//=========================================================================

void dumpLaTeX(FILE* file, const node_t* node)
{
    CHECK(file != NULL, ;);
    CHECK(node != NULL, ;);

    switch(node->type)
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

        case OP_POW:
            dumpLaTeX(file, node->left);
            fprintf(file, " ^ ");
            dumpLaTeX(file, node->right);
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
        printExpression(node->left, type);
    }

    if(node->type == VAR)
    {
        printf("%s", node->data.varValue);
    }
    if(node->type == NUM)
    {
        printf("%lf", node->data.dblValue);
    }
    if(node->type == OP)
    {
        switch (node->data.opValue)
        {
        case OP_ADD:
            printf("+");

        case OP_SUB:
            printf("-");

        case OP_MUL:
            printf("*");

        case OP_DIV:
            printf("/");

        case OP_POW:
            printf("^");

        case OP_SIN:
            printf("sin");

        case OP_COS:
            printf("cos");
        
        case OP_EXP:
            printf("exp");

        case OP_LN:
            printf("ln");
        }        
    }
    if(type == ALL)
    {
        printf(")");
    }

    if(node->right != NULL)
    {
        printExpression(node->right, type);
    }      

    return DIFF_SUCCESS;
}

//=========================================================================

node_t* makeAST(char* str)
{
    CHECK(str != NULL, NULL);
    s = str;

    node_t* root = getG();
    CHECK(root != NULL, NULL);

    return root;
}

//-------------------------------------------------------------------

node_t* getG()
{
    node_t* val = getE();
    CHECK(*s == '\0', NULL);

    return val;
}

//-------------------------------------------------------------------

node_t* getN()
{
    if(isdigit(*s))
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
    else
    {
        const char* elem = s;
        ++s;

        return createVar(elem);
    }
}

//-------------------------------------------------------------------

node_t* getE()
{
    node_t* val = getT();

    while((*s == '+') || (*s == '-'))
    {
        char op = *s;
        ++s;

        node_t* val_2 = getT();
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

node_t* getT()
{
    node_t* val = getL();

    while((*s == '*') || (*s == '/'))
    {
        char op = *s;
        ++s;

        node_t* val_2 = getL();
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

node_t* getP()
{
    node_t* val = 0;
    if(*s == '(')
    {
        ++s;
        val = getE();
        CHECK(*s == ')', NULL);
        ++s;
    }
    else
    {
        val = getN();
    }

    return val;
}

//-------------------------------------------------------------------

node_t* getL()
{
    node_t* val = getP();

    if(*s == '^')
    {
        ++s;
        node_t* power = getP();
        val = Pow(val, power);
    }
    CHECK(*s != '^', NULL);

    return val;
}

//=========================================================================

int isNum(node_t* node)
{
    return(node->type == NUM);
}

//-------------------------------------------------------------------

int isVar(node_t* node)
{
    return(node->type == VAR);
}

//-------------------------------------------------------------------

int isOp(node_t* node)
{
    return(node->type == OP);
}

//-------------------------------------------------------------------

double getVal(node_t* node)
{
    return(node->data.dblValue);
}

//-------------------------------------------------------------------

int isZERO(node_t* node)
{
    if((node->type == NUM) && (node->data.dblValue == 0))
    {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------

int isONE(node_t* node)
{
    if((node->type == NUM) && (node->data.dblValue == 1))
    {
        return true;
    }
    return false;
}
