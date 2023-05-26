/*
 * 1) Écrire un programme réalisant dix alternances ping/pong à base de
   commuter.
   Quelle hypothèse sur l'ordonnanceur à court terme (qui choisit le nouvel
   élu) est nécessaire ?
   (squelette disponible dans exo1.c)

   Pour mettre en évidence le besoin d'hypothèse sur l'ordonnancement, on
   peut utiliser un ordonnanceur aléatoire (le choix du processus élu est au
   hasard parmi les prêts). Dans main, ajouter avant proc_init :
       sched_set_scheduler (&sched_aleatoire);

   On peut aussi utiliser un ordonnanceur à priorité absolue (le processus
   ayant la plus forte priorité est toujours le processus élu):
   - dans main, ajouter avant proc_init :
       sched_set_scheduler (&sched_prio_tempsreel);
   - dans main, fixer la priorité des processus: 
       sched_set_priority_other (proc_ping, 3);
       sched_set_priority_other (proc_pong, 6);
       sched_set_priority (1); // prio du processus initial
*/

/* ping/pong processus: utilisation de commuter */
   
#include "processus.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

#define N 10

processus_t pmain, p1, p2;

static void ping_or_pong (void *texte)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("%s\n", (char *)texte);
        proc_commuter();
    }
    printf ("%s a fini\n", (char *)texte);
    proc_continuer(pmain);
}


int main ()
{
    // sched_set_scheduler (&sched_aleatoire);  /* pour voir */
    proc_init ();
    pmain = proc_self();
    printf ("Debut\n");
    p1 = proc_activer("P1", ping_or_pong, "Ping");
    p2 = proc_activer("P2", ping_or_pong, "Pong");

    sched_set_priority_other (p1, 3);
    sched_set_priority_other (p2, 6);
    sched_set_priority (1); // prio du processus initial


    proc_suspendre();

    return 0;
}
