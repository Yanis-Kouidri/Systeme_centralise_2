#include "processus.h"
#include "scheduler.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void sched_ajouter_pret (processus_t qui);
processus_t sched_choisir_elu (void);
int sched_nb_prets (void);


void empty(__attribute__((unused)) void *unused)
{
    /* nop */
}

void check(int line, processus_t got, processus_t expected)
{
    if (got == NULL) {
        printf("Error: line %d; got NULL, expected %s.\n", line, proc_nom(expected));
        exit(1);
    } else if (got != expected) {
        printf("Error: line %d: got %s, expected %s.\n", line, proc_nom(got), proc_nom(expected));
        exit(1);
    }
}

int main() {
    sched_verbose = 0;
    sched_set_scheduler (&sched_prio_dynamique);
    processus_t p1 = proc_activer ("P1", empty, NULL);
    processus_t p2 = proc_activer ("P2", empty, NULL);
    processus_t p3 = proc_activer ("P3", empty, NULL);
    processus_t p4 = proc_activer ("P4", empty, NULL);
    processus_t p5 = proc_activer ("P5", empty, NULL);
    
    sched_set_priority_other (p1, 1);
    sched_set_priority_other (p2, 1);
    sched_set_priority_other (p3, 2);
    sched_set_priority_other (p4, 3);
    sched_set_priority_other (p5, 4);

    processus_t p;
    /* files : 1=<p1,p2>, 2=<p3>, 3=<p4>, 4=<p5> */
    p = sched_choisir_elu();
    check(__LINE__, p, p5);
    sched_ajouter_pret(p5);
    
    /* files : 1=<p1,p2>, 2=<p3>, 3=<p4,p5> */
    p = sched_choisir_elu();
    check(__LINE__, p, p4);
    p = sched_choisir_elu();
    check(__LINE__, p, p5);
    sched_ajouter_pret(p5);
    sched_ajouter_pret(p4);
    
    /* files : 1=<p1,p2>, 2=<p3,p5,p4>, 3=<> */
    p = sched_choisir_elu();
    check(__LINE__, p, p3);
    p = sched_choisir_elu();
    check(__LINE__, p, p5);
    p = sched_choisir_elu();
    check(__LINE__, p, p4);
    sched_ajouter_pret(p3);
    sched_ajouter_pret(p4);
    sched_ajouter_pret(p5);

    /* files : 1=<p1,p2,p3,p4,p5>, 2=<> */
    p = sched_choisir_elu();
    check(__LINE__, p, p1);
    sched_ajouter_pret(p);
    p = sched_choisir_elu();
    check(__LINE__, p, p2);
    sched_ajouter_pret(p);
    p = sched_choisir_elu();
    check(__LINE__, p, p3);
    sched_ajouter_pret(p);
    p = sched_choisir_elu();
    check(__LINE__, p, p4);
    sched_ajouter_pret(p);
    p = sched_choisir_elu();
    check(__LINE__, p, p5);
    sched_ajouter_pret(p);
    /* files : 0=<p1,p2,p3,p4,p5>, 2=<> */
    
    /* tout le monde remonte à son max. */
    /* files : 1=<p1,p2>, 2=<p3>, 3=<p4>, 4=<p5> */
    p = sched_choisir_elu();
    check(__LINE__, p, p5);
    sched_ajouter_pret(p5);
    
    /* files : 1=<p1,p2>, 2=<p3>, 3=<p4,p5> */
    p = sched_choisir_elu();
    check(__LINE__, p, p4);
    p = sched_choisir_elu();
    check(__LINE__, p, p5);

    printf("Tout est bon.\n");
}
