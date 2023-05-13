/* Préemption basique:
 *  - le quantum de temps donné à chaque processus est fixe;
 *  - il n'y a pas de priorité (si les processus prêts sont gérés en FIFO,
 *    ce qui est le cas, l'ordonnancement est FIFO).
 *
 * Bug: il n'y a pas réinitialisation du quantum en cas de commutation
 * forcée (commuter ou suspendre). Ce n'est pas compliqué à corriger, mais
 * ça pollue processus.c qui, actuellement, n'a aucune référence à la
 * préemption.
 */

extern int preemption_verbose;

/* Active ou désactive la commutation. */
void preemption_activate (int do_it);

/*****************************************************************/
/* Interface bas niveau.
 * A priori non nécessaire, mais on ne sait jamais. */

/* Active la préemption. Le handler sera exécuté à chaque quantum. */
void preemption_start (void (*handler)(void));
/* Arrête la préemption. */
void preemption_stop (void);

