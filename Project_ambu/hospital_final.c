#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> // For sleep/usleep

#define MAX_HOSPITALS 15

// Structure for node
struct node
{
    char hospital_name[50];
    int casualtiesPresent;
    int weight;
    int hospital_index; // 0-based index
    struct node *link;
};

typedef struct node * NODE;

// Forward declarations for refactored functions
void handle_emergency_case(int src, NODE *adjList, char hospital_names[MAX_HOSPITALS][50], int weights[MAX_HOSPITALS][MAX_HOSPITALS], int moneyFactor);
void handle_non_emergency_case(int src, int dest, NODE *adjList, char hospital_names[MAX_HOSPITALS][50], int weights[MAX_HOSPITALS][MAX_HOSPITALS], int moneyFactor, int hospitals);

// Helper: check if file is CSV by extension
bool is_csv_file(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return dot && strcmp(dot, ".csv") == 0;
}

// Read matrix from .txt or .csv
void readMatrixFromFile(int matrix[MAX_HOSPITALS][MAX_HOSPITALS], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[512];
    for (int i = 0; i < MAX_HOSPITALS; i++) {
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
        while (token && j < MAX_HOSPITALS) {
            matrix[i][j++] = atoi(token);
            if (is_csv_file(filename))
                token = strtok(NULL, ",\n");
            else
                token = strtok(NULL, " \n");
        }
        if (j != MAX_HOSPITALS) {
            printf("Matrix row %d in %s does not have %d columns.\n", i+1, filename, MAX_HOSPITALS);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}

// Read hospital names from .txt or .csv
void readHospitalNamesFromFile(char hospital_names[MAX_HOSPITALS][50], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[256];
    for (int i = 0; i < MAX_HOSPITALS; i++) {
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

// Function to insert a node at the rear of the linked list
NODE insert_rear(char hospital_name[], int casualties, int weight, int hospital_idx, NODE first)
{
    NODE cur, temp;
    temp = (NODE)malloc(sizeof(struct node));
    if (temp == NULL)
    {
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(temp->hospital_name, hospital_name);
    temp->casualtiesPresent = casualties;
    temp->weight = weight;
    temp->hospital_index = hospital_idx; // Assign the hospital index
    temp->link = NULL;

    if (first == NULL)
    {
        return temp;
    }

    cur = first;
    while (cur->link != NULL)
    {
        cur = cur->link;
    }

    cur->link = temp;
    return first;
}

// Function to display the linked list
void display(NODE first)
{
    if (first == NULL)
    {
        printf("  No connected hospitals found.\n");
        return;
    }
    NODE cur = first;
    printf("  ");
    while (cur != NULL)
    {
        printf("%s (Casualties: %d, Road Weight: %d)", cur->hospital_name, cur->casualtiesPresent, cur->weight);

        if (cur->link != NULL)
            printf(" -> ");
        
        cur = cur->link;
    }
    printf("\n");
}

// Function to create an adjacency list from matrices
NODE *createAdjacencyList(int hospitals, int matrix[MAX_HOSPITALS][MAX_HOSPITALS], int casualtiesMatrix[MAX_HOSPITALS][MAX_HOSPITALS], int weights[MAX_HOSPITALS][MAX_HOSPITALS], char hospital_names[MAX_HOSPITALS][50])
{
    NODE *adjList = (NODE *)malloc(hospitals * sizeof(NODE));

    if (adjList == NULL)
    {
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < hospitals; i++)
    {
        adjList[i] = NULL;
        for (int j = 0; j < hospitals; j++)
        {
            if (matrix[i][j] != 0)
            {
                // Pass j as the 0-based hospital_index for the neighbor node
                adjList[i] = insert_rear(hospital_names[j], casualtiesMatrix[i][j], weights[i][j], j, adjList[i]);
            }
        }
    }

    return adjList;
}

// Function to display the adjacency list
void displayAdjacencyList(NODE *adjList, int hospitals, char hospital_names[MAX_HOSPITALS][50])
{
    printf("Displaying the adjacency lists for all hospitals:\n\n");

    for (int i = 0; i < hospitals; i++)
    {
        printf("Hospital: %s\n", hospital_names[i]);
        printf("Connected Hospitals / Neighbors:\n");
        display(adjList[i]);  // Assuming this prints the adjacency list of the ith hospital
        printf("------------------------------\n\n");
    }
    printf("End of adjacency lists.\n");
}

// Function to print hospital names using hospital numbers
void printHospitalName(int hospitalNumber, char hospital_names[MAX_HOSPITALS][50])
{
    if (hospitalNumber >= 0 && hospitalNumber < MAX_HOSPITALS)
    {
        printf("Hospital %d: %s\n", hospitalNumber, hospital_names[hospitalNumber - 1]);
    }
    else
    {
        printf("Invalid hospital number.\n");
    }
}

// Function to find the shortest path using Floyd-Warshall algorithm
void findShortestPath(int hospitals, int weights[MAX_HOSPITALS][MAX_HOSPITALS], int src, int dest, char hospital_names[MAX_HOSPITALS][50])
{
    // Implement Floyd-Warshall algorithm
    for (int k = 0; k < hospitals; k++)
    {
        for (int i = 0; i < hospitals; i++)
        {
            for (int j = 0; j < hospitals; j++)
            {
                if (weights[i][k] + weights[k][j] < weights[i][j])
                {
                    weights[i][j] = weights[i][k] + weights[k][j];
                }
            }
        }
    }

    // Print the shortest path
    printf("Shortest path from %s to %s: %d\n", hospital_names[src], hospital_names[dest], weights[src][dest]);
}

// Function to print admission difficulty based on casualties
void printAdmissionDifficulty(int casualties)
{
    if (casualties >= 40)
    {
        printf("It is very difficult to get admitted. It is better if you try another hospital...\n");
    }
    else if (casualties >= 30 && casualties < 40)
    {
        printf("You may get admitted but may have to wait...\n");
    }
    else if (casualties < 30)
    {
        printf("You may get admitted easily...\n");
    }
    else
    {
        printf("Invalid casualty value.\n");
    }
}

// Function to check if a patient ID already exists
bool isPatientIdExist(int patientId)
{
    FILE *patientFile = fopen("patient_details.txt", "r");
    if (patientFile == NULL)
    {
        return false; // File doesn't exist, so the ID is not found
    }

    char line[256];
    while (fgets(line, sizeof(line), patientFile) != NULL)
    {
        int existingPatientId;
        if (sscanf(line, "Patient ID: %d", &existingPatientId) == 1)
        {
            if (existingPatientId == patientId)
            {
                fclose(patientFile);
                return true; // ID is found
            }
        }
    }

    fclose(patientFile);
    return false; // ID is not found
}

// Function to handle patient details

bool isValidBloodGroup(const char *bg) {
    const char *valid[] = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(bg, valid[i]) == 0) return true;
    }
    return false;
}

bool handlePatientDetails(int src, int nearestHospital, int averageWeight, char hospital_names[MAX_HOSPITALS][50], int moneyFactor)
{
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
        if (fgets(name, sizeof(name), stdin)) {
            name[strcspn(name, "\n")] = 0; // Remove trailing newline
            if (strlen(name) > 0 && isalpha(name[0])) {
                break;
            } else if (strlen(name) == 0) {
                printf("Name cannot be empty.\n");
            } else {
                printf("Invalid name. Must start with a letter.\n");
            }
        } else {
            printf("Error reading name. Please try again.\n");
            // Clear stdin to prevent infinite loop if fgets fails due to EOF or error
            clearerr(stdin);
            while(getchar() != '\n' && !feof(stdin) && !ferror(stdin)); // consume rest of line
        }
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
        if (fgets(areaOfTreatment, sizeof(areaOfTreatment), stdin)) {
            areaOfTreatment[strcspn(areaOfTreatment, "\n")] = 0; // Remove trailing newline
            if (strlen(areaOfTreatment) > 0) {
                break;
            } else {
                printf("Area of treatment cannot be empty.\n");
            }
        } else {
            printf("Error reading area of treatment. Please try again.\n");
            // Clear stdin to prevent infinite loop if fgets fails due to EOF or error
            clearerr(stdin);
            while(getchar() != '\n' && !feof(stdin) && !ferror(stdin)); // consume rest of line
        }
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

// Simulate ambulance movement with progress bar
void simulateAmbulanceMovement(const char* srcName, const char* destName, int steps, int delayMs) {
    printf("\nAmbulance is moving from %s to %s...\n", srcName, destName);
    printf("[                                                  ] 0%%");
    fflush(stdout);
    for (int i = 1; i <= steps; ++i) {
        usleep(delayMs * 1000); // delayMs milliseconds
        int progress = (i * 50) / steps; // 50 chars wide
        printf("\r[");
        for (int j = 0; j < 50; ++j) {
            if (j < progress) printf("#");
            else printf(" ");
        }
        printf("] %d%%", (i * 100) / steps);
        fflush(stdout);
    }
    printf("\nAmbulance has arrived at %s!\n\n", destName);
}

// Structure for ambulance
struct Ambulance {
    int id;
    int location; // 1-based hospital index
    char status[16]; // e.g., "available", "busy"
    char type[20];   // e.g., "BLS", "ALS"
};
#define MAX_AMBULANCES 20

// Read ambulances from file
// Format: AmbulanceID,CurrentHospitalIndex (1-based),Status,Type (e.g., BLS/ALS)
int readAmbulances(struct Ambulance ambulances[], int maxAmb, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    char line[128];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < maxAmb) {
        if (line[0] == '/' || line[0] == '\\' || line[0] == '\n') continue; // skip comments/empty
        int id, loc;
        char status[16];
        char type[20]; // Temp buffer for type
        int items_scanned = sscanf(line, "%d,%d,%15[^,],%19s", &id, &loc, status, type);

        if (items_scanned >= 3) { // Need at least ID, location, and status
            ambulances[count].id = id;
            ambulances[count].location = loc;
            strncpy(ambulances[count].status, status, 15);
            ambulances[count].status[15] = '\0';

            if (items_scanned == 4) {
                strncpy(ambulances[count].type, type, 19);
                ambulances[count].type[19] = '\0';
            } else {
                // If type is missing, assign "Unknown"
                strncpy(ambulances[count].type, "Unknown", 19);
                ambulances[count].type[19] = '\0';
            }
            count++;
        }
    }
    fclose(f);
    return count;
}
// Find nearest available ambulance to a given hospital
// TODO: Future enhancement: Consider patient's needs or severity to match with ambulance type.
// For now, it dispatches the nearest available ambulance regardless of its type.
int findNearestAmbulance(struct Ambulance ambulances[], int ambCount, int src, int weights[MAX_HOSPITALS][MAX_HOSPITALS]) {
    int minDist = INT_MAX, ambIdx = -1;
    for (int i = 0; i < ambCount; ++i) {
        if (strcmp(ambulances[i].status, "available") != 0) continue;
        int dist = weights[ambulances[i].location-1][src-1];
        if (dist > 0 && dist < minDist) {
            minDist = dist;
            ambIdx = i;
        }
    }
    return ambIdx;
}

// Update ambulance status and location in file
void updateAmbulanceStatus(int ambId, int newLocation, const char* newStatus, const char* filename) {
    struct Ambulance ambulances[MAX_AMBULANCES];
    int count = readAmbulances(ambulances, MAX_AMBULANCES, filename);
    FILE* f = fopen(filename, "w");
    if (!f) return;
    fprintf(f, "// Format: AmbulanceID,CurrentHospitalIndex (1-based),Status,Type\n");
    for (int i = 0; i < count; ++i) {
        if (ambulances[i].id == ambId) {
            ambulances[i].location = newLocation;
            strncpy(ambulances[i].status, newStatus, 15);
            ambulances[i].status[15] = '\0';
            // Note: The type of the ambulance is not changed by this function,
            // so we write back the existing type.
        }
        fprintf(f, "%d,%d,%s,%s\n", ambulances[i].id, ambulances[i].location, ambulances[i].status, ambulances[i].type);
    }
    fclose(f);
}


// Display all ambulances and their locations, status, and estimated time to src
void displayAllAmbulances(struct Ambulance ambulances[], int ambCount, char hospital_names[MAX_HOSPITALS][50], int src, int weights[MAX_HOSPITALS][MAX_HOSPITALS]) {
    printf("\nAmbulance Status List (all ambulances):\n");
    int amb_count_display = 0; // Renamed to avoid conflict with outer scope 'count' if any, and make it clear it's for display
    for (int i = 0; i < ambCount; ++i) {
        int delay = weights[ambulances[i].location-1][src-1];
        // Display type information as well
        printf("  Ambulance %d (%s) at %s | Status: %s | ", ambulances[i].id, ambulances[i].type, hospital_names[ambulances[i].location-1], ambulances[i].status);
        if (ambulances[i].location == src) {
            printf("Already at emergency location (0 seconds to reach)\n");
        } else if (delay >= 0 && delay < 10000) {
            printf("Estimated time to reach emergency: %d seconds\n", delay);
        } else {
            printf("Cannot reach emergency location directly\n");
        }
        amb_count_display++;
    }
    printf("Total ambulances: %d\n", amb_count_display);
}

// Function to handle emergency case (input1 == 'y')
void handle_emergency_case(int src, NODE *adjList, char hospital_names[MAX_HOSPITALS][50], int weights[MAX_HOSPITALS][MAX_HOSPITALS], int moneyFactor) {
    // Validate source hospital input
    while (1) {
        printf("\nSelect a number corresponding to your nearest location.\n");
        printf("1.Rajaji nagar  2.Sahakar Nagar  3.Sanjaynagar  4.Yeshwanthpur  5.Nagarbavi\n");
        printf("6.Bannerghatta  7.Shanti Nagar  8.Marathahalli  9.Sarjapur  10.Jayanagar\n");
        printf("11.Bommasandra  12.Whitefield  13.Krishnarajapuram  14.Yelahanka  15.Kengeri: \n");
        if (scanf("%d", &src) == 1 && src >= 1 && src <= MAX_HOSPITALS) break;
        printf("Invalid nearest location hospital number. Please enter a number between 1 and %d.\n", MAX_HOSPITALS);
        while (getchar() != '\n');
    }

    // Show all ambulances and their time delays BEFORE any hospital/casualty logic
    struct Ambulance ambulances[MAX_AMBULANCES];
    int ambCount = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
    printf("\n=== AMBULANCE LIST (before hospital selection) ===\n");
    displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights);
    printf("=== END OF AMBULANCE LIST ===\n\n");
    fflush(stdout);

    printf("\nFinding the nearest hospital possible...\n");

    // Traverse the adjacency list of the input hospital (src)
    NODE cur = adjList[src - 1];
    int minWeight = INT_MAX;
    int nearestHospital = -1;

    while (cur != NULL)
    {
        // Use cur->hospital_index (0-based)
        // Check if the current edge weight is smaller than the minimum
        if (cur->weight < minWeight)
        {
            minWeight = cur->weight;
            nearestHospital = cur->hospital_index; // Store 0-based index
        }
        // For display, use hospital_index + 1 if you want 1-based, or hospital_names[cur->hospital_index]
        printf("\nCasualty Level at Hospital %s (%d): %d - ", hospital_names[cur->hospital_index], cur->hospital_index + 1, cur->casualtiesPresent);
        printAdmissionDifficulty(cur->casualtiesPresent);

        cur = cur->link;
    }

    // Check if there is a self-loop with a smaller weight
    // src is 1-based from user input, so src-1 for 0-based array access
    if (weights[src - 1][src - 1] < minWeight)
    {
        minWeight = weights[src - 1][src - 1];
        nearestHospital = src - 1; // Store 0-based index
    }

    int averageWeight = minWeight;
    double optimalCost = averageWeight * moneyFactor;

    // nearestHospital is now 0-based
    if (nearestHospital != -1)
    {
        // Show ambulance list again BEFORE dispatch
        printf("\n=== AMBULANCE LIST (before dispatch) ===\n");
        displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights); // src is 1-based here for ambulance logic
        printf("=== END OF AMBULANCE LIST ===\n\n");
        fflush(stdout);

        // Display: src is 1-based, nearestHospital is 0-based, so use hospital_names[nearestHospital]
        // and nearestHospital + 1 for 1-based display number
        printf("\nNearest hospital to Region %d (%s) is Hospital %s (%d) with a road rating of %d\n", src, hospital_names[src-1], hospital_names[nearestHospital], nearestHospital + 1, averageWeight);
        printf("\nOptimal Cost: %.2lf INR\n", optimalCost);

        // Find and dispatch the nearest available ambulance
        // findNearestAmbulance expects src to be 1-based
        int ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights);
        if (ambIdx != -1) {
            printf("\nDispatching Ambulance %d from %s...\n", ambulances[ambIdx].id, hospital_names[ambulances[ambIdx].location-1]);
            // simulateAmbulanceMovement expects names, src-1 for source name, nearestHospital for dest name (0-based indices for hospital_names)
            simulateAmbulanceMovement(hospital_names[src-1], hospital_names[nearestHospital], 30, 80);
            // updateAmbulanceStatus expects 1-based newLocation
            updateAmbulanceStatus(ambulances[ambIdx].id, nearestHospital + 1, "busy", "ambulance_locations.txt");
        } else {
            printf("No available ambulance could be dispatched!\n");
        }
        fflush(stdout);

        // Show all ambulances and their time delays AFTER dispatch
        int ambCountAfter = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
        printf("\n=== AMBULANCE LIST (after dispatch) ===\n");
        displayAllAmbulances(ambulances, ambCountAfter, hospital_names, src, weights); // src is 1-based
        printf("=== END OF AMBULANCE LIST ===\n\n");
        fflush(stdout);

        // handlePatientDetails expects nearestHospital to be 1-based for its internal logic accessing hospital_names[nearestHospital - 1]
        if (handlePatientDetails(src, nearestHospital + 1, averageWeight, hospital_names, moneyFactor)) {
            printf("Patient can be admitted to the hospital.\n\n");
        } else {
            printf("Patient can not be admitted due to invalid input.\n\n");
        }
        fflush(stdout);
    }
    else
    {
        // src is 1-based
        printf("\nNo adjacent hospitals found for Hospital %d (%s)\n", src, hospital_names[src-1]);
    }
}

