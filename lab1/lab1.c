#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STUDENTS 100
#define DB_FILE "students.dat"

struct student {
    char famil[20];
    char name[20];
    char facult[20];
    int Nomzach;
};

int load_students(struct student *stud) {
    FILE *f;
    int count = 0;
    f = fopen(DB_FILE, "r");
    if (f == NULL) return 0;
    while (count < MAX_STUDENTS &&
           fscanf(f, "%19s %19s %19s %d",
                  stud[count].famil, stud[count].name,
                  stud[count].facult, &stud[count].Nomzach) == 4) {
        count++;
    }
    fclose(f);
    return count;
}

void save_students(struct student *stud, int count) {
    FILE *f;
    int i;
    f = fopen(DB_FILE, "w");
    if (f == NULL) {
        printf("Error saving database\n");
        return;
    }
    for (i = 0; i < count; i++) {
        fprintf(f, "%s %s %s %d\n",
                stud[i].famil, stud[i].name,
                stud[i].facult, stud[i].Nomzach);
    }
    fclose(f);
    printf("Saved %d students to %s\n", count, DB_FILE);
}

void task1(void) {
    int a[10];
    int i, min, max;

    srand(time(NULL));
    printf("Array: ");
    for (i = 0; i < 10; i++) {
        a[i] = rand() % 101 - 50;
        printf("%4d", a[i]);
    }

    min = a[0];
    max = a[0];
    for (i = 1; i < 10; i++) {
        if (a[i] < min) min = a[i];
        if (a[i] > max) max = a[i];
    }

    printf("\nMax: %d\n", max);
    printf("Min: %d\n", min);
    printf("Difference: %d\n", max - min);
}

void task2(void) {
    int a[10];
    int i;

    srand(time(NULL));
    printf("Random array [0, 99]:\n");
    for (i = 0; i < 10; i++) {
        a[i] = rand() % 100;
        printf("%4d", a[i]);
    }
    printf("\n");
}

void task3(void) {
    int n, i;
    int *a;

    printf("Enter array size: ");
    scanf("%d", &n);

    a = (int *)malloc(n * sizeof(int));
    if (a == NULL) {
        printf("Memory allocation error\n");
        return;
    }

    srand(time(NULL));
    printf("Array of %d elements:\n", n);
    for (i = 0; i < n; i++) {
        a[i] = rand() % 101 - 50;
        printf("%4d", a[i]);
    }
    printf("\n");

    free(a);
}

void task4(void) {
    int rows, cols, i, j, sum;
    int **a;

    printf("Enter number of rows: ");
    scanf("%d", &rows);
    printf("Enter number of columns: ");
    scanf("%d", &cols);

    a = (int **)malloc(rows * sizeof(int *));
    for (i = 0; i < rows; i++)
        a[i] = (int *)malloc(cols * sizeof(int));

    srand(time(NULL));
    printf("\nMatrix %dx%d:\n", rows, cols);
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            a[i][j] = rand() % 41 - 20;
            printf("%4d", a[i][j]);
        }
        printf("\n");
    }

    printf("\nColumn sums:\n");
    for (j = 0; j < cols; j++) {
        sum = 0;
        for (i = 0; i < rows; i++)
            sum += a[i][j];
        printf("Column %d: %d\n", j + 1, sum);
    }

    for (i = 0; i < rows; i++)
        free(a[i]);
    free(a);
}

void task5(void) {
    struct student stud[MAX_STUDENTS];
    char search[20];
    int count, i, found, field, cmd;
    int search_num;
    char *val;

    count = load_students(stud);
    printf("Loaded %d students from database\n\n", count);

    while (1) {
        printf("\n1 - Add student\n");
        printf("2 - List all\n");
        printf("3 - Search\n");
        printf("4 - Save and exit\n");
        printf("Choice: ");
        scanf("%d", &cmd);

        if (cmd == 1) {
            if (count >= MAX_STUDENTS) {
                printf("Database is full\n");
                continue;
            }
            printf("  Last name: ");
            scanf("%19s", stud[count].famil);
            printf("  First name: ");
            scanf("%19s", stud[count].name);
            printf("  Faculty: ");
            scanf("%19s", stud[count].facult);
            printf("  Record book number: ");
            scanf("%d", &stud[count].Nomzach);
            count++;
            printf("Added. Total: %d\n", count);

        } else if (cmd == 2) {
            if (count == 0) {
                printf("Database is empty\n");
                continue;
            }
            for (i = 0; i < count; i++) {
                printf("%d) %s %s, faculty %s, record book %d\n",
                       i + 1, stud[i].famil, stud[i].name,
                       stud[i].facult, stud[i].Nomzach);
            }

        } else if (cmd == 3) {
            printf("Search by:\n");
            printf("1 - Last name\n");
            printf("2 - First name\n");
            printf("3 - Faculty\n");
            printf("4 - Record book number\n");
            printf("Choice: ");
            scanf("%d", &field);

            found = 0;
            if (field >= 1 && field <= 3) {
                printf("Enter search value: ");
                scanf("%19s", search);
                for (i = 0; i < count; i++) {
                    val = NULL;
                    if (field == 1) val = stud[i].famil;
                    else if (field == 2) val = stud[i].name;
                    else val = stud[i].facult;
                    if (strcmp(val, search) == 0) {
                        printf("Found: %s %s, faculty %s, record book %d\n",
                               stud[i].famil, stud[i].name,
                               stud[i].facult, stud[i].Nomzach);
                        found = 1;
                    }
                }
            } else if (field == 4) {
                printf("Enter record book number: ");
                scanf("%d", &search_num);
                for (i = 0; i < count; i++) {
                    if (stud[i].Nomzach == search_num) {
                        printf("Found: %s %s, faculty %s, record book %d\n",
                               stud[i].famil, stud[i].name,
                               stud[i].facult, stud[i].Nomzach);
                        found = 1;
                    }
                }
            }
            if (!found)
                printf("Student not found\n");

        } else if (cmd == 4) {
            save_students(stud, count);
            break;
        }
    }
}

int main(void) {
    int choice;

    printf("Lab 1 - Simple data structures\n");
    printf("1 - Max-min difference\n");
    printf("2 - Random array\n");
    printf("3 - Dynamic array\n");
    printf("4 - Matrix column sums\n");
    printf("5 - Student search\n");
    printf("Choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: task1(); break;
        case 2: task2(); break;
        case 3: task3(); break;
        case 4: task4(); break;
        case 5: task5(); break;
        default: printf("Invalid choice\n"); break;
    }

    return 0;
}
