
/* ping/pong processus: utilisation de commuter */
   
#include "processus.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

#define N 10

static void ping_or_pong (void *texte)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("%s\n", (char *)texte);
    }
    printf ("%s a fini\n", (char *)texte);
}


int main ()
{
    /* sched_set_scheduler (&sched_aleatoire); */ /* pour voir */
    proc_init ();
    printf ("Debut\n");

    proc_suspendre();
    return 0;
}