// Function to handle non-emergency case (input1 == 'n')
void handle_non_emergency_case(int src, int dest, NODE *adjList, char hospital_names[MAX_HOSPITALS][50], int weights[MAX_HOSPITALS][MAX_HOSPITALS], int moneyFactor, int hospitals) {
    // Validate source hospital input
    while (1) {
        printf("\nSelect the number corresponding to your nearest location: \n");
        printf("1.Rajaji nagar   2.Sahakar Nagar   3.Sanjaynagar         4.Yeshwanthpur   5.Nagarbavi\n");
        printf("6.Bannerghatta   7.Shanti Nagar    8.Marathahalli        9.Sarjapur       10.Jayanagar\n");
        printf("11.Bommasandra   12.Whitefield     13.Krishnarajapuram   14.Yelahanka     15.Kengeri: \n");
        if (scanf("%d", &src) == 1 && src >= 1 && src <= MAX_HOSPITALS) break;
        printf("Invalid nearest location hospital number. Please enter a number between 1 and %d.\n", MAX_HOSPITALS);
        while (getchar() != '\n');
    }

    // Show all ambulances and their time delays BEFORE hospital selection
    struct Ambulance ambulances[MAX_AMBULANCES];
    int ambCount = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
    printf("\n=== AMBULANCE LIST (before hospital selection) ===\n");
    displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights);
    printf("=== END OF AMBULANCE LIST ===\n\n");
    fflush(stdout);

    // Validate destination hospital input
    while (1) {
        printf("\nSelect the hospital you want to go: \n");
        printf("1.Suguna_Hospital(Rajajinagar)\t\t2.Aster_CMI_Hospital(Sahakarnagar)\t3.MS_Ramaiah_Hospital(Sanjaynagar)\n");
        printf("4.People's_Tree_Hospital(Yeshwanthpur) \t5.Fortis_Hospital(Nagarbhavi)\t\t6.Appolo_Hospital(Bannerghatta)\n");
        printf("7.HCG_Hospital(Shantinagar)\t\t8.Cloudnine_Hospital(Marathahalli)\t9.Columbia_Asia(Sarjapur)\n");
        printf("10.Sagar_Hospital(Jayanagar)\t\t11.Narayana_Hrudayalaya(Bommasandra)\t12.Manipal_Hospital(Whitefield)\n");
        printf("13.Koshys_Hospital(Krishnarajapuram)\t14.Sparsh_Hospital(Yelahanka)\t\t15.BGS_Gleneagles_Hospital(Kengeri)\n");
        if (scanf("%d", &dest) == 1 && dest >= 1 && dest <= MAX_HOSPITALS) break;
        printf("Invalid destination hospital number. Please enter a number between 1 and %d.\n", MAX_HOSPITALS);
        while (getchar() != '\n');
    }

    printf("\nFinding the optimal route: \n");

    // Implement Dijkstra's algorithm to find the optimal route
    int distance[hospitals];
    int previous[hospitals];
    bool visited[hospitals];

    // Initialize distances and previous nodes
    for (int i = 0; i < hospitals; i++)
    {
        distance[i] = INT_MAX; //INFINITY
        previous[i] = -1;
        visited[i] = false;
    }

    // Set distance to source to 0
    distance[src - 1] = 0;

    // Find the optimal route
    for (int count = 0; count < hospitals - 1; count++)
    {
        int u = -1;
        int minDistance = INT_MAX;

        // Select the node with the minimum distance
        for (int v = 0; v < hospitals; v++)
        {
            if (!visited[v] && distance[v] < minDistance)
            {
                u = v;
                minDistance = distance[v];
            }
        }

        // Mark the selected node as visited
        visited[u] = true;

        // Update distances of the adjacent nodes
        NODE cur = adjList[u];
        while (cur != NULL)
        {
            // Use cur->hospital_index directly as it's 0-based
            int v = cur->hospital_index;
            if (!visited[v] && distance[u] + cur->weight < distance[v])
            {
                distance[v] = distance[u] + cur->weight;
                previous[v] = u;
            }
            cur = cur->link;
        }
    }

    // Display the optimal route and average edge weight
    printf("\nOptimal route from %s to %s: \n", hospital_names[src - 1], hospital_names[dest - 1]);
    int current = dest - 1;
    int edgeCount = 0;
    int totalWeight = 0;
    int route[20];
    int routeLen = 0;
    while (current != -1)
    {
        route[routeLen++] = current;
        int prev = previous[current];
        if (prev != -1)
        {
            totalWeight += weights[prev][current];
            edgeCount++;
            printf("%s <- ", hospital_names[current]);
        }
        else {
            printf("%s", hospital_names[current]);
        }
        current = prev;
    }
    printf("\n");
    // Calculate and display the average edge weight
    if (edgeCount > 0)
    {
        double averageWeight = (double)totalWeight / edgeCount;
        printf("\nAverage Edge Weight: %.2lf\n", averageWeight);
        double optimalCost = averageWeight * moneyFactor;
        printf("Optimal Cost: %.2lf INR\n", optimalCost);

        // Find and dispatch the nearest available ambulance
        int ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights);
        if (ambIdx != -1) {
            printf("\nDispatching Ambulance %d from %s...\n", ambulances[ambIdx].id, hospital_names[ambulances[ambIdx].location-1]);
            for (int i = routeLen-1; i > 0; --i) {
                simulateAmbulanceMovement(hospital_names[route[i]], hospital_names[route[i-1]], 15, 60);
            }
            updateAmbulanceStatus(ambulances[ambIdx].id, dest, "busy", "ambulance_locations.txt");
        } else {
            printf("No available ambulance could be dispatched!\n");
        }

        // Show all ambulances and their time delays AFTER dispatch
        int ambCountAfter = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
        printf("\n=== AMBULANCE LIST (after dispatch) ===\n");
        displayAllAmbulances(ambulances, ambCountAfter, hospital_names, src, weights);
        printf("=== END OF AMBULANCE LIST ===\n\n");
        fflush(stdout);

        if (handlePatientDetails(src, dest, averageWeight, hospital_names, moneyFactor)) {
            printf("Patient can be admitted to the hospital.\n\n");
        } else {
            printf("Patient can not be admitted due to invalid input.\n\n");
        }
    }
    else
    {
        printf("No direct or adjacent edge found between the source and destination.\n");
    }
}


