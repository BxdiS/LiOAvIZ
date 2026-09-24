#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    char inf[256];
    int priority;
    struct node *next;
};

struct node *head = NULL;
struct node *last = NULL;

struct node *get_struct(void) {
    struct node *p;
    char s[256];

    p = (struct node *)malloc(sizeof(struct node));
    if (p == NULL) {
        printf("Memory error\n");
        exit(1);
    }

    printf("Enter name: ");
    scanf("%255s", s);
    strcpy(p->inf, s);
    p->next = NULL;
    p->priority = 0;
    return p;
}

void review(void) {
    struct node *struc = head;
    if (head == NULL) {
        printf("List is empty\n");
        return;
    }
    while (struc) {
        printf("  %s\n", struc->inf);
        struc = struc->next;
    }
}

void review_pq(void) {
    struct node *struc = head;
    if (head == NULL) {
        printf("List is empty\n");
        return;
    }
    while (struc) {
        printf("  %s (priority %d)\n", struc->inf, struc->priority);
        struc = struc->next;
    }
}

struct node *find(char *name) {
    struct node *struc = head;
    if (head == NULL) {
        printf("List is empty\n");
        return NULL;
    }
    while (struc) {
        if (strcmp(name, struc->inf) == 0)
            return struc;
        struc = struc->next;
    }
    printf("Not found\n");
    return NULL;
}

void del(char *name) {
    struct node *struc = head;
    struct node *prev;
    int flag = 0;

    if (head == NULL) {
        printf("List is empty\n");
        return;
    }

    if (strcmp(name, struc->inf) == 0) {
        flag = 1;
        head = struc->next;
        free(struc);
        struc = head;
    } else {
        prev = struc;
        struc = struc->next;
    }

    while (struc) {
        if (strcmp(name, struc->inf) == 0) {
            flag = 1;
            if (struc->next) {
                prev->next = struc->next;
                free(struc);
                struc = prev->next;
            } else {
                prev->next = NULL;
                last = prev;
                free(struc);
                return;
            }
        } else {
            prev = struc;
            struc = struc->next;
        }
    }

    if (flag == 0)
        printf("Not found\n");
}

void free_list(void) {
    struct node *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
    last = NULL;
}

/* priority queue */

void pq_add(void) {
    struct node *p;
    struct node *cur, *prev;

    p = get_struct();
    if (p == NULL) return;

    printf("Enter priority: ");
    scanf("%d", &p->priority);

    if (head == NULL || p->priority > head->priority) {
        p->next = head;
        head = p;
        if (last == NULL) last = p;
        return;
    }

    prev = head;
    cur = head->next;
    while (cur && cur->priority >= p->priority) {
        prev = cur;
        cur = cur->next;
    }
    prev->next = p;
    p->next = cur;
    if (cur == NULL) last = p;
}

void task1(void) {
    int cmd;
    char name[256];
    struct node *found;

    head = NULL;
    last = NULL;

    printf("priority queue\n\n");

    while (1) {
        printf("\n1 - Add\n2 - View\n3 - Find\n4 - Delete\n5 - Exit\n> ");
        scanf("%d", &cmd);

        if (cmd == 1) {
            pq_add();
        } else if (cmd == 2) {
            review_pq();
        } else if (cmd == 3) {
            printf("Enter name: ");
            scanf("%255s", name);
            found = find(name);
            if (found)
                printf("Found: %s (priority %d)\n", found->inf, found->priority);
        } else if (cmd == 4) {
            printf("Enter name: ");
            scanf("%255s", name);
            del(name);
        } else if (cmd == 5) {
            free_list();
            break;
        }
    }
}

/* queue (FIFO) */

void queue_add(void) {
    struct node *p;

    p = get_struct();
    if (p == NULL) return;

    if (head == NULL) {
        head = p;
        last = p;
    } else {
        last->next = p;
        last = p;
    }
}

void queue_remove(void) {
    struct node *tmp;

    if (head == NULL) {
        printf("Queue is empty\n");
        return;
    }

    printf("Removed: %s\n", head->inf);
    tmp = head;
    head = head->next;
    free(tmp);
    if (head == NULL) last = NULL;
}

void task2(void) {
    int cmd;
    char name[256];
    struct node *found;

    head = NULL;
    last = NULL;

    printf("queue (FIFO)\n\n");

    while (1) {
        printf("\n1 - Enqueue\n2 - Dequeue\n3 - View\n4 - Find\n5 - Delete\n6 - Exit\n> ");
        scanf("%d", &cmd);

        if (cmd == 1) {
            queue_add();
        } else if (cmd == 2) {
            queue_remove();
        } else if (cmd == 3) {
            review();
        } else if (cmd == 4) {
            printf("Enter name: ");
            scanf("%255s", name);
            found = find(name);
            if (found)
                printf("Found: %s\n", found->inf);
        } else if (cmd == 5) {
            printf("Enter name: ");
            scanf("%255s", name);
            del(name);
        } else if (cmd == 6) {
            free_list();
            break;
        }
    }
}

/* stack (LIFO) */

void stack_push(void) {
    struct node *p;

    p = get_struct();
    if (p == NULL) return;

    p->next = head;
    head = p;
    if (last == NULL) last = p;
}

void stack_pop(void) {
    struct node *tmp;

    if (head == NULL) {
        printf("Stack is empty\n");
        return;
    }

    printf("Popped: %s\n", head->inf);
    tmp = head;
    head = head->next;
    free(tmp);
    if (head == NULL) last = NULL;
}

void task3(void) {
    int cmd;
    char name[256];
    struct node *found;

    head = NULL;
    last = NULL;

    printf("stack (LIFO)\n\n");

    while (1) {
        printf("\n1 - Push\n2 - Pop\n3 - View\n4 - Find\n5 - Delete\n6 - Exit\n> ");
        scanf("%d", &cmd);

        if (cmd == 1) {
            stack_push();
        } else if (cmd == 2) {
            stack_pop();
        } else if (cmd == 3) {
            review();
        } else if (cmd == 4) {
            printf("Enter name: ");
            scanf("%255s", name);
            found = find(name);
            if (found)
                printf("Found: %s\n", found->inf);
        } else if (cmd == 5) {
            printf("Enter name: ");
            scanf("%255s", name);
            del(name);
        } else if (cmd == 6) {
            free_list();
            break;
        }
    }
}

int main(void) {
    int choice;

    printf("lab3\n");
    printf("1 - priority queue\n");
    printf("2 - queue (FIFO)\n");
    printf("3 - stack (LIFO)\n");
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: task1(); break;
        case 2: task2(); break;
        case 3: task3(); break;
        default: printf("wrong\n"); break;
    }

    return 0;
}
