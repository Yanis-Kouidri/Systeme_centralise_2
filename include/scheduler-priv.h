#ifndef _SCHEDULER_PRIV_H_
#define _SCHEDULER_PRIV_H_

#include "scheduler.h"

/* Interface privée des schedulers, EXCLUSIVEMENT pour le module processus.
 *
 * Ces primitives ne sont pas protégées contre une préemption intempestive
 * et doivent être appelées en mode non interruptible.
 */

/* Insère le processus à sa place dans les prêts. */
void sched_ajouter_pret (processus_t qui);

/* Extrait un nouvel élu parmi les prêts (le processus en tête des prêts
 * pour le scheduler FIFO). */
processus_t sched_choisir_elu (void);

/* Nombre de processus prêts. */
int sched_nb_prets (void);

/* Initialisation du scheduler. */
void sched_init (void);

/* Pour debugger. */
void sched_lister_prets (void);

/****************************************************************/

/* L'interface objet d'un scheduler quelconque. */
struct scheduler {
    char *(*nom) (void);
    /* Initialisation du scheduler. */
    void (*init) (void);

    /* Les primitives privées. */
    processus_t (*choisir_elu) (void);
    void        (*ajouter_pret) (processus_t qui);

    /* Les primitives publiques. */
    int  (*nb_prets) (void);
    void (*lister_prets) (void);
    int  (*priorite_min) (void);
    int  (*priorite_max) (void);
    int  (*get_priority) (void);
    void (*set_priority) (int prio);
    void (*set_priority_other) (processus_t qui, int prio);
};

#endif
