#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#include "diff.h"
#include "diff_tree.h"
#include "debug.h"


char* s = NULL;

static int Conv_Const  = DO;
static int Conv_Neutrl = DO;
static int Count_Conv  = 0;
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
            return Add(Mul(dL, cR), Mul(cL, dR));

        case OP_DIV:
            return Div(Sub(Mul(dL, cR), Mul(cL, dR)), Pow(cR, createNum(2)));

        case OP_POW:
            return Mul(Mul(Pow(cL, Sub(cR, createNum(1))), cR), dL);

        case OP_SIN:
            return Mul(Cos(cR), dR);

        case OP_COS:
            return Mul(Mul(Sin(cR), createNum(-1)), dR);
        
        case OP_EXP:
            return Mul(Exp(cR), dR);

        case OP_LN:
            return Mul(Div(createNum(1), cR), dR);

        case OP_OPENBRT:
            return dL;

        default:
            break;
        }

    default:
        break;
    }
}

//=========================================================================

int convolveExpression(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);

    while(Conv_Const != STOP)
    {
        convolveConst(node);

        if(Count_Conv == 0)
        {
            Conv_Const = STOP;
        }
        Count_Conv = 0;
    }
    while(Conv_Neutrl != STOP)
    {
        convolveNeutral(node);

        if(Count_Conv == 0)
        {
            Conv_Neutrl = STOP;
        }
        Count_Conv = 0;
    }

    return DIFF_SUCCESS;
}

//=========================================================================

void convolveConst(node_t* node)
{
    CHECK(node != NULL, ;);
    
    if((node->type == OP) && ((node->left) != NULL) && isNum(node->left) && ((node->left) != NULL) && isNum(node->right))
    {
        ++Count_Conv;
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

        case OP_POW:
            val = pow(getVal(node->left), getVal(node->right));
            node->data.dblValue = val;
            break;            
        }
        node->data.opValue = 0;

        treeNodeDtor(node->left);
        treeNodeDtor(node->right);
        node->left  = NULL;
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
        ++Count_Conv;
        node->type = NUM;
        node->data.dblValue = 0;

        treeNodeDtor(node->left);
        treeNodeDtor(node->right);
        node->left  = NULL;
        node->right = NULL;

        return;
    }
    if(isOP(OP_POW) && (isZERO(node->left) || isZERO(node->right)))
    {
        ++Count_Conv;
        node->type = NUM;
        node->data.dblValue = 1;

        treeNodeDtor(node->left);
        treeNodeDtor(node->right);
        node->left  = NULL;
        node->right = NULL;

        return;
    }

    if((isOP(OP_MUL) || isOP(OP_DIV) || isOP(OP_POW)) && isONE(node->left))
    {
        ++Count_Conv;
        if(node->data.opValue == node->right->data.opValue)
        {
            node->left = node->right->left;
            node->right = node->right->right;
            return;
        }
        treeNodeDtor(node->left);
        node->left = NULL;
        node->type = node->right->type;
        node->data = node->right->data;

        return;
    }
    if((isOP(OP_MUL) || isOP(OP_DIV) || isOP(OP_POW)) && isONE(node->right))
    {
        ++Count_Conv;
        if(node->data.opValue == node->left->data.opValue)
        {
            node->right = node->left->right;
            node->left  = node->left->left;
            return;
        }
        if(islOP(OP_COS) || islOP(OP_SIN) || islOP(OP_EXP) || islOP(OP_LN))
        {
            node->right = copyNode(node->left);
            treeNodeDtor(node->left);
            node->left = NULL;
            node->type = node->right->type;
            node->data = node->right->data;
            
            return;            
        }
        treeNodeDtor(node->right);
        node->right = NULL;
        node->type = node->left->type;
        node->data = node->left->data;

        return;
    }

    if(isSin(node) && isZERO(node->right))
    {
        ++Count_Conv;
        node->type = NUM;
        node->data.dblValue = 0;
        treeNodeDtor(node->right);
        node->right = NULL;

        return;           
    }
    if(isCos(node) && isZERO(node->right))
    {
        ++Count_Conv;
        node->type = NUM;
        node->data.dblValue = 1;
        treeNodeDtor(node->right);
        node->right = NULL;

        return;           
    }
    if(isExp(node) && isZERO(node->right))
    {
        ++Count_Conv;
        node->type = NUM;
        node->data.dblValue = 1;
        treeNodeDtor(node->right);
        node->right = NULL;

        return;           
    }
    if(isOpenBrt(node) && isOpenBrt(node->left) && (node->right == NULL))
    {
        ++Count_Conv;
        node->right = node->left->right;
        node->left = node->left->left;
    }
    if((isCos(node) && isCos(node->right)) || (isSin(node) && isSin(node->right)))
    {
        ++Count_Conv;
        node->right = node->right->right;
    }
    if((isExp(node) && isExp(node->right)) || (isLn(node) && isLn(node->right)))
    {
        ++Count_Conv;
        node->right = node->right->right;
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

//-------------------------------------------------------------------

node_t* Ln(node_t* node)
{
    CHECK(node != NULL, NULL);
    return createNode(OP_LN, NULL, node);
}

//-------------------------------------------------------------------

node_t* Bracket(node_t* node)
{
    CHECK(node != NULL, NULL);
    return createNode(OP_OPENBRT, node, createOp(OP_CLOSBRT));
}

//=========================================================================

int dump(tree_t* tree)
{
    CHECK(tree != NULL, ERR_TREE_NULL_PTR);

    FILE* outfile = fopen("texdump.tex", "w");
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
        if ((node->left == NULL) && (node->right == NULL) && (node->data.opValue != OP_CLOSBRT))
        {
            fprintf(file, "\\text{error}");
            return;    
        }

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
            fprintf(file, " ^ {");
            dumpLaTeX(file, node->right);
            fprintf(file, "}");
            break;            

        case OP_COS:
            fprintf(file, "cos");
            dumpLaTeX(file, node->right);
            break; 

        case OP_SIN:
            fprintf(file, "sin");
            dumpLaTeX(file, node->right);
            break; 

        case OP_EXP:
            fprintf(file, "e^{");
            dumpLaTeX(file, node->right);
            fprintf(file, "}");
            break; 

        case OP_LN:
            fprintf(file, "ln");
            dumpLaTeX(file, node->right);
            break; 

        case OP_OPENBRT:
            fprintf(file, "(");
            dumpLaTeX(file, node->left);
            dumpLaTeX(file, node->right);
            break;

        case OP_CLOSBRT:
            fprintf(file, ")");
            break;

        default:
            fprintf(file, "\\text{error}");
            break;
        }
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
            break;

        case OP_SUB:
            printf("-");
            break;

        case OP_MUL:
            printf("*");
            break;

        case OP_DIV:
            printf("/");
            break;

        case OP_POW:
            printf("^");
            break;

        case OP_SIN:
            printf("sin");
            break;

        case OP_COS:
            printf("cos");
            break;
        
        case OP_EXP:
            printf("exp");
            break;

        case OP_LN:
            printf("ln");
            break;

        case OP_OPENBRT:
            printf("(");
            break;
        
        case OP_CLOSBRT:
            printf(")");
            break;
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
    int tmp = 1;

    if(*s == '-')
    {
        ++s;
        tmp = -1;
    }
    if(isdigit(*s))
    {
        int val = 0;
        char* sOld = s;

        while(isdigit(*s))
        {
            val = (val * 10) + (*s - '0');
            ++s;
        }
        val *= tmp;
        CHECK(s > sOld, NULL);

        return createNum(val);
    }
    else
    {
        const char* elem = s;
        ++s;

        if(tmp == -1)
        {
            return Mul(createVar(elem), createNum(-1));
        }
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

        return Bracket(val);
    }
    val = getN();

    return val;
}