int main()
{
    int hospitals = MAX_HOSPITALS;
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

    // Prompt for file paths
    printf("\n--- Data File Configuration ---\n");
    prompt_filepath("Enter adjacency matrix file (.txt/.csv)", matrix_file, sizeof(matrix_file), "matrix.txt");
    prompt_filepath("Enter casualties matrix file (.txt/.csv)", casualties_file, sizeof(casualties_file), "casualtiesMatrix.txt");
    prompt_filepath("Enter weights matrix file (.txt/.csv)", weights_file, sizeof(weights_file), "weights.txt");
    prompt_filepath("Enter hospital names file (.txt/.csv)", hospital_names_file, sizeof(hospital_names_file), "hospital_names.txt");
    printf("-------------------------------\n");

    int matrix[MAX_HOSPITALS][MAX_HOSPITALS];
    int casualtiesMatrix[MAX_HOSPITALS][MAX_HOSPITALS];
    int weights[MAX_HOSPITALS][MAX_HOSPITALS];
    char hospital_names[MAX_HOSPITALS][50];
    int moneyFactor = 500;

    // Read matrices from files
    readMatrixFromFile(matrix, matrix_file);
    readMatrixFromFile(casualtiesMatrix, casualties_file);
    readMatrixFromFile(weights, weights_file);
    readHospitalNamesFromFile(hospital_names, hospital_names_file);

    NODE *adjList = createAdjacencyList(hospitals, matrix, casualtiesMatrix, weights, hospital_names);

    for (;;)
    {
        // Validate choice input
        while (1) {
            printf("\nEnter your choice\n1.Finding_Hospital\n2.Print_Hospital_Name\n3.Display_Hospitals_List\n4:Exit: ");
            if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 4) break;
            printf("Invalid choice. Please select a valid option (1-4).\n");
            while (getchar() != '\n'); // clear input buffer
        }

        printf("\n");

        if (choice < 1 || choice > 4) {
            printf("Invalid choice. Please select a valid option.\n");
            continue; // This is redundant because we validated, but keep it as is
        }

        switch (choice)
        {
        case 1:
            // Validate emergency input
            while (1) {
                printf("Is it a case of emergency?(y/n): \n");
                scanf(" %c", &input1);
                if (input1 == 'y' || input1 == 'n') break;
                printf("Invalid input. Please enter 'y' or 'n'.\n");
                while (getchar() != '\n');
            }

            if (input1 == 'y')
            {
                handle_emergency_case(src, adjList, hospital_names, weights, moneyFactor);
                break;
            }
            else if (input1 == 'n')
            {
                handle_non_emergency_case(src, dest, adjList, hospital_names, weights, moneyFactor, hospitals);
                break;
            }
            else
            {
                printf("Enter valid input...\n");
            }
            break;

        case 2:
            // Validate near_hosp input
            while (1) {
                printf("Enter the hospital number to print its name: ");
                if (scanf("%d", &near_hosp) == 1 && near_hosp >= 1 && near_hosp <= hospitals) break;
                printf("Invalid hospital number. Please enter a number between 1 and %d.\n", hospitals);
                while (getchar() != '\n');
            }
            printHospitalName(near_hosp, hospital_names);
            break;

        case 3:
            displayAdjacencyList(adjList, hospitals, hospital_names);
            break;

        case 4:
            return 0;

        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }
    }
}

