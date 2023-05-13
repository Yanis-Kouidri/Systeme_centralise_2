
/* Exemple 2 du cours.
 */
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"

coroutine_t c1, c2, c3;

void code1 (void *unused)
{
    cor_transferer(c1, c2);
    cor_transferer(c1, c2);
    cor_transferer(c3, c2);
    printf("code1 : fin atteinte.\n");
}

void code2 (void *unused)
{
    cor_transferer(c2, c1);
    cor_transferer(c1, c1);
    printf("code2 : fin atteinte.\n");
}

int main()
{
    cor_verbose = 1;   /* pour tracer les transferts */
    c1 = cor_creer("C1", code1, NULL);
    c2 = cor_creer("C2", code2, NULL);
    c3 = cor_creer("C3", NULL, NULL);
    cor_transferer(c3, c1);
    printf("main : fin atteinte.\n");
    return 0;
}
