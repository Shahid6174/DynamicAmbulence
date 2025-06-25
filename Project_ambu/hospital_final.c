#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> // For sleep/usleep
#include <sys/stat.h>
#include <errno.h>

#define AMBULANCE_BUSY_TIME 5
#define MAX_LINE 1000
#define MAX_PATIENTS 100
#define MAX_FIELD_LEN 100
#define FILENAME "patient_details.txt"
#define MAX_PHONE_NUMBER_LEN 15 
#define MIN_FUEL_THRESHOLD 20  // Minimum fuel percentage required for dispatch
#define REFUEL_TIME_MS 3000    // Time taken to refuel (3 seconds)
#define MAX_PHONE_NUMBER_LEN 15

char current_patient[10]; // Global variable to store current patient ID

struct AvailableLaterArgs {
    int ambId;
    int hospital;
    char filename[128];
};


// Function to get a specific patient parameter by ID
float get_patient_param(const char* patient_id, const char* param) {
    FILE *file = fopen("patient_details.txt", "r"); 
    if (!file) {
        printf("Could not open file.\n");
        return -1.0f;
    }

    char line[256];
    int found = 0;

    // Find the patient block
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Patient ID:", 11) == 0) {
            char id[32];
            sscanf(line, "Patient ID: %s", id);
            if (strcmp(id, patient_id) == 0) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        fclose(file);
        return -1.0f; // Patient not found
    }

    // Now, search for the parameter in the next lines
    float result = -1.0f;
    size_t param_len = strlen(param);
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "-----------------", 17) == 0) {
            break; // End of this patient's block
        }
        if (strncmp(line, param, param_len) == 0 && line[param_len] == ':') {
            // Found the parameter
            char *value_str = line + param_len + 1;
            while (*value_str == ' ') value_str++; // Skip spaces
            // Try to extract float from the value
            result = strtof(value_str, NULL);
            break;
        }
    }

    fclose(file);
    return result;
}


// Function to set a specific patient parameter by ID
int set_patient_param(const char* patient_id, const char* param, float new_value) {
    FILE *file = fopen("patient_details.txt", "r");
    if (!file) {
        printf("Could not open file for reading.\n");
        return 0;
    }

    FILE *temp = fopen("temp_patient_details.txt", "w");
    if (!temp) {
        printf("Could not open temp file for writing.\n");
        fclose(file);
        return 0;
    }

    char line[256];
    int found_patient = 0, updated = 0;
    size_t param_len = strlen(param);

    while (fgets(line, sizeof(line), file)) {
        // Check for patient block
        if (strncmp(line, "Patient ID:", 11) == 0) {
            char id[32];
            sscanf(line, "Patient ID: %s", id);
            found_patient = (strcmp(id, patient_id) == 0);
        }

        // If in the correct patient block, look for the parameter
        if (found_patient && strncmp(line, param, param_len) == 0 && line[param_len] == ':') {
            // Write the updated parameter line with float value
            fprintf(temp, "%s: %.2f\n", param, new_value);
            updated = 1;
            found_patient = 0; // Only update the first occurrence in the block
        } else {
            // Write the line as is
            fputs(line, temp);
        }

        // End of patient block
        if (found_patient && strncmp(line, "-----------------", 17) == 0) {
            found_patient = 0;
        }
    }

    fclose(file);
    fclose(temp);

    // Replace original file with updated file
    if (updated) {
        remove("patient_details.txt");
        rename("temp_patient_details.txt", "patient_details.txt");
    } else {
        remove("temp_patient_details.txt");
    }

    return updated;
}


// Helper: check if file is CSV by extension
bool is_csv_file(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    return dot && strcmp(dot, ".csv") == 0;
}

