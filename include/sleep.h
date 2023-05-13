/* Le processus appelant dort n secondes.
 * Le sommeil est réalisé en suspendant/continuant le processus appelant.
 */
void proc_sleep (int sec);

/* Le processus appelant dort n millisecondes. */
void proc_msleep (int msec);

/* Le processus appelant dort n microsecondes. */
void proc_usleep (int usec);
