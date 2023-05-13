
/* ping/pong coroutines. */
   
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>

#define N 10

void ping_or_pong (void *texte)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("%s\n", (char *)texte);
    }
}

int main()
{
    printf ("Debut\n");
    printf ("Fin\n");
    return 0;
}
