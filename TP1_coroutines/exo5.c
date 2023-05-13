/* Jeu stupide.
 * 4 joueurs: bleu, vert, jaune, rouge.
 * Quand bleu à la main, il la donne à vert, jaune ou rouge (aléatoirement).
 * Quand vert à la main, il la donne à bleu ou rouge.
 * Quand rouge à la main, il la donne à vert ou bleu.
 * Quand jaune à la main, il la donne à vert ou retourne au programme principal qui termine.
 */
   
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* renvoie une valeur entre 1 et max (inclus) */
int jeter_des(int max) {
    return 1 + (random() % max);
}

void joueur_bleu (void *unused)
{
    int coup = 1;
    while (1) {
        printf("bleu %d\n", coup++);
        int des = jeter_des(3);
        /* ... */
    }
}

void joueur_vert (void *unused)
{
    int coup = 1;
    while (1) {
        printf("vert %d\n", coup++);
        int des = jeter_des(2);
        /* ... */
    }
}

void joueur_rouge (void *unused)
{
    int coup = 1;
    while (1) {
        printf("rouge %d\n", coup++);
        int des = jeter_des(2);
        /* ... */
    }
}

void joueur_jaune (void *unused)
{
    int coup = 1;
    while (1) {
        printf("jaune %d\n", coup++);
        int des = jeter_des(2);
        /* ... */
    }
}

int main()
{
    srandom(getpid());
    /* ... */
    printf ("Debut du jeu\n");
    /* ... */
    printf ("Fin du jeu\n");
    return 0;
}
