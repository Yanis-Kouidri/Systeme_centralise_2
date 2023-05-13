
/* Distributeur de travaux, avec simulation de temps partagé.
 * Chaque travail nécessite trois étapes; À chaque étape, un travail
 * affiche son numéro ainsi que le numéro d'étape; il redonne la main
 * au distributeur à chaque étape.
 *
 * Résultat attendu: doit afficher 3 fois chaque nombre 1 à N
 * dans le désordre et terminer par "fini."
 */
#include "coroutines.h"
#include <stdio.h>
#include <unistd.h>             /* for getpid */
#include <stdlib.h>

#define N 10

/* XXXX Une coroutine par travail, plus le distributeur et le prog principal */

/* Vrai si un travail est terminé. */
static int fini[N];

void travail (void *pi)
{
    int i;
    i = *(int *)pi;
    printf ("Travail %d (step 1)\n", i);
    /* XXXX rendre la main au distributeur */
    printf ("Travail %d (step 2)\n", i);
    /* XXXX rendre la main au distributeur */
    printf ("Travail %d (step 3)\n", i);
    fini[i] = 1; /* enregistre que le travail est fini. */
    /* XXXX rendre la main au distributeur */
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
    }
    /* XXXX Quand tous les travaux sont finis, rend la main au prog. princ. */
}


int main()
{
    int i;
    srandom (getpid()); /* init du générateur de nombres aléatoires */
    /* XXXX Crée le distributeur de travail. */
    /* XXXX Crée les travaux */
    for (i = 0; i < N; i++) {
        int *m = malloc (sizeof (int));
        *m = i;
        fini[i] = 0;
    }
    /* XXXX Prog principal. */
    /* XXXX Donne la main au distributeur. */
    printf ("Fini.\n");
    return 0;
}
