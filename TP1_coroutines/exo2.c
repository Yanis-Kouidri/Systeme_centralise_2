
/* Distributeur de travaux, avec simulation de temps partagé.
 * Chaque travail nécessite trois étapes; À chaque étape, un travail
 * affiche son numéro ainsi que le numéro d'étape; il redonne la main
 * au distributeur à chaque étape.
 *
 * Résultat attendu: doit afficher 3 fois chaque nombre 1 à N
 * dans le désordre et terminer par "fini."
 *
 *
 * 2) On souhaite réaliser un distributeur de travaux. Chaque travail (supposé
 *    long) est décomposé en trois parties. Entre chaque partie, le travail
 *    doit redonner la main au distributeur pour donner une chance aux autres
 *    travaux de progresser.
 *    (squelette disponible dans exo2.c)
 *
 */

#include "coroutines.h"
#include <stdio.h>
#include <unistd.h>             /* for getpid */
#include <stdlib.h>

#define N 10

/* XXXX Une coroutine par travail, plus le distributeur et le prog principal */
coroutine_t tab_co[N];
coroutine_t dist, cm;

/* Vrai si un travail est terminé. */
static int fini[N];

void travail (void *pi)
{
    int i;
    i = *(int *)pi;
    printf ("Travail %d (step 1)\n", i);
    /* XXXX rendre la main au distributeur */
    cor_transferer(tab_co[i], dist);

    printf ("Travail %d (step 2)\n", i);
    /* XXXX rendre la main au distributeur */
    cor_transferer(tab_co[i], dist);

    printf ("Travail %d (step 3)\n", i);
    fini[i] = 1; /* enregistre que le travail est fini. */
    /* XXXX rendre la main au distributeur */
    cor_transferer(tab_co[i], dist);
}

/* Vrai si tous les travaux sont terminés. */
int tous_finis (void)
{
    int i;
    for (i = 0; i < N; i++) {
        if (! fini[i])
          return 0;
    }
    return 1;
}

void distribuer (void *unused)
{
    while (! tous_finis()) {
        int n;
        /* Choisit un travail non terminé. */
        do {
            n = random() % N;
        } while (fini[n]);
        /* XXXX Donne la main à ce travail. */
        cor_transferer(dist, tab_co[n]);
    }
    /* XXXX Quand tous les travaux sont finis, rend la main au prog. princ. */
    cor_transferer(dist, cm);
}


int main()
{
    int i;
    srandom (getpid()); /* init du générateur de nombres aléatoires */
    /* XXXX Crée le distributeur de travail. */
    dist = cor_creer("Cdist", distribuer, NULL);
    cm = cor_creer("CM", NULL, NULL);
    

    /* XXXX Crée les travaux */
    for (i = 0; i < N; i++) {
        int *m = malloc (sizeof (int));
        *m = i;
        fini[i] = 0;
        tab_co[i] = cor_creer("C", travail, m );
    }
    /* XXXX Prog principal. */
    cor_transferer(cm, dist);

    /* XXXX Donne la main au distributeur. */
    printf ("Fini.\n");
    return 0;
}

