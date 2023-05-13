/** Implantation simple d'une queue.
 * Exemple:

 queue_t myqueue = NULL;
 enqueue(&myqueue, "coucou");
 if (length(&myqueue) > 3) { printf("pas mal plein"); }
 char *val = (char *) dequeue(&myqueue);
 printf("got %s\n", val);
*/

/* type opaque queue */
typedef struct elem *queue_t;

/* Ajoute un élément à la fin de la queue.
 * Exemple :
   queue_t maqueue;
   enqueue(&maqueue, "coucou");
*/
void enqueue(queue_t *queue, void *val);

/* Extrait et renvoie l'élément en tête.
 * La queue doit être non vide.
 * Exemple :
   char *val = (char *) dequeue(&maqueue);
*/
void *dequeue(queue_t *queue);

/* Renvoie vrai si la queue est vide */
int isempty(queue_t *queue);

/* Renvoie le nombre d'éléments dans la queue. */
int length(queue_t *queue);
