#include "coroutines.h"
#include "processus.h"
#include "TSD.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

/*
 * Gestion de la file des processus prêts:
 * il s'agit d'une file circulaire doublement chaînée.
 * les_prets contient la tête de la file.
 *
 * le processus élu ne doit jamais être dans la file des prêts!
 *
 */

/* État d'un processus.
 * Il faut toujours assurer
 *  invariant (p->status == ELU) <=> (elu == p)
 *  invariant (p->status == PRET) <=> (p est dans le file des prêts)
 *  invariant (p->status == SUSPENDU|MORT) <=> ((elu != p && p notin les_prets)
 *
 * typedef enum { proc_PRET, proc_ELU, proc_SUSPENDU, proc_MORT } proc_status;
 * (défini dans processus.h pour les hooks)
 */
struct processus {
    processus_t suivant;        /* liste circulaire des prêts */
    processus_t precedent;      /* id */
    processus_t debug_suivant;  /* liste pour lister_tous */
    proc_status status;
    char *nom;
    void (*code) (void *);      /* Le code du processus */
    void *arg;                  /* et son argument. */
    coroutine_t cor;            /* la coroutine sous-jacente. */
    int noninterruptible;       /* le "niveau" de non-interruptabilité. */
    sigset_t mask;              /* Masque à restaurer */
    void *tsd[MAX_NB_CLEFS];    /* infos privées à chaque processus */
};

/* Pour lister. Dans le même ordre que proc_status. */
char *proc_status_nom[4] = { "PRET", "ELU", "SUSPENDU", "MORT" };

/* Pour debugger */
static processus_t tous_les_processus = NULL;

/* Liste (circulaire) des processus PRETs. */
static processus_t les_prets = NULL;

/* Le processus ELU. */
static processus_t elu = NULL;

/* Le processus principal (exécutant main).
 * Il ne sert que pour afficher les états lors de la terminaison, le
 * programme principal devant être logiquement suspendu. */
static processus_t proc_principal = NULL;

/* Les destructeurs pour les données spécifiques. */
static void (*TSD_destructeurs[MAX_NB_CLEFS]) (void *);

int proc_verbose = 0;

/********************************/

/* Insère p à la fin de la file (donc entre les_prets->precedent et les_prets) */
static void sched_ajouter_pret (processus_t p)
{
    if (les_prets != NULL) { /* la file n'est pas vide */
        p->suivant = les_prets;
        p->precedent = les_prets->precedent;
        p->suivant->precedent = p;
        p->precedent->suivant = p;
    } else {                    /* la file était vide */
        les_prets = p;
        p->suivant = p;
        p->precedent = p;
    }
}

/* Le processus en tête est extrait de la file des prêts. */
static processus_t sched_choisir_elu (void)
{
    processus_t p;
    assert (les_prets != NULL);
    p = les_prets;
    if (p->suivant != p) { /* la file ne devient pas vide */
        p->suivant->precedent = p->precedent;
        p->precedent->suivant = p->suivant;
        les_prets = p->suivant;
    } else {                    /* la file devient vide */
        les_prets = NULL;
    }
    p->suivant = NULL;          /* par sécurité */
    p->precedent = NULL;        /* id */
    return p;
}

/****************************************************************/

/*
 * Rend interruptible.
 * Il faut bien assurer d'exécuter rendre_interruptible en exclusion mutuelle,
 *  pour cela, on commence par masquer tous les signaux.
 * Puis on décrémente le compteur (si nécessaire), et s'il devient nul, on
 *  remet en place le masque sauvegardé. Sinon, on remet le masque à l'appel de
 *  rendre_interruptible (quelqu'il soit).
 * Ça a l'air un peu tordu, mais ainsi rendre_interruptible et
 * rendre_noninterruptible ne dépendent pas de la manière dont les masques
 * seront traités par ailleurs.
 *
 * XXXX Au démarrage du processus, il est possible que rendre_interruptible
 * soit appelé alors que rendre_noninterruptible n'avait jamais été appelé
 * (pour ce processus) et donc que elu->noninterruptible == 0.
 * La création du processus hérite correctement des masques des processus,
 * donc ça marche, mais je reste sceptique.
 */
void rendre_interruptible (void)
{
    sigset_t protect, protectorig;
    sigset_t *masque_a_reinstaller;
    sigfillset (&protect);
    sigprocmask (SIG_SETMASK, &protect, &protectorig);
    masque_a_reinstaller = &protectorig;
    if (elu->noninterruptible > 0) {
        elu->noninterruptible--;
    }
    if (elu->noninterruptible == 0)
      masque_a_reinstaller = &(elu->mask);
    if (proc_verbose)
      printf ("PROC: interruptible %s down to %d\n",
              elu->nom, elu->noninterruptible);
    sigprocmask (SIG_SETMASK, masque_a_reinstaller, NULL);
}

