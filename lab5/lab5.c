#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **create_matrix(int rows, int cols) {
    int **m;
    int i, j;

    m = (int **)malloc(rows * sizeof(int *));
    if (m == NULL) {
        printf("Memory error\n");
        exit(1);
    }
    for (i = 0; i < rows; i++) {
        m[i] = (int *)malloc(cols * sizeof(int));
        if (m[i] == NULL) {
            printf("Memory error\n");
            exit(1);
        }
        for (j = 0; j < cols; j++)
            m[i][j] = 0;
    }
    return m;
}

void free_matrix(int **m, int rows) {
    int i;
    for (i = 0; i < rows; i++)
        free(m[i]);
    free(m);
}

void print_matrix(int **m, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++)
            printf("%3d", m[i][j]);
        printf("\n");
    }
}

/* random undirected graph: symmetric, loops appear with probability p_loop */
void fill_adj(int **a, int n, double p, double p_loop) {
    int i, j;
    for (i = 0; i < n; i++) {
        a[i][i] = ((double)rand() / ((double)RAND_MAX + 1.0) < p_loop) ? 1 : 0;
        for (j = i + 1; j < n; j++) {
            a[i][j] = ((double)rand() / ((double)RAND_MAX + 1.0) < p) ? 1 : 0;
            a[j][i] = a[i][j];
        }
    }
}

/* size = number of edges, loops counted as one edge each */
int adj_size(int **a, int n) {
    int i, j, size = 0;
    for (i = 0; i < n; i++) {
        size += a[i][i];
        for (j = i + 1; j < n; j++)
            size += a[i][j];
    }
    return size;
}

/* number of distinct adjacent vertices (loop is not counted) */
void count_neighbors(int **a, int n, int *neigh) {
    int i, j;
    for (i = 0; i < n; i++) {
        neigh[i] = 0;
        for (j = 0; j < n; j++)
            if (j != i && a[i][j] == 1)
                neigh[i]++;
    }
}

/* print isolated, end and dominating vertices by number of neighbours */
void print_vertices(int *neigh, int n) {
    int i;

    printf("Isolated:");
    for (i = 0; i < n; i++)
        if (neigh[i] == 0) printf(" %d", i + 1);
    printf("\n");

    printf("End:");
    for (i = 0; i < n; i++)
        if (neigh[i] == 1) printf(" %d", i + 1);
    printf("\n");

    printf("Dominating:");
    for (i = 0; i < n; i++)
        if (neigh[i] == n - 1) printf(" %d", i + 1);
    printf("\n");
}

/* adjacency matrix */
void task1(int **a, int n) {
    int *neigh;

    printf("\nAdjacency matrix:\n");
    print_matrix(a, n, n);

    printf("Graph size: %d\n", adj_size(a, n));

    neigh = (int *)malloc(n * sizeof(int));
    count_neighbors(a, n, neigh);
    print_vertices(neigh, n);
    free(neigh);
}

/* incidence matrix */
void task2(int **a, int n) {
    int **inc;
    int *neigh;
    int m, i, j, e;

    m = adj_size(a, n);
    if (m == 0) {
        printf("\nGraph has no edges\n");
        return;
    }

    inc = create_matrix(n, m);
    e = 0;
    /* loops first: a loop contributes 2 to the vertex degree */
    for (i = 0; i < n; i++)
        if (a[i][i] == 1) {
            inc[i][e] = 2;
            e++;
        }
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++)
            if (a[i][j] == 1) {
                inc[i][e] = 1;
                inc[j][e] = 1;
                e++;
            }

    printf("\nIncidence matrix:\n");
    print_matrix(inc, n, m);

    /* size = number of columns (edges, loops included) */
    printf("Graph size: %d\n", m);

    neigh = (int *)malloc(n * sizeof(int));
    count_neighbors(a, n, neigh);
    print_vertices(neigh, n);

    free(neigh);
    free_matrix(inc, n);
}

int main(void) {
    int **a;
    int n, choice;
    double p, p_loop;

    srand(time(NULL));

    printf("lab5\n");
    printf("Enter number of vertices: ");
    scanf("%d", &n);
    if (n < 1) {
        printf("wrong\n");
        return 0;
    }

    printf("Enter edge probability (0..1): ");
    scanf("%lf", &p);
    if (p < 0.0 || p > 1.0) {
        printf("wrong\n");
        return 0;
    }

    printf("Enter loop probability (0..1): ");
    scanf("%lf", &p_loop);
    if (p_loop < 0.0 || p_loop > 1.0) {
        printf("wrong\n");
        return 0;
    }

    a = create_matrix(n, n);
    fill_adj(a, n, p, p_loop);

    /* same graph is reused for every menu item */
    while (1) {
        printf("\n1 - adjacency matrix\n");
        printf("2 - incidence matrix\n");
        printf("0 - exit\n");
        printf("> ");
        if (scanf("%d", &choice) != 1)
            break;

        if (choice == 0)
            break;
        if (choice == 1)
            task1(a, n);
        else if (choice == 2)
            task2(a, n);
        else
            printf("wrong\n");
    }

    free_matrix(a, n);
    return 0;
}
