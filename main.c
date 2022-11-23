#include <stdio.h>
#include "diff.h"
#include "diff_tree.h"
#include "debug.h"



int main()
{
    char str[80];
    scanf("%s", str);

    int val = getG(str);
    printf("val = %d\n", val);
    
    return 0;
}
