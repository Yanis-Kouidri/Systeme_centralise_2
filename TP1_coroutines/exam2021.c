#include <stdio.h>
#include "coroutines.h"
coroutine_t c1,c2,c3;
void foo() {
int a = 7;
printf("F0 %d\n", a);
cor_transferer(c3,c2);
a++;
printf("F1 %d\n", a);
}
void codeA (void *unused) {
printf("A 1\n");
foo();
printf("A 2\n");
cor_transferer(c2,c1);
printf("A fini\n");
}
void codeB (void *unused) {
printf("C 1\n");
cor_transferer(c2,c1);
printf("C 2\n");
cor_transferer(c1,c3);
printf("C fini\n");
}
int main() {
c1 = cor_creer("C1", codeA, NULL);
c2 = cor_creer("C2", codeB, NULL);
c3 = cor_creer("C3", NULL, NULL);
foo();
printf ("main fini\n");
}