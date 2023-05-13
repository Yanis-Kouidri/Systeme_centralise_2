/* Processus synchronisés avec une file de messages. */

/* - Crée NBCONSOMMATEURS processus qui bouclent en extrayant un message (take).
 * - Crée NBPRODUCTEURS processus qui déposent chacun NBMSGPARPROD messages (put).
 * - Crée un processus de vérification qui observe l'état du système quand tous
 *   les autres sont bloqués ou terminés.
 *
 * Quand plus aucun producteur et consommateur n'est actif, le processus de
 * vérification vérifie que le bon nombre de messages a été produit et que
 * tous ont été consommés.
 */

#include "processus.h"
#include "scheduler.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*****************/

#define NBPRODUCTEURS 3
#define NBCONSOMMATEURS 6
#define NBMSGPARPROD 4

/****************************************************************/

/* La file des messages et la file des consommateurs en attente. */
queue_t msgs = NULL;
queue_t readers = NULL;

/* Ajoute un msg, en débloquant un éventuel consommateur. */
void put(char *m) {
    /* ajouter m à msgs.
     * si readers est non vide, extraire un processus et le continuer.
     */
    /* XXXX TODO */
}

/* Extrait un message, en bloquant si c'est vide. */
char *take(void) {
    /* tant que msgs est vide, ajouter proc_self() à readers et se suspendre.
     * extraire un message de la file.
     */
    /* XXXX TODO */
}

/****************************************************************/

/* Variables de vérification : comptent les messages produits et consommés. */
int nbprod = 0;
int nbcons = 0;

/* Produit NBMSGPARPROD messages puis se termine. */
void producteur(void *num) {
    int myid = *(int *)num;
    int i;
    for (i = 0; i < NBMSGPARPROD; i++) {
        char msg[10];
        sprintf(msg, "msg %d/%d", myid, i);
        printf ("Prod %d : put %s...\n", myid, msg);
        put(strdup(msg));
        nbprod++;
        printf ("Prod %d : put %s... done\n", myid, msg);
        proc_commuter();
    }
}

/* Boucle en extrayant un message et l'affichant */
void consommateur(void *num) {
    int myid = *(int *)num;
    while (1) {
        printf ("Cons %d : take...\n", myid);
        char *msg = take();
        nbcons++;
        printf ("Cons %d : take... -> %s\n", myid, msg);
        proc_commuter();
    }
}

/* Processus de vérification, quand tous les autres sont bloqués ou terminés. */
#pragma GCC diagnostic ignored "-Wunused-parameter"
void checker(void *unused) {
    while (1) {
        if (proc_suis_je_seul()) {
            if (nbprod != NBPRODUCTEURS * NBMSGPARPROD) {
                printf("ECHEC : il n'y a pas eu %d messages produits comme attendu, mais %d messages.\n", NBPRODUCTEURS * NBMSGPARPROD, nbprod);
            } else if (nbcons == nbprod) {
                printf("Parfait, les %d messages produits ont été consommés.\n", nbprod);
            } else if (nbprod > nbcons) {
                printf("ECHEC : il reste %d messages non consommés.\n", nbprod - nbcons);
            } else { // nbprod < nbcons
                printf("ECHEC : %d messages de trop ont été consommés !\n", nbcons - nbprod);
            }
            exit(0);
        }
        proc_commuter();
    }
}

/* Créé les NBPRODUCTEURS producteurs, NBCONSOMMATEURS consommateurs, et le processsus de vérification.
 * Utilise un ordonnanceur aléatoire pour que l'enchaînement des productions et consommations soient arbitraires.
 */
int main ()
{
    sched_set_scheduler (&sched_aleatoire);
    proc_init ();
    for (int i = 0; i < NBPRODUCTEURS; i++) {
        int *mi = malloc(sizeof(int));
        *mi = i;
        proc_activer ("prod", producteur, mi);
    }
    for (int i = 0; i < NBCONSOMMATEURS; i++) {
        int *mi = malloc(sizeof(int));
        *mi = i;
        proc_activer ("cons", consommateur, mi);
    }
    proc_activer ("checker", checker, NULL);
    proc_suspendre ();
    return 0;
}
