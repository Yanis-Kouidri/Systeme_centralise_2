#include "exceptions.h"
#include "TSD.h"
#include "processus.h"          /* uniquement pour proc_init */
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>

struct exception {
    char *nom;
};

exception_t exception_any;

static int clef_pile_exception;
static int clef_derniere_exception;

exception_t exception_creer (char *nom)
{
    exception_t e;
    e = malloc (sizeof (struct exception));
    e->nom = strdup (nom);
    return e;
}

char *exception_nom (exception_t e)
{
    return e->nom;
}

exception_t exception_derniere_exception (void)
{
    return TSD_get (clef_derniere_exception);
}


void exception_init_catcher (exception_catcher_t *c, exception_t e)
{
    c->e = e;
    c->levee = 0;
    c->prev = TSD_get (clef_pile_exception);
    TSD_set (clef_pile_exception, c);
}

void exception_depiler (exception_catcher_t *c)
{
    TSD_set (clef_pile_exception, c->prev);
}

void exception_lever (exception_t e)
{
    exception_catcher_t *c;
    /* On va remonter la pile jusqu'à trouver e ou exception_any.
     * Au passage, on dépile. */
    c = TSD_get (clef_pile_exception);
    while ((c != NULL) && (c->e != e) && (c->e != exception_any)) {
        c = c->prev;
    }
    if (c == NULL) {
        fprintf (stderr, "EXCEPTIONS: PANIC: exception `%s' non capturee.\n", e->nom);
        abort();
    }
    TSD_set (clef_derniere_exception, e);
    TSD_set (clef_pile_exception, c->prev);
    c->levee = 1;
    setcontext (&(c->retour));
    /* NOTREACHED */
}

void exception_init (void)
{
    static int init_done = 0;
    if (! init_done) {
        init_done = 1;
        proc_init();
        exception_any = exception_creer ("Toutes_les_exceptions");
        clef_pile_exception = TSD_creer_clef (NULL);
        assert (clef_pile_exception != -1);
        clef_derniere_exception = TSD_creer_clef (NULL);
        assert (clef_derniere_exception != -1);
    }   
}
