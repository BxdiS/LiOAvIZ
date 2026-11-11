#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* adjacency list */
struct adjlist {
    int n;
    int **nb;   /* nb[i] - neighbors of vertex i */
    int *cnt;
    int *cap;
};

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

/* random undirected graph: symmetric, no loops */
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

struct adjlist *create_list(int n) {
    struct adjlist *l;
    int i;

    l = (struct adjlist *)malloc(sizeof(struct adjlist));
    if (l == NULL) {
        printf("Memory error\n");
        exit(1);
    }
    l->n = n;
    l->nb = (int **)malloc(n * sizeof(int *));
    l->cnt = (int *)malloc(n * sizeof(int));
    l->cap = (int *)malloc(n * sizeof(int));
    for (i = 0; i < n; i++) {
        l->cap[i] = 4;
        l->nb[i] = (int *)malloc(l->cap[i] * sizeof(int));
        l->cnt[i] = 0;
    }
    return l;
}

void free_list(struct adjlist *l) {
    int i;
    for (i = 0; i < l->n; i++)
        free(l->nb[i]);
    free(l->nb);
    free(l->cnt);
    free(l->cap);
    free(l);
}

/* append j to neighbors of i if absent */
void list_add(struct adjlist *l, int i, int j) {
    int k;
    for (k = 0; k < l->cnt[i]; k++)
        if (l->nb[i][k] == j)
            return;
    if (l->cnt[i] == l->cap[i]) {
        l->cap[i] *= 2;
        l->nb[i] = (int *)realloc(l->nb[i], l->cap[i] * sizeof(int));
    }
    l->nb[i][l->cnt[i]++] = j;
}

int list_has(struct adjlist *l, int i, int j) {
    int k;
    for (k = 0; k < l->cnt[i]; k++)
        if (l->nb[i][k] == j)
            return 1;
    return 0;
}

struct adjlist *matrix_to_list(int **a, int n) {
    struct adjlist *l;
    int i, j;

    l = create_list(n);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            if (a[i][j])
                list_add(l, i, j);
    return l;
}

void print_list(struct adjlist *l) {
    int i, k;
    for (i = 0; i < l->n; i++) {
        printf("%d:", i + 1);
        for (k = 0; k < l->cnt[i]; k++)
            printf(" %d", l->nb[i][k] + 1);
        printf("\n");
    }
}

/* --- unary matrix operations --- */

/* identification of vertices u, v: merged vertex keeps number u (u < v) */
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

    /* edges not touching u or v */
    for (i = 0; i < n; i++) {
        if (i == u || i == v) continue;
        for (j = 0; j < n; j++) {
            if (j == u || j == v) continue;
            if (a[i][j])
                b[map[i]][map[j]] = 1;
        }
    }
    /* merged vertex gets union of neighborhoods */
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

/* contraction of edge (u, v) = identification of adjacent vertices */
int **contract_edge(int **a, int n, int u, int v, int *new_n) {
    return identify_vertices(a, n, u, v, new_n);
}

/* split vertex v: new vertex n+1 adjacent to v and to all neighbors of v */
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

/* --- unary list operations --- */

struct adjlist *list_identify(struct adjlist *l, int u, int v) {
    struct adjlist *r;
    int *map;
    int i, k, x;

    r = create_list(l->n - 1);
    map = (int *)malloc(l->n * sizeof(int));
    for (i = 0; i < l->n; i++) {
        if (i == v) map[i] = -1;
        else if (i > v) map[i] = i - 1;
        else map[i] = i;
    }

    for (i = 0; i < l->n; i++) {
        if (i == v) continue;
        for (k = 0; k < l->cnt[i]; k++) {
            x = l->nb[i][k];
            if (x == v) x = u;      /* edges to v now go to merged vertex */
            if (x != i)
                list_add(r, map[i], map[x]);
        }
    }

    free(map);
    return r;
}

struct adjlist *list_contract(struct adjlist *l, int u, int v) {
    return list_identify(l, u, v);
}

struct adjlist *list_split(struct adjlist *l, int v) {
    struct adjlist *r;
    int i, k, w;

    w = l->n;   /* new vertex index */
    r = create_list(l->n + 1);
    for (i = 0; i < l->n; i++)
        for (k = 0; k < l->cnt[i]; k++)
            list_add(r, i, l->nb[i][k]);
    /* new vertex adjacent to v and to all neighbors of v */
    for (k = 0; k < l->cnt[v]; k++) {
        list_add(r, w, l->nb[v][k]);
        list_add(r, l->nb[v][k], w);
    }
    list_add(r, w, v);
    list_add(r, v, w);
    return r;
}

/* --- binary matrix operations --- */

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

/* intersection: common vertices, edges present in both graphs */
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

