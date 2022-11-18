#include <stdio.h>
#include <bintree.h>



int main()
{
    tree_t tree = {0};
    tree_ctor(&tree);
    elem_t fictiondata = 0;
    insert_root(&tree, 0);

    
    return 0;
}
