#include "preemption.h"
#include "processus.h"
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <assert.h>


int preemption_verbose = 0;


/*  a timeval structure: seconds, microseconds */
static struct timeval PREEMPTION_DELAY = {0, 100};

static struct timeval NULL_TIMEVAL = {0, 0};

/* Faut-il utiliser ITIMER_VIRTUAL ou ITIMER_REAL ? VIRTUAL paraît logique,
 * mais si le processus léger se bloque (appel système), le temps n'est pas
 * décompté. Si on utilise REAL, pas équitable et les appels systèmes
 * peuvent être interrompus.
 *
 * Avec SA_RESTART, les appels systèmes sont redémarrés au sein même du
 * noyau unix. Ça marche donc alors même qu'on change le contexte (dont la
 * pile) dans le handler.
 */

#if 1
   /* Temps réel */
#  define PREEMPTION_SIG   SIGALRM
#  define PREEMPTION_TIMER ITIMER_REAL
#else
   /* Temps virtuel */
#  define PREEMPTION_SIG   SIGVTALRM
#  define PREEMPTION_TIMER ITIMER_VIRTUAL
#endif

/*****************************************************************/

static int preemption_is_active = 0;

static void armer_timer (struct timeval val)
{
    struct itimerval delai;
    delai.it_value = val;
    delai.it_interval = NULL_TIMEVAL;
    setitimer (PREEMPTION_TIMER, &delai, NULL);
}

/* Handler pour le signal de commutation. */
static void commutateur (void)
{
    rendre_noninterruptible ();
    if (preemption_verbose)
      printf ("PROC: preemption\n");
    /* Il est important de réarmer le timer avant de commuter, vu qu'on
     * ne sait pas où on va arriver. */
    armer_timer (PREEMPTION_DELAY);
    proc_commuter();
    rendre_interruptible ();
}

/* Active ou désactive la commutation. */
void preemption_activate (int do_it)
{
    if (do_it && !preemption_is_active) {
        if (preemption_verbose)
          printf ("PROC: activate preemption\n");
        preemption_is_active = 1;
        preemption_start (commutateur);
    } else if (!do_it && preemption_is_active) {
        if (preemption_verbose)
          printf ("PROC: deactivate preemption\n");
        preemption_is_active = 0;
        preemption_stop();
    }
}

/****************************************************************/

/* Active la préemption. Le handler sera exécuté à chaque quantum. */
void preemption_start (void (*handler)(void))
{
    struct sigaction act;
    
    act.sa_handler = handler;
    sigemptyset (&(act.sa_mask));
    act.sa_flags = 0;
    act.sa_flags &= ~SA_SIGINFO; /* use sa_handler, not sa_sigaction */
    act.sa_flags &= ~SA_RESETHAND; /* do not reset the handler to SIG_DFL */
    act.sa_flags |= SA_NODEFER; /* do not block the signal in handler */
    act.sa_flags |= SA_RESTART; /* restart interrupted system call */
    sigaction (PREEMPTION_SIG, &act, NULL);
    armer_timer (PREEMPTION_DELAY);
}

/* Arrête la préemption. */
void preemption_stop (void)
{
    armer_timer (NULL_TIMEVAL);
}

