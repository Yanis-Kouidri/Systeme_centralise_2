
/* ping/pong processus: utilisation de continuer/suspendre (exclusivement). 

2) Écrire un programme réalisant dix alternances ping/pong à base de
   continuer/suspendre. Il est interdit d'utiliser commuter.
   A-t-on encore besoin de l'hypothèse précédente ?
   Donner une solution qui ne dépend aucunement de la stratégie
   d'ordonnancement.
   (squelette disponible dans exo2.c)


    C'est dur !!!!!!!!
    Mais j'ai réussi donc ça va
*/
   
#include "processus.h"
#include "scheduler.h"
#include <stdio.h>

#define N 10

processus_t pmain, p1, p2;


void ping(void *unused) {
    proc_continuer(pmain);
    proc_suspendre();

    int i;
    for (i = 0; i < N; i++) {
        printf("Ping\n");
        proc_continuer(p2);
        proc_suspendre();
    }

    proc_continuer(pmain); /* redonne la main à main */

}

void pong(void *unused) {

    proc_continuer(p1);
    proc_suspendre();

    int i;
    for (i = 0; i < N; i++) {
        printf("\tPong\n"); 
        proc_continuer(p1);
        proc_suspendre();
        
    }
}

int main ()
{
    sched_set_scheduler (&sched_aleatoire); /* ça devrait marcher avec */
    proc_init ();
    pmain = proc_self();
    printf ("Debut\n");
    
    p1 = proc_activer("P1", ping, NULL);
    proc_suspendre();

    p2 = proc_activer("P2", pong, NULL);
    proc_suspendre();
    
    printf ("Fin\n");
    return 0;
}

