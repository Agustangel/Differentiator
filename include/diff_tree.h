#pragma once

#include <stdio.h>
#include <stdint.h>


//=========================================================================

typedef uint_fast16_t treeStatus_t;
typedef struct node_t node_t;

typedef struct treeData_t
{
    double       dblValue;
    const char*  varValue;
    enum node_op opValue;

} treeData_t;


struct node_t
{
    enum node_type type;
    treeData_t     data;

    node_t* left;
    node_t* right;

};


typedef struct tree_t
{
    node_t* root;
    size_t  size;

    treeStatus_t status;

} tree_t;

//=========================================================================

enum tree_codes
{
    TREE_SUCCESS              = 0,
    ERR_TREE_NULL_PTR         = 1,
    ERR_TREE_OUT_MEMORY       = 2,
    ERR_TREE_BAD_SIZE         = 3,
    ERR_TREE_BAD_POSITION     = 4,
    ERR_TREE_BAD_STRING       = 5,
    ERR_TREE_BAD_FILE         = 6
};

//-------------------------------------------------------------------

enum node_type
{
    NUM = 1,
    VAR = 2,
    OP  = 3
};

enum node_op
{
    plus    = 1,
    minus   = 2,
    mul     = 3,
    div     = 4,
    pow     = 5,
    sqrt    = 6,
    bracket = 7,
};

enum node_codes
{
    LEFT  = 0,
    RIGHT = 1
};

//-------------------------------------------------------------------

enum print_mode
{
    PREORDER  = 1,
    POSTORDER = 2,
    INORDER   = 3
};

enum
{
    DATA_POISON  = 0,
    ERROR_NUMBER = 5
};
