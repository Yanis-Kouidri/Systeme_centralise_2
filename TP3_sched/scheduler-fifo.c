#include "processus.h"
#include "scheduler-priv.h"
#include "TSD.h"
#include "queue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Scheduler basique: une seule file FIFO, sans priorité.
 * Ce scheduler est équitable.
 */

/* Clef d'accès au TSD qui stocke les informations d'ordonnancement. */
static int clef_scheduler = -1;

/* Chaque processus possède une structure "pret", qui sert pour stocker
 * les informations associés et sa priorité (si nécessaire).
 */
typedef struct pret {
    processus_t  qui;           /* pour retrouver le processus concerné */
} *pret_t;

/* Gestion de la file des processus prêts. */
static queue_t les_prets = NULL;

/****************************************************************/

/* Initialisation du module. */
static void sched_fifo_init (void)
{
    if (clef_scheduler == -1)
      clef_scheduler = TSD_creer_clef (free);
    les_prets = NULL;
}

/* Obtention de la donnée spécifique du processus `qui'.
 * Si la structure n'existe pas (première appel pour ce processus),
 * elle est créée et initialisée.
 */
static pret_t get_info (processus_t qui)
{
    pret_t info = TSD_get_other (qui, clef_scheduler);
    if (info == NULL) {         /* première fois => création */
        info = malloc (sizeof (struct pret));
        info->qui = qui;
        TSD_set_other (qui, clef_scheduler, info);
    }
    return info;
}

/****************************************************************/

/* Ajoute qui à la fin de la file. */
static void sched_fifo_ajouter_pret (processus_t qui)
{
    pret_t p = get_info (qui);
    queue_add (&les_prets, p);
}
/* Extrait le processus en tête de la file des prêts. */
static processus_t sched_fifo_choisir_elu (void)
{
    pret_t info;
    assert (! queue_isempty(&les_prets));
    info = (pret_t) queue_take(&les_prets);
    if (sched_verbose)
      printf ("SCHED: %s: selectionné\n", proc_nom(info->qui));
    return info->qui;
}

/* Nombre de processus prêts. */
static int sched_fifo_nb_prets (void)
{
    return queue_length(&les_prets);
}

/****************************************************************/

/* Pas de priorité => tout le monde à 1. */

static int sched_fifo_priorite_min(void)
{
    return 1;
}

static int sched_fifo_priorite_max(void)
{
    return 1;
}

static int  sched_fifo_get_priority (void)
{
    return 1;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static void sched_fifo_set_priority (int prio)
{
    /* nop */
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static void sched_fifo_set_priority_other (processus_t qui, int prio)
{
    /* nop */
}

/****************************************************************/

/* Pour debugger. */
static void sched_fifo_lister_prets (void)
{
    rendre_noninterruptible();
    printf ("SCHED: liste des prets:");
    if (queue_isempty(&les_prets)) {
        printf (" <vide>");
    } else {
        int len = queue_length(&les_prets);
        for (int i = 0; i < len; i++) {
            pret_t p = (pret_t) queue_get(&les_prets, i);
            printf (" %s/%p", proc_nom (p->qui), p->qui);
        }
    }
    printf ("\n");
    rendre_interruptible();
}

/****************************************************************/

static char *sched_fifo_nom (void)
{
    return "FIFO";
}

struct scheduler sched_fifo = {
    sched_fifo_nom,
    sched_fifo_init,
    sched_fifo_choisir_elu,
    sched_fifo_ajouter_pret,
    sched_fifo_nb_prets,
    sched_fifo_lister_prets,
    sched_fifo_priorite_min,
    sched_fifo_priorite_max,
    sched_fifo_get_priority,
    sched_fifo_set_priority,
    sched_fifo_set_priority_other
};

