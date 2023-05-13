/* Configuration selon le système d'exploitation. */

/****************************************************************/

/* Coroutines. */

/* Comment spécifie-t-on la pile pour un contexte :
 * - va-t-elle vers les adresses croissantes (UP=1) ou décroissantes (UP=0)
 * - faut-il fixer ss_sp à l'adresse haute de la zone mémoire (TOP=1)
 *    ou à l'adresse basse de la zone (TOP=0)
 * Sachant que l'on donne aussi la taille (ss_size), les deux sont
 * indépendants.
 *
 * Sur Solaris/sparc : UP=0, TOP=1
 * Sur Linux/i386    : UP=0, TOP=0
 */
#define CTX_STACK_GOES_UP 0
#define CTX_STACK_SP_TOP  0

/****************************************************************/
