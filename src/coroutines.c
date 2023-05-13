#include "config.h"
#include "coroutines.h"
#include <ucontext.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

int cor_verbose = 0;

#ifndef PAGESIZE
#  define PAGESIZE (sysconf(_SC_PAGESIZE))
#endif

/* Rq: une page est utilisée comme "red zone", pour éviter les débordements de
 * la pile. */
#define COR_STACKPAGES 10
#define COR_STACKSIZE (COR_STACKPAGES * PAGESIZE)

struct coroutine {
    char *nom;
    void (*code) (void *);
    void *arg;
    ucontext_t uc;
    void *stack;
};

static void enveloppe (void *cv)
{
    coroutine_t c = (coroutine_t)cv;
    if (cor_verbose)
      printf ("COR: activating %s/%p\n", c->nom, &(c->uc));
    (c->code) (c->arg);
    if (cor_verbose)
      printf ("COR: end enveloppe %s/%p\n", c->nom, &(c->uc));
    fprintf (stderr, "COROUTINES: fin sale.\n");
}

/*
 * Quelques pièges:
 *  - le contexte construit par makecontext doit déjà être initialisé
 *    par getcontext (cf man makecontext)
 *  - la signification de uc_stack.ss_sp est confuse :
 *    - sous solaris, la pile croît vers le bas, mais il faut mettre dans
 *      uc_stack.ss_sp l'adresse de sommet de pile (cf man
 *      sigaltstack, mais ce n'est vraiment pas clair, surtout que
 *      pour sigaltstack il semble falloir donner la base de la pile).
 *    - sous Linux/i386, la pile croît vers le bas et il faut mettre l'adresse
 *      de base de la zone mémoire.
 *  - bien faire attention à aligner la pile sur une frontière de page.
 *  - pour les coroutines, c'est un erreur d'atteindre la fin du context.
 *    Donc uc_link == 0 (cf man ucontext).
 *  - si le code est NULL, c'est qu'on ne fera pas de transfert vers cette
 *    coroutine (elle ne pourra servir que de réceptacle), il est donc inutile de
 *    lui allouer une pile. Tant pis si l'utilisateur fait un transfert vers lui.
 *
 * XXXX plutôt que d'allouer la pile avec valloc, pourquoi ne pas plutôt
 * faire (zerofd = open ("/dev/zero", O_RDWR))
 *   mmap (NULL, COR_STACKSIZE, PROT_READ|PROT_WRITE,
 *         MAP_PRIVATE | MAP_NORESERVE, zerofd, 0);
 * La MAP_NORESERVE évite d'allouer physiquement la mémoire tant que ce n'est
 * pas nécessaire.
 */
coroutine_t cor_creer (char *nom, void (*code) (void *arg), void *arg)
{
    coroutine_t t;
    int err;
    t = malloc (sizeof (struct coroutine));
    if (cor_verbose)
      printf ("COR: creating %s/%p\n", nom, &(t->uc));
    t->nom = strdup (nom);
    t->code = code;
    t->arg = arg;
    t->stack = (void*)4; /* une adresse non NULL et non valloc */
    err = getcontext (&(t->uc));
    if (err == -1) {
        perror ("PANIC: COR: getcontext");
        exit (1);
    }
    if ((t == NULL) || (t->stack != (void*)4)) {
        /* panique: quelqu'un a fait transfert vers ce contexte alors qu'il
         * n'était pas prévu pour : pas de code, pas de pile.
         * Note : pas de pile => la variable locale t n'existe sans doute pas.
         */
        write(2, "COROUTINES: PANIQUE: transfert vers coroutine vide.\n", 52);
        exit(1);
    }
    if (code == NULL) {
        t->stack = NULL; /* pas la peine de mettre une pile inutile. */
    } else {
        t->stack = valloc (COR_STACKSIZE);
        if (t->stack == NULL) {
            perror ("PANIC: COR: valloc failed");
            exit (1);
        }
        t->uc.uc_link = 0; /* process will exit when this context returns */
        t->uc.uc_stack.ss_size = COR_STACKSIZE;
        t->uc.uc_stack.ss_flags = 0;
        /* Solaris et Linux ne sont pas d'accord... */
#if CTX_STACK_SP_TOP
        t->uc.uc_stack.ss_sp = (char *)t->stack + COR_STACKSIZE;
#else
        t->uc.uc_stack.ss_sp = (char *)t->stack;
#endif
        /* redzone: protect last page of stack */
#if CTX_STACK_GOES_UP
        mprotect (t->stack + COR_STACKSIZE - PAGESIZE, PAGESIZE, PROT_NONE);
#else
        mprotect (t->stack, PAGESIZE, PROT_NONE);
#endif
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
        makecontext (&(t->uc), enveloppe, 1, t);
    }
    return t;
}


void cor_transferer (coroutine_t suspendue, coroutine_t active)
{
    ucontext_t uc;
    int err;
    if (cor_verbose)
      printf ("COR: transfer %s/%p -> %s/%p\n", suspendue->nom, &(suspendue->uc),
            active->nom, &(active->uc));
    uc = active->uc;
    err = swapcontext (&(suspendue->uc), &uc);
    if (err == -1)
      perror ("PANIC: COR: swapcontext");
    if (cor_verbose)
      printf ("COR: transfer back from %p -> %p\n", &(suspendue->uc), &(active->uc));
}

/* Une coroutine ne peut pas se détruire elle-même.
 * Il faut nécessairement que ce soit une autre coroutine (un autre processus)
 * qui appelle cor_detruire. En effet, la pile de la coroutine est détruite.
 * Il vaut mieux ne plus essayer de revenir à la coroutine détruite ! */
void cor_detruire (coroutine_t c)
{
    free (c->nom);
#if CTX_STACK_GOES_UP
        mprotect (c->stack + COR_STACKSIZE - PAGESIZE, PAGESIZE, PROT_WRITE);
#else
        mprotect (c->stack, PAGESIZE, PROT_WRITE);
#endif
    free (c->stack);
    free (c);
}
