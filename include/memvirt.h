#ifndef _MEMVIRT_H_
#define _MEMVIRT_H_

/* Mémoire virtuelle par processus.
 */

/* Taille d'une page de la mémoire virtuelle. */
int MV_getpagesize();

/* Base de la mémoire centrale/virtuelle (alignée sur une frontière de page).*/
/* MV_getbase() renvoie tjs la même valeur après l'initialisation:
   stable (MV_getbase() == k) && (k != NULL) */
void *MV_getbase();

/* Initialise base avec une mémoire centrale/virtuelle de nbpages_centr pages
 * et un swap de nbpages_swap.
 * La taille d'une page est PAGESIZE définie dans <sys/param.h> */
void MV_init (int nbpages_centr, int nbpages_swap);

/* Pour les traces */
extern int MV_verbose;

/* Descripteur sur lequel les traces sont écrites. */
extern int MV_verbose_fd;

/* Statistiques.
 * MV_nb_defauts compte le nombre de fois où le noyau a dû prendre des
 *  mesures pour rendre une page valide. Cela n'entraine pas nécessairement
 *  une lecture ou une écriture, la page pouvant être correcte.
 * MV_nb_ecritures compte le nombre effectif d'éjection (écriture dans le
 *  swap).
 * MV_nb_lectures compte le nombre effectif de chargement (lecture depuis
 *  le swap).
 */
extern int MV_nb_defauts;
extern int MV_nb_ecritures;
extern int MV_nb_lectures;

/* Et il n'y a rien d'autre ! */

/* Pour debugger.
 * Indique si il faut dumper les infos de la mémoire centrale et/ou
 * de la mémoire de swap. */
void MV_dump (int centrale, int swap);

#endif
