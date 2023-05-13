#include "processus.h"
#include "TSD.h"
#include "memvirt.h"
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>


int MV_verbose = 0;

/* Descripteur sur lequel les traces sont écrites. */
int MV_verbose_fd = 1;

/* Statistiques. Voir memvirt.h pour la signification. */
int MV_nb_defauts = 0;
int MV_nb_ecritures = 0;
int MV_nb_lectures = 0;

/* Tellement utilisé qu'il vaut mieux être sur que ce n'est pas une fonction!
 * (ce qui est le cas si on utilise PAGESIZE) */
static int pagesize;

int MV_getpagesize() {
    return pagesize;
}

/* Clef pour obtenir les infos privées de chaque processus. */
static int MV_clef;

/* Adresse de début de la mémoire centrale (et virtuelle). */
static caddr_t base = NULL;

void *MV_getbase (void)
{
    return base;
}

/* Nombre de pages de la mémoire centrale/virtuelle et du swap. */
static int nbpagescentr;
static int nbpagesswap;

/* Taille (en octets)  de la mémoire centrale/virtuelle et du swap. */
static int taille_espace_centr;
static int taille_espace_swap;

/* Descripteur de fichier du swap. */
static int swapfd;

/* Numéro de la page de la mémoire centrale qui contient l'adresse spécifiée. */
#define num_page(adresse)  (((adresse) - base) / pagesize)

/* Global: pour chaque page de la mémoire centrale, qui la possède.
 *
 * Pour éviter d'écrire une page non modifiée, `valide' indique si la
 * page a été accédée une fois (et son contenu est donc bon), et
 * `modifiee' indique si un accès en écriture a eu lieu.
 * Le drapeau `valide' est  positionné après le traitement d'un accès
 * (SIGSEGV sur page non lisible), et il est remis à zéro à chaque commutation.
 * Le drapeau `modifiee' est positionné lors d'un accès sur page valide
 * (SIGSEGV sur page lisible non modifiable), et remis à zéro lors de
 * l'éjection de la page (écriture dans le swap).
 */
typedef struct {
    processus_t proprio;
    char        valide;
    char        modifiee;
} descripteur_page_centrale;

/* Le tableau (alloué dynamiquement) des infos de la mémoire centrale. */
static descripteur_page_centrale *infos_memcentrale;

/* Global: pour chaque page de la mémoire de swap, qui l'utilise ? */
/* En fait, un booléen indiquant si la page est utilisée suffit. */
typedef struct {
    processus_t proprio;
} descripteur_page_swap;

/* Le tableau (alloué dynamiquement) des infos globales du swap. */
static descripteur_page_swap *infos_swap;

/* Par processus: où est rangée (dans le swap) la page virtuelle ? */
typedef struct {
    int page_swap;
} virt_2_swap;

/* infos_proc est un tableau (alloué dynamiquement à la création
 * du procesus cf init_info_processus) des infos locales. */
typedef virt_2_swap  *infos_proc;

/****************************************************************/

static void connect_sigsegv_handler (void); /* further */

/*
 * Toutes les procédures (sauf les deux hooks) sont appelées depuis
 * le handler de SIGSEGV. Il faut donc savoir se limiter considérablement sur
 * ce qu'on fait. Plus précisément, les seules fonctions signal-safe sont
 * les appels systèmes. MV_trace est fourni pour pouvoir faire des traces:
 *     MV_trace ("MV: charger: proc %s, page %d nouvelle\n", proc_nom(proc_self()), npage);
 */
static void MV_trace (char *fmt, ...)
{
    static char buf[512];
    static va_list vap;
    if (MV_verbose) {
        va_start(vap, fmt);
        vsnprintf (buf, 512, fmt, vap);
        write (MV_verbose_fd, buf, strlen(buf));
    }
}


/* Marque les pages de la mémoire centrale utilisées par le processus
 * courant comme non allouées;
 * Libère les pages de swap du processus courant. */
static void liberer_ressources (void)
{
    /* XXXX A COMPLETER XXXX */
}

/* Trouve une page de swap et la réserve pour un processus donné. */
static int allouer_page_swap (processus_t pourqui)
{
    /* XXXX A COMPLETER XXXX */
}

/* Transfère la page virtuelle npage dans le swap.
 * La page virtuelle peut
 *  - n'être allouée à personne => rien à faire.
 *  - être allouée au processus qui veut la sauver => rien à faire.
 *  - être allouée à un autre processus
 *    Il faut trouver la page de swap correspondante (éventuellement
 *    allouer cette page) et écrire le contenu de la page virtuelle dedans.
 */
static void sauver_page (int npage)
{
    /* XXXX A COMPLETER XXXX */
}

/*
 * Charge la page virtuelle npage depuis le swap pour le processus élu.
 * Si la page virtuelle appartient déjà au processus => rien à faire.
 * Si la page virtuelle n'a pas de page de swap correspondante
 *   => elle est initialisée à zéro.
 * Sinon on charge depuis le swap la page correspondante.
 */
static void charger_page (int npage)
{
    /* XXXX A COMPLETER XXXX */
}

/* Initialise les infos privées à chaque processus.
 * Utilisé comme hook de création. */