//-------------------------------------------------------------------

node_t* getL()
{
    if(strncmp(s, "cos", 3) == 0)
    {
        s += 3;
        return Cos(getP());
    }
    if(strncmp(s, "sin", 3) == 0)
    {
        s += 3;
        return Sin(getP());
    }
    if(strncmp(s, "exp", 3) == 0)
    {
        s += 3;
        return Exp(getP());        
    }
    if(strncmp(s, "ln", 2) == 0)
    {
        s += 2;
        return Ln(getP());        
    }

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
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return(node->type == NUM);
}

//-------------------------------------------------------------------

int isVar(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return(node->type == VAR);
}

//-------------------------------------------------------------------

int isOp(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return(node->type == OP);
}

//-------------------------------------------------------------------

double getVal(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return(node->data.dblValue);
}

//-------------------------------------------------------------------

int isZERO(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return((node->type == NUM) && (node->data.dblValue == 0));
}

//-------------------------------------------------------------------

int isONE(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return((node->type == NUM) && (node->data.dblValue == 1));
}

//-------------------------------------------------------------------

int isSin(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return((node->type == OP) && (node->data.opValue == OP_SIN));
}

//-------------------------------------------------------------------

int isCos(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return((node->type == OP) && (node->data.opValue == OP_COS));
}

//-------------------------------------------------------------------

int isExp(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return((node->type == OP) && (node->data.opValue == OP_EXP));
}

//-------------------------------------------------------------------

int isLn(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return((node->type == OP) && (node->data.opValue == OP_LN));
}

//-------------------------------------------------------------------

int isOpenBrt(node_t* node)
{
    CHECK(node != NULL, ERR_DIFF_NULL_PTR);
    return((node->type == OP) && (node->data.opValue == OP_OPENBRT));
}