/*
 * Rend non interruptible.
 * On masque tout et on augmente le niveau de non-interruptabilité.
 * Si c'est le premier masquage, on sauve le masque.
 */
void rendre_noninterruptible (void)
{
    sigset_t protect, protectorig;
    sigfillset (&protect);
    sigprocmask (SIG_SETMASK, &protect, &protectorig);
    elu->noninterruptible++;
    if (elu->noninterruptible == 1) {
        /* Première fois qu'on masque. */
        elu->mask = protectorig;
    }
    if (proc_verbose)
      printf ("PROC: noninterruptible %s up to %d\n",
              elu->nom, elu->noninterruptible);
}


/****************************************************************/

/* Vrai si il n'y a aucun processus prêt. */
int proc_suis_je_seul (void)
{
    /* seul si la file des prêts est vide. */
    return (les_prets == NULL);
}

/****************/

/* Initialisation du module: crée le processus principal. */
void proc_init (void)
{
    static int init_done = 0;
    if (! init_done) {
        int i;
        init_done = 1;
        for (i = 0; i < MAX_NB_CLEFS; i++)
          TSD_destructeurs[i] = NULL;
        elu = malloc (sizeof (struct processus));
        elu->nom = strdup ("PROC_MAIN");
        elu->code = NULL;
        elu->arg = NULL;
        elu->cor = cor_creer ("COR_MAIN", NULL, 0);
        elu->status = proc_ELU;
        elu->noninterruptible = 0;
        sigprocmask (SIG_SETMASK, NULL, &(elu->mask));
        elu->debug_suivant = NULL;
        tous_les_processus = elu;
        proc_principal = elu;
    }
}

static void changer_elu (proc_status nouveau_status, processus_t nouvel_elu);
  /* plus loin */

static void affiche_fini (void)
{
    processus_t p;
    int msg_affiche = 0;
    for (p = tous_les_processus; p != NULL; p = p->debug_suivant) {
        if ( (p->status == proc_SUSPENDU) && (p != proc_principal)) {
            if (! msg_affiche) {
                printf ("PROC: Il reste des suspendus:\n");
                msg_affiche = 1;
            }
            printf ("    %s\n", p->nom);
        }
    }
}

/* L'enveloppe d'un processus, pour assurer la terminaison propre.
 * S'il reste des processus prêts, on en choisit un. */
static void enveloppe (void *pt)
{
    processus_t p = (processus_t)pt;
    int i;

    if (proc_verbose)
      printf ("PROC: activating %s\n", p->nom);

    if (proc_creation_hook != NULL)
      (*proc_creation_hook) ();

    (p->code) (p->arg);

    if (proc_verbose)
      printf ("PROC: end enveloppe %s\n", p->nom);

    /* Appel des destructeurs de TSD. */
    for (i = 0; i < MAX_NB_CLEFS; i++) {
        if ((p->tsd[i] != NULL) && (TSD_destructeurs[i] != NULL)) {
            void *val = p->tsd[i];
            p->tsd[i] = NULL;
            (*TSD_destructeurs[i]) (val);
        }
    }

    if (proc_suis_je_seul ()) {
        printf ("PROC: fin du dernier processus, tchao.\n");
        affiche_fini();
        exit (0);
    }
    changer_elu (proc_MORT, sched_choisir_elu());
}

/*
 * Création et activation d'un nouveau processus.
 */
processus_t proc_activer (char *nom, void (*code) (void *arg), void *arg)
{
    char *nom_cor;
    processus_t p;
    int i;

    if (elu == NULL)          /* juste au cas où... */
      proc_init ();

    p = malloc (sizeof (struct processus));
    p->nom = strdup(nom);
    nom_cor = malloc (4 + strlen (nom) + 1);
    sprintf (nom_cor, "COR_%s", nom);
    p->code = code;
    p->arg = arg;
    p->cor = cor_creer (nom_cor, enveloppe, p);
    free (nom_cor);
    p->noninterruptible = 0;
    /* héritage du masque du processus créateur. */
    p->mask = elu->mask;

    for (i = 0; i < MAX_NB_CLEFS; i++)
      p->tsd[i] = NULL;

    p->status = proc_PRET;
    sched_ajouter_pret (p);

    p->debug_suivant = tous_les_processus;
    tous_les_processus = p;

    if (proc_verbose)
      printf ("PROC: activer %s\n", p->nom);

    return p;
}

