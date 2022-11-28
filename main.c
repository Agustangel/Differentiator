#include <stdio.h>
#include "diff.h"
#include "diff_tree.h"
#include "debug.h"



int main()
{
    tree_t tree;
    treeCtor(&tree);

    char str[80];
    scanf("%s", str);

    node_t* root = makeAST(str);
    //tree.root = root;
    //dumpGraphTree(&tree);

    node_t* diff_root = differentiate(root);
    
    return 0;
}
