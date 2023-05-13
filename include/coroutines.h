#ifndef _COROUTINES_H_
#define _COROUTINES_H_

typedef struct coroutine *coroutine_t;

extern int cor_verbose;

coroutine_t cor_creer (char *nom, void (*code) (void *arg), void *arg);

void cor_transferer (coroutine_t suspendue, coroutine_t active);

/* Récuperation des ressources associées à une coroutines.
 * Une coroutine ne peut pas de détruire elle-même.
 * Il faut nécessairement que ce soit une autre coroutine (un autre processus)
 * qui appelle cor_detruire. En effet, la pile de la coroutine est détruite.
 */
void cor_detruire (coroutine_t);

#endif
