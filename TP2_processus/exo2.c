
/* ping/pong processus: utilisation de continuer/suspendre (exclusivement). 

2) Écrire un programme réalisant dix alternances ping/pong à base de
   continuer/suspendre. Il est interdit d'utiliser commuter.
   A-t-on encore besoin de l'hypothèse précédente ?
   Donner une solution qui ne dépend aucunement de la stratégie
   d'ordonnancement.
   (squelette disponible dans exo2.c)

*/
   
#include "processus.h"
#include "scheduler.h"
#include <stdio.h>

#define N 10

processus_t pmain, p1, p2;


int main ()
{
    sched_set_scheduler (&sched_aleatoire); /* ça devrait marcher avec */
    proc_init ();

    p1 = proc_activer("P1", ping, NULL);
    p2 = proc_activer("P2", pong, NULL);

    printf ("Debut\n");
    pmain = proc_self();

    int i;
    for (i = 0; i < N; i++) {
        proc_continuer(p1);
    }
    proc_suspendre();
    printf ("Fin\n");
    return 0;
}



void ping(void *unused) {

    printf("Ping\n");
    proc_continuer(pong);
    proc_suspendre();

}

void pong(void *unused) {

    printf("\tPong\n");
    proc_continuer(ping);
    proc_suspendre();
    
    proc_continuer(pmain); /* redonne la main à main */
}
