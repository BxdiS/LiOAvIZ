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

/* random graph: symmetric, no loops */
void fill_adj(int **a, int n) {
    int i, j;
    for (i = 0; i < n; i++) {
        a[i][i] = 0;
        for (j = i + 1; j < n; j++) {
            a[i][j] = rand() % 2;
            a[j][i] = a[i][j];
        }
    }
}

/* --- one-graph matrix operations --- */

/* merge vertices u and v: the new vertex keeps number u (u < v) */
int **identify_vertices(int **a, int n, int u, int v, int *new_n) {
    int **b;
    int *map;
    int i, j, w;

    b = create_matrix(n - 1, n - 1);
    map = (int *)malloc(n * sizeof(int));
    for (i = 0; i < n; i++) {
        if (i == v) map[i] = -1;
        else if (i > v) map[i] = i - 1;
        else map[i] = i;
    }

    /* edges that do not touch u or v */
    for (i = 0; i < n; i++) {
        if (i == u || i == v) continue;
        for (j = 0; j < n; j++) {
            if (j == u || j == v) continue;
            if (a[i][j])
                b[map[i]][map[j]] = 1;
        }
    }
    /* the new vertex gets all neighbors of u and of v */
    for (w = 0; w < n; w++) {
        if (w == u || w == v) continue;
        if (a[u][w] || a[v][w]) {
            b[u][map[w]] = 1;
            b[map[w]][u] = 1;
        }
    }

    free(map);
    *new_n = n - 1;
    return b;
}

/* shrink edge (u, v) = merge two vertices that are linked */
int **contract_edge(int **a, int n, int u, int v, int *new_n) {
    return identify_vertices(a, n, u, v, new_n);
}

/* split vertex v: new vertex n+1 is linked to v and to all neighbors of v */
int **split_vertex(int **a, int n, int v, int *new_n) {
    int **b;
    int i, j;

    b = create_matrix(n + 1, n + 1);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            b[i][j] = a[i][j];
    for (i = 0; i < n; i++) {
        if (a[v][i]) {
            b[n][i] = 1;
            b[i][n] = 1;
        }
    }
    b[v][n] = 1;
    b[n][v] = 1;

    *new_n = n + 1;
    return b;
}

/* --- two-graph matrix operations --- */

/* union: V = V1 + V2, U = U1 + U2 */
int **graph_union(int **a, int n1, int **b, int n2, int *new_n) {
    int **c;
    int n, i, j;

    n = n1 > n2 ? n1 : n2;
    c = create_matrix(n, n);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            if (i < n1 && j < n1 && a[i][j]) c[i][j] = 1;
            if (i < n2 && j < n2 && b[i][j]) c[i][j] = 1;
        }
    *new_n = n;
    return c;
}

/* intersection: common vertices, edges that are in both graphs */
int **graph_intersection(int **a, int n1, int **b, int n2, int *new_n) {
    int **c;
    int n, i, j;

    n = n1 < n2 ? n1 : n2;
    c = create_matrix(n, n);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            if (a[i][j] && b[i][j])
                c[i][j] = 1;
    *new_n = n;
    return c;
}

/* ring sum: edges that are only in one graph, no vertices without edges */
int **graph_ring_sum(int **a, int n1, int **b, int n2, int *new_n) {
    int **c, **r;
    int n, m, i, j, deg;

    n = n1 > n2 ? n1 : n2;
    c = create_matrix(n, n);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            int e1 = (i < n1 && j < n1) ? a[i][j] : 0;
            int e2 = (i < n2 && j < n2) ? b[i][j] : 0;
            c[i][j] = e1 ^ e2;
        }

    /* count vertices that have at least one edge */
    m = 0;
    for (i = 0; i < n; i++) {
        deg = 0;
        for (j = 0; j < n; j++)
            deg += c[i][j];
        if (deg > 0) m++;
    }
    if (m == n) {
        *new_n = n;
        return c;
    }

    /* remove vertices that have no edges */
    r = create_matrix(m, m);
    {
        int *map, ci;
        map = (int *)malloc(n * sizeof(int));
        ci = 0;
        for (i = 0; i < n; i++) {
            deg = 0;
            for (j = 0; j < n; j++)
                deg += c[i][j];
            map[i] = deg > 0 ? ci++ : -1;
        }
        for (i = 0; i < n; i++) {
            if (map[i] < 0) continue;
            for (j = 0; j < n; j++) {
                if (map[j] < 0) continue;
                r[map[i]][map[j]] = c[i][j];
            }
        }
        printf("(vertices without edges removed:");
        ci = 0;
        for (i = 0; i < n; i++)
            if (map[i] >= 0) {
                printf(" %d->%d", i + 1, map[i] + 1);
                ci++;
            }
        printf(")\n");
        free(map);
    }

    free_matrix(c, n);
    *new_n = m;
    return r;
}

