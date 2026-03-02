#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10

typedef struct ChildNode {
    int pid;
    struct ChildNode *next;
} ChildNode;

typedef struct PCB {
    int parent; /* parent's index, -1 for root */
    ChildNode *children;
} PCB;

PCB *pcbs[MAX_PROCESSES];

void print_hierarchy() {
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (pcbs[i] != NULL) {
            printf("Process %d: parent=%d; children=", i, pcbs[i]->parent);
            ChildNode *c = pcbs[i]->children;
            if (c == NULL) {
                printf("none");
            } else {
                int first = 1;
                while (c) {
                    if (!first) printf(" ");
                    printf("%d", c->pid);
                    first = 0;
                    c = c->next;
                }
            }
            printf("\n");
        }
    }
}

void init_hierarchy() {
    /* allocate PCB[0] as root, others NULL */
    for (int i = 0; i < MAX_PROCESSES; ++i) pcbs[i] = NULL;
    pcbs[0] = malloc(sizeof(PCB));
    if (pcbs[0] == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    pcbs[0]->parent = -1;
    pcbs[0]->children = NULL;
    print_hierarchy();
}

/* append child node with pid to parent's children list */
int append_child(int parent, int child_pid) {
    ChildNode *node = malloc(sizeof(ChildNode));
    if (!node) return -1;
    node->pid = child_pid;
    node->next = NULL;
    ChildNode **cur = &pcbs[parent]->children;
    while (*cur) cur = &((*cur)->next);
    *cur = node;
    return 0;
}

void destroy_descendants_recursive(ChildNode *list) {
    if (!list) return;
    ChildNode *next = list->next;
    int q = list->pid;
    /* first destroy q's children recursively */
    if (pcbs[q]) {
        destroy_descendants_recursive(pcbs[q]->children);
        /* free pcb q */
        free(pcbs[q]);
        pcbs[q] = NULL;
    }
    free(list);
    destroy_descendants_recursive(next);
}

void create_child() {
    int p;
    if (scanf("%d", &p) != 1) return;
    if (p < 0 || p >= MAX_PROCESSES || pcbs[p] == NULL) {
        printf("Process %d does not exist\n", p);
        return;
    }
    int q = -1;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (pcbs[i] == NULL) { q = i; break; }
    }
    if (q == -1) {
        printf("No more available PCBs\n");
        return;
    }
    pcbs[q] = malloc(sizeof(PCB));
    if (!pcbs[q]) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    pcbs[q]->parent = p;
    pcbs[q]->children = NULL;
    append_child(p, q);
    print_hierarchy();
}

void destroy_descendants_of_parent() {
    int p;
    if (scanf("%d", &p) != 1) return;
    if (p < 0 || p >= MAX_PROCESSES || pcbs[p] == NULL) {
        printf("Process %d does not exist\n", p);
        return;
    }
    /* destroy all descendants (children, grandchildren, etc.) of p */
    if (pcbs[p]->children != NULL) {
        destroy_descendants_recursive(pcbs[p]->children);
        pcbs[p]->children = NULL;
    }
    print_hierarchy();
}

void free_all() {
    /* destroy children of root if any */
    if (pcbs[0]) {
        if (pcbs[0]->children) {
            destroy_descendants_recursive(pcbs[0]->children);
            pcbs[0]->children = NULL;
        }
    }
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (pcbs[i]) {
            free(pcbs[i]);
            pcbs[i] = NULL;
        }
    }
}

int main() {
    for (int i = 0; i < MAX_PROCESSES; ++i) pcbs[i] = NULL;
    int choice;
    while (scanf("%d", &choice) == 1) {
        switch (choice) {
            case 1:
                init_hierarchy();
                break;
            case 2:
                create_child();
                break;
            case 3:
                destroy_descendants_of_parent();
                break;
            case 4:
                free_all();
                return 0;
            default:
                /* ignore unknown choices */
                break;
        }
    }
    free_all();
    return 0;
}
