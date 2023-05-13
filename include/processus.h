#ifndef _PROCESSUS_H_
#define _PROCESSUS_H_

#include "procvar.h"

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

/* Suicide du processus. */
void proc_exit (void);

/* Vrai si il n'y a aucun processus prêt. */
int proc_suis_je_seul (void);

/* Renvoie l'identité du processus appelant. */
processus_t proc_self (void);

void rendre_interruptible (void);
void rendre_noninterruptible (void);

/*****************************************************************/

/* Obtenir et  positionner l'information privée associée à une clef, pour le processus spécifié.
 * CE N'EST PAS UNE INTERFACE PUBLIQUE mais réservée à l'implantation du noyau.
 * La memoire virtuelle et le scheduler a besoin d'obtenir les infos
 * privées d'un autre processus.
 */
void *TSD_get_other (processus_t p, int clef);
void *TSD_set_other (processus_t p, int clef, void *new);

/*****************************************************************/
/* Pour debugger */
void proc_lister (processus_t p);
void proc_lister_tous (void);
char *proc_nom (processus_t p);

#endif