// Read matrix from .txt or .csv
void readMatrixFromFile(int matrix[15][15], const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[512];
    for (int i = 0; i < 15; i++)
    {
        if (!fgets(line, sizeof(line), file))
        {
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
        while (token && j < 15)
        {
            matrix[i][j++] = atoi(token);
            if (is_csv_file(filename))
                token = strtok(NULL, ",\n");
            else
                token = strtok(NULL, " \n");
        }
        if (j != 15)
        {
            printf("Matrix row %d in %s does not have 15 columns.\n", i + 1, filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}

// Read hospital names from .txt or .csv
void readHospitalNamesFromFile(char hospital_names[15][50], const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[256];
    for (int i = 0; i < 15; i++)
    {
        if (!fgets(line, sizeof(line), file))
        {
            printf("Error reading hospital names from file %s.\n", filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        if (is_csv_file(filename))
        {
            char *token = strtok(line, ",\n");
            if (token)
                strncpy(hospital_names[i], token, 49);
            else
                hospital_names[i][0] = '\0';
        }
        else
        {
            line[strcspn(line, "\n")] = '\0';
            strncpy(hospital_names[i], line, 49);
        }
        hospital_names[i][49] = '\0';
    }
    fclose(file);
}

// Prompt user for file path, use default if empty
void prompt_filepath(const char *prompt, char *out, size_t outsize, const char *def)
{
    printf("%s [%s]: ", prompt, def);
    fgets(out, outsize, stdin);
    out[strcspn(out, "\n")] = '\0';
    if (strlen(out) == 0)
        strncpy(out, def, outsize - 1);
}

struct node
{
    char hospital_name[50];
    int casualtiesPresent;
    int weight;
    struct node *link;
};

typedef struct node *NODE;

// Add Patient Record Structure
struct Patient
{
    int id;
    char name[50];
    int age;
    char bloodGroup[5];
    char gender;
    char address[100];
    char condition[100]; // e.g., "critical", "stable"
    char vaccinesDone;
    char areaOfTreatment[50];
    char insurance[5];
    char phoneNumber[MAX_PHONE_NUMBER_LEN]; // Changed to string for easier handling
    char hospitalAssigned[50];
    double optimalCost;
    float severity;
    float current_treatment_cost;
    float total_expenditure;
};

// Function to insert a node at the rear of the linked list
NODE insert_rear(char hospital_name[], int casualties, int weight, NODE first)
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
void display(NODE first) {
    if (first == NULL) {
        printf("  No connected hospitals found.\n");
        return;
    }
    NODE cur = first;
    while (cur != NULL) {
        printf("%s (Casualties: %d, Road Weight: %d)", 
               cur->hospital_name, cur->casualtiesPresent, cur->weight);
        if (cur->link != NULL)
            printf(" -> ");
        cur = cur->link;
    }
    printf("\n");
}

// Function to create an adjacency list from matrices
NODE *createAdjacencyList(int hospitals, int matrix[15][15], int casualtiesMatrix[15][15], int weights[15][15], char hospital_names[15][50])
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
                adjList[i] = insert_rear(hospital_names[j], casualtiesMatrix[i][j], weights[i][j], adjList[i]);
            }
        }
    }

    return adjList;
}

// Update the displayAdjacencyList call
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
void printHospitalName(int hospitalNumber, char hospital_names[15][50])
{
    if (hospitalNumber >= 0 && hospitalNumber < 15)
    {
        printf("Hospital %d: %s\n", hospitalNumber, hospital_names[hospitalNumber - 1]);
    }
    else
    {
        printf("Invalid hospital number.\n");
    }
}

// Function to find the shortest path using Floyd-Warshall algorithm
void findShortestPath(int hospitals, int weights[15][15], int src, int dest, char hospital_names[15][50])
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

bool isValidBloodGroup(const char *bg)
{
    const char *valid[] = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    for (int i = 0; i < 8; i++)
    {
        if (strcmp(bg, valid[i]) == 0)
            return true;
    }
    return false;
}

bool handlePatientDetails(int src, int nearestHospital, int averageWeight, char hospital_names[15][50], int moneyFactor, int severity)
{
    char name[50], bloodGroup[5], insurance[5], areaOfTreatment[50];
    int age, patientId;
    long long phoneNumber;
    char vaccinesDone;

    // Patient ID
    while (true)
    {
        printf("Enter unique 4-digit Patient ID: ");
        scanf("%d", &patientId);
        if (patientId >= 1000 && patientId <= 9999 && !isPatientIdExist(patientId))
            break;
        if (patientId < 1000 || patientId > 9999)
        {
            printf("Invalid Patient ID. Must be a 4-digit number.\n");
        }
        else
        {
            printf("Patient already exists. Proceeding with hospital assignment...\n");
            sprintf(current_patient, "%d", patientId);; // Set current patient ID for further operations
            return true;
        }
    }

    // Name
    while (true)
    {
        printf("Name: ");
        scanf("%s", name);
        if (isalpha(name[0]))
            break;
        printf("Invalid name. Must start with a letter.\n");
    }

    // Age
    while (true)
    {
        printf("Age: ");
        scanf("%d", &age);
        if (age > 0 && age <= 120)
            break;
        printf("Age must be between 1 and 120.\n");
    }

    // Blood Group
    while (true)
    {
        printf("Blood Group (e.g., A+): ");
        scanf("%s", bloodGroup);
        if (isValidBloodGroup(bloodGroup))
            break;
        printf("Invalid blood group.\n");
    }

    // Vaccination
    while (true)
    {
        printf("Have you received both doses of the COVID-19 vaccine? (y/n): ");
        scanf(" %c", &vaccinesDone);
        if (vaccinesDone == 'y' || vaccinesDone == 'n')
            break;
        printf("Enter 'y' or 'n' only.\n");
    }

    // Area of Treatment
    while (true)
    {
        printf("Area of Treatment required: ");
        scanf("%s", areaOfTreatment);
        if (strlen(areaOfTreatment) > 0)
            break;
        printf("Area of treatment cannot be empty.\n");
    }

    // Insurance
    while (true)
    {
        printf("Insurance? (yes/no): ");
        scanf("%s", insurance);
        if (strcmp(insurance, "yes") == 0 || strcmp(insurance, "no") == 0)
            break;
        printf("Please enter 'yes' or 'no'.\n");
    }

    // Phone Number
    while (true)
    {
        printf("Phone Number: ");
        scanf("%lld", &phoneNumber);
        if (phoneNumber >= 1000000000LL && phoneNumber <= 9999999999LL)
            break;
        printf("Phone number must be 10 digits.\n");
    }

    FILE *patientFile = fopen("patient_details.txt", "a");
    if (patientFile == NULL)
    {
        printf("Error opening patient details file.\n");
        return false;
    }
    fprintf(patientFile, "Patient ID: %d\n", patientId);
    fprintf(patientFile, "Name: %s\n", name);
    fprintf(patientFile, "Age: %d\n", age);
    fprintf(patientFile, "Blood Group: %s\n", bloodGroup);
    fprintf(patientFile, "Vaccines Done: %c\n", vaccinesDone);
    fprintf(patientFile, "Area of Treatment: %s\n", areaOfTreatment);
    fprintf(patientFile, "Insurance: %s\n", insurance);
    fprintf(patientFile, "Phone Number: %lld\n", phoneNumber);
    fprintf(patientFile, "Hospital Assigned: %s\n", hospital_names[nearestHospital - 1]);
    fprintf(patientFile, "Optimal Cost: %.2lf INR\n", (double)averageWeight * moneyFactor);
    fprintf(patientFile, "Severity: %f\n", (float)severity);
    fprintf(patientFile, "Current_Treatment_Cost(INR): %.2lf\n", (double)averageWeight * moneyFactor * (severity == 5 ? 2.0 : (severity == 4 ? 1.5 : (severity == 3 ? 1.2 : (severity == 2 ? 1.0 : 0.8)))));
    fprintf(patientFile, "Total_expenditure(INR): %.2lf\n", (double)averageWeight * moneyFactor * (severity == 5 ? 2.0 : (severity == 4 ? 1.5 : (severity == 3 ? 1.2 : (severity == 2 ? 1.0 : 0.8)))));
    fprintf(patientFile, "-----------------\n");

    fclose(patientFile);
    printf("Patient details successfully recorded.\n\n");
    return true;
}

// Simulate ambulance movement with progress bar
void simulateAmbulanceMovement(const char *srcName, const char *destName, int steps, int delayMs)
{
    printf("\nAmbulance is moving from %s to %s...\n", srcName, destName);
    printf("[                                                  ] 0%%");
    fflush(stdout);
    for (int i = 1; i <= steps; ++i)
    {
        usleep(delayMs * 1000);          // delayMs milliseconds
        int progress = (i * 50) / steps; // 50 chars wide
        printf("\r[");
        for (int j = 0; j < 50; ++j)
        {
            if (j < progress)
                printf("#");
            else
                printf(" ");
        }
        printf("] %d%%", (i * 100) / steps);
        fflush(stdout);
    }
    printf("\nAmbulance has arrived at %s!\n\n", destName);
}

// Structure for ambulance
struct Ambulance
{
    int id;
    int location;    // 1-based hospital index
    char status[16]; // e.g., "available", "busy"
    int fuel; // Fuel level in percentage (0-100)
};
#define MAX_AMBULANCES 20
void refuelAmbulance(struct Ambulance *ambulance);
void updateAmbulanceStatus(int ambId, int newLocation, const char *newStatus, int newFuel, const char *filename);
void displayAllAmbulances(struct Ambulance ambulances[], int ambCount, char hospital_names[15][50], int src, int weights[15][15]);
int readAmbulances(struct Ambulance ambulances[], int maxAmb, const char *filename);
int findNearestAmbulance(struct Ambulance ambulances[], int ambCount, int src, int weights[15][15]);

// Read ambulances from file
int readAmbulances(struct Ambulance ambulances[], int maxAmb, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;
    char line[128];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < maxAmb) {
        if (line[0] == '/' || line[0] == '\\' || line[0] == '\n')
            continue;
        int id, loc, fuel;
        char status[16];
        if (sscanf(line, "%d,%d,%15[^,],%d", &id, &loc, status, &fuel) == 4) {
            ambulances[count].id = id;
            ambulances[count].location = loc;
            strncpy(ambulances[count].status, status, 15);
            ambulances[count].status[15] = '\0';
            ambulances[count].fuel = fuel;
            count++;
        }
    }
    fclose(f);
    return count;
}
// Find nearest available ambulance to a given hospital
int findNearestAmbulance(struct Ambulance ambulances[], int ambCount, int src, int weights[15][15]) {
    int minDist = INT_MAX, ambIdx = -1;
    for (int i = 0; i < ambCount; ++i) {
        if (strcmp(ambulances[i].status, "available") != 0 || 
            ambulances[i].fuel < MIN_FUEL_THRESHOLD)
            continue;
        int dist = weights[ambulances[i].location - 1][src - 1];
        if (dist > 0 && dist < minDist) {
            minDist = dist;
            ambIdx = i;
        }
    }
    
    if (ambIdx != -1 && ambulances[ambIdx].fuel < MIN_FUEL_THRESHOLD * 2) {
        // If fuel is low but above minimum threshold, refuel before dispatch
        refuelAmbulance(&ambulances[ambIdx]);
        updateAmbulanceStatus(ambulances[ambIdx].id, 
                            ambulances[ambIdx].location, 
                            "available", 
                            100, 
                            "ambulance_locations.txt");
    }
    
    return ambIdx;
}

// Update ambulance status and location in file
void updateAmbulanceStatus(int ambId, int newLocation, const char *newStatus, int newFuel, const char *filename) {
    struct Ambulance ambulances[MAX_AMBULANCES];
    int count = readAmbulances(ambulances, MAX_AMBULANCES, filename);
    FILE *f = fopen(filename, "w");
    if (!f)
        return;
    fprintf(f, "// Format: AmbulanceID,CurrentHospitalIndex,Status,FuelLevel\n");
    for (int i = 0; i < count; ++i) {
        if (ambulances[i].id == ambId) {
            ambulances[i].location = newLocation;
            strncpy(ambulances[i].status, newStatus, 15);
            ambulances[i].status[15] = '\0';
            ambulances[i].fuel = newFuel;
        }
        fprintf(f, "%d,%d,%s,%d\n", 
                ambulances[i].id, 
                ambulances[i].location, 
                ambulances[i].status,
                ambulances[i].fuel);
    }
    fclose(f);
}

// Display all ambulances and their locations, status, and estimated time to src
void displayAllAmbulances(struct Ambulance ambulances[], int ambCount, char hospital_names[15][50], int src, int weights[15][15]) {
    printf("\nAmbulance Status List (all ambulances):\n");
    int count = 0;
    for (int i = 0; i < ambCount; ++i) {
        int delay = weights[ambulances[i].location - 1][src - 1];
        printf("  Ambulance %d at %s | Status: %s | Fuel: %d%% | ", 
               ambulances[i].id, 
               hospital_names[ambulances[i].location - 1], 
               ambulances[i].status,
               ambulances[i].fuel);
        
        if (ambulances[i].location == src) {
            printf("Already at emergency location (0 seconds to reach)\n");
        } else if (delay >= 0 && delay < 10000) {
            printf("Estimated time to reach emergency: %d seconds\n", delay);
        } else {
            printf("Cannot reach emergency location directly\n");
        }
        count++;
    }
    printf("Total ambulances: %d\n", count);
}

struct Patient* searchPatientById(int id) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        perror("Error opening patient_details.txt for search");
        return NULL;
    }

    char line[MAX_FIELD_LEN * 2]; // Use a larger buffer for lines
    struct Patient *foundPatient = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Patient ID:")) {
            int current_id;
            sscanf(line, "Patient ID: %d", &current_id);

            if (current_id == id) {
                foundPatient = (struct Patient*)malloc(sizeof(struct Patient));
                if (!foundPatient) {
                    perror("Memory allocation failed in searchPatientById");
                    fclose(file);
                    return NULL;
                }
                foundPatient->id = id; // Set the ID

                // Initialize string fields to empty strings to avoid garbage if sscanf fails
                foundPatient->name[0] = '\0';
                foundPatient->bloodGroup[0] = '\0';
                foundPatient->areaOfTreatment[0] = '\0';
                foundPatient->insurance[0] = '\0';
                foundPatient->phoneNumber[0] = '\0';
                foundPatient->hospitalAssigned[0] = '\0';
                foundPatient->vaccinesDone = ' '; // Default for char if not found/invalid

                // Now read the rest of the fields. Be very careful with parsing.
                // Each fgets reads a new line. sscanf attempts to parse it.

                // Name
                if (fgets(line, sizeof(line), file)) {
                    // Check if it matches "Name: " and then extract.
                    // If no name is provided (e.g., "Name:\n"), sscanf will return 0.
                    // We still set a null terminator if it failed.
                    if (sscanf(line, "Name: %[^\n]", foundPatient->name) != 1) {
                        foundPatient->name[0] = '\0'; // Ensure it's an empty string
                    }
                } else { goto cleanup_error; }

                // Age
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Age: %d", &foundPatient->age) != 1) {
                        foundPatient->age = 0; // Default or error value
                        fprintf(stderr, "Warning: Could not parse age for ID %d. Setting to 0.\n", id);
                    }
                } else { goto cleanup_error; }

                // Blood Group
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Blood Group: %[^\n]", foundPatient->bloodGroup) != 1) {
                        foundPatient->bloodGroup[0] = '\0'; // Ensure empty
                    }
                } else { goto cleanup_error; }

                // Vaccines Done
                if (fgets(line, sizeof(line), file)) {
                    char temp_char;
                    if (sscanf(line, "Vaccines Done: %c", &temp_char) == 1 &&
                        (temp_char == 'Y' || temp_char == 'N' || temp_char == 'y' || temp_char == 'n')) {
                        foundPatient->vaccinesDone = toupper(temp_char);
                    } else {
                        foundPatient->vaccinesDone = 'U'; // 'U' for unknown/unspecified
                        fprintf(stderr, "Warning: Could not parse or validate Vaccines Done for ID %d. Setting to 'U'.\n", id);
                    }
                } else { goto cleanup_error; }

                // Area of Treatment
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Area of Treatment: %[^\n]", foundPatient->areaOfTreatment) != 1) {
                        foundPatient->areaOfTreatment[0] = '\0';
                    }
                } else { goto cleanup_error; }

                // Insurance
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Insurance: %[^\n]", foundPatient->insurance) != 1) {
                        foundPatient->insurance[0] = '\0';
                    }
                } else { goto cleanup_error; }

                // Phone Number
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Phone Number: %[^\n]", foundPatient->phoneNumber) != 1) {
                        foundPatient->phoneNumber[0] = '\0';
                    }
                } else { goto cleanup_error; }

                // Hospital Assigned
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Hospital Assigned: %[^\n]", foundPatient->hospitalAssigned) != 1) {
                        foundPatient->hospitalAssigned[0] = '\0';
                    }
                } else { goto cleanup_error; }

                // Optimal Cost
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Optimal Cost: %lf INR", &foundPatient->optimalCost) != 1) {
                        foundPatient->optimalCost = 0.0; // Default or error value
                        fprintf(stderr, "Warning: Could not parse Optimal Cost for ID %d. Setting to 0.0.\n", id);
                    }
                } else { goto cleanup_error; }
                //severity
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Severity: %f", &foundPatient->severity) != 1) {
                        foundPatient->severity = 0.0f;
                        fprintf(stderr, "Warning: Could not parse Severity for ID %d. Setting to 0.0.\n", id);
                    }
                } else { goto cleanup_error; }

                // Current_Treatment_Cost(INR)
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Current_Treatment_Cost(INR): %f", &foundPatient->current_treatment_cost) != 1) {
                        foundPatient->current_treatment_cost = 0.0f;
                        fprintf(stderr, "Warning: Could not parse Current_Treatment_Cost(INR) for ID %d. Setting to 0.0.\n", id);
                    }
                } else { goto cleanup_error; }

                // Total_expenditure(INR)
                if (fgets(line, sizeof(line), file)) {
                    if (sscanf(line, "Total_expenditure(INR): %f", &foundPatient->total_expenditure) != 1) {
                        foundPatient->total_expenditure = 0.0f;
                        fprintf(stderr, "Warning: Could not parse Total_expenditure(INR) for ID %d. Setting to 0.0.\n", id);
                    }
                } else { goto cleanup_error; }

                // Read and verify the separator line
                if (fgets(line, sizeof(line), file) == NULL || strstr(line, "---") == NULL) {
                     fprintf(stderr, "Warning: Missing or malformed separator for ID %d. File format issue?\n", id);
                     goto cleanup_error;
                }

                fclose(file);
                return foundPatient;

            cleanup_error: // Label for error cleanup and return
                free(foundPatient); // Free if reading fails
                foundPatient = NULL;
                fprintf(stderr, "Error reading complete record for ID %d. Data might be incomplete.\n", id);
            }
        }
    }
    fclose(file);
    return NULL; // Patient not found or error occurred
}

