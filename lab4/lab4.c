#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node *left;
    struct Node *right;
};

struct Node *root = NULL;

/* given: create tree */
struct Node *CreateTree(struct Node *root, struct Node *r, int data) {
    if (r == NULL) {
        r = (struct Node *)malloc(sizeof(struct Node));
        if (r == NULL) {
            printf("Memory error\n");
            exit(0);
        }
        r->left = NULL;
        r->right = NULL;
        r->data = data;
        if (root == NULL) return r;
        if (data > root->data) root->left = r;
        else root->right = r;
        return r;
    }
    if (data > r->data)
        CreateTree(r, r->left, data);
    else
        CreateTree(r, r->right, data);
    return root;
}

/* given: print tree (rotated 90 deg) */
void print_tree(struct Node *r, int l) {
    int i;
    if (r == NULL) return;
    print_tree(r->right, l + 1);
    for (i = 0; i < l; i++)
        printf("   ");
    printf("%d\n", r->data);
    print_tree(r->left, l + 1);
}

/* create tree without duplicates */
struct Node *CreateTreeUnique(struct Node *root, struct Node *r, int data) {
    if (r == NULL) {
        r = (struct Node *)malloc(sizeof(struct Node));
        if (r == NULL) {
            printf("Memory error\n");
            exit(0);
        }
        r->left = NULL;
        r->right = NULL;
        r->data = data;
        if (root == NULL) return r;
        if (data > root->data) root->left = r;
        else root->right = r;
        return r;
    }
    if (data == r->data) {
        printf("Duplicate %d, skipped\n", data);
        return root;
    }
    if (data > r->data)
        CreateTreeUnique(r, r->left, data);
    else
        CreateTreeUnique(r, r->right, data);
    return root;
}

/* search */
struct Node *search(struct Node *r, int data) {
    if (r == NULL) return NULL;
    if (data == r->data) return r;
    if (data > r->data) return search(r->left, data);
    return search(r->right, data);
}

/* count occurrences */
int count_val(struct Node *r, int data) {
    if (r == NULL) return 0;
    if (data == r->data) return 1 + count_val(r->right, data);
    if (data > r->data) return count_val(r->left, data);
    return count_val(r->right, data);
}

/* free tree */
void free_tree(struct Node *r) {
    if (r == NULL) return;
    free_tree(r->left);
    free_tree(r->right);
    free(r);
}

/* tree with duplicates */
void task1(void) {
    int cmd, val;
    struct Node *found;

    root = NULL;

    printf("tree (with dup)\n\n");

    while (1) {
        printf("\n1 - Add\n2 - Print\n3 - Search\n4 - Count\n5 - Complexity\n6 - Exit\n> ");
        scanf("%d", &cmd);

        if (cmd == 1) {
            printf("Enter number: ");
            scanf("%d", &val);
            root = CreateTree(root, root, val);
        } else if (cmd == 2) {
            if (root == NULL)
                printf("Tree is empty\n");
            else
                print_tree(root, 0);
        } else if (cmd == 3) {
            printf("Enter value: ");
            scanf("%d", &val);
            found = search(root, val);
            if (found)
                printf("Found: %d\n", found->data);
            else
                printf("Not found\n");
        } else if (cmd == 4) {
            printf("Enter value: ");
            scanf("%d", &val);
            printf("Count: %d\n", count_val(root, val));
        } else if (cmd == 5) {
            printf("Search complexity:\n");
            printf("  Average: O(log n)\n");
            printf("  Worst:   O(n)\n");
        } else if (cmd == 6) {
            free_tree(root);
            root = NULL;
            break;
        }
    }
}

/* tree without duplicates */
void task2(void) {
    int cmd, val;
    struct Node *found;

    root = NULL;

    printf("tree (no dup)\n\n");

    while (1) {
        printf("\n1 - Add\n2 - Print\n3 - Search\n4 - Count\n5 - Complexity\n6 - Exit\n> ");
        scanf("%d", &cmd);

        if (cmd == 1) {
            printf("Enter number: ");
            scanf("%d", &val);
            root = CreateTreeUnique(root, root, val);
        } else if (cmd == 2) {
            if (root == NULL)
                printf("Tree is empty\n");
            else
                print_tree(root, 0);
        } else if (cmd == 3) {
            printf("Enter value: ");
            scanf("%d", &val);
            found = search(root, val);
            if (found)
                printf("Found: %d\n", found->data);
            else
                printf("Not found\n");
        } else if (cmd == 4) {
            printf("Enter value: ");
            scanf("%d", &val);
            printf("Count: %d\n", count_val(root, val));
        } else if (cmd == 5) {
            printf("Search complexity:\n");
            printf("  Average: O(log n)\n");
            printf("  Worst:   O(n)\n");
        } else if (cmd == 6) {
            free_tree(root);
            root = NULL;
            break;
        }
    }
}

int main(void) {
    int choice;

    printf("lab4\n");
    printf("1 - tree (with dup)\n");
    printf("2 - tree (no dup)\n");
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: task1(); break;
        case 2: task2(); break;
        default: printf("wrong\n"); break;
    }

    return 0;
}
