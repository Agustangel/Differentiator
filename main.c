#include <stdio.h>
#include "diff.h"
#include "diff_tree.h"
#include "debug.h"
#include <stdlib.h>
#include <onegin.h>


int main()
{
    tree_t tree;
    treeCtor(&tree);

    FILE* text = fopen("expression.txt", "r");
    CHECK(text != NULL, ERR_DIFF_NULL_PTR);

    int count = count_symbols(text);
    CHECK(count >= 0, ERR_DIFF_NEGATIVE_COUNT);

    char* str = (char*) calloc(count, sizeof(char));
    fill_buffer(text, str, sizeof(char), count);

    fclose(text);

    node_t* root = makeAST(str);
    node_t* diff_root = differentiate(root);
    tree.root = diff_root;

    convolveExpression(tree.root);

    dumpGraphTree(&tree);
    dump(&tree);

    return 0;
}