// Function to update patient records
bool updatePatientRecord(struct Patient *patient)
{
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        perror("Error opening files for update");
        if (file) fclose(file);
        if (temp) fclose(temp);
        return false;
    }

    char line[MAX_FIELD_LEN * 2]; // Increased buffer size for lines
    bool found = false;
    int line_count_in_record = 11; // Number of lines per patient record (including separator)

    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Patient ID:"))
        {
            int id_from_file;
            sscanf(line, "Patient ID: %d", &id_from_file);

            if (id_from_file == patient->id)
            {
                found = true;
                // Write the updated patient's data
                fprintf(temp, "Patient ID: %d\n", patient->id); // ID comes first in write
                fprintf(temp, "Name: %s\n", patient->name);
                fprintf(temp, "Age: %d\n", patient->age);
                fprintf(temp, "Blood Group: %s\n", patient->bloodGroup);
                fprintf(temp, "Vaccines Done: %c\n", patient->vaccinesDone);
                fprintf(temp, "Area of Treatment: %s\n", patient->areaOfTreatment);
                fprintf(temp, "Insurance: %s\n", patient->insurance);
                fprintf(temp, "Phone Number: %s\n", patient->phoneNumber);
                fprintf(temp, "Hospital Assigned: %s\n", patient->hospitalAssigned);
                fprintf(temp, "Optimal Cost: %.2lf INR\n", patient->optimalCost);
                fprintf(temp, "-----------------\n"); // Separator line

                for (int i = 0; i < (line_count_in_record - 1); i++) {
                    if (fgets(line, sizeof(line), file) == NULL) {
                        // Handle unexpected EOF if file format is inconsistent
                        fprintf(stderr, "Warning: Unexpected EOF while skipping record lines.\n");
                        break;
                    }
                }
            }
            else
            {
                // If not the target patient, write the ID line and then the rest of its record
                fputs(line, temp); // Write the "Patient ID" line

                // Write the remaining (line_count_in_record - 1) lines of this record
                for (int i = 0; i < (line_count_in_record - 1); i++) {
                     if (fgets(line, sizeof(line), file) != NULL) {
                        fputs(line, temp);
                     } else {
                        // Handle unexpected EOF
                        break;
                     }
                }
            }
        }
        else
        {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);

    if (found)
    {
        remove(FILENAME);
        rename("temp.txt", FILENAME);
    }
    else
    {
        remove("temp.txt"); // Clean up temp file if patient not found
    }

    return found;
}