/* ring sum: edges present in exactly one graph, no isolated vertices */
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

    /* count vertices with at least one edge */
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

    /* drop isolated vertices */
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
        printf("(isolated vertices removed:");
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

/* cartesian product G1 X G2 */
int **graph_product(int **a, int n1, int **b, int n2, int *new_n) {
    int **c;
    int n, i, j, k, l;

    n = n1 * n2;
    c = create_matrix(n, n);
    for (i = 0; i < n1; i++)
        for (k = 0; k < n2; k++)
            for (j = 0; j < n1; j++)
                for (l = 0; l < n2; l++) {
                    if (i == j && b[k][l]) c[i * n2 + k][j * n2 + l] = 1;
                    if (k == l && a[i][j]) c[i * n2 + k][j * n2 + l] = 1;
                }
    *new_n = n;
    return c;
}

/* generate matrices and convert to adjacency lists */
void task1(int **m1, int n1, int **m2, int n2) {
    struct adjlist *l1, *l2;

    printf("\nMatrix M1 (%dx%d):\n", n1, n1);
    print_matrix(m1, n1, n1);
    printf("Matrix M2 (%dx%d):\n", n2, n2);
    print_matrix(m2, n2, n2);

    l1 = matrix_to_list(m1, n1);
    l2 = matrix_to_list(m2, n2);

    printf("Adjacency list of G1:\n");
    print_list(l1);
    printf("Adjacency list of G2:\n");
    print_list(l2);

    free_list(l1);
    free_list(l2);
}

/* unary operations on matrix and list forms */
void task2(int **m1, int n1, int **m2, int n2) {
    int **a, **r;
    int n, rn, g, u, v;
    struct adjlist *l, *rl;

    printf("Choose graph (1 or 2): ");
    scanf("%d", &g);
    if (g == 1) { a = m1; n = n1; }
    else if (g == 2) { a = m2; n = n2; }
    else { printf("wrong\n"); return; }

    printf("\nMatrix:\n");
    print_matrix(a, n, n);

    /* a) identification of vertices */
    printf("\nIdentification of vertices\n");
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

    /* b) contraction of an edge */
    printf("\nContraction of edge\n");
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

    /* c) splitting of a vertex */
    printf("\nSplitting of vertex\n");
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

    /* same operations on adjacency list */
    l = matrix_to_list(a, n);

    printf("\n--- adjacency list form ---\n");
    printf("Adjacency list:\n");
    print_list(l);

    printf("\nIdentification of vertices\n");
    printf("Enter u v: ");
    scanf("%d %d", &u, &v);
    if (u < 1 || u > n || v < 1 || v > n || u == v) {
        printf("wrong\n");
        free_list(l);
        return;
    }
    u--; v--;
    if (u > v) { int t = u; u = v; v = t; }
    rl = list_identify(l, u, v);
    printf("Result:\n");
    print_list(rl);
    free_list(rl);

    printf("\nContraction of edge\n");
    printf("Enter u v: ");
    scanf("%d %d", &u, &v);
    if (u < 1 || u > n || v < 1 || v > n || u == v) {
        printf("wrong\n");
        free_list(l);
        return;
    }
    u--; v--;
    if (u > v) { int t = u; u = v; v = t; }
    if (!list_has(l, u, v)) {
        printf("No edge %d-%d\n", u + 1, v + 1);
    } else {
        rl = list_contract(l, u, v);
        printf("Result:\n");
        print_list(rl);
        free_list(rl);
    }

    printf("\nSplitting of vertex\n");
    printf("Enter v: ");
    scanf("%d", &v);
    if (v < 1 || v > n) {
        printf("wrong\n");
        free_list(l);
        return;
    }
    v--;
    rl = list_split(l, v);
    printf("Result:\n");
    print_list(rl);
    free_list(rl);

    free_list(l);
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

/* cartesian product */
void task4(int **m1, int n1, int **m2, int n2) {
    int **r;
    int rn, i, k;

    r = graph_product(m1, n1, m2, n2, &rn);

    printf("\nCartesian product G1 X G2 (%dx%d):\n", rn, rn);
    printf("Vertices: ");
    for (i = 0; i < n1; i++)
        for (k = 0; k < n2; k++)
            printf(" %d=z%dv%d", i * n2 + k + 1, i + 1, k + 1);
    printf("\n");
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

    printf("1 - generate matrices, adjacency lists\n");
    printf("2 - unary operations (identify, contract, split)\n");
    printf("3 - union, intersection, ring sum\n");
    printf("4 - cartesian product\n");
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: task1(m1, n1, m2, n2); break;
        case 2: task2(m1, n1, m2, n2); break;
        case 3: task3(m1, n1, m2, n2); break;
        case 4: task4(m1, n1, m2, n2); break;
        default: printf("wrong\n"); break;
    }

    free_matrix(m1, n1);
    free_matrix(m2, n2);
    return 0;
}
