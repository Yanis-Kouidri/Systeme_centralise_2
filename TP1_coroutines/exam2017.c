#include <stdio.h>
#include <stdlib.h>
#include "coroutines.h"
coroutine_t c1,c2,c3;
void foo() {
int a = 1;
printf("F0 %d\n", a);
cor_transferer(c1,c2);
a++;
printf("F1 %d\n", a);
cor_transferer(c2,c3);
printf("F2 %d\n", a);
}
void codeA (void *unused) {
printf("A 1\n");
foo();
printf("A fini\n");
}
void codeB (void *unused) {
printf("B 1\n");
cor_transferer(c2, c3);
printf("B 2\n");
cor_transferer(c3, c1);
printf("B 3\n");
cor_transferer(c3, c2);
printf("B fini\n");
}
void codeC (void *unused) {
printf("C 1\n");
foo();
printf("C fini\n");
}
int main() {
c1 = cor_creer("C1", codeA, NULL);
c2 = cor_creer("C2", codeB, NULL);
c3 = cor_creer("C3", codeC, NULL);
cor_transferer(c1,c1);
printf ("main fini\n");
}