// Function to generate statistics
void generatePatientStatistics()
{
    FILE *file = fopen("patient_details.txt", "r");
    if (!file)
        return;

    int totalPatients = 0;
    int vaccinated = 0;
    int withInsurance = 0;
    double totalCost = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Patient ID:"))
        {
            totalPatients++;
        }
        else if (strstr(line, "Vaccines Done: y"))
        {
            vaccinated++;
        }
        else if (strstr(line, "Insurance: yes"))
        {
            withInsurance++;
        }
        else if (strstr(line, "Optimal Cost:"))
        {
            double cost;
            sscanf(line, "Optimal Cost: %lf", &cost);
            totalCost += cost;
        }
    }

    fclose(file);

    printf("\n=== Patient Statistics ===\n");
    printf("Total Patients: %d\n", totalPatients);
    printf("Vaccinated Patients: %d (%.1f%%)\n", vaccinated,
           totalPatients > 0 ? (vaccinated * 100.0 / totalPatients) : 0);
    printf("Patients with Insurance: %d (%.1f%%)\n", withInsurance,
           totalPatients > 0 ? (withInsurance * 100.0 / totalPatients) : 0);
    printf("Average Cost per Patient: %.2f INR\n",
           totalPatients > 0 ? (totalCost / totalPatients) : 0);
    printf("=======================\n");
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

double calculateHospitalScore(int distance, int rating) {
    return distance - (rating * 0.5);
}


void saveFeedback(int hospitalNum, const char *feedback, int rating, char hospital_names[][50]) {
    FILE *file = fopen("hospital_feedback.txt", "r");
    FILE *temp = fopen("temp_feedback.txt", "w");
    if (!file || !temp) {
        printf("Error opening feedback files.\n");
        return;
    }

    char line[1000];
    bool found = false;
    bool inTargetHospital = false;
    bool justWroteFeedback = false;

    while (fgets(line, sizeof(line), file)) {
        // Check if this is our target hospital's header
        if (strstr(line, hospital_names[hospitalNum-1])) {
            inTargetHospital = true;
            found = true;
            fputs(line, temp);
            // Add new feedback right after hospital name with a space
            fprintf(temp, "Rating: %d stars\n", rating);
            fprintf(temp, "Feedback: \"%s\"\n\n", feedback);
            justWroteFeedback = true;
            continue;
        }

        // If this is a feedback line and we're not adding new feedback
        if (strstr(line, "Feedback:")) {
            fputs(line, temp);
            fprintf(temp, "\n"); // Add space after existing feedback
            continue;
        }

        // Write all other lines normally
        fputs(line, temp);
    }

    if (!found) {
        // If hospital not found, add new entry at end
        fprintf(temp, "\n%s\n", hospital_names[hospitalNum-1]);
        fprintf(temp, "Rating: %d stars\n", rating);
        fprintf(temp, "Feedback: \"%s\"\n\n", feedback);
        fprintf(temp, "-----------------------------------------------------\n");
    }

    fclose(file);
    fclose(temp);

    // Replace original file with temp file
    remove("hospital_feedback.txt");
    rename("temp_feedback.txt", "hospital_feedback.txt");
}

void displayHospitalFeedback(int hospitalNum, char hospital_names[][50]) {
    FILE *file = fopen("hospital_feedback.txt", "r");
    if (!file) {
        printf("\n=== Feedback for %s ===\n", hospital_names[hospitalNum-1]);
        printf("Status: Not Rated\n");
        printf("No feedback available yet.\n");
        printf("================================\n");
        return;
    }

    char line[1000];
    bool found = false;
    bool inTargetHospital = false;
    int feedbackCount = 0;
    double totalRating = 0;

    printf("\n=== Feedback for %s ===\n", hospital_names[hospitalNum-1]);
    
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;
        
        // Check if this is our target hospital
        if (strstr(line, hospital_names[hospitalNum-1])) {
            found = true;
            inTargetHospital = true;
            continue;
        }

        // Stop when we hit the next separator
        if (inTargetHospital && strstr(line, "-----------------------------------------------------")) {
            break;
        }

        // Process feedback and ratings
        if (inTargetHospital && strlen(line) > 0) {
            if (strstr(line, "Rating:")) {
                feedbackCount++;
                int rating;
                sscanf(line, "Rating: %d", &rating);
                totalRating += rating;
                printf("\nFeedback #%d:\n", feedbackCount);
            }
            printf("%s\n", line);
        }
    }

    if (!found || feedbackCount == 0) {
        printf("Status: Not Rated\n");
        printf("No feedback available yet.\n");
    } else {
        double averageRating = totalRating / feedbackCount;
        printf("\nAverage Rating: %.1f stars\n", averageRating);
        printf("Total Feedback Entries: %d\n", feedbackCount);
    }
    
    printf("================================\n");
    fclose(file);
}

