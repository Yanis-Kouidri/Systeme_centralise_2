
/* Doit afficher: "1 2 3 4 5"
 *  et terminer par "c'est fini."
 */
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"

void code1 (void *unused)
{
    printf("1\n");
    printf("3\n");
}

void code2 (void *unused)
{
    printf("2\n");
    printf("5\n");
}

void code3 (void *unused)
{
    printf("4\n");
}

int main()
{
    printf ("C'est fini.\n");
    return 0;
}
