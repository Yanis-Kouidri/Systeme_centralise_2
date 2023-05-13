#ifndef _PPROCESSUS_H_
#define _PPROCESSUS_H_

/* Couche de processus préemptifs.
 * Peut être utilisée en remplacement de processus, mais il faut éviter de
 * l'utiliser pour construire des couches supérieures.
 *
 * Cette couche se déguise en couche processus. Il suffit donc de remplacer
 * "processus.h" par "pprocessus.h" pour avoir des processus préemptifs.
 *
 * Ne jamais inclure à la fois pprocessus.h et processus.h
 * Ne jamais avoir à la fois des appels à pprocessus et à processus.
 */

#include "procvar.h"

void pproc_init (void);
processus_t pproc_activer (char *nom, void (*code) (void *arg), void *arg);
void pproc_continuer (processus_t p);
void pproc_suspendre (void);
void pproc_commuter (void);
void pproc_exit (void);
int pproc_suis_je_seul (void);
processus_t pproc_self (void);

void rendre_interruptible (void);
void rendre_noninterruptible (void);

void *TSD_get_other (processus_t p, int clef);

/* Pour ceux qui veulent utiliser le préfixe pproc. */
#define pproc_verbose proc_verbose
#define pproc_status proc_status
#define pproc_status_nom proc_status_nom
#define pproc_pre_commuter_hook proc_pre_commuter_hook
#define pproc_post_commuter_hook proc_post_commuter_hook


/*****************************************************************/
void pproc_lister (processus_t p);
void pproc_lister_tous (void);
char *pproc_nom (processus_t p);

/*****************************************************************/

#ifndef NOYAU

/* Déguisement en couche processus. */

#define proc_init pproc_init
#define proc_activer pproc_activer
#define proc_continuer pproc_continuer
#define proc_suspendre pproc_suspendre
#define proc_commuter pproc_commuter
#define proc_exit pproc_exit
#define proc_suis_je_seul pproc_suis_je_seul
#define proc_self pproc_self

#define proc_lister pproc_lister
#define proc_lister_tous pproc_lister_tous
#define proc_nom pproc_nom

#endif /* NOYAU */

#endif
