#include "sleep.h"
#include "processus.h"
#include "scheduler.h"
#include <sys/time.h>
#include <stdlib.h>

/* Note: utilise un processus système bouclant: il faut assurer que le
 * scheduler lui donne la main de temps en temps, et évite que
 * ce processus ne l'accapare (scheduler équitable).
 */

/* TERMINAISON (sujet chaud!):
 * 
 * Vis-à-vis de proc_suis_je_seul:
 *
 * On utilise un processus de réveil qui se suspend s'il n'y a plus de
 * processus endormi. Cela assure une terminaison correcte des
 * applications en ne pertubant pas suis_je_seul.
 * Par contre, même si ce processus est un processus
 * système, il est indispensable de le compter parmi les vivants,
 * sinon on détecterait une fausse terminaison dans le cas où il se
 * trouverait seul avec des processus utilisateur endormis.
 *
 * Vis-à-vis des couches supérieures (moniteur/sémaphore):
 * le processus système est créé avec proc_activer, il n'est donc
 * pas connu et ni compté. Cependant, comme les processus endormis sont
 * bloqués avec proc_suspendre, ils ne sont pas comptés non plus dans
 * les bloqués et sont considérés comme vivants-non-bloqués. Par conséquent,
 * la terminaison se passe bien.
 */

struct sleeping {
    struct sleeping *next;
    processus_t      qui;
    struct timeval   end;
};

/* La liste des processus qui ont fait une demande de sommeil. */
static struct sleeping *sleepers = NULL;

/* Le processus de réveil. */
static processus_t le_reveilleur = NULL;

static int reveilleur_est_bloque = 0;

/* Le serveur de sommeil.
 * Il regarde l'heure jusqu'à ce qu'un dormeur doit être réveillé.
 * Ce dormeur est alors débloqué.
 * Le serveur se suspend s'il n'y a aucun dormeur (il est débloqué lors
 * d'une demande).
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void sleeper (void *unused)
{
    sched_set_priority (sched_priorite_min());
    while (1) {
        struct sleeping *sl, *prev;
        struct timeval now;
        rendre_noninterruptible();
        gettimeofday (&now, NULL);
        /* Recherche le premier dormant qui peut être réveillé. */
        sl = sleepers;
        prev = NULL;
        while (sl != NULL) {
            if ((now.tv_sec > sl->end.tv_sec)
                || ((now.tv_sec == sl->end.tv_sec) && (now.tv_usec >= sl->end.tv_usec)))
              break;
            prev = sl;
            sl = sl->next;
        }
        if (sl != NULL) {
            /* Un de trouvé: on l'enlève de la liste, */
            if (prev == NULL)
              sleepers = sl->next;
            else
              prev->next = sl->next;
            /* et on le réveille. */
            proc_continuer (sl->qui);
        }
        if (sleepers == NULL) {  /* plus personne => on se suspend. */
            reveilleur_est_bloque = 1;
            proc_suspendre();
        } else {
            proc_commuter();
        }
        rendre_interruptible();
#if 0
        /* Non, on fait de l'attente active, ce qui permet de donner la
         * main à des non-dormeurs. */
        if (sl == NULL) {
            static int sleep_step = 10 * 1000; /* 10ms = 0.01s */
            struct timeval it;
            it.tv_sec = sleep_step / 1000000;
            it.tv_usec = sleep_step % 1000000;
            select (0, NULL, NULL, NULL, &it);
        }
#endif
    } 
}

/* Le processus courant demande à s'endormir pour n secondes.
 * Le serveur de sommeil est créé si nécessaire, et le processus se suspend.
 */
void proc_sleep (int sec)
{
    proc_usleep (sec * 1000000);
}


/* Le processus appelant dort n millisecondes. */
void proc_msleep (int msec)
{
    proc_usleep (msec * 1000);
}

/* Le processus courant demande à s'endormir pour n micro-secondes.
 * Le serveur de sommeil est créé si nécessaire, et le processus se suspend.
 */
void proc_usleep (int usec)
{
    struct sleeping sl; /* soyons joueur: la structure est dans la pile
                         * du processus qui s'endort. */
    rendre_noninterruptible();
    gettimeofday (&(sl.end), NULL);
    sl.end.tv_sec += (usec / 1000000);
    sl.end.tv_usec += (usec % 1000000);
    sl.qui = proc_self();
    sl.next = sleepers;
    sleepers = &sl;
    if (sl.next == NULL) {      /* pas d'autre demande en cours */
        if (le_reveilleur == NULL) { /* toute première demande */
            le_reveilleur = proc_activer ("sleeper", sleeper, NULL);
        } else {
            if (reveilleur_est_bloque) {
                reveilleur_est_bloque = 0;
                proc_continuer (le_reveilleur);
            }
        }
    }
    /* Il y aurait un problème de synchro ici, si proc_activer/continuer
     * donnait la main au processus créé. Heureusement que ce n'est pas
     * le cas, et vu que ce processus est non-interruptible, il va
     * nécessairement pouvoir se suspendre avant que le processus de
     * réveil n'obtienne la main et n'envisage de le débloquer. */
    proc_suspendre();
    rendre_interruptible();
}
