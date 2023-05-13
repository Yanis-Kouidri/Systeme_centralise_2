
/* ping/pong processus: utilisation de continuer/suspendre (exclusivement). */
   
#include "processus.h"
#include "scheduler.h"
#include <stdio.h>

#define N 10

int main ()
{
    sched_set_scheduler (&sched_aleatoire); /* ça devrait marcher avec */
    proc_init ();
    printf ("Debut\n");
    printf ("Fin\n");
    return 0;
}