/* Continue le processus suspendu. */
void proc_continuer (processus_t p)
{
    if (proc_verbose)
      printf ("PROC: continuer %s\n", p->nom);
    assert (p->status == proc_SUSPENDU);
    p->status = proc_PRET;
    sched_ajouter_pret (p);
}

/* Change le processus élu avec celui spécifié.
 * nouveau_status est le nouveau status que prend le processus précédemment
 *  élu et doit être PRET, SUSPENDU ou MORT.
 *
 * changer_elu doit être appelé dans l'état NONINTERRUPTIBLE.
 */
static void changer_elu (proc_status nouveau_status, processus_t nouvel_elu)
{
    processus_t ancien_elu;

    if (nouvel_elu == elu)      /* pas de changement du processus actif */
      return;

    assert (nouvel_elu->status == proc_PRET);
    nouvel_elu->status = proc_ELU;

    ancien_elu = elu;
    assert (ancien_elu->status == proc_ELU);
    ancien_elu->status = nouveau_status;

    if (proc_verbose)
      printf ("PROC: %s devient %s => commuter %s\n", ancien_elu->nom,
              proc_status_nom[nouveau_status], nouvel_elu->nom);

    if (proc_pre_commuter_hook != NULL)
      (*proc_pre_commuter_hook) (nouveau_status);

    /* Attention à bien changer l'élu après le hook,
     * sinon proc_self et proc_nom ne sont pas utilisables dans le hook. */
    elu = nouvel_elu;
    cor_transferer (ancien_elu->cor, nouvel_elu->cor);

    if (proc_post_commuter_hook != NULL)
      (*proc_post_commuter_hook) ();
}

/* Suspend le processus courant. */
void proc_suspendre (void)
{
    if (proc_suis_je_seul()) {
        printf ("PROC: suspendre du dernier processus executable.\n");
        elu->status = proc_SUSPENDU;   /* L'invariant est invalidé, */
        affiche_fini();                /* mais c'est pour l'affichage. */
        exit (1);
    }
    changer_elu (proc_SUSPENDU, sched_choisir_elu ());
}

/* Commutation avec le premier processus prêt. */
void proc_commuter (void)
{
    if (! proc_suis_je_seul()) { /* petite optimisation non nécessaire */
        sched_ajouter_pret (elu);
        changer_elu (proc_PRET, sched_choisir_elu ());
    }
}

/*********************************/

/* Renvoie l'identité du processus appelant. */
processus_t proc_self (void)
{
    return elu;
}

/*****************************************************************/

void proc_lister (processus_t p)
{
    printf ("PROC %s/%p, coroutine %p, etat %s",
            p->nom, p, p->cor, proc_status_nom[p->status]);
    if (p->status == proc_PRET) {
        printf (" suiv = %p, prec = %p", p->suivant, p->precedent);
    }
    printf ("\n");
}

void proc_lister_tous (void)
{
    processus_t p;
    printf ("PROC: lister_tous:\n");
    for (p = tous_les_processus; p != NULL; p = p->debug_suivant) {
        proc_lister (p);
    }
    printf ("---\n");
}

char *proc_nom (processus_t p)
{
    return p->nom;
}

/*****************************************************************/

void (*proc_creation_hook) () = NULL;

void (*proc_pre_commuter_hook) (proc_status ps) = NULL;

void (*proc_post_commuter_hook) () = NULL;


/*****************************************************************/

int TSD_creer_clef (void (*destructeur) (void *))
{
    static int clef = 0;
    int ret;
    rendre_noninterruptible();
    if (clef == MAX_NB_CLEFS) {
        ret = -1;
    } else {
        ret = clef;
        TSD_destructeurs[ret] = destructeur;
        clef++;
    }
    rendre_interruptible();
    return ret;
}

void *TSD_get (int clef)
{
    assert ((clef >= 0) && (clef < MAX_NB_CLEFS));
    return elu->tsd[clef];
}

void *TSD_set (int clef, void *new)
{
    void *old;
    assert ((clef >= 0) && (clef < MAX_NB_CLEFS));
    old = elu->tsd[clef];
    elu->tsd[clef] = new;
    return old;
}

void *TSD_get_other (processus_t p, int clef)
{
    assert ((clef >= 0) && (clef < MAX_NB_CLEFS));
    return p->tsd[clef];
}

void *TSD_set_other (processus_t p, int clef, void *new)
{
    void *old;
    assert ((clef >= 0) && (clef < MAX_NB_CLEFS));
    old = p->tsd[clef];
    p->tsd[clef] = new;
    return old;
}
