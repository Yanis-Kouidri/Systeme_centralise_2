#ifndef _PROCVAR_H_
#define _PROCVAR_H_

typedef struct processus *processus_t;

extern int proc_verbose;

/* Hooks pour la mémoire virtuelle */

/* Un hook appelé après la création du processus, et
 * deux hooks appelés avant le changement de processus et après le changement
 *  de processus.
 * pre_commuter_hook est appelé avec l'état futur du "vieux" processus (celui
 *  qui perd la main)
 * post_commuter_hook est appelé sans argument.
 *
 * En dépit du nom limitatif, les hooks sont appelés à tout changement de
 * processus, ie pour suspendre, commuter et à la mort.
 *
 * Attention: on ne peut pas faire n'importe quoi à l'intérieur des hooks!
 * 1) la couche processus n'est pas réentrante. Il est donc interdit
 *    d'appeler une procédure de la couche processus (même indirectement
 *    par exemple au travers des couches sémaphores ou moniteurs).
 *    Les seules actions possibles vis-à-vis de la couche processus sont:
 *     - appel de TSD_get, TSD_get_other, TSD_set
 *     - appel de proc_nom, proc_self
 *     - changement des hooks.
 *    Dans creation_hook, proc_self est le processus en train d'être créé,
 *    dans pre_commuter_hook, proc_self est celui qui va perdre la main,
 *    dans post_commuter_hook, proc_self est celui qui a obtenu la main.
 *    (idem pour TSD_get, proc_nom, etc...)
 * 2) Avec les couches pprocessus (processus préemptifs), sémaphores ou
 *    moniteurs, les hooks sont exécutés dans l'état NON INTERRUPTIBLE.
 *    Attention à ne pas bloquer le processus!
 *
 * Il n'y a pas de proc_terminaison_hook: pre_commuter_hook(proc_MORT) est
 *  là pour ça.
 *
 * Se souvenir qu'une info privée non initialisée vaut NULL.
 */

/* État d'un processus.
 * (pre_commuter_hook ne peut jamais recevoir ELU, ou c'est très louche) */
typedef enum { proc_PRET, proc_ELU, proc_SUSPENDU, proc_MORT } proc_status;

/* Les noms correspondants (pour debugger). */
extern char *proc_status_nom[];

extern void (*proc_creation_hook) (void);
extern void (*proc_pre_commuter_hook) (proc_status ps);
extern void (*proc_post_commuter_hook) (void);


#endif
