#include <stdio.h>
#include <stdlib.h>

#include "diff.h"
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

node_t* createNum(int val)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = NUM;
    node->data.dblValue = val;

    node->left  = NULL;
    node->right = NULL;
 
    return node;
}

//=========================================================================

node_t* createVar(const char* val)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = VAR;
    node->data.varValue = val;

    node->left  = NULL;
    node->right = NULL;
 
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

//=========================================================================

int dumpGraphTree(tree_t* tree)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);

    FILE* file_dot = fopen("graphdump.dot", "wb");
    CHECK(file_dot !=  NULL, ERR_TREE_BAD_FILE);

    fprintf(file_dot, "digraph {\n");
    fprintf(file_dot, "\tnode[shape = \"cube\", color = \"#800000\", fontsize = 15, style = \"filled\", fillcolor = \"#88CEFA\"];\n"
                      "\tedge[color = \"#190970\", fontsize = 11];\n");

    node_dump_graph(tree->root, file_dot);
    fprintf(file_dot, "}");

    fclose(file_dot);

    system("dot -Tjpeg -ograph_phys.jpeg graph_phys.dot");
    system("convert graph_log.jpeg graph_phys.jpeg -append graph_array.jpeg");
    system("gwenview graph_array.jpeg");

    return TREE_SUCCESS;
}

//=========================================================================

int dumpGraphNode(node_t* node, FILE* dot_out)
{
    CHECK(node    !=  NULL, ERR_TREE_NULL_PTR);
    CHECK(dot_out !=  NULL, ERR_TREE_BAD_FILE);

    switch (node->type)
    {
        case NUM:
            fprintf(dot_out, "\n\t\t\"%lg\"[shape = \"ellipse\", color=\"#900000\", style=\"filled\", \
                               fillcolor = \"#D0FDFF\"];\n", node->data.dblValue);
            if(node->left != NULL)
            {
                fprintf(dot_out, "\t\t\"%s\"->", node->data.dblValue);
                fprintfConnection(node->left, LEFT, dot_out);
                dumpGraphNode(node->left, dot_out);
            }
            if(node->right != NULL)
            {
                fprintf(dot_out, "\t\t\"%s\"->", node->data.dblValue);
                fprintfConnection(node->right, RIGHT, dot_out);
                dumpGraphNode(node->right, dot_out);
            }
            break;

        case VAR:
            fprintf(dot_out, "\n\t\t\"%c\"[shape = \"ellipse\", color=\"#900000\", style=\"filled\", \
                               fillcolor = \"#D0FDFF\"];\n", node->data.varValue);
            if(node->left != NULL)
            {
                fprintf(dot_out, "\t\t\"%s\"->", node->data.varValue);
                fprintfConnection(node->left, LEFT, dot_out);
                dumpGraphNode(node->left, dot_out);
            }
            if(node->right != NULL)
            {
                fprintf(dot_out, "\t\t\"%s\"->", node->data.varValue);
                fprintfConnection(node->right, RIGHT, dot_out);
                dumpGraphNode(node->right, dot_out);
            }
            break;

        case OP:
            switch (node->data.opValue)
            {
                case OP_ERROR:
                    break;

                case OP_ADD:
                    fprintf(dot_out, "\n\t\t\"+\"[shape = \"ellipse\", color=\"#900000\", \
                                       style=\"filled\", fillcolor = \"#D0FDFF\"];\n");
                    break;

                case OP_SUB:
                    fprintf(dot_out, "\n\t\t\"-\"[shape = \"ellipse\", color=\"#900000\", \
                                       style=\"filled\", fillcolor = \"#D0FDFF\"];\n");
                    break;

                case OP_MUL:
                    fprintf(dot_out, "\n\t\t\"*\"[shape = \"ellipse\", color=\"#900000\", \
                                       style=\"filled\", fillcolor = \"#D0FDFF\"];\n");
                    break;

                case OP_DIV:
                    fprintf(dot_out, "\n\t\t\"/\"[shape = \"ellipse\", color=\"#900000\", \
                                       style=\"filled\", fillcolor = \"#D0FDFF\"];\n");
                    break;

                default:
                    break;
            }

            if(node->left != NULL)
            {
                fprintf(dot_out, "\t\t\"%s\"->", node->data.opValue);
                fprintfConnection(node->left, LEFT, dot_out);
                dumpGraphNode(node->left, dot_out);
            }
            if(node->right != NULL)
            {
                fprintf(dot_out, "\t\t\"%s\"->", node->data.opValue);
                fprintfConnection(node->right, RIGHT, dot_out);
                dumpGraphNode(node->right, dot_out);
            }
            break;

        default:
            break; 
    }

    return TREE_SUCCESS;
}

//=========================================================================

int fprintfConnection(node_t* node, char* ANSW, FILE* dot_out)
{
    CHECK(ANSW    !=  NULL, ERR_TREE_NULL_PTR);
    CHECK(node    !=  NULL, ERR_TREE_NULL_PTR);
    CHECK(dot_out !=  NULL, ERR_TREE_BAD_FILE);

    switch (node->type)
    {
        case NUM:
            fprintf(dot_out, "\"%lg\"[label = \"%s\"];\n", node->data.dblValue, ANSW);
            break;

        case VAR:
            fprintf(dot_out, "\"%c\"[label = \"%s\"];\n", node->data.varValue, ANSW);
            break;

        case OP:
            switch (node->data.opValue)
            {
                case OP_ERROR:
                    break;

                case OP_ADD:
                    fprintf(dot_out, "\"+\"[label = \"%s\"];\n", ANSW);
                    break;

                case OP_SUB:
                    fprintf(dot_out, "\"-\"[label = \"%s\"];\n", ANSW);
                    break;

                case OP_MUL:
                    fprintf(dot_out, "\"*\"[label = \"%s\"];\n", ANSW);
                    break;

                case OP_DIV:
                    fprintf(dot_out, "\"/\"[label = \"%s\"];\n", ANSW);
                    break;

                default:
                    break;
            }

        default:
            break;
    }

    return TREE_SUCCESS;
}

//=========================================================================
