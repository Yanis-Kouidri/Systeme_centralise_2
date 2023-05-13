/* Thread-Specific Data.
 * Implanté dans processus.c */

#ifndef _TSD_H_
#define _TSD_H_

#define MAX_NB_CLEFS 20

/* Crée une nouvelle clef. Renvoie -1 en cas d'erreur.
 * Le nombre de clef est limité à MAX_NB_CLEFS.
 * Si destructeur est non-NULL, il spécifie le destructeur pour les valeurs
 * associées à la clef créée. Quand un processus se termine, le destructeur
 * correspondant à chaque donnée spécifique non-NULL est appelée.
 * L'ordre d'appel des destructeurs n'est pas spécifié.
 */
int TSD_creer_clef (void (*destructeur) (void *));

/* Obtenir et  positionner l'information privée associée à une clef, pour le processus courant.
 * TSD_set renvoie l'ancienne information.
 * Initialement, les infos privées sont NULL. */
void *TSD_get (int clef);
void *TSD_set (int clef, void *);

/* TSD_get_other et TSD_set_other sont décrits dans processus.h
 * (dépendance vers processus_t)
 */

#endif
