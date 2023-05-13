#include "processus.h"
#include "scheduler-priv.h"
#include "TSD.h"
#include "queue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Scheduler avec priorité absolue dynamique.
 *
 * Un processus de priorité i s'exécute i/j plus souvent qu'un
 * processus de priorité j.
 * Ce scheduler est équitable.
 */

/* Il est plus simple que PRIO_MIN soit 0 (indexage des tableaux).
 * Le niveau 0 est réservé (pour rebasculer à leur niveau max tous les
 * processus ensemble). */
#define PRIO_MIN 0
#define PRIO_MAX 10

/* Clef d'accès au TSD qui stocke les informations d'ordonnancement. */
static int clef_scheduler = -1;

/* Chaque processus possède une structure "pret", qui sert pour stocker
 * sa priorité et sa place dans une file des prêts.
 */
typedef struct pret {
    int          prio_max;      /* priorité max du processs */
    int          prio_courante; /* priorité courante du processs */
    processus_t  qui;           /* pour retrouver le processus concerné */
} *pret_t;

/* Les listes des processus PRETs. */
/* Taille = PRIO_MAX+1 et non PRIO_MAX-PRIO_MIN+1 pour simplifier l'indexage
 * des tableaux. */
static queue_t les_prets[PRIO_MAX + 1];

/****************************************************************/

/* Initialisation du module. */
static void sched_prio_init (void)
{
    int i;
    if (clef_scheduler == -1)
      clef_scheduler = TSD_creer_clef (free);
    assert (clef_scheduler != -1);
    for (i = PRIO_MIN; i <= PRIO_MAX; i++)
      les_prets[i] = NULL;
}

/****************************************************************/

static int sched_prio_priorite_min(void)
{
    return PRIO_MIN+1;          /* PRIO_MIN is reserved */
}

static int sched_prio_priorite_max(void)
{
    return PRIO_MAX;
}

/* Obtention de la donnée spécifique du processus `qui' qui contient ses priorités.
 * Si la structure n'existe pas (première appel pour ce processus),
 * elle est créée et initialisée avec la priorité du processus créateur.
 */
static pret_t get_info (processus_t qui)
{
    pret_t info = TSD_get_other (qui, clef_scheduler);
    if (info == NULL) {         /* première fois => création */
        info = malloc (sizeof (struct pret));
        if (qui != proc_self()) { /* qui est le père? */
            pret_t autreinfo = get_info (proc_self());
            info->prio_max = autreinfo->prio_max;
            info->prio_courante = info->prio_max;
        } else { /* seul le proc principal peut être dans cette situation. */
            info->prio_max = PRIO_MAX;
            info->prio_courante = PRIO_MAX;
        }
        info->qui = qui;
        TSD_set_other (qui, clef_scheduler, info);
    }
    return info;
}

/****************************************************************/

/* Ajoute p dans les prêts.
 * (selon la politique d'ordonnancement. Ici à la fin de la file
 * correspondant à la priorité courante de p). */
static void sched_prio_ajouter_pret (processus_t qui)
{
    /* XXXX A COMPLETER XXXX */
}

/* Extrait le processus en tête des prêts.
 * (selon la politique d'ordonnancement). */
static processus_t sched_prio_choisir_elu (void)
{
    /* XXXX A COMPLETER XXXX */
}

/* Nombre de processus prêts. */
static int sched_prio_nb_prets (void)
{
    int sum = 0;
    int prio;
    for (prio = PRIO_MAX; prio >= PRIO_MIN; prio--) {
        sum += queue_length(&(les_prets[prio]));
    }
    return sum;
}

/****************************************************************/

/* Obtention de la priorité maximale du processus courant. */
static int sched_prio_get_priority (void)
{
    pret_t info;
    int priorite;
    rendre_noninterruptible();
    info = get_info (proc_self());
    priorite = info->prio_max;
    rendre_interruptible();
    return priorite;
}

/* Positionnement de la priorité maximale du processus courant. */
static void sched_prio_set_priority (int prio)
{
    pret_t info;
    if (prio < sched_prio_priorite_min())
      prio = sched_prio_priorite_min();
    if (prio > sched_prio_priorite_max())
      prio = sched_prio_priorite_max();
    rendre_noninterruptible();
    info = get_info (proc_self());
    if (sched_verbose)
      printf ("SCHED: %s: set priority %d -> %d\n", proc_nom(proc_self()), info->prio_max, prio);
    info->prio_max = prio;
    if (info->prio_courante > info->prio_max)
      info->prio_courante = info->prio_max;
    rendre_interruptible();
}

/* Positionnement de la priorité maximale d'un autre processus. */
static void sched_prio_set_priority_other (processus_t qui, int prio)
{
    pret_t info;
    if (prio < sched_prio_priorite_min())
      prio = sched_prio_priorite_min();
    if (prio > sched_prio_priorite_max())
      prio = sched_prio_priorite_max();
    rendre_noninterruptible();
    info = get_info (qui);
    if (sched_verbose)
      printf ("SCHED: %s: set priority %d -> %d\n", proc_nom(qui), info->prio_max, prio);
    info->prio_max = prio;
    /* La priorité courante devient le min(ancienne priorité courante,
     *                                     nouvelle priorité max). */
    /* Le processus était dans une file de prêts, et sa priorité change. */
    if (info->prio_courante > prio) {
        /* est-il dans la file des prêts (ou suspendu sinon) ? */
        int pos = queue_length(&(les_prets[info->prio_courante])) - 1;
        while (pos >= 0 && queue_get(&(les_prets[info->prio_courante]), pos) != info)
          pos--;
        if (pos >= 0) { // il est dans une file des prêts, on le transfère dans la bonne
            queue_remove(&(les_prets[info->prio_courante]), pos);
            info->prio_courante = prio;
            queue_add (&(les_prets[info->prio_courante]), info);
        }
    }
    rendre_interruptible();
}


/****************************************************************/

/* Pour debugger. */
static void sched_prio_lister_prets (void)
{
    int prio;
    rendre_noninterruptible();
    printf ("SCHED: liste des prets:");
    for (prio = PRIO_MAX; prio >= PRIO_MIN; prio--) {
        printf (" [");
        int len = queue_length(&(les_prets[prio]));
        for (int i = 0; i < len; i++) {
            pret_t p = queue_get(&(les_prets[prio]), i);
            printf (" %s/%p (max=%d, cour=%d)", proc_nom (p->qui), p->qui, p->prio_max, p->prio_courante);
        }
        printf (" ]");
    }    
    printf ("\n");
    rendre_interruptible();
}

/****************************************************************/

static char *sched_prio_nom (void)
{
    return "Avec priorité dynamique (version étudiant)";
}

struct scheduler sched_prio_dynamique = {
    sched_prio_nom,
    sched_prio_init,
    sched_prio_choisir_elu,
    sched_prio_ajouter_pret,
    sched_prio_nb_prets,
    sched_prio_lister_prets,
    sched_prio_priorite_min,
    sched_prio_priorite_max,
    sched_prio_get_priority,
    sched_prio_set_priority,
    sched_prio_set_priority_other
};
