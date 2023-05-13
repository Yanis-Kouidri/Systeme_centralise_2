/* Test de mémoire virtuelle, en utilisant la couche processus.
   Il ne doit pas y avoir d'autres messages que les traces.
 */

#include "processus.h"
#include "sleep.h"
#include "memvirt.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

/* Pour éviter swap exhausted, il faut avoir
   NBPROC*NBPAGES_VIRT <= NBPAGES_SWAP */
#define NBPAGES_VIRT 10
#define NBPAGES_SWAP 100
#define NBPROC 10
#define NBITER 50

void code (void *arg)
{
    int moi = *(int *)arg;
    int i;
    int p;
    int pagesaccedees[NBITER];
    char *base = (char*)MV_getbase();
    int pagesize = MV_getpagesize();

    for (i = 0; i < NBITER; i++) {
        int val;
        p = random() % NBPAGES_VIRT;
        pagesaccedees[i] = p;
        val = base[p*pagesize + p];
        if ((val != 0) && (val != moi)) {
            printf ("**** PANIC: %s: page %d n'est pas a moi mais a %d!\n",
                    proc_nom (proc_self()), p, val);
            exit (0);
        }
        base[p*pagesize + p] = moi;
        if (random() % 2)
          proc_msleep (100);
    }
    /* Avant de mourir vérifions que tout est bon. */
    for (i = 0; i < NBITER; i++) {
        p = pagesaccedees[i];
        if (base[p*pagesize + p] != moi) {
            printf ("**** PANIC: %s: page %d n'est pas a moi!\n",
                    proc_nom (proc_self()), p);
        }
    }
    printf ("Fin de %s. defauts/ecriture/lecture: %d/%d/%d\n",
            proc_nom (proc_self()),
            MV_nb_defauts, MV_nb_ecritures, MV_nb_lectures);
}

int main ()
{
    int i;
    char nom[50];
    proc_verbose = 0;
    MV_verbose = 1;
    srandom (getpid());
    proc_init ();
    MV_init (NBPAGES_VIRT, NBPAGES_SWAP);
    for (i = 1; i <= NBPROC; i++) {
        int *pi = malloc (sizeof(int));
        sprintf (nom, "PRO%d", i);
        *pi = i;
        proc_activer (nom, code, pi);
    }
    proc_suspendre ();
    return 3;
}
