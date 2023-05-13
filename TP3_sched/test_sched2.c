/* Plusieurs processus qui incrémentent chacun leur compteur, avec
 * préemption activée.
 * La valeur des compteurs doit être en relation avec les priorités:
 *  - fifo => mêmes valeurs
 *  - prio statique => ne marche pas (utilise sleep)
 *  - prio dynamique => même rapport que le rapport des priorités
 *  - noyau factice => politique FIFO => mêmes valeurs.
 *
 * ATTENTION : à partir de gcc 4.4, il faut impérativement compiler sans
 * optimisation, sinon la boucle est éliminée !
 */

#include "processus.h"
#include "preemption.h"
#include "scheduler.h"
#include "sleep.h"
#include <stdio.h>

struct arguments {
    int prio;
    long long *cpt;
};

#pragma GCC optimize ("-O0")
void compteur (void *a)
{
    struct arguments *arg = (struct arguments *) a;
    long long *cpt = arg->cpt;
    sched_set_priority (arg->prio);
    while (1) {
        (*cpt)++;
    }
}

int main ()
{
    struct arguments arg1, arg2, arg3;
    long long cpt1, cpt2, cpt3;
    int prio1, prio2, prio3;
    proc_verbose = 0;
    preemption_verbose = 0;

    sched_verbose = 1;
    sched_set_scheduler (&sched_prio_dynamique);
    proc_init ();
    printf("Création de trois processus qui incrémentent chacun un compteur.\n");
    printf("Le rapport des compteurs doit être à peu près égal au rapport des priorités.\n");
    sched_verbose = 0;
    
    sched_set_priority (sched_priorite_max());

    prio1 = sched_priorite_min();
    prio2 = (sched_priorite_min() + sched_priorite_max()) / 2;
    prio3 = sched_priorite_max();
    cpt1 = cpt2 = cpt3 = 0;

    arg1.prio = prio1;
    arg1.cpt = &cpt1;
    
    arg2.prio = prio2;
    arg2.cpt = &cpt2;
    
    arg3.prio  = prio3;
    arg3.cpt = &cpt3;
    
    proc_activer ("1", compteur, &arg1);
    proc_activer ("2", compteur, &arg2);
    proc_activer ("3", compteur, &arg3);
    preemption_activate (1);
    
    proc_sleep (5);

    /*preemption_activate (0);*/
    printf ("cpt1=%lld, cpt2=%lld, cpt3=%lld\n", cpt1, cpt2, cpt3);
    printf ("p1/p2 = %.3f cpt1/cpt2 = %.3f\n",
            (float) prio1 / prio2, (float) cpt1 / cpt2);
    printf ("p2/p3 = %.3f cpt2/cpt3 = %.3f\n",
            (float) prio2 / prio3, (float) cpt2 / cpt3);
    printf ("p1/p3 = %.3f cpt1/cpt3 = %.3f\n",
            (float) prio1 / prio3, (float) cpt1 / cpt3);
    
    return 0;
}
