/* ping/pong préemptif (couche processus) avec scheduler à priorité */
/* Doit afficher de longues séries de PING/PONG en alternance,
 * puis de PANG/PUNG en alternance,
 * puis conclure avec "fin du dernier processus, tchao."
 */

#include "processus.h"
#include "preemption.h"
#include "scheduler.h"
#include "sleep.h"
#include <stdio.h>

void ping (void *s)
{    int i, k, j;
    for (i = 0; i < 5000; i++) {
        printf ("%s\n", (char *)s);
        k = 0;
        for (j = 0; j < 1000; j++)
          k = k+j;
    }
    if (k != 0) /* utilisation de k pour embêter l'optimiseur */
    printf ("**** %s done\n", (char *)s);
}

int main ()
{
    processus_t p;
    proc_verbose = 0;
    preemption_verbose = 0;

    sched_verbose = 1;
    sched_set_scheduler (&sched_prio_dynamique);
    proc_init ();    
    sched_verbose = 0;
    
    p = proc_activer ("PING", ping, "PING");
    sched_set_priority_other (p, 5);
    p = proc_activer ("PONG", ping, "    PONG");
    sched_set_priority_other (p, 5);
    p = proc_activer ("PANG", ping, "        PANG");
    sched_set_priority_other (p, 1);
    p = proc_activer ("PUNG", ping, "            PUNG");
    sched_set_priority_other (p, 1);
    p = proc_activer ("PENG", ping, "                PENG");
    sched_set_priority_other (p, 2);
    p = proc_activer ("PYNG", ping, "                    PYNG");
    sched_set_priority_other (p, 5);
    preemption_activate (1);
    /* proc_sleep ne marche pas avec scheduler-prio. */
    proc_sleep (2);
    printf ("**************** PYNG priority down to 2 ****************\n");
    sched_set_priority_other (p, 2);
    proc_suspendre ();
    return 3;
}
