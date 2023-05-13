
/* ping/pong coroutines. */
   
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define N 10

void ping (void *unused)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("ping\n");
    }
    printf ("Ping a fini\n");
}

void pong (void *unused)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("     pong\n");
    }
    printf ("Pong a fini\n");
}

int main()
{
    printf ("Debut\n");
    printf ("Fin\n");
    return 0;
}
