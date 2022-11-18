#pragma once

const double _PI_    = 3.1415926535;
const double _NUM_E_ = 2.7182818284;
const double EPSILON = 1e-6;

//=========================================================================

enum differentiator_codes
{
    DIFF_SUCCESS              = 0,
    ERR_DIFF_NULL_PTR         = 1
};

enum node_type
{
    NUM = 1,
    VAR = 2,
    OP  = 3
};

enum oprion_type
{
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MUL = 3,
    OP_DIV = 4
};
