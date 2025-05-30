
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

// Helper: check if file is CSV by extension
bool is_csv_file(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return dot && strcmp(dot, ".csv") == 0;
}

// Read matrix from .txt or .csv
void readMatrixFromFile(int matrix[15][15], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[512];
    for (int i = 0; i < 15; i++) {
        if (!fgets(line, sizeof(line), file)) {
            printf("Error reading matrix from file %s.\n", filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        char *token;
        int j = 0;
        if (is_csv_file(filename))
            token = strtok(line, ",\n");
        else
            token = strtok(line, " \n");
        while (token && j < 15) {
            matrix[i][j++] = atoi(token);
            if (is_csv_file(filename))
                token = strtok(NULL, ",\n");
            else
                token = strtok(NULL, " \n");
        }
        if (j != 15) {
            printf("Matrix row %d in %s does not have 15 columns.\n", i+1, filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}

// NEW: Read hospital capacities from file
void readCapacitiesFromFile(int capacities[15], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[256];
    for (int i = 0; i < 15; i++) {
        if (!fgets(line, sizeof(line), file)) {
            printf("Error reading capacities from file %s.\n", filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        capacities[i] = atoi(line);
    }
    fclose(file);
}

// Read hospital names from .txt or .csv
void readHospitalNamesFromFile(char hospital_names[15][50], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[256];
    for (int i = 0; i < 15; i++) {
        if (!fgets(line, sizeof(line), file)) {
            printf("Error reading hospital names from file %s.\n", filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        if (is_csv_file(filename)) {
            char *token = strtok(line, ",\n");
            if (token) strncpy(hospital_names[i], token, 49);
            else hospital_names[i][0] = '\0';
        } else {
            line[strcspn(line, "\n")] = '\0';
            strncpy(hospital_names[i], line, 49);
        }
        hospital_names[i][49] = '\0';
    }
    fclose(file);
}

// Prompt user for file path, use default if empty
void prompt_filepath(const char *prompt, char *out, size_t outsize, const char *def) {
    printf("%s [%s]: ", prompt, def);
    fgets(out, outsize, stdin);
    out[strcspn(out, "\n")] = '\0';
    if (strlen(out) == 0) strncpy(out, def, outsize-1);
}

struct node {
    char hospital_name[50];
    int casualtiesPresent;
    int weight;
    struct node *link;
};

typedef struct node * NODE;

// Function to insert a node at the rear of the linked list
NODE insert_rear(char hospital_name[], int casualties, int weight, NODE first) {
    NODE cur, temp;
    temp = (NODE)malloc(sizeof(struct node));
    if (temp == NULL) {
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(temp->hospital_name, hospital_name);
    temp->casualtiesPresent = casualties;
    temp->weight = weight;
    temp->link = NULL;
    if (first == NULL) {
        return temp;
    }
    cur = first;
    while (cur->link != NULL) {
        cur = cur->link;
    }
    cur->link = temp;
    return first;
}

// Function to display the linked list
void display(NODE first) {
    if (first == NULL) {
        printf("  No connected hospitals found.\n");
        return;
    }
    NODE cur = first;
    printf("  ");
    while (cur != NULL) {
        printf("%s (Casualties: %d, Road Weight: %d)", cur->hospital_name, cur->casualtiesPresent, cur->weight);
        if (cur->link != NULL)
            printf(" -> ");
        cur = cur->link;
    }
    printf("\n");
}

// Function to create an adjacency list from matrices
NODE *createAdjacencyList(int hospitals, int matrix[15][15], int casualtiesMatrix[15][15], int weights[15][15], char hospital_names[15][50]) {
    NODE *adjList = (NODE *)malloc(hospitals * sizeof(NODE));
    if (adjList == NULL) {
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < hospitals; i++) {
        adjList[i] = NULL;
        for (int j = 0; j < hospitals; j++) {
            if (matrix[i][j] != 0) {
                adjList[i] = insert_rear(hospital_names[j], casualtiesMatrix[i][j], weights[i][j], adjList[i]);
            }
        }
    }
    return adjList;
}

// Function to display the adjacency list
void displayAdjacencyList(NODE *adjList, int hospitals, char hospital_names[15][50]) {
    printf("Displaying the adjacency lists for all hospitals:\n\n");
    for (int i = 0; i < hospitals; i++) {
        printf("Hospital: %s\n", hospital_names[i]);
        printf("Connected Hospitals / Neighbors:\n");
        display(adjList[i]);
        printf("------------------------------\n\n");
    }
    printf("End of adjacency lists.\n");
}

// Function to print hospital names using hospital numbers
void printHospitalName(int hospitalNumber, char hospital_names[15][50]) {
    if (hospitalNumber >= 0 && hospitalNumber < 15) {
        printf("Hospital %d: %s\n", hospitalNumber, hospital_names[hospitalNumber]);
    } else {
        printf("Invalid hospital number.\n");
    }
}

// Function to find the shortest path using Floyd-Warshall algorithm
void findShortestPath(int hospitals, int weights[15][15], int src, int dest, char hospital_names[15][50]) {
    for (int k = 0; k < hospitals; k++) {
        for (int i = 0; i < hospitals; i++) {
            for (int j = 0; j < hospitals; j++) {
                if (weights[i][k] != INT_MAX && weights[k][j] != INT_MAX && weights[i][k] + weights[k][j] < weights[i][j]) {
                    weights[i][j] = weights[i][k] + weights[k][j];
                }
            }
        }
    }
    printf("Shortest path from %s to %s: %d\n", hospital_names[src], hospital_names[dest], weights[src][dest]);
}

// Function to print admission difficulty based on casualties
void printAdmissionDifficulty(int casualties) {
    if (casualties >= 40) {
        printf("It is very difficult to get admitted. It is better if you try another hospital...\n");
    } else if (casualties >= 30 && casualties < 40) {
        printf("You may get admitted but may have to wait...\n");
    } else if (casualties < 30) {
        printf("You may get admitted easily...\n");
    } else {
        printf("Invalid casualty value.\n");
    }
}

// Function to check if a patient ID already exists
bool isPatientIdExist(int patientId) {
    FILE *patientFile = fopen("patient_details.txt", "r");
    if (patientFile == NULL) {
        return false;
    }
    char line[256];
    while (fgets(line, sizeof(line), patientFile) != NULL) {
        int existingPatientId;
        if (sscanf(line, "Patient ID: %d", &existingPatientId) == 1) {
            if (existingPatientId == patientId) {
                fclose(patientFile);
                return true;
            }
        }
    }
    fclose(patientFile);
    return false;
}

// NEW: Check if hospital has capacity
bool hasCapacity(int hospitalIndex, int capacities[15], int casualtiesMatrix[15][15]) {
    return casualtiesMatrix[hospitalIndex][hospitalIndex] < capacities[hospitalIndex];
}

// Function to handle patient details
bool isValidBloodGroup(const char *bg) {
    const char *valid[] = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(bg, valid[i]) == 0) return true;
    }
    return false;
}

bool handlePatientDetails(int src, int nearestHospital, int averageWeight, char hospital_names[15][50], int moneyFactor, int casualtiesMatrix[15][15], int capacities[15]) {
    char name[50], bloodGroup[5], insurance[5], areaOfTreatment[50];
    int age, patientId;
    long long phoneNumber;
    char vaccinesDone;

    // Patient ID
    while (true) {
        printf("Enter unique 4-digit Patient ID: ");
        scanf("%d", &patientId);
        if (patientId >= 1000 && patientId <= 9999 && !isPatientIdExist(patientId)) break;
        if (patientId < 1000 || patientId > 9999) {
            printf("Invalid Patient ID. Must be a 4-digit number.\n");
        } else {
            printf("Patient already exists. Proceeding with hospital assignment...\n");
            return true;
        }
    }

    // Name
    while (true) {
        printf("Name: ");
        scanf("%s", name);
        if (isalpha(name[0])) break;
        printf("Invalid name. Must start with a letter.\n");
    }

    // Age
    while (true) {
        printf("Age: ");
        scanf("%d", &age);
        if (age > 0 && age <= 120) break;
        printf("Age must be between 1 and 120.\n");
    }

    // Blood Group
    while (true) {
        printf("Blood Group (e.g., A+): ");
        scanf("%s", bloodGroup);
        if (isValidBloodGroup(bloodGroup)) break;
        printf("Invalid blood group.\n");
    }

    // Vaccination
    while (true) {
        printf("Have you received both doses of the COVID-19 vaccine? (y/n): ");
        scanf(" %c", &vaccinesDone);
        if (vaccinesDone == 'y' || vaccinesDone == 'n') break;
        printf("Enter 'y' or 'n' only.\n");
    }

    // Area of Treatment
    while (true) {
        printf("Area of Treatment required: ");
        scanf("%s", areaOfTreatment);
        if (strlen(areaOfTreatment) > 0) break;
        printf("Area of treatment cannot be empty.\n");
    }

    // Insurance
    while (true) {
        printf("Insurance? (yes/no): ");
        scanf("%s", insurance);
        if (strcmp(insurance, "yes") == 0 || strcmp(insurance, "no") == 0) break;
        printf("Please enter 'yes' or 'no'.\n");
    }

    // Phone Number
    while (true) {
        printf("Phone Number: ");
        scanf("%lld", &phoneNumber);
        if (phoneNumber >= 1000000000LL && phoneNumber <= 9999999999LL) break;
        printf("Phone number must be 10 digits.\n");
    }

    // NEW: Update casualties count
    casualtiesMatrix[nearestHospital - 1][nearestHospital - 1]++;

    FILE *patientFile = fopen("patient_details.txt", "a");
    if (patientFile == NULL) {
        printf("Error opening patient details file.\n");
        return false;
    }

    fprintf(patientFile, "Name: %s\n", name);
    fprintf(patientFile, "Age: %d\n", age);
    fprintf(patientFile, "Blood Group: %s\n", bloodGroup);
    fprintf(patientFile, "Patient ID: %d\n", patientId);
    fprintf(patientFile, "Vaccines Done: %c\n", vaccinesDone);
    fprintf(patientFile, "Area of Treatment: %s\n", areaOfTreatment);
    fprintf(patientFile, "Insurance: %s\n", insurance);
    fprintf(patientFile, "Phone Number: %lld\n", phoneNumber);
    fprintf(patientFile, "Hospital Assigned: %s\n", hospital_names[nearestHospital - 1]);
    fprintf(patientFile, "Optimal Cost: %.2lf INR\n", (double)averageWeight * moneyFactor);
    fprintf(patientFile, "-----------------\n");

    fclose(patientFile);
    printf("Patient details successfully recorded.\n\n");
    return true;
}
#ifndef TEST_HOSPITAL
int main()
{
    int hospitals = 15;
    int choice;
    int near_hosp, src, dest;
    char input1;
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);
    localTime = localtime(&currentTime);
    printf("\n\nDYNAMIC AMBULANCE DISPATCH SYSTEM\n\n");
    printf("Hospitals and Casualties data are obtained on %s\n", asctime(localTime));

    char matrix_file[128] = "matrix.txt";
    char casualties_file[128] = "casualtiesMatrix.txt";
    char weights_file[128] = "weights.txt";
    char hospital_names_file[128] = "hospital_names.txt";
    // NEW: Capacities file
    char capacities_file[128] = "capacities.txt";

    // Prompt for file paths
    printf("\n--- Data File Configuration ---\n");
    prompt_filepath("Enter adjacency matrix file (.txt/.csv)", matrix_file, sizeof(matrix_file), "matrix.txt");
    prompt_filepath("Enter casualties matrix file (.txt/.csv)", casualties_file, sizeof(casualties_file), "casualtiesMatrix.txt");
    prompt_filepath("Enter weights matrix file (.txt/.csv)", weights_file, sizeof(weights_file), "weights.txt");
    prompt_filepath("Enter hospital names file (.txt/.csv)", hospital_names_file, sizeof(hospital_names_file), "hospital_names.txt");
    prompt_filepath("Enter hospital capacities file (.txt)", capacities_file, sizeof(capacities_file), "capacities.txt");
    printf("-------------------------------\n");

    int matrix[15][15];
    int casualtiesMatrix[15][15];
    int weights[15][15];
    char hospital_names[15][50];
    // NEW: Hospital capacities array
    int capacities[15];
    int moneyFactor = 500;

    // Read matrices from files
    readMatrixFromFile(matrix, matrix_file);
    readMatrixFromFile(casualtiesMatrix, casualties_file);
    readMatrixFromFile(weights, weights_file);
    readHospitalNamesFromFile(hospital_names, hospital_names_file);
    // NEW: Read capacities
    readCapacitiesFromFile(capacities, capacities_file);

    NODE *adjList = createAdjacencyList(hospitals, matrix, casualtiesMatrix, weights, hospital_names);

    for (;;) {
        // Validate choice input
        while (1) {
            printf("\nEnter your choice\n1.Finding_Hospital\n2.Print_Hospital_Name\n3.Display_Hospitals_List\n4:Exit: ");
            if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 4) break;
            printf("Invalid choice. Please select a valid option (1-4).\n");
            while (getchar() != '\n');
        }

        printf("\n");

        if (choice < 1 || choice > 4) {
            printf("Invalid choice. Please select a valid option.\n");
            continue;
        }

        switch (choice) {
        case 1:
            while (1) {
                printf("Is it a case of emergency?(y/n): \n");
                scanf(" %c", &input1);
                if (input1 == 'y' || input1 == 'n') break;
                printf("Invalid input. Please enter 'y' or 'n'.\n");
                while (getchar() != '\n');
            }

            if (input1 == 'y') {
                while (1) {
                    printf("\nSelect a number corresponding to your nearest location.\n");
                    printf("1.Rajaji nagar  2.Sahakar Nagar  3.Sanjaynagar  4.Yeshwanthpur  5.Nagarbavi\n");
                    printf("6.Bannerghatta  7.Shanti Nagar  8.Marathahalli  9.Sarjapur  10.Jayanagar\n");
                    printf("11.Bommasandra  12.Whitefield  13.Krishnarajapuram  14.Yelahanka  15.Kengeri: \n");
                    if (scanf("%d", &src) == 1 && src >= 1 && src <= hospitals) break;
                    printf("Invalid nearest location hospital number. Please enter a number between 1 and 15.\n");
                    while (getchar() != '\n');
                }

                printf("\nFinding the nearest hospital with available capacity...\n");

                // NEW: Find nearest hospital with capacity
                NODE cur = adjList[src - 1];
                int minWeight = INT_MAX;
                int nearestHospital = -1;
                int candidates[15][2]; // Store hospital index and weight
                int candidateCount = 0;

                // Collect all adjacent hospitals
                while (cur != NULL) {
                    int currentHospitalNumber = atoi(cur->hospital_name);
                    if (hasCapacity(currentHospitalNumber - 1, capacities, casualtiesMatrix)) {
                        candidates[candidateCount][0] = currentHospitalNumber;
                        candidates[candidateCount][1] = cur->weight;
                        candidateCount++;
                    }
                    printf("\nCasualty Level at Hospital %d: %d - ", currentHospitalNumber, cur->casualtiesPresent);
                    printAdmissionDifficulty(cur->casualtiesPresent);
                    cur = cur->link;
                }

                // Check self-loop
                if (weights[src - 1][src - 1] != 0 && hasCapacity(src - 1, capacities, casualtiesMatrix)) {
                    candidates[candidateCount][0] = src;
                    candidates[candidateCount][1] = weights[src - 1][src - 1];
                    candidateCount++;
                }

                // Find hospital with minimum weight
                for (int i = 0; i < candidateCount; i++) {
                    if (candidates[i][1] < minWeight) {
                        minWeight = candidates[i][1];
                        nearestHospital = candidates[i][0];
                    }
                }

                int averageWeight = minWeight;

                if (nearestHospital != -1) {
                    printf("\nNearest hospital to Region %d with capacity is Hospital %s with a road rating of %d\n", src, hospital_names[nearestHospital - 1], averageWeight);
                    printf("\nOptimal Cost: %.2lf INR\n", (double)averageWeight * moneyFactor);
                    if (handlePatientDetails(src, nearestHospital, averageWeight, hospital_names, moneyFactor, casualtiesMatrix, capacities)) {
                        printf("Patient can be admitted to the hospital.\n\n");
                    } else {
                        printf("Patient cannot be admitted due to invalid input.\n\n");
                    }
                } else {
                    printf("\nNo adjacent hospitals with available capacity found for Region %d\n", src);
                }
            } else if (input1 == 'n') {
                while (1) {
                    printf("\nSelect the number corresponding to your nearest location: \n");
                    printf("1.Rajaji nagar   2.Sahakar Nagar   3.Sanjaynagar         4.Yeshwanthpur   5.Nagarbavi\n");
                    printf("6.Bannerghatta   7.Shanti Nagar    8.Marathahalli        9.Sarjapur       10.Jayanagar\n");
                    printf("11.Bommasandra   12.Whitefield     13.Krishnarajapuram   14.Yelahanka     15.Kengeri: \n");
                    if (scanf("%d", &src) == 1 && src >= 1 && src <= 15) break;
                    printf("Invalid nearest location hospital number. Please enter a number between 1 and 15.\n");
                    while (getchar() != '\n');
                }

                while (1) {
                    printf("\nSelect the hospital you want to go: \n");
                    printf("1.Suguna_Hospital(Rajajinagar)\t\t2.Aster_CMI_Hospital(Sahakarnagar)\t3.MS_Ramaiah_Hospital(Sanjaynagar)\n");
                    printf("4.People's_Tree_Hospital(Yeshwanthpur) \t5.Fortis_Hospital(Nagarbhavi)\t\t6.Appolo_Hospital(Bannerghatta)\n");
                    printf("7.HCG_Hospital(Shantinagar)\t\t8.Cloudnine_Hospital(Marathahalli)\t9.Columbia_Asia(Sarjapur)\n");
                    printf("10.Sagar_Hospital(Jayanagar)\t\t11.Narayana_Hrudayalaya(Bommasandra)\t12.Manipal_Hospital(Whitefield)\n");
                    printf("13.Koshys_Hospital(Krishnarajapuram)\t14.Sparsh_Hospital(Yelahanka)\t\t15.BGS_Gleneagles_Hospital(Kengeri)\n");
                    if (scanf("%d", &dest) == 1 && dest >= 1 && dest <= 15) break;
                    printf("Invalid destination hospital number. Please enter a number between 1 and 15.\n");
                    while (getchar() != '\n');
                }

                // NEW: Check if destination hospital has capacity
                if (!hasCapacity(dest - 1, capacities, casualtiesMatrix)) {
                    printf("Selected hospital %s is at full capacity. Finding nearest alternative...\n", hospital_names[dest - 1]);
                    int minWeight = INT_MAX;
                    int newDest = -1;
                    NODE cur = adjList[src - 1];
                    while (cur != NULL) {
                        int currentHospitalNumber = atoi(cur->hospital_name);
                        if (hasCapacity(currentHospitalNumber - 1, capacities, casualtiesMatrix) && cur->weight < minWeight) {
                            minWeight = cur->weight;
                            newDest = currentHospitalNumber;
                        }
                        cur = cur->link;
                    }
                    if (newDest != -1) {
                        dest = newDest;
                        printf("Rerouted to Hospital %s with available capacity.\n", hospital_names[dest - 1]);
                    } else {
                        printf("No alternative hospitals with capacity found.\n");
                        break;
                    }
                }

                printf("\nFinding the optimal route: \n");

                int distance[hospitals];
                int previous[hospitals];
                bool visited[hospitals];

                for (int i = 0; i < hospitals; i++) {
                    distance[i] = INT_MAX;
                    previous[i] = -1;
                    visited[i] = false;
                }

                distance[src - 1] = 0;

                for (int count = 0; count < hospitals - 1; count++) {
                    int u = -1;
                    int minDistance = INT_MAX;

                    for (int v = 0; v < hospitals; v++) {
                        if (!visited[v] && distance[v] < minDistance) {
                            u = v;
                            minDistance = distance[v];
                        }
                    }

                    visited[u] = true;

                    NODE cur = adjList[u];
                    while (cur != NULL) {
                        int v = atoi(cur->hospital_name) - 1;
                        if (!visited[v] && distance[u] + cur->weight < distance[v]) {
                            distance[v] = distance[u] + cur->weight;
                            previous[v] = u;
                        }
                        cur = cur->link;
                    }
                }

                printf("\nOptimal route from %s to %s: \n", hospital_names[src - 1], hospital_names[dest - 1]);
                int current = dest - 1;
                int edgeCount = 0;
                int totalWeight = 0;

                while (current != -1) {
                    printf("%s", hospital_names[current]);
                    int prev = previous[current];
                    if (prev != -1) {
                        totalWeight += weights[prev][current];
                        edgeCount++;
                        printf(" <- ");
                    }
                    current = prev;
                }
                printf("\n");

                if (edgeCount > 0) {
                    double averageWeight = (double)totalWeight / edgeCount;
                    printf("\nAverage Edge Weight: %.2lf\n", averageWeight);
                    double optimalCost = averageWeight * moneyFactor;
                    printf("Optimal Cost: %.2lf INR\n", optimalCost);
                    if (handlePatientDetails(src, dest, averageWeight, hospital_names, moneyFactor, casualtiesMatrix, capacities)) {
                        printf("Patient can be admitted to the hospital.\n\n");
                    } else {
                        printf("Patient cannot be admitted due to invalid input.\n\n");
                    }
                } else {
                    printf("No direct or adjacent edge found between the source and destination.\n");
                }
            } else {
                printf("Enter valid input...\n");
            }
            break;

        case 2:
            while (1) {
                printf("Enter the hospital number to print its name: ");
                if (scanf("%d", &near_hosp) == 1 && near_hosp >= 1 && near_hosp <= hospitals) break;
                printf("Invalid hospital number. Please enter a number between 1 and %d.\n", hospitals);
                while (getchar() != '\n');
            }
            printHospitalName(near_hosp - 1, hospital_names);
            break;

        case 3:
            displayAdjacencyList(adjList, hospitals, hospital_names);
            break;

        case 4:
            // NEW: Free adjacency list memory
            for (int i = 0; i < hospitals; i++) {
                NODE cur = adjList[i];
                while (cur != NULL) {
                    NODE temp = cur;
                    cur = cur->link;
                    free(temp);
                }
            }
            free(adjList);
            return 0;

        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }
    }
   
}
 #endif
