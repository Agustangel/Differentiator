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
    DIFF_SUCCESS              = 0,
    ERR_DIFF_NULL_PTR         = 1
};

node_t* differentiate(node_t* node);
