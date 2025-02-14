#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Implementation of B+ Tree Using Employee management System.

#define ORDER 4

typedef struct Employee {
    int id;
    char name[100];
    char department[50];
} Employee;

typedef struct Node {
    bool isLeaf;
    int numKeys;
    int keys[ORDER - 1];
    struct Node *children[ORDER];
    Employee *records[ORDER - 1];
    struct Node *next;
} Node;

typedef struct BPlusTree {
    Node *root;
} BPlusTree;

BPlusTree *initializeTree();
Node *createNode(bool isLeaf);
void insert(BPlusTree *tree, int id, char *name, char *department);
void splitChild(Node *parent, int index, Node *child);
void insertNonFull(Node *node, int id, Employee *record);
Employee *search(BPlusTree *tree, int id);
void display(Node *node);
void displayRange(Node *node, int startID, int endID);
void deleteEmployee(BPlusTree *tree, int id);
void freeTree(Node *node);

BPlusTree *initializeTree() {
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    tree->root = createNode(true);
    return tree;
}

Node *createNode(bool isLeaf) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->isLeaf = isLeaf;
    node->numKeys = 0;
    node->next = NULL;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void insert(BPlusTree *tree, int id, char *name, char *department) {
    Node *root = tree->root;

    Employee *newEmployee = (Employee *)malloc(sizeof(Employee));
    newEmployee->id = id;
    strncpy(newEmployee->name, name, 99);
    newEmployee->name[99] = '\0';
    strncpy(newEmployee->department, department, 49);
    newEmployee->department[49] = '\0';

    if (root->numKeys == ORDER - 1) {
        Node *newRoot = createNode(false);
        tree->root = newRoot;
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFull(newRoot, id, newEmployee);
    } else {
        insertNonFull(root, id, newEmployee);
    }
}

void splitChild(Node *parent, int index, Node *child) {
    Node *newChild = createNode(child->isLeaf);
    newChild->numKeys = (ORDER - 1) / 2;

    for (int i = 0; i < (ORDER - 1) / 2; i++) {
        newChild->keys[i] = child->keys[i + (ORDER + 1) / 2];
        if (child->isLeaf) {
            newChild->records[i] = child->records[i + (ORDER + 1) / 2];
        }
    }

    if (!child->isLeaf) {
        for (int i = 0; i <= (ORDER - 1) / 2; i++) {
            newChild->children[i] = child->children[i + (ORDER + 1) / 2];
        }
    }

    child->numKeys = (ORDER - 1) / 2;

    for (int i = parent->numKeys; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = newChild;

    for (int i = parent->numKeys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = child->keys[(ORDER - 1) / 2];

    parent->numKeys++;

    if (child->isLeaf) {
        newChild->next = child->next;
        child->next = newChild;
    }
}

void insertNonFull(Node *node, int id, Employee *record) {
    int i = node->numKeys - 1;

    if (node->isLeaf) {
        while (i >= 0 && id < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->records[i + 1] = node->records[i];
            i--;
        }
        node->keys[i + 1] = id;
        node->records[i + 1] = record;
        node->numKeys++;
    } else {
        while (i >= 0 && id < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->numKeys == ORDER - 1) {
            splitChild(node, i, node->children[i]);
            if (id > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], id, record);
    }
}

Employee *search(BPlusTree *tree, int id) {
    Node *node = tree->root;
    while (node != NULL) {
        int i = 0;
        while (i < node->numKeys && id > node->keys[i]) {
            i++;
        }
        if (i < node->numKeys && id == node->keys[i]) {
            return node->isLeaf ? node->records[i] : NULL;
        }
        node = node->isLeaf ? NULL : node->children[i];
    }
    return NULL;
}

void display(Node *node) {
    if (node == NULL) return;

    if (node->isLeaf) {
        for (int i = 0; i < node->numKeys; i++) {
            printf("ID: %d, Name: %s, Department: %s\n", node->records[i]->id, node->records[i]->name, node->records[i]->department);
        }
    } else {
        for (int i = 0; i < node->numKeys; i++) {
            display(node->children[i]);
            printf("%d ", node->keys[i]);
        }
        display(node->children[node->numKeys]);
    }
}

void displayRange(Node *node, int startID, int endID) {
    if (node == NULL) return;

    if (node->isLeaf) {
        for (int i = 0; i < node->numKeys; i++) {
            if (node->keys[i] >= startID && node->keys[i] <= endID) {
                printf("ID: %d, Name: %s, Department: %s\n", node->records[i]->id, node->records[i]->name, node->records[i]->department);
            }
        }
    } else {
        for (int i = 0; i <= node->numKeys; i++) {
            displayRange(node->children[i], startID, endID);
        }
    }
}

void deleteEmployee(BPlusTree *tree, int id) {
    Node *node = tree->root;
    while (node != NULL) {
        int i = 0;
        while (i < node->numKeys && id > node->keys[i]) {
            i++;
        }
        if (i < node->numKeys && id == node->keys[i]) {
            if (node->isLeaf) {
                for (int j = i; j < node->numKeys - 1; j++) {
                    node->keys[j] = node->keys[j + 1];
                    node->records[j] = node->records[j + 1];
                }
                node->numKeys--;
                printf("Employee with ID %d deleted successfully.\n", id);
                return;
            } else {
                printf("Deletion for internal nodes is not implemented.\n");
                return;
            }
        }
        node = node->isLeaf ? NULL : node->children[i];
    }
    printf("Employee with ID %d not found.\n", id);
}

void freeTree(Node *node) {
    if (node == NULL) return;

    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; i++) {
            freeTree(node->children[i]);
        }
    }
    free(node);
}

int main() {
    BPlusTree *tree = initializeTree();
    int choice, id, startID, endID;
    char name[100], department[50];

    while (1) {
        printf("\nEmployee Management System using B+ Tree\n");
        printf("1. Insert Employee\n");
        printf("2. Search Employee\n");
        printf("3. Display All Employees\n");
        printf("4. Display Employees in Range\n");
        printf("5. Delete Employee\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter Employee ID: ");
                scanf("%d", &id);
                printf("Enter Employee Name: ");
                getchar();
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                printf("Enter Employee Department: ");
                scanf("%s", department);
                insert(tree, id, name, department);
                break;

            case 2:
                printf("Enter Employee ID to Search: ");
                scanf("%d", &id);
                Employee *emp = search(tree, id);
                if (emp) {
                    printf("Employee Found - ID: %d, Name: %s, Department: %s\n", emp->id, emp->name, emp->department);
                } else {
                    printf("Employee Not Found\n");
                }
                break;

            case 3:
                printf("All Employees:\n");
                display(tree->root);
                break;

            case 4:
                printf("Enter Start ID: ");
                scanf("%d", &startID);
                printf("Enter End ID: ");
                scanf("%d", &endID);
                printf("Employees in Range [%d, %d]:\n", startID, endID);
                displayRange(tree->root, startID, endID);
                break;

            case 5:
                printf("Enter Employee ID to Delete: ");
                scanf("%d", &id);
                deleteEmployee(tree, id);
                break;

            case 6:
                freeTree(tree->root);
                free(tree);
                printf("Exiting the system...\n");
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
