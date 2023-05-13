/* 1) Écrire un programme qui affiche 1,2,3,4,5 avec 4 coroutines :
    - une procédure affiche 1 et 3
    - une procédure affiche 2 et 5
    - une procédure affiche 4
    - le programme conclut avec un message indiquant la fin.
   (squelette disponible dans exo1.c)
*/

/* Doit afficher: "1 2 3 4 5"
 *  et terminer par "c'est fini."
 */
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"

coroutine_t c1, c2, c3, cm;

void code1 (void *unused)
{
    printf("1\n");
    cor_transferer(c1, c2);
    printf("3\n");
    cor_transferer(c1, c3);
}

void code2 (void *unused)
{
    printf("2\n");
    cor_transferer(c2, c1);
    printf("5\n");
    cor_transferer(c2, cm);
}

void code3 (void *unused)
{
    printf("4\n");
    cor_transferer(c3, c2);
}

int main()
{
    c1 = cor_creer("C1", code1, NULL);
    c2 = cor_creer("C2", code2, NULL);
    c3 = cor_creer("C3", code3, NULL);
    cm = cor_creer("CM", NULL, NULL);
    cor_transferer(cm, c1);
    printf ("C'est fini.\n");
    return 0;
}
