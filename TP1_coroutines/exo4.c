
/* ping/pong coroutines. 
4) Idem 3, mais avec une seule procédure ping_or_pong, et en passant en
   paramètre le message à afficher.
   Il est interdit d'utiliser tout mécanisme qui permet de tester quel
   message est en cours d'affichage (que ce soit une comparaison avec texte,
   une variable tour qui alterne, ...)
   (squelette disponible dans exo4.c)
*/
   
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>

#define N 10

coroutine_t c1, c2, cm;

void ping_or_pong (void *texte)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("%s\n", (char *)texte);
        cor_transferer(c2, c2);
    }
    cor_transferer(c1, cm);
}

int main()
{
    c1 = cor_creer("C1", ping_or_pong, "ping");
    c2 = cor_creer("C2", ping_or_pong, "    pong");
    cm = cor_creer("CM", NULL, NULL);
    printf ("Debut\n");
    cor_transferer(cm, c1);
    printf ("Fin\n");
    return 0;
}
