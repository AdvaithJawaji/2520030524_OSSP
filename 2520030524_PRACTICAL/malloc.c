#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *a, *b;

    a = (int*)malloc(5 * sizeof(int));
    b = (int*)calloc(5, sizeof(int));
    a = (int*)realloc(a, 10 * sizeof(int));

    printf("Memory allocated using malloc: %p\n", a);
    printf("Memory allocated using calloc: %p\n", b);
    printf("Memory reallocated using realloc: %p\n", a);

    free(a);
    free(b);

    printf("Memory freed successfully.\n");

    return 0;
}

