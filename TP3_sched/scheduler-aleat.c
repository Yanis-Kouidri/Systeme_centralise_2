#include "processus.h"
#include "scheduler-priv.h"
#include "queue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> /* pour initialiser le générateur aléatoire */

/*
 * Scheduler aléatoire: choisit au hasard parmi les prêts.
 * Ce scheduler est pseudo-équitable (aussi équitable que le générateur
 * pseudo-aléatoire est aléatoire).
 */

/* Gestion de la file des processus prêts */
static queue_t les_prets = NULL;

/****************************************************************/

static void sched_aleat_init (void)
{
    srandom ((int) time(NULL));
}

/* Ajoute p à la fin de la file. */
static void sched_aleat_ajouter_pret (processus_t qui)
{
    /* XXXX A COMPLETER XXXX */
}

/* Extrait un processus au hasard dans la file des prêts. */
static processus_t sched_aleat_choisir_elu (void)
{
    /* XXXX A COMPLETER XXXX */
}

/* Nombre de processus prêts. */
static int sched_aleat_nb_prets (void)
{
    return queue_length(&les_prets);
}

/* Pour debugger. */
static void sched_aleat_lister_prets (void)
{
    rendre_noninterruptible();
    printf ("SCHED: liste des prets:");
    if (queue_isempty(&les_prets)) {
        printf (" <vide>");
    } else {
        int len = queue_length(&les_prets);
        for (int i = 0; i < len; i++) {
            processus_t p = (processus_t) queue_get(&les_prets, i);
            printf (" %s/%p", proc_nom (p), p);
        }
    }
    printf ("\n");
    rendre_interruptible();
}

/****************************************************************/

/* Pas de priorité => tout le monde à 1. */

static int sched_aleat_priorite_min(void)
{
    return 1;
}

static int sched_aleat_priorite_max(void)
{
    return 1;
}

static int  sched_aleat_get_priority (void)
{
    return 1;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static void sched_aleat_set_priority (int prio)
{
    /* nop */
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static void sched_aleat_set_priority_other (processus_t qui, int prio)
{
    /* nop */
}

/****************************************************************/

static char *sched_aleat_nom (void)
{
    return "Aleatoire (version étudiant)";
}

struct scheduler sched_aleatoire = {
    sched_aleat_nom,
    sched_aleat_init,
    sched_aleat_choisir_elu,
    sched_aleat_ajouter_pret,
    sched_aleat_nb_prets,
    sched_aleat_lister_prets,
    sched_aleat_priorite_min,
    sched_aleat_priorite_max,
    sched_aleat_get_priority,
    sched_aleat_set_priority,
    sched_aleat_set_priority_other
};