double calculateAverageFeedbackRating(int hospitalNum, char hospital_names[][50]) {
    FILE *file = fopen("hospital_feedback.txt", "r");
    if (!file) {
        return 0.0;
    }

    char line[1000];
    bool inTargetHospital = false;
    int totalRating = 0;
    int feedbackCount = 0;

    while (fgets(line, sizeof(line), file)) {
        // Check if this is our target hospital
        if (strstr(line, hospital_names[hospitalNum-1])) {
            inTargetHospital = true;
            continue;
        }

        // Stop when we hit the next separator
        if (inTargetHospital && strstr(line, "-----------------------------------------------------")) {
            break;
        }

        // Count ratings while we're in the target hospital section
        if (inTargetHospital && strstr(line, "Rating:")) {
            int rating;
            if (sscanf(line, "Rating: %d", &rating) == 1) {
                totalRating += rating;
                feedbackCount++;
            }
        }
    }

    fclose(file);
    return feedbackCount > 0 ? (double)totalRating / feedbackCount : 0.0;
}

void refuelAmbulance(struct Ambulance *ambulance) {
    printf("\nAmbulance %d fuel low (%.0d%%). Sending to refuel station...\n", 
           ambulance->id, ambulance->fuel);
    printf("[                                                  ] 0%%");
    fflush(stdout);
    
    for (int i = 1; i <= 50; ++i) {
        usleep(REFUEL_TIME_MS * 1000 / 50);
        printf("\r[");
        for (int j = 0; j < 50; ++j) {
            printf(j < i ? "#" : " ");
        }
        printf("] %d%%", i * 2);
        fflush(stdout);
    }
    
    ambulance->fuel = 100;
    printf("\nAmbulance %d refueled to 100%%\n", ambulance->id);
}

