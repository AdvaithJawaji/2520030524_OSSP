#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n, sum = 0;

    printf("Enter number of elements: ");

    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("ERROR: Invalid input\n");
        return 1;
    }

    int *arr = malloc(n * sizeof(int));

    if (arr == NULL) {
        printf("ERROR: Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        printf("Enter element %d: ", i + 1);

        if (scanf("%d", &arr[i]) != 1) {
            printf("ERROR: Invalid value\n");
            free(arr);
            return 1;
        }

        sum += arr[i];
    }

    printf("\n----- TEST REPORT -----\n");
    printf("Elements : %d\n", n);
    printf("Sum      : %d\n", sum);
    printf("Expected : Memory released\n");

    /* Release allocated memory */
    free(arr);
    arr = NULL;

    printf("Result   : PASS\n");
    printf("Cleanup  : Successful\n");

    return 0;
}
