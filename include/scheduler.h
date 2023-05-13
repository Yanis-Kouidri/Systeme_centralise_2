#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/* Le scheduler "statique". Il s'agit du choix d'un nouveau
 * processus prêt, avec éventuellement gestion de priorités (statiques
 * ou dynamiques). La partie "temporelle" du scheduler (choix du
 * quantum de temps) se fait dans le module préemption. */

/* Interface publique, utilisable par tout programme utilisateur. */

#include "processus.h"

extern int sched_verbose;

/* Ces primitives se protègent contre une préemption intempestive
 * (rendre_noninterruptible). */

/* Renvoie les valeurs minimales et maximales pour une priorité.
 * (plus c'est grand, plus c'est prioritaire). */
int sched_priorite_min(void);
int sched_priorite_max(void);

/* Obtention et positionnement de la priorité du processus appelant. */
int  sched_get_priority (void);
void sched_set_priority (int prio);

/* Positionnement de la priorité d'un autre processus (qui peut être
 * dans n'importe quel état). */
void sched_set_priority_other (processus_t qui, int prio);

/****************/

/* Nom du scheduler courant. */
char *sched_nom (void);

typedef struct scheduler *scheduler_t;

/* Les schedulers prédéfinis: */

/* Une seule file FIFO, sans priorité (équitable). */
extern struct scheduler sched_fifo;

/* Priorité absolue fixe, FIFO dans un même niveau, sans distinction des
 * processus système (non équitable). */
extern struct scheduler sched_prio_tempsreel;

/* Priorité dynamique (équitable). */
extern struct scheduler sched_prio_dynamique;
extern struct scheduler sched_prio2_dynamique; /* pour le tp3 */

/* Choix aléatoire (pseudo-équitable). */
extern struct scheduler sched_aleatoire;

/* Changement de scheduler.
 * Doit être appelé avant l'initialisation de la couche processus. */
void sched_set_scheduler (scheduler_t);

#endif
