/* Test de mémoire virtuelle, en utilisant la couche processus.
 * Deux pages, deux processus :
 *  P1 écrit un message dans chacune de ses pages,
 *  P2 prend la main et écrit un message dans chacune de ses pages (donc à la
 *     même adresse virtuelle que pour P1)
 *  P1 reprend la main et vérifie qu'il y a ses messages
 *  P2 reprend la main et vérifie qu'il y a ses messages
 */

#include "processus.h"
#include "memvirt.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Pour éviter swap exhausted, il faut avoir
 *  NBPROC*NBPAGES_VIRT <= NBPAGES_SWAP.
 * Ici NBPROC = 2 (plus le pgm principal qui n'accède pas à la mémoire virtuelle. */
#define NBPAGES_VIRT 2
#define NBPAGES_SWAP 4

static int pagesize;
static char *base;

char *msg_p1[NBPAGES_VIRT] = {
    "coucou, ceci est le message de P1 dans la page 0",
    "ceci est le message de P1 dans la page 1"
};
char *msg_p2[NBPAGES_VIRT] = {
    "ah ah ah ah",
    "je tente d'écraser la page de P1"
};

static void afficher_erreur(int page, char *quelproc, char *lautreproc, char **msg_moi, char **msg_lautre)
{
    int i;
    for (i = 0; i < NBPAGES_VIRT; i++) {
        if (strcmp(&(base[page]), msg_lautre[i]) == 0) {
            printf("%s, page %d : je trouve le contenu de la page %d de %s !\n", quelproc, page, i, lautreproc);
            return;
        }
    }
    for (i = 0; i < NBPAGES_VIRT; i++) {
        if (strcmp(&(base[page]), msg_moi[i]) == 0) {
            printf("%s, page %d : je trouve le contenu de ma page %d à la place !\n", quelproc, page, i);
            return;
        }
    }
    printf("%s, page %d : je ne retrouve pas mon contenu de la page !\n", quelproc, page);
}

void code1 (void *unused)
{
    /* (2) */
    int ok = 1;

    printf("P1 : j'écris ma page 0.\n");
    strcpy(&(base[0]), msg_p1[0]);
    printf("P1 : j'écris ma page 1.\n");
    strcpy(&(base[pagesize]), msg_p1[1]);
    proc_commuter(); /* (3) P2 prend la main */
    /* (5) */
    if (strcmp(&(base[0]), msg_p1[0]) != 0) {
        afficher_erreur(0, "P1", "P2", msg_p1, msg_p2);
        ok = 0;
    }
    if (strcmp(&(base[pagesize]), msg_p1[1]) != 0) {
        afficher_erreur(1, "P1", "P2", msg_p1, msg_p2);
        ok = 0;
    }
    if (ok) {
        printf("P1 : ma mémoire virtuelle fonctionne correctement !\n");
    }
    proc_suspendre(); /* (6) P2 reprend la main */
}

void code2 (void *unused)
{
    /* (4) */
    int ok = 1;

    printf("P2 : j'écris ma page 0.\n");
    strcpy(&(base[0]), msg_p2[0]);
    printf("P2 : j'écris ma page 1.\n");
    strcpy(&(base[pagesize]), msg_p2[1]);
    proc_commuter(); /* (5) P1 reprend la main */
    /* (6) */
    if (strcmp(&(base[0]), msg_p2[0]) != 0) {
        afficher_erreur(0, "P2", "P1", msg_p2, msg_p1);
        ok = 0;
    }
    if (strcmp(&(base[pagesize]), msg_p2[1]) != 0) {
        afficher_erreur(1, "P2", "P1", msg_p2, msg_p1);
        ok = 0;
    }
    if (ok) {
        printf("P2 : ma mémoire virtuelle fonctionne correctement !\n");
        exit (0); /* pouf */
    }
    /* NOTREACHED */
    proc_suspendre();
}

int main ()
{
    /* MV_verbose = 1;*/
    sched_set_scheduler (&sched_fifo); /* important */
    proc_init ();
    MV_init (NBPAGES_VIRT, NBPAGES_SWAP);
    base = (char*)MV_getbase();
    pagesize = MV_getpagesize();
    
    proc_activer ("P1", code1, NULL);
    proc_activer ("P2", code2, NULL);
    proc_suspendre (); /* (1) P1 prend la main */
    return 3;
}