/* make two random matrices and show them */
void task1(int **m1, int n1, int **m2, int n2) {
    printf("\nMatrix M1 (%dx%d):\n", n1, n1);
    print_matrix(m1, n1, n1);
    printf("Matrix M2 (%dx%d):\n", n2, n2);
    print_matrix(m2, n2, n2);
}

/* one-graph operations on the adjacency matrix */
void task2(int **m1, int n1, int **m2, int n2) {
    int **a, **r;
    int n, rn, g, u, v;

    printf("Choose graph (1 or 2): ");
    scanf("%d", &g);
    if (g == 1) { a = m1; n = n1; }
    else if (g == 2) { a = m2; n = n2; }
    else { printf("wrong\n"); return; }

    printf("\nMatrix:\n");
    print_matrix(a, n, n);

    /* a) merge vertices */
    printf("\nMerge vertices\n");
    printf("Enter u v: ");
    scanf("%d %d", &u, &v);
    if (u < 1 || u > n || v < 1 || v > n || u == v) {
        printf("wrong\n");
        return;
    }
    u--; v--;
    if (u > v) { int t = u; u = v; v = t; }
    r = identify_vertices(a, n, u, v, &rn);
    printf("Result (%dx%d):\n", rn, rn);
    print_matrix(r, rn, rn);
    free_matrix(r, rn);

    /* b) shrink an edge */
    printf("\nShrink edge\n");
    printf("Enter u v: ");
    scanf("%d %d", &u, &v);
    if (u < 1 || u > n || v < 1 || v > n || u == v) {
        printf("wrong\n");
        return;
    }
    u--; v--;
    if (u > v) { int t = u; u = v; v = t; }
    if (!a[u][v]) {
        printf("No edge %d-%d\n", u + 1, v + 1);
    } else {
        r = contract_edge(a, n, u, v, &rn);
        printf("Result (%dx%d):\n", rn, rn);
        print_matrix(r, rn, rn);
        free_matrix(r, rn);
    }

    /* c) split a vertex */
    printf("\nSplit vertex\n");
    printf("Enter v: ");
    scanf("%d", &v);
    if (v < 1 || v > n) {
        printf("wrong\n");
        return;
    }
    v--;
    r = split_vertex(a, n, v, &rn);
    printf("Result (%dx%d):\n", rn, rn);
    print_matrix(r, rn, rn);
    free_matrix(r, rn);
}

/* union, intersection, ring sum */
void task3(int **m1, int n1, int **m2, int n2) {
    int **r;
    int rn;

    printf("\nMatrix M1 (%dx%d):\n", n1, n1);
    print_matrix(m1, n1, n1);
    printf("Matrix M2 (%dx%d):\n", n2, n2);
    print_matrix(m2, n2, n2);

    r = graph_union(m1, n1, m2, n2, &rn);
    printf("Union G1 + G2 (%dx%d):\n", rn, rn);
    print_matrix(r, rn, rn);
    free_matrix(r, rn);

    r = graph_intersection(m1, n1, m2, n2, &rn);
    printf("Intersection G1 * G2 (%dx%d):\n", rn, rn);
    print_matrix(r, rn, rn);
    free_matrix(r, rn);

    r = graph_ring_sum(m1, n1, m2, n2, &rn);
    printf("Ring sum G1 (+) G2 (%dx%d):\n", rn, rn);
    print_matrix(r, rn, rn);
    free_matrix(r, rn);
}

int main(void) {
    int **m1, **m2;
    int n1, n2, choice;

    srand(time(NULL));

    printf("lab6\n");
    printf("Enter number of vertices of G1: ");
    scanf("%d", &n1);
    printf("Enter number of vertices of G2: ");
    scanf("%d", &n2);
    if (n1 < 1 || n2 < 1) {
        printf("wrong\n");
        return 0;
    }

    m1 = create_matrix(n1, n1);
    m2 = create_matrix(n2, n2);
    fill_adj(m1, n1);
    fill_adj(m2, n2);

    printf("1 - make two random matrices\n");
    printf("2 - one-graph operations (merge, shrink, split)\n");
    printf("3 - union, intersection, ring sum\n");
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: task1(m1, n1, m2, n2); break;
        case 2: task2(m1, n1, m2, n2); break;
        case 3: task3(m1, n1, m2, n2); break;
        default: printf("wrong\n"); break;
    }

    free_matrix(m1, n1);
    free_matrix(m2, n2);
    return 0;
}
