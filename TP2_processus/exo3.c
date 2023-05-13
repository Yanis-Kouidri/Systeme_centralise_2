
/* ping/pong processus avec TSD. */
   
#include "TSD.h"
#include "processus.h"
#include "scheduler.h"
#include <stdio.h>

#define N 10

/* Ne pas changer l'interface de la procédure boucle.
 * Utiliser une donnée spécifique (TSD) pour obtenir le texte à afficher. */
static void boucle (void)
{
    int i;
    for (i = 0; i < N; i++) {
        printf ("%s\n", /* XXXX ce qu'il faut ici */);
        /* XXXX et ici aussi. */
    }
    printf ("**** done\n");
}

/* XXXX */

int main ()
{
    proc_init ();
    printf ("Debut\n");
    
    printf ("Fin\n");
    return 0;
}