static void init_info_processus (void)
{
    infos_proc info;
    int i;
    info = malloc (sizeof (virt_2_swap) * nbpagescentr);
    for (i = 0; i < nbpagescentr; i++) {
        info[i].page_swap = -1;
    }
    TSD_set (MV_clef, info);
}

/* Hook appelé avant la commutation.
 * Le paramètre `ps' indique l'état que va prendre le processus qui
 * perd la main (cf processus.h)
 */
static void pre_commuter (proc_status ps)
{
    /* XXXX A COMPLETER XXXX */
}

/* Le signal sigsegv n'est pas remis en place lors de cette procédure, sauf
 * si tout se passe bien.
 * À la fin du handler, le système reexécute l'instruction ayant entraîné le
 * SIGSEGV. Si il n'y a plus de gestionnaire, le comportement normal de
 * SIGSEGV est obtenu (core dumped).
 */
void handler_sigsegv (int sig, siginfo_t *sip, void *unused)
{
    caddr_t badaddress;
    int numpage;
    assert (sig == SIGSEGV);
    if (sip->si_code != SEGV_ACCERR)     /* ACCERR = invalid permissions */
      return;
    badaddress = sip->si_addr;
    if ((badaddress < base) || (badaddress >= base + taille_espace_centr))
      return;
    /* Ok, c'est pour nous! */
    numpage = num_page(badaddress);
    /* XXXX A COMPLETER XXXX */
    connect_sigsegv_handler();
}

/** Installe le gestionnaire de signal SIGSEGV. */
static void connect_sigsegv_handler (void)
{
    struct sigaction act;
    /* RESETHAND = reset to SIGDFL,
       NODEFER = SIGSEGV is not blocked in handler
       SIGINFO = pass siginfo */
    act.sa_flags = SA_RESETHAND | SA_NODEFER | SA_SIGINFO;
    /* Don't be interrupted by SIGALRM or SIGVTALRM. */
    sigemptyset (&act.sa_mask);
    sigaddset (&act.sa_mask, SIGALRM);
    sigaddset (&act.sa_mask, SIGVTALRM);
    act.sa_sigaction = handler_sigsegv;
    sigaction (SIGSEGV, &act, NULL);
}

void MV_init (int nbpages_centr, int nbpages_swap)
{
    static int init_done = 0;
    int i;

    assert (! init_done);
    init_done = 1;
    
    proc_init(); /* pour être sûr que c'est fait. */

    pagesize = sysconf(_SC_PAGESIZE);

    nbpagescentr = nbpages_centr;
    taille_espace_centr = nbpagescentr * pagesize;

    nbpagesswap = nbpages_swap;
    taille_espace_swap = nbpagesswap * pagesize;

    /*swapfd = open ("/tmp/swap", O_RDWR | O_EXCL | O_CREAT, 0644); */
    swapfd = open ("/tmp/swap", O_RDWR | O_TRUNC | O_CREAT, 0644);
    if (swapfd == -1) {
        perror ("Can't open /tmp/swap");
        exit (1);
    }

    base = valloc (taille_espace_centr);
    memset (base, 0, taille_espace_centr); /* pas nécessaire. */
    mprotect (base, taille_espace_centr, PROT_NONE);

    infos_memcentrale = malloc(sizeof(descripteur_page_centrale)*nbpagescentr);
    for (i = 0; i < nbpagescentr; i++) {
        infos_memcentrale[i].modifiee = 0;
        infos_memcentrale[i].valide = 0;
        infos_memcentrale[i].proprio = NULL;
    }

    infos_swap = malloc (sizeof (descripteur_page_swap) * nbpagesswap);
    for (i = 0; i < nbpagesswap; i++) {
        infos_swap[i].proprio = NULL;
    }

    /* Crée la clef pour les données de la MV privées à chaque processus. */
    /* XXXX A COMPLETER XXXX */

    /* Le processus principal est aussi un processus: initialisation de ses
     * données privées. */
    init_info_processus();
    
    /* Connecter le gestionnaire de SIGSEGV. */
    connect_sigsegv_handler();

    /* Connecter les hooks nécessaires. */
    proc_creation_hook = init_info_processus;
    proc_pre_commuter_hook = pre_commuter;
    proc_post_commuter_hook = NULL;

    MV_trace ("MV: init done.\n");
}

/****************************************************************/

/* Pour debugger.
 * Indique si il faut dumper les infos de la mémoire centrale et/ou
 * de la mémoire de swap. */
void MV_dump (int centrale, int swap)
{
    int i;
    printf ("-------------\n");
    printf ("MV: base=%p, %d pages virtuelles, %d pages swap\n",
            base, nbpagescentr, nbpagesswap);
    if (centrale) {
        printf ("MV: memoire centrale\n");
        for (i = 0; i < nbpagescentr; i++) {
            printf ("  %02d: ", i);
            if (infos_memcentrale[i].proprio != NULL) {
                printf ("%s\n", proc_nom (infos_memcentrale[i].proprio));
            } else {
                printf ("available\n");
            }
        }
    }
    if (swap) {
        printf ("MV: memoire de swap\n");
        for (i = 0; i < nbpagesswap; i++) {
            printf ("  %02d: ", i);
            if (infos_swap[i].proprio != NULL) {
                printf ("%s\n", proc_nom (infos_swap[i].proprio));
            } else {
                printf ("available\n");
            }
        }
    }
    printf ("-------------\n");
}

