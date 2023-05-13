/** Implantation simple d'une queue.
 * Exemple:

 queue_t myqueue = NULL;
 queue_add(&myqueue, "coucou");
 if (queue_length(&myqueue) > 3) { printf("pas mal plein"); }
 char *val1 = (char *) queue_take(&myqueue);
 printf("got %s\n", val1);
 ..
 char *val2 = (char *) queue_get(&myqueue, 3);
 printf("le troisième élément est %s\n", val2);
 char *val3 = (char *) queue_remove(&myqueue, 5);
*/

/* type opaque queue */
typedef struct elem *queue_t;

/* Ajoute un élément à la fin de la queue.
 * Exemple :
   queue_t maqueue;
   queue_add(&maqueue, "coucou");
*/
void queue_add(queue_t *queue, void *val);

/* Enlève l'élément en tête et le renvoie.
 * La queue doit être non vide.
 * Exemple :
   char *val = (char *) queue_take(&maqueue);
*/
void *queue_take(queue_t *queue);

/* Renvoie le i-ième élément de la liste.
 * Ne retire par l'élément de la queue.
 * La tête est l'élément 0, le dernier est length(&queue)-1.
 * La queue doit être de longueur >= i.
 * Exemple :
   char *val = (char *) queue_get(&maqueue, 4);
*/
void *queue_get(queue_t *queue, int position);

/* Enlève le i-ième élément de la queue et le renvoie.
 * La tête est l'élément 0, le dernier est length(&queue)-1.
 * La queue doit être de longueur >= i.
 * Exemple :
   char *val = (char *) queue_remove(&maqueue, 4);
*/
void *queue_remove(queue_t *queue, int place);

/* Renvoie vrai si la queue est vide */
int queue_isempty(queue_t *queue);

/* Renvoie le nombre d'éléments dans la queue. */
int queue_length(queue_t *queue);
