#pragma once

const double _PI_    = 3.1415926535;
const double _NUM_E_ = 2.7182818284;
const double EPSILON = 1e-6;


#define dL differentiate(node->left)
#define dR differentiate(node->right)

#define cL node->left
#define cR node->right

//=========================================================================

enum differentiator_codes
{
    DIFF_ERROR                = -1,
    DIFF_SUCCESS              =  0,
    ERR_DIFF_NULL_PTR         =  1,
    ERR_DIFF_BAD_FILE         =  2
    
};

node_t* differentiate(node_t* node);
node_t* Add(node_t* left, node_t* right);
node_t* Sub(node_t* left, node_t* right);
node_t* Mul(node_t* left, node_t* right);
node_t* Div(node_t* left, node_t* right);
node_t* Pow(node_t* node, int power);
node_t* Sin(node_t* node);
node_t* Cos(node_t* node);
node_t* Exp(node_t* node);
