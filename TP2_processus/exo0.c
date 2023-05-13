/* exo0 processus: exemple 2 du cours */
   
#include "processus.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"

processus_t pmain, p1, p2;
int compteur;

void code1(void *unused) {
    for (int i = 0; i < 1000; i++) {
        printf("P1 : %d\n", i);
        compteur++;
        proc_continuer(p2);
        proc_suspendre();
    }
}

void code2(void *unused) {
    proc_suspendre();
    while (compteur < 10) {
        printf("P2 : %d\n", compteur);
        proc_continuer(p1);
        proc_suspendre();
    }
    proc_continuer(pmain); /* redonne la main à main */
}

int main() {
    proc_init();
    pmain = proc_self();
    p2 = proc_activer("P2", code2, NULL);
    p1 = proc_activer("P1", code1, NULL);
    proc_suspendre();
    return 0; /* terminaison du système */
}
