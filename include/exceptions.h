#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

typedef struct exception *exception_t;

/* Initialisation du module. */
void exception_init (void);

/* Crée une nouvelle exception.
 * Le nom est purement indicatif, et n'a aucune signification interne.
 */
exception_t exception_creer (char *nom);

/* Nom de l'exception. */
char *exception_nom (exception_t);

/* Lève l'exception e.
 * Le contrôle est transféré au plus proche traitant d'exception
 *  pour `e' ou tout.
 * Si aucun traitant n'existe, panic.
 */
void exception_lever (exception_t e);

/* Renvoie la dernière exception qui a été levée dans ce processus.
 * A priori, utile (et significatif) que dans un traitant d'exception.
 */
exception_t exception_derniere_exception (void);


/* Capture des exceptions:
 *
 * S'utilise sous la forme:
 *
 * exception_essayer_avec (e) {
 *      ... code normal ...
 * } exception_capturer {
 *      traitement d'exception.
 * }
 * exception_fin
 *
 * Il est strictement interdit de sortir d'un bloc exception sans que soit
 * le code ait atteint la fin du bloc normal, soit l'exception ait été levée
 * (pas de sortie par break, return ou goto)
 *
 * On peut par contre faire ce qu'on veut dans le traitement d'exception
 * (y compris lever une exception).
 *
 * exception_essayer_avec_tous (qui capture n'importe quelle exception) est
 * a priori réservée aux couches système (en particulier couche processus).
 *
 */

/* **************** PRIVE **************** */

#include <ucontext.h>

typedef struct exception_catcher {
    exception_t e;
    int          levee;
    ucontext_t   retour;
    struct exception_catcher *prev;
} exception_catcher_t;


#define exception_essayer_avec(e) \
    { exception_catcher_t catcher; \
      exception_init_catcher (&catcher, e); \
      getcontext (&catcher.retour); \
      if (! catcher.levee) {
        
#define exception_essayer_avec_tous() exception_essayer_avec(exception_any)

#define exception_capturer exception_depiler(&catcher); } else
     
#define exception_fin }

extern exception_t exception_any;
     
void exception_init_catcher (exception_catcher_t *, exception_t);

void exception_depiler (exception_catcher_t *c);

#endif
