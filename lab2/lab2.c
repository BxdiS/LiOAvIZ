#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* matrix mul */

void fill_matrix(int *m, int n) {
    int i;
    for (i = 0; i < n * n; i++)
        m[i] = rand() % 100 + 1;
}

void multiply(int *a, int *b, int *c, int n) {
    int i, j, r, sum;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            sum = 0;
            for (r = 0; r < n; r++)
                sum += a[i * n + r] * b[r * n + j];
            c[i * n + j] = sum;
        }
    }
}

void task1(void) {
    int sizes[] = {100, 200, 400, 1000, 2000, 4000, 10000};
    int num_sizes = 7;
    double times[7];
    double max_time;
    int k, i, bar;

    printf("matrix mul, O(n^3)\n\n");
    printf("%10s %12s\n", "n", "time (s)");
    printf("------------------------\n");

    for (k = 0; k < num_sizes; k++) {
        int n = sizes[k];
        int *a, *b, *c;
        clock_t start, end;

        a = (int *)malloc((long)n * n * sizeof(int));
        b = (int *)malloc((long)n * n * sizeof(int));
        c = (int *)malloc((long)n * n * sizeof(int));

        if (a == NULL || b == NULL || c == NULL) {
            printf("%10d %12s\n", n, "no memory");
            times[k] = -1;
            free(a); free(b); free(c);
            continue;
        }

        srand(time(NULL));
        fill_matrix(a, n);
        fill_matrix(b, n);

        start = clock();
        multiply(a, b, c, n);
        end = clock();

        times[k] = (double)(end - start) / CLOCKS_PER_SEC;
        printf("%10d %12.4f\n", n, times[k]);

        free(a);
        free(b);
        free(c);
    }

    /* graph */
    max_time = 0;
    for (k = 0; k < num_sizes; k++)
        if (times[k] > max_time) max_time = times[k];

    printf("\n-- graph --\n");
    for (k = 0; k < num_sizes; k++) {
        printf("%6d | ", sizes[k]);
        if (times[k] < 0) {
            printf("skipped\n");
            continue;
        }
        bar = (max_time > 0) ? (int)(times[k] / max_time * 50) : 0;
        for (i = 0; i < bar; i++) printf("#");
        printf(" %.4f\n", times[k]);
    }

}

/* sort alg */

void shell_sort(int *items, int count) {
    int i, j, gap, k;
    int x, a[5];

    a[0] = 9; a[1] = 5; a[2] = 3; a[3] = 2; a[4] = 1;

    for (k = 0; k < 5; k++) {
        gap = a[k];
        for (i = gap; i < count; ++i) {
            x = items[i];
            for (j = i - gap; (x < items[j]) && (j >= 0); j = j - gap)
                items[j + gap] = items[j];
            items[j + gap] = x;
        }
    }
}

void qs(int *items, int left, int right) {
    int i, j;
    int x, y;

    i = left; j = right;
    x = items[(left + right) / 2];

    do {
        while ((items[i] < x) && (i < right)) i++;
        while ((x < items[j]) && (j > left)) j--;

        if (i <= j) {
            y = items[i];
            items[i] = items[j];
            items[j] = y;
            i++; j--;
        }
    } while (i <= j);

    if (left < j) qs(items, left, j);
    if (i < right) qs(items, i, right);
}

int cmp_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void fill_random(int *arr, int n) {
    int i;
    for (i = 0; i < n; i++)
        arr[i] = rand() % (n * 10);
}

void fill_ascending(int *arr, int n) {
    int i;
    for (i = 0; i < n; i++)
        arr[i] = i;
}

void fill_descending(int *arr, int n) {
    int i;
    for (i = 0; i < n; i++)
        arr[i] = n - i;
}

void fill_mountain(int *arr, int n) {
    int i;
    int half = n / 2;
    for (i = 0; i < half; i++)
        arr[i] = i;
    for (i = half; i < n; i++)
        arr[i] = n - i;
}

double measure_shell(int *src, int n) {
    int *tmp;
    clock_t start, end;

    tmp = (int *)malloc(n * sizeof(int));
    memcpy(tmp, src, n * sizeof(int));

    start = clock();
    shell_sort(tmp, n);
    end = clock();

    free(tmp);
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double measure_qs(int *src, int n) {
    int *tmp;
    clock_t start, end;

    tmp = (int *)malloc(n * sizeof(int));
    memcpy(tmp, src, n * sizeof(int));

    start = clock();
    qs(tmp, 0, n - 1);
    end = clock();

    free(tmp);
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double measure_qsort(int *src, int n) {
    int *tmp;
    clock_t start, end;

    tmp = (int *)malloc(n * sizeof(int));
    memcpy(tmp, src, n * sizeof(int));

    start = clock();
    qsort(tmp, n, sizeof(int), cmp_int);
    end = clock();

    free(tmp);
    return (double)(end - start) / CLOCKS_PER_SEC;
}

void run_test(char *label, int *arr, int n) {
    printf("%-14s  Shell: %.6f  QuickSort: %.6f  qsort: %.6f\n",
           label,
           measure_shell(arr, n),
           measure_qs(arr, n),
           measure_qsort(arr, n));
}

void task2(void) {
    int sizes[] = {1000, 5000, 10000, 50000, 100000};
    int num_sizes = 5;
    int k;

    printf("sort alg timing\n\n");

    srand(time(NULL));

    for (k = 0; k < num_sizes; k++) {
        int n = sizes[k];
        int *arr;

        arr = (int *)malloc(n * sizeof(int));
        if (arr == NULL) {
            printf("n = %d: no memory\n", n);
            continue;
        }

        printf("--- n = %d ---\n", n);

        fill_random(arr, n);
        run_test("Random", arr, n);

        fill_ascending(arr, n);
        run_test("Ascending", arr, n);

        fill_descending(arr, n);
        run_test("Descending", arr, n);

        fill_mountain(arr, n);
        run_test("Mountain", arr, n);

        printf("\n");
        free(arr);
    }
}

int main(void) {
    int choice;

    printf("lab2\n");
    printf("1 - matrix mul\n");
    printf("2 - sort alg\n");
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: task1(); break;
        case 2: task2(); break;
        default: printf("wrong\n"); break;
    }

    return 0;
}