void setAmbulanceAvailableLater(int ambId, int hospital, const char *filename) {
    sleep(AMBULANCE_BUSY_TIME);

    // Read ambulances from file to get the correct fuel value
    struct Ambulance ambulances[MAX_AMBULANCES];
    int ambCount = readAmbulances(ambulances, MAX_AMBULANCES, filename);
    int fuel = 100; // Default if not found

    for (int i = 0; i < ambCount; ++i) {
        if (ambulances[i].id == ambId) {
            fuel = ambulances[i].fuel;
            break;
        }
    }

    updateAmbulanceStatus(ambId, hospital, "available", fuel, filename);
    printf("Ambulance %d is now available at %d\n", ambId, hospital);
}


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

    // Prompt for file paths
    printf("\n--- Data File Configuration ---\n");
    prompt_filepath("Enter adjacency matrix file (.txt/.csv)", matrix_file, sizeof(matrix_file), "matrix.txt");
    prompt_filepath("Enter casualties matrix file (.txt/.csv)", casualties_file, sizeof(casualties_file), "casualtiesMatrix.txt");
    prompt_filepath("Enter weights matrix file (.txt/.csv)", weights_file, sizeof(weights_file), "weights.txt");
    prompt_filepath("Enter hospital names file (.txt/.csv)", hospital_names_file, sizeof(hospital_names_file), "hospital_names.txt");
    printf("-------------------------------\n");

    int matrix[15][15];
    int casualtiesMatrix[15][15];
    int weights[15][15];
    char hospital_names[15][50];
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
        while (1)
        {
            printf("\nEnter your choice\n");
            printf("1. Finding Hospital\n");
            printf("2. Print Hospital Name\n");
            printf("3. Display Hospitals List\n");
            printf("4. Search Patient Record\n");
            printf("5. Update Patient Record\n");
            printf("6. Generate Statistics\n");
            printf("7. Give Feedback\n");
            printf("8. Exit\n");

            if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 8)
                break;
            printf("Invalid choice. Please select a valid option (1-8).\n");
            while (getchar() != '\n')
                ;
        }

        printf("\n");

        if (choice < 1 || choice > 8)
        {
            printf("Invalid choice. Please select a valid option.\n");
            continue; // This is redundant because we validated, but keep it as is
        }

        switch (choice)
        {
        case 1:
            // Validate emergency input
            while (1)
            {
                printf("Is it a case of emergency?(y/n): \n");
                scanf(" %c", &input1);
                if (input1 == 'y' || input1 == 'n')
                    break;
                printf("Invalid input. Please enter 'y' or 'n'.\n");
                while (getchar() != '\n')
                    ;
            }

            if (input1 == 'y')
            {
                // Validate severity input
                int severity;
               
                while (1)
                {
                    printf("Enter the severity of the emergency (1-5): \n");
                    if (scanf("%d", &severity) == 1 && severity >= 1 && severity <= 5)
                        break;
                    printf("Invalid severity. Please enter a number between 1 and 5.\n");
                    while (getchar() != '\n')
                        ;
                }
                 // Validate source hospital input
                while (1)
                {
                    printf("\nSelect a number corresponding to your nearest location.\n");
                    printf("1.Rajaji nagar  2.Sahakar Nagar  3.Sanjaynagar  4.Yeshwanthpur  5.Nagarbavi\n");
                    printf("6.Bannerghatta  7.Shanti Nagar  8.Marathahalli  9.Sarjapur  10.Jayanagar\n");
                    printf("11.Bommasandra  12.Whitefield  13.Krishnarajapuram  14.Yelahanka  15.Kengeri: \n");
                    if (scanf("%d", &src) == 1 && src >= 1 && src <= hospitals)
                        break;
                    printf("Invalid nearest location hospital number. Please enter a number between 1 and 15.\n");
                    while (getchar() != '\n')
                        ;
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
                    // Extract hospital number from the name
                    int currentHospitalNumber;
                    if (sscanf(cur->hospital_name, "%d", &currentHospitalNumber) == 1)
                    {
                        // Check if the current edge weight is smaller than the minimum
                        if (cur->weight < minWeight)
                        {
                            minWeight = cur->weight;
                            nearestHospital = currentHospitalNumber;
                        }
                        printf("\nCasualty Level at Hospital %d: %d - ", currentHospitalNumber, cur->casualtiesPresent);
                        printAdmissionDifficulty(cur->casualtiesPresent);
                    }
                    else
                    {
                        printf("\nError extracting hospital number from the name: %s\n", cur->hospital_name);
                    }

                    cur = cur->link;
                }

                // Check if there is a self-loop with a smaller weight
                if (weights[src - 1][src - 1] < minWeight)
                {
                    minWeight = weights[src - 1][src - 1];
                    nearestHospital = src;
                }

                int averageWeight = minWeight;
                double optimalCost = averageWeight * moneyFactor;

                // Adjust the optimal cost based on severity
                if (severity == 5) optimalCost = optimalCost * 2.0;
                else if (severity == 4) optimalCost = optimalCost * 1.5;
                else if (severity == 3) optimalCost = optimalCost * 1.2;
                else if (severity == 2) optimalCost = optimalCost * 1.0;
                else optimalCost = optimalCost * 0.8;

                if (nearestHospital != -1) {
                    // Display feedback for the nearest hospital
                    printf("\n=== Current Feedback for %s ===\n", hospital_names[nearestHospital-1]);
                    double avgRating = calculateAverageFeedbackRating(nearestHospital, hospital_names);
                    if (avgRating > 0) {
                        printf("Average User Rating: %.1f stars\n", avgRating);
                    } else {
                        printf("Status: Not Rated\n");
                    }
                    displayHospitalFeedback(nearestHospital, hospital_names);
                }
                if (nearestHospital != -1)
                {
                    // Show ambulance list again BEFORE dispatch
                    printf("\n=== AMBULANCE LIST (before dispatch) ===\n");
                    displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights);
                    printf("=== END OF AMBULANCE LIST ===\n\n");
                    fflush(stdout);

                    printf("\nNearest hospital to Region %d is Hospital %s with a road rating of %d\n", src, hospital_names[nearestHospital - 1], averageWeight);
                    printf("\nEffective Cost (Including severity Surcharge): %.2lf INR\n", optimalCost);

                    
                    // Severity-based ambulance dispatch
                    if (severity >= 4) {
                        printf("\nHIGH SEVERITY CASE (Level %d) - Dispatching ambulance immediately!\n", severity);
                    } else if (severity >= 2) {
                        printf("\nMEDIUM SEVERITY CASE (Level %d) - Dispatching ambulance.\n", severity);
                    } else {
                        printf("\nLOW SEVERITY CASE (Level %d) - Non-emergency dispatch.\n", severity);
                    }

                    // Find and dispatch the nearest available ambulance
                    int ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights);
                    if (ambIdx != -1)
                    {
                        printf("\nDispatching Ambulance %d from %s...\n", ambulances[ambIdx].id, hospital_names[ambulances[ambIdx].location - 1]);
                        simulateAmbulanceMovement(hospital_names[src - 1], hospital_names[nearestHospital - 1], 30, 80);
                        int fuelUsed = (int)(averageWeight * 0.5);
                        ambulances[ambIdx].fuel = ambulances[ambIdx].fuel - fuelUsed;
                        updateAmbulanceStatus(ambulances[ambIdx].id, 
                                            nearestHospital, 
                                            "busy", 
                                            ambulances[ambIdx].fuel,
                                            "ambulance_locations.txt");
                        setAmbulanceAvailableLater(ambulances[ambIdx].id, nearestHospital, "ambulance_locations.txt");
                    }
                    else
                    {
                        printf("No available ambulance could be dispatched!\n");
                    }
                    fflush(stdout);

                    // Show all ambulances and their time delays AFTER dispatch
                    int ambCountAfter = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
                    printf("\n=== AMBULANCE LIST (after dispatch) ===\n");
                    displayAllAmbulances(ambulances, ambCountAfter, hospital_names, src, weights);
                    printf("=== END OF AMBULANCE LIST ===\n\n");
                    fflush(stdout);

                    if (handlePatientDetails(src, nearestHospital, averageWeight, hospital_names, moneyFactor, severity))
                    {   
                        float totalExp;
        
                        totalExp = get_patient_param(current_patient,"Total_expenditure(INR)");
                        totalExp += optimalCost;
                        set_patient_param(current_patient, "Total_expenditure(INR)", totalExp);
                        set_patient_param(current_patient,"Current_Treatment_Cost(INR)", (float)optimalCost);
                        set_patient_param(current_patient,"Severity", (float)severity);
                        printf("Patient can be admitted to the hospital.\n\n");
                    }
                    else
                    {
                        printf("Patient can not be admitted due to invalid input.\n\n");
                    }
                    fflush(stdout);
                    break;
                }
                else
                {
                    printf("\nNo adjacent hospitals found for Hospital %d\n", src);
                }
            }
            else if (input1 == 'n')
            {
                // Validate source hospital input
                while (1)
                {
                    printf("\nSelect the number corresponding to your nearest location: \n");
                    printf("1.Rajaji nagar   2.Sahakar Nagar   3.Sanjaynagar         4.Yeshwanthpur   5.Nagarbavi\n");
                    printf("6.Bannerghatta   7.Shanti Nagar    8.Marathahalli        9.Sarjapur       10.Jayanagar\n");
                    printf("11.Bommasandra   12.Whitefield     13.Krishnarajapuram   14.Yelahanka     15.Kengeri: \n");
                    if (scanf("%d", &src) == 1 && src >= 1 && src <= 15)
                        break;
                    printf("Invalid nearest location hospital number. Please enter a number between 1 and 15.\n");
                    while (getchar() != '\n')
                        ;
                }

                // Show all ambulances and their time delays BEFORE hospital selection
                struct Ambulance ambulances[MAX_AMBULANCES];
                int ambCount = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
                printf("\n=== AMBULANCE LIST (before hospital selection) ===\n");
                displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights);
                printf("=== END OF AMBULANCE LIST ===\n\n");
                fflush(stdout);

                // Validate destination hospital input
                while (1)
                {
                    printf("\nSelect the hospital you want to go: \n");
                    printf("1.Suguna_Hospital(Rajajinagar)\t\t2.Aster_CMI_Hospital(Sahakarnagar)\t3.MS_Ramaiah_Hospital(Sanjaynagar)\n");
                    printf("4.People's_Tree_Hospital(Yeshwanthpur) \t5.Fortis_Hospital(Nagarbhavi)\t\t6.Appolo_Hospital(Bannerghatta)\n");
                    printf("7.HCG_Hospital(Shantinagar)\t\t8.Cloudnine_Hospital(Marathahalli)\t9.Columbia_Asia(Sarjapur)\n");
                    printf("10.Sagar_Hospital(Jayanagar)\t\t11.Narayana_Hrudayalaya(Bommasandra)\t12.Manipal_Hospital(Whitefield)\n");
                    printf("13.Koshys_Hospital(Krishnarajapuram)\t14.Sparsh_Hospital(Yelahanka)\t\t15.BGS_Gleneagles_Hospital(Kengeri)\n");
                    if (scanf("%d", &dest) == 1 && dest >= 1 && dest <= 15)
                        break;
                    printf("Invalid destination hospital number. Please enter a number between 1 and 15.\n");
                    while (getchar() != '\n')
                        ;
                }

                if (src == dest) {
                    printf("\nSource and destination are the same hospital.\n");
                    printf("Hospital: %s\n", hospital_names[src-1]);
                    
                    printf("No travel required - Cost: 0 INR\n\n");
                    
                    // Handle patient details even for same location
                    if (handlePatientDetails(src, dest, 0, hospital_names, moneyFactor, false)) {
                        printf("Patient can be admitted to the hospital.\n\n");
                    } else {
                        printf("Patient cannot be admitted due to invalid input.\n\n");
                    }
                } else {
                    printf("\nFinding the optimal route: \n");

                    // Initialize arrays for Dijkstra's algorithm
                    int distance[hospitals];
                    int previous[hospitals];
                    bool visited[hospitals];
                    
                    for (int i = 0; i < hospitals; i++) {
                        distance[i] = INT_MAX;
                        previous[i] = -1;
                        visited[i] = false;
                    }
                    
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
                            int v = atoi(cur->hospital_name) - 1;
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
                        else
                        {
                            printf("%s", hospital_names[current]);
                        }
                        current = prev;
                    }
                    printf("\n");
                    // Calculate and display the average edge weight
                    if (edgeCount > 0)
                    {
                        double averageWeight = (double)totalWeight / edgeCount;
                        
                        // Consider alternative hospitals within 20% longer route
                        double threshold = averageWeight * 1.2;
                        int bestHospital = dest;
                        double bestScore = calculateHospitalScore(averageWeight, calculateAverageFeedbackRating(dest, hospital_names));
                        
                        for (int alt = 0; alt < hospitals; alt++) {
                            if (alt != dest - 1) {
                                double altWeight = distance[alt];
                                if (altWeight <= threshold) {
                                    double altScore = calculateHospitalScore(altWeight, calculateAverageFeedbackRating(alt + 1, hospital_names));
                                    if (altScore < bestScore) {
                                        bestScore = altScore;
                                        bestHospital = alt + 1;
                                    }
                                }
                            }
                        }
                        
                        if (bestHospital != dest) {
                            printf("\nBased on distance and hospital rating:\n");
                            double origRating = calculateAverageFeedbackRating(dest, hospital_names);
                            double recRating = calculateAverageFeedbackRating(bestHospital, hospital_names);
                            if(origRating >0)
                                printf("Original hospital: %s (Hospital Rating: %.1f stars)\n", 
                                        hospital_names[dest-1], origRating);
                            else
                                printf("Original hospital: %s (Hospital Rating: Not Rated)\n", 
                                        hospital_names[dest-1]);
                            if(recRating > 0)
                                printf("Recommended hospital: %s (Hospital Rating: %.1f stars)\n", 
                                        hospital_names[bestHospital-1], recRating);
                            else
                                printf("Recommended hospital: %s (Hospital Rating: Not Rated)\n", 
                                        hospital_names[bestHospital-1]);
                            printf("Would you like to switch to the recommended hospital? (y/n): ");
                            char choice;
                            scanf(" %c", &choice);
                            if (choice == 'y' || choice == 'Y') {
                                dest = bestHospital;
                                
                                // Reset arrays for new route calculation
                                for (int i = 0; i < hospitals; i++) {
                                    distance[i] = INT_MAX;
                                    previous[i] = -1;
                                    visited[i] = false;
                                }
                                
                                // Set distance to source to 0
                                distance[src - 1] = 0;

                                // Find the optimal route for new destination
                                for (int count = 0; count < hospitals - 1; count++) {
                                    int u = -1;
                                    int minDistance = INT_MAX;

                                    // Select the node with the minimum distance
                                    for (int v = 0; v < hospitals; v++) {
                                        if (!visited[v] && distance[v] < minDistance) {
                                            u = v;
                                            minDistance = distance[v];
                                        }
                                    }

                                    // Mark the selected node as visited
                                    visited[u] = true;

                                    // Update distances of the adjacent nodes
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

                                // Display the new optimal route
                                printf("\nNew optimal route from %s to %s: \n", 
                                    hospital_names[src - 1], hospital_names[dest - 1]);
                                current = dest - 1;
                                edgeCount = 0;
                                totalWeight = 0;
                                routeLen = 0;
                                
                                while (current != -1) {
                                    route[routeLen++] = current;
                                    int prev = previous[current];
                                    if (prev != -1) {
                                        totalWeight += weights[prev][current];
                                        edgeCount++;
                                        printf("%s <- ", hospital_names[current]);
                                    } else {
                                        printf("%s", hospital_names[current]);
                                    }
                                    current = prev;
                                }
                                printf("\n");
                            }
                            
                        }
                        
                        // Display feedback for the chosen/recommended hospital
                        printf("\n=== Current Feedback for %s ===\n", hospital_names[dest-1]);
                        double avgRating = calculateAverageFeedbackRating(dest, hospital_names);
                        if (avgRating > 0) {
                            printf("Hospital Rating: %.1f stars\n", avgRating);
                        } else {
                            printf("Status: Not Rated\n");
                        }
                        displayHospitalFeedback(dest, hospital_names);
                        
                        double currentRating = calculateAverageFeedbackRating(dest, hospital_names);
                        if(currentRating > 0)
                            printf("Current Hospital Rating: %.1f stars\n", currentRating);
                        else
                            printf("Current Hospital Rating: Not Rated\n");
                        printf("Average Edge Weight: %.2lf\n", averageWeight);
                        double optimalCost = averageWeight * moneyFactor;
                        printf("Optimal Cost: %.2lf INR\n", optimalCost);
                        
                        // Find and dispatch the nearest available ambulance
                        int ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights);
                        if (ambIdx != -1)
                        {
                            printf("\nDispatching Ambulance %d from %s...\n", ambulances[ambIdx].id, hospital_names[ambulances[ambIdx].location - 1]);
                            for (int i = routeLen - 1; i > 0; --i)
                            {
                                simulateAmbulanceMovement(hospital_names[route[i]], hospital_names[route[i - 1]], 15, 60);
                            }
                            int fuelUsed = (int)(averageWeight * 0.5);
                            ambulances[ambIdx].fuel = ambulances[ambIdx].fuel - fuelUsed;
                            updateAmbulanceStatus(ambulances[ambIdx].id, 
                                                dest, 
                                                "busy", 
                                                ambulances[ambIdx].fuel,
                                                "ambulance_locations.txt");
                            setAmbulanceAvailableLater(ambulances[ambIdx].id, dest, "ambulance_locations.txt");
                        }
                        else
                        {
                            printf("No available ambulance could be dispatched!\n");
                        }

                        // Show all ambulances and their time delays AFTER dispatch
                        int ambCountAfter = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
                        printf("\n=== AMBULANCE LIST (after dispatch) ===\n");
                        displayAllAmbulances(ambulances, ambCountAfter, hospital_names, src, weights);
                        printf("=== END OF AMBULANCE LIST ===\n\n");
                        fflush(stdout);

                        if (handlePatientDetails(src, dest, averageWeight, hospital_names, moneyFactor, 1))
                        {   
                            printf("Patient can be admitted to the hospital.\n\n");
                            float totalExp;
                            totalExp = get_patient_param(current_patient,"Total_expenditure(INR)");
                            totalExp += optimalCost;
                            set_patient_param(current_patient, "Total_expenditure(INR)", totalExp);
                            set_patient_param(current_patient,"Current_Treatment_Cost(INR)", (float)optimalCost);
                            set_patient_param(current_patient,"Severity", (float)1);
                        }
                        else
                        {
                            printf("Patient can not be admitted due to invalid input.\n\n");
                        }
                        break;
                    }
                    else
                    {
                        printf("No direct or adjacent edge found between the source and destination.\n");
                    }
                }
            }
            break;
            

        case 2:
            while (1) {
                printf("Enter the hospital number to print its name and feedback: ");
                if (scanf("%d", &near_hosp) == 1 && near_hosp >= 1 && near_hosp <= hospitals)
                    break;
                printf("Invalid hospital number. Please enter a number between 1 and %d.\n", hospitals);
                while (getchar() != '\n');
            }
            printHospitalName(near_hosp, hospital_names);
            
            // Get and display average feedback rating instead of static rating
            double avgRating = calculateAverageFeedbackRating(near_hosp, hospital_names);
            if (avgRating > 0) {
                printf("\nHospital Feedback Rating: %.1f stars (based on user feedback)\n", avgRating);
            } else {
                printf("\nNo user feedback ratings available yet\n");
            }
            
            displayHospitalFeedback(near_hosp, hospital_names);
            break;

        case 3:
            displayAdjacencyList(adjList, hospitals, hospital_names);
            break;

        case 4:
        {
            int searchId;
            printf("Enter Patient ID to search: ");
            scanf("%d", &searchId);

            struct Patient *patient = searchPatientById(searchId);
            if (patient)
            {
                printf("\nPatient Found:\n");
                printf("Name: %s\n", patient->name);
                printf("Age: %d\n", patient->age);
                printf("Blood Group: %s\n", patient->bloodGroup);
                printf("Patient ID: %d\n", patient->id);
                printf("Vaccines Done: %c\n", patient->vaccinesDone);
                printf("Area of Treatment: %s\n", patient->areaOfTreatment);
                printf("Insurance: %s\n", patient->insurance);
                printf("Phone Number: %s\n", patient->phoneNumber);
                printf("Hospital Assigned: %s\n", patient->hospitalAssigned);
                printf("Optimal Cost: %.2f INR\n", patient->optimalCost);
                printf("Severity: %.2f\n", patient->severity);
                printf("Current Treatment Cost: %.2f INR\n", patient->current_treatment_cost);
                printf("Total Expenditure: %.2f INR\n", patient->total_expenditure);
                free(patient);
            }
            else
            {
                printf("Patient not found.\n");
            }
            break;
        }

         case 5:
        {
           int updateId;
            printf("Enter Patient ID to update: ");
            if (scanf("%d", &updateId) != 1) {
                printf("Invalid input for Patient ID.\n");
                clear_input_buffer();
                return 1;
            }
            clear_input_buffer(); // Clear buffer after scanf

            struct Patient *patient = searchPatientById(updateId);
            if (patient)
            {
                char input_buffer[MAX_FIELD_LEN]; // Buffer for string inputs
                char char_input;
                int int_input;
                double double_input;
                char num_str[30]; // Buffer for numerical inputs as strings

                printf("\n--- Updating Patient ID: %d ---\n", patient->id);
                printf("Current Name: %s\n", patient->name);
                printf("Current Age: %d\n", patient->age);
                printf("Current Blood Group: %s\n", patient->bloodGroup);
                printf("Current Vaccines Done: %c\n", patient->vaccinesDone);
                printf("Current Area of Treatment: %s\n", patient->areaOfTreatment);
                printf("Current Insurance: %s\n", patient->insurance);
                printf("Current Phone Number: %s\n", patient->phoneNumber);
                printf("Current Hospital Assigned: %s\n", patient->hospitalAssigned);
                printf("Current Optimal Cost: %.2lf\n", patient->optimalCost);
                printf("Current Severity: %.2f\n", patient->severity);
                printf("Current Current Treatment Cost: %.2f INR\n", patient->current_treatment_cost);
                printf("Current Total Expenditure: %.2f INR\n", patient->total_expenditure  );
                printf("-------------------------------\n");

                // Name
                printf("Enter new name (leave blank to keep '%s'): ", patient->name);
                fgets(input_buffer, MAX_FIELD_LEN, stdin);
                input_buffer[strcspn(input_buffer, "\n")] = 0; // Remove newline
                if (strlen(input_buffer) > 0) {
                    strcpy(patient->name, input_buffer);
                }
                
                // Age
                printf("Enter new age (leave blank to keep '%d'): ", patient->age);
                fgets(num_str, sizeof(num_str), stdin);
                num_str[strcspn(num_str, "\n")] = 0; // Remove newline
                if (strlen(num_str) > 0) {
                    if (sscanf(num_str, "%d", &int_input) == 1) {
                        patient->age = int_input;
                    } else {
                        printf("Invalid input for age. Keeping original.\n");
                    }
                }
                
                // Blood Group
                printf("Enter new blood group (leave blank to keep '%s'): ", patient->bloodGroup);
                fgets(input_buffer, MAX_FIELD_LEN, stdin);
                input_buffer[strcspn(input_buffer, "\n")] = 0; // Remove newline
                if (strlen(input_buffer) > 0) {
                    strcpy(patient->bloodGroup, input_buffer);
                }
                
                // Vaccines Done
                printf("Enter vaccines done (Y/N, leave blank to keep '%c'): ", patient->vaccinesDone);
                fgets(input_buffer, MAX_FIELD_LEN, stdin); // Read as string
                input_buffer[strcspn(input_buffer, "\n")] = 0; // Remove newline
                if (strlen(input_buffer) > 0) {
                    if (sscanf(input_buffer, "%c", &char_input) == 1 &&
                        (char_input == 'Y' || char_input == 'N' || char_input == 'y' || char_input == 'n')) {
                        patient->vaccinesDone = toupper(char_input); // Convert to uppercase for consistency
                    } else {
                        printf("Invalid input for vaccines done. Keeping original.\n");
                    }
                }
                
                // Area of Treatment
                printf("Enter new area of treatment (leave blank to keep '%s'): ", patient->areaOfTreatment);
                fgets(input_buffer, MAX_FIELD_LEN, stdin);
                input_buffer[strcspn(input_buffer, "\n")] = 0; // Remove newline
                if (strlen(input_buffer) > 0) {
                    strcpy(patient->areaOfTreatment, input_buffer);
                }
                
                // Insurance
                printf("Enter new insurance details (leave blank to keep '%s'): ", patient->insurance);
                fgets(input_buffer, MAX_FIELD_LEN, stdin);
                input_buffer[strcspn(input_buffer, "\n")] = 0; // Remove newline
                if (strlen(input_buffer) > 0) {
                    strcpy(patient->insurance, input_buffer);
                }
                
                // Phone Number
                printf("Enter new phone number (leave blank to keep '%s'): ", patient->phoneNumber);
                fgets(input_buffer, MAX_FIELD_LEN, stdin);
                input_buffer[strcspn(input_buffer, "\n")] = 0; // Remove newline
                if (strlen(input_buffer) > 0) {
                    strcpy(patient->phoneNumber, input_buffer);
                }
                
                // Hospital Assigned
                printf("Enter new hospital assigned (leave blank to keep '%s'): ", patient->hospitalAssigned);
                fgets(input_buffer, MAX_FIELD_LEN, stdin);
                input_buffer[strcspn(input_buffer, "\n")] = 0; // Remove newline
                if (strlen(input_buffer) > 0) {
                    strcpy(patient->hospitalAssigned, input_buffer);
                }
                
                // Optimal Cost
                printf("Enter new optimal cost (leave blank to keep '%.2lf'): ", patient->optimalCost);
                fgets(num_str, sizeof(num_str), stdin);
                num_str[strcspn(num_str, "\n")] = 0; // Remove newline
                if (strlen(num_str) > 0) {
                    if (sscanf(num_str, "%lf", &double_input) == 1) {
                        patient->optimalCost = double_input;
                    } else {
                        printf("Invalid input for optimal cost. Keeping original.\n");
                    }
                }

                if (updatePatientRecord(patient))
                {
                    printf("Record updated successfully.\n");
                }
                else
                {
                    printf("Error updating record.\n");
                }
                free(patient); // Free the dynamically allocated patient struct
            }
            else
            {
                printf("Patient not found.\n");
            }
            break;
        }        

        case 6:
        {
            generatePatientStatistics();
            break;
        }

        case 7:
        {
            int feedbackHospital;
            char feedback[500];
            int rating;

            // Get hospital number
            while (1) {
                printf("\nEnter hospital number (1-15) to provide feedback: ");
                if (scanf("%d", &feedbackHospital) == 1 && feedbackHospital >= 1 && feedbackHospital <= 15) {
                    break;
                }
                printf("Invalid hospital number. Please enter a number between 1 and 15.\n");
                while (getchar() != '\n');
            }
            while (getchar() != '\n'); // Clear input buffer

            // Get rating
            while (1) {
                printf("Enter rating (1-5 stars): ");
                if (scanf("%d", &rating) == 1 && rating >= 1 && rating <= 5) {
                    break;
                }
                printf("Invalid rating. Please enter a number between 1 and 5.\n");
                while (getchar() != '\n');
            }
            while (getchar() != '\n'); // Clear input buffer

            // Get feedback
            printf("Enter your feedback (max 500 characters):\n");
            fgets(feedback, sizeof(feedback), stdin);
            feedback[strcspn(feedback, "\n")] = 0; // Remove trailing newline

            // Save feedback
            saveFeedback(feedbackHospital, feedback, rating, hospital_names);
            printf("\nThank you for your feedback!\n");
            break;
        }

        case 8:
            return 0;

        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }
    }
}