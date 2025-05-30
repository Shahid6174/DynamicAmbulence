#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

// Include necessary structs and function declarations from hospital_routing.c
struct node {
    char hospital_name[50];
    int casualtiesPresent;
    int weight;
    struct node *link;
};
typedef struct node * NODE;

// Function declarations
NODE insert_rear(char hospital_name[], int casualties, int weight, NODE first);
NODE *createAdjacencyList(int hospitals, int matrix[15][15], int casualtiesMatrix[15][15], int weights[15][15], char hospital_names[15][50]);
bool hasCapacity(int hospitalIndex, int capacities[15], int casualtiesMatrix[15][15]);

// Test data for 15 hospitals
#define HOSPITALS 15
int test_matrix[15][15] = {
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0}
};
int test_casualties[15][15] = {
    {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15}
};
int test_weights[15][15] = {
    {9999, 5, 10, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999},
    {5, 9999, 8, 12, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999},
    {10, 8, 9999, 9999, 15, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999},
    {9999, 12, 9999, 9999, 10, 20, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999},
    {9999, 9999, 15, 10, 9999, 9999, 18, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999},
    {9999, 9999, 9999, 20, 9999, 9999, 25, 30, 9999, 9999, 9999, 9999, 9999, 9999, 9999},
    {9999, 9999, 9999, 9999, 18, 25, 9999, 9999, 22, 9999, 9999, 9999, 9999, 9999, 9999},
    {9999, 9999, 9999, 9999, 9999, 30, 9999, 9999, 15, 28, 9999, 9999, 9999, 9999, 9999},
    {9999, 9999, 9999, 9999, 9999, 9999, 22, 15, 9999, 9999, 20, 9999, 9999, 9999, 9999},
    {9999, 9999, 9999, 9999, 9999, 9999, 9999, 28, 9999, 9999, 10, 25, 9999, 9999, 9999},
    {9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 20, 10, 9999, 9999, 30, 9999, 9999},
    {9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 25, 9999, 9999, 12, 18, 9999},
    {9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 30, 12, 9999, 9999, 15},
    {9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 18, 9999, 9999, 20},
    {9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 15, 20, 9999}
};
int test_capacities[15] = {50, 60, 45, 70, 55, 65, 40, 75, 50, 60, 80, 55, 70, 45, 60};
char test_hospital_names[15][50] = {
    "Suguna_Hospital",
    "Aster_CMI_Hospital",
    "MS_Ramaiah_Hospital",
    "People's_Tree_Hospital",
    "Fortis_Hospital",
    "Appolo_Hospital",
    "HCG_Hospital",
    "Cloudnine_Hospital",
    "Columbia_Asia",
    "Sagar_Hospital",
    "Narayana_Hrudayalaya",
    "Manipal_Hospital",
    "Koshys_Hospital",
    "Sparsh_Hospital",
    "BGS_Gleneagles_Hospital"
};

// Test Case 1: Nearest hospital has capacity
void test_hospitalSelection_nearest_available() {
    int casualties[15][15];
    memcpy(casualties, test_casualties, sizeof(test_casualties));
    NODE *adjList = createAdjacencyList(HOSPITALS, test_matrix, casualties, test_weights, test_hospital_names);

    int minWeight = INT_MAX, nearestHospital = -1;
    NODE cur = adjList[0]; // Source hospital 0
    while (cur != NULL) {
        int hospitalIndex = -1;
        for (int i = 0; i < HOSPITALS; i++) {
            if (strcmp(cur->hospital_name, test_hospital_names[i]) == 0) {
                hospitalIndex = i;
                break;
            }
        }
        if (hasCapacity(hospitalIndex, test_capacities, casualties) && cur->weight < minWeight) {
            minWeight = cur->weight;
            nearestHospital = hospitalIndex + 1;
        }
        cur = cur->link;
    }

    // Hospital 0 is connected to 2 (weight 5) and 3 (weight 10), both have capacity
    // Should select hospital 2 (Aster_CMI_Hospital, weight 5)
    assert(nearestHospital == 2);
    assert(minWeight == 5);

    // Free memory
    for (int i = 0; i < HOSPITALS; i++) {
        NODE cur = adjList[i];
        while (cur) {
            NODE temp = cur;
            cur = cur->link;
            free(temp);
        }
    }
    free(adjList);
    printf("test_hospitalSelection_nearest_available passed!\n");
}

// Test Case 2: Nearest hospital full, reroute to next nearest
void test_hospitalSelection_reroute() {
    int casualties[15][15];
    memcpy(casualties, test_casualties, sizeof(test_casualties));
    casualties[1][1] = 60; // Set hospital 2 (Aster_CMI_Hospital) to full capacity
    NODE *adjList = createAdjacencyList(HOSPITALS, test_matrix, casualties, test_weights, test_hospital_names);

    int minWeight = INT_MAX, nearestHospital = -1;
    NODE cur = adjList[0]; // Source hospital 0
    while (cur != NULL) {
        int hospitalIndex = -1;
        for (int i = 0; i < HOSPITALS; i++) {
            if (strcmp(cur->hospital_name, test_hospital_names[i]) == 0) {
                hospitalIndex = i;
                break;
            }
        }
        if (hasCapacity(hospitalIndex, test_capacities, casualties) && cur->weight < minWeight) {
            minWeight = cur->weight;
            nearestHospital = hospitalIndex + 1;
        }
        cur = cur->link;
    }

    // Hospital 2 is full (60/60), should reroute to hospital 3 (MS_Ramaiah_Hospital, weight 10)
    assert(nearestHospital == 3);
    assert(minWeight == 10);

    // Free memory
    for (int i = 0; i < HOSPITALS; i++) {
        NODE cur = adjList[i];
        while (cur) {
            NODE temp = cur;
            cur = cur->link;
            free(temp);
        }
    }
    free(adjList);
    printf("test_hospitalSelection_reroute passed!\n");
}

// Test Case 3: All adjacent hospitals full
void test_hospitalSelection_no_capacity() {
    int casualties[15][15];
    memcpy(casualties, test_casualties, sizeof(test_casualties));
    casualties[1][1] = 60; // Hospital 2 full
    casualties[2][2] = 45; // Hospital 3 full
    NODE *adjList = createAdjacencyList(HOSPITALS, test_matrix, casualties, test_weights, test_hospital_names);

    int minWeight = INT_MAX, nearestHospital = -1;
    NODE cur = adjList[0]; // Source hospital 0
    while (cur != NULL) {
        int hospitalIndex = -1;
        for (int i = 0; i < HOSPITALS; i++) {
            if (strcmp(cur->hospital_name, test_hospital_names[i]) == 0) {
                hospitalIndex = i;
                break;
            }
        }
        if (hasCapacity(hospitalIndex, test_capacities, casualties) && cur->weight < minWeight) {
            minWeight = cur->weight;
            nearestHospital = hospitalIndex + 1;
        }
        cur = cur->link;
    }

    // All adjacent hospitals (2 and 3) are full, no hospital selected
    assert(nearestHospital == -1);
    assert(minWeight == INT_MAX);

    // Free memory
    for (int i = 0; i < HOSPITALS; i++) {
        NODE cur = adjList[i];
        while (cur) {
            NODE temp = cur;
            cur = cur->link;
            free(temp);
        }
    }
    free(adjList);
    printf("test_hospitalSelection_no_capacity passed!\n");
}

int main() {
    printf("Running hospital selection tests...\n");
    test_hospitalSelection_nearest_available();
    test_hospitalSelection_reroute();
    test_hospitalSelection_no_capacity();
    printf("All hospital selection tests passed successfully!\n");
    return 0;
}