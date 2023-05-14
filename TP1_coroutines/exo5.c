/* 
 * 5) Un jeu stupide, avec 4 joueurs: bleu, vert, jaune, rouge.
   Quand bleu à la main, il la donne à vert, jaune ou rouge (aléatoirement).
   Quand vert à la main, il la donne à bleu ou rouge.
   Quand rouge à la main, il la donne à vert ou bleu.
   Quand jaune à la main, il la donne à vert ou retourne au programme principal qui termine.
   On démarre par l'un quelconque des joueurs.
 */
   
#include "coroutines.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

#define NB_JOUEUR 4

coroutine_t cb, cv, cr, cj, cm;

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
        switch (des)
        {
        case 1:
            cor_transferer(cb, cv);
            break;
        
        case 2:
            cor_transferer(cb, cj);
            break;
        
        case 3:
            cor_transferer(cb, cr);
            break;
        
        default:
            printf(" Erreur de selection du prochain joueur");
            break;
        }
        /* ... */
    }
}

void joueur_vert (void *unused)
{
    int coup = 1;
    while (1) {
        printf("vert %d\n", coup++);
        int des = jeter_des(2);
        switch (des)
        {
        case 1:
            cor_transferer(cv, cb);
            break;
        
        case 2:
            cor_transferer(cv, cr);
            break;
        
        default:
            printf(" Erreur de selection du prochain joueur");
            break;
        }
        /* ... */
    }
}

void joueur_rouge (void *unused)
{
    int coup = 1;
    while (1) {
        printf("rouge %d\n", coup++);
        int des = jeter_des(2);

        switch (des)
        {
        case 1:
            cor_transferer(cr, cv);
            break;
        
        case 2:
            cor_transferer(cr, cb);
            break;
        
        default:
            printf(" Erreur de selection du prochain joueur");
            break;
        }
        /* ... */
    }
}

void joueur_jaune (void *unused)
{
    int coup = 1;
    while (1) {
        printf("jaune %d\n", coup++);
        int des = jeter_des(2);

        switch (des)
        {
        case 1:
            cor_transferer(cj, cv);
            break;
        
        case 2:
            cor_transferer(cj, cm);
            break;
        
        default:
            printf(" Erreur de selection du prochain joueur");
            break;
        }
        /* ... */
    }
}

int main()
{
    cb = cor_creer("CB", joueur_bleu, NULL);
    cv = cor_creer("CV", joueur_vert, NULL);
    cr = cor_creer("CR", joueur_rouge, NULL);
    cj = cor_creer("CJ", joueur_jaune, NULL);
    cm = cor_creer("CM", NULL, NULL);
    srandom(getpid());

    int premier_joueur = jeter_des(NB_JOUEUR);
    /* ... */
    printf ("Debut du jeu\n");

    switch (premier_joueur)
    {
    case 1:
        cor_transferer(cm, cb);
        break;
    case 2:
        cor_transferer(cm, cv);
        break;
    case 3:
        cor_transferer(cm, cr);
        break;
    case 4:
        cor_transferer(cm, cj);
        break;
    
    default:
        printf(" Erreur de selection du premier joueur");
        break;
    }
    /* ... */
    printf ("Fin du jeu\n");
    return 0;
}
