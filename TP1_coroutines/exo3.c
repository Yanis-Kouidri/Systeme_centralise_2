
/* 
3) Écrire un programme réalisant dix alternances ping/pong.
   (squelette disponible dans exo3.c)
ping/pong coroutines. */
   
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define N 10

coroutine_t c1, c2, cm;

void ping (void *unused)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("ping\n");
        cor_transferer(c1, c2);
    }
    printf ("Ping a fini\n");
    cor_transferer(c1, c2);
}

void pong (void *unused)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("     pong\n");
        cor_transferer(c2, c1);
    }
    printf ("Pong a fini\n");
    cor_transferer(c2, cm);
}

int main()
{
    c1 = cor_creer("C1", ping, NULL);
    c2 = cor_creer("C2", pong, NULL);
    cm = cor_creer("CM", NULL, NULL);
    printf ("Debut\n");
    cor_transferer(cm, c1);
    printf ("Fin\n");
    return 0;
}
