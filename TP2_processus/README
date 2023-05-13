                              TP2: processus
                              ==============

Noyau et compilation
--------------------

Cf Makefile

L'interface processus.h est dans le répertoire include et ci-dessous en fin du fichier.

Exercices
---------

0) exo0.c est l'exemple 2 du cours.

1) Écrire un programme réalisant dix alternances ping/pong à base de
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

2) Écrire un programme réalisant dix alternances ping/pong à base de
   continuer/suspendre. Il est interdit d'utiliser commuter.
   A-t-on encore besoin de l'hypothèse précédente ?
   Donner une solution qui ne dépend aucunement de la stratégie
   d'ordonnancement.
   (squelette disponible dans exo2.c)

3) Écrire un programme réalisant dix alternances ping/pong à base de
   commuter, en utilisant une donnée spécifique (TSD).
   [ TSD_set_other est interdit, c'est un hack pour le TP4 ]
   (squelette disponible dans exo3.c)

4) Fournir une implantation des files de messages bloquantes
   où un consommateur se bloque si la file est vide.
   (squelette disponible dans exo4.c, et queue.h pour une implantation de queues)
   Question bonus : comprendre ce qu'il se passe si on remplace la
   boucle "tant que" par un "if" (dans take).

----------------------------------------------------------------

L'interface processus.h (simplifiée)
------------------------------------

typedef struct processus *processus_t;

extern int proc_verbose;

/* Initialisation du module: crée le processus principal. */
void proc_init (void);

/* Création et activation d'un nouveau processus. */
processus_t proc_activer (char *nom, void (*code) (void *arg), void *arg);

/* Continue le processus suspendu. */
void proc_continuer (processus_t p);

/* Suspend le processus courant. */
void proc_suspendre (void);

/* Commutation avec le premier processus prêt. */
void proc_commuter (void);

/* Comme son nom l'indique... */
processus_t proc_self (void);

void rendre_interruptible (void);
void rendre_noninterruptible (void);

/*****************************************************************/
/* Pour debugger */
void proc_lister (processus_t p);
void proc_lister_tous (void);
char *proc_nom (processus_t p);

/****************************************************************/

Note : proc_suis_je_seul(), présent dans processus.h, ne doit jamais être
utilisé.

