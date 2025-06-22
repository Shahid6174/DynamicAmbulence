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

#define MAX_LINE 1000
#define MAX_PATIENTS 100
#define MAX_FIELD_LEN 100
#define FILENAME "patient_details.txt"
#define MAX_PHONE_NUMBER_LEN 15 

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

// Add Patient Record Structure from 'main' branch
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

// Function to display the linked list (from 'main' branch)
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

// Update the displayAdjacencyList call (from 'main' branch)
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
    if (hospitalNumber >= 1 && hospitalNumber <= 15) // Corrected condition
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
                if (weights[i][k] != INT_MAX && weights[k][j] != INT_MAX && 
                    weights[i][k] + weights[k][j] < weights[i][j])
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
    FILE *patientFile = fopen(FILENAME, "r"); // Use FILENAME define
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

// Function to handle patient details (adapted from 'main' branch, ensure parameters match call)
bool handlePatientDetails(int assigned_hospital_idx_0_based, int averageWeight, char hospital_names[15][50], int moneyFactor)
{
    struct Patient p; // Create a patient struct to fill

    // Patient ID
    while (true)
    {
        printf("Enter unique 4-digit Patient ID: ");
        if (scanf("%d", &p.id) != 1) {
             while(getchar()!='\n'); // Clear buffer
             printf("Invalid input. Please enter a number.\n");
             continue;
        }
        while(getchar()!='\n'); // Clear buffer
        if (p.id >= 1000 && p.id <= 9999 && !isPatientIdExist(p.id))
            break;
        if (p.id < 1000 || p.id > 9999)
        {
            printf("Invalid Patient ID. Must be a 4-digit number.\n");
        }
        else
        {
            printf("Patient ID %d already exists. Cannot add new record with this ID.\n", p.id);
            // If you want to proceed to assign hospital to existing patient, that's a different logic path.
            // For now, returning false as we are in "add new patient" context.
            return false; 
        }
    }

    // Name
    while (true)
    {
        printf("Name: ");
        fgets(p.name, sizeof(p.name), stdin);
        p.name[strcspn(p.name, "\n")] = 0; // Remove newline
        if (strlen(p.name) > 0 && isalpha(p.name[0]))
            break;
        printf("Invalid name. Must start with a letter and cannot be empty.\n");
    }

    // Age
    while (true)
    {
        printf("Age: ");
        if (scanf("%d", &p.age) != 1) {
             while(getchar()!='\n');
             printf("Invalid input. Please enter a number for age.\n");
             continue;
        }
        while(getchar()!='\n');
        if (p.age > 0 && p.age <= 120)
            break;
        printf("Age must be between 1 and 120.\n");
    }
    
    // Gender
    while(true){
        printf("Gender (M/F/O): ");
        scanf(" %c", &p.gender);
        while(getchar()!='\n');
        p.gender = toupper(p.gender);
        if(p.gender == 'M' || p.gender == 'F' || p.gender == 'O') break;
        printf("Invalid gender. Please enter M, F, or O.\n");
    }

    // Address
    printf("Address: ");
    fgets(p.address, sizeof(p.address), stdin);
    p.address[strcspn(p.address, "\n")] = 0;

    // Condition
    printf("Condition (e.g., critical, stable): ");
    fgets(p.condition, sizeof(p.condition), stdin);
    p.condition[strcspn(p.condition, "\n")] = 0;


    // Blood Group
    while (true)
    {
        printf("Blood Group (e.g., A+): ");
        scanf("%s", p.bloodGroup);
        while(getchar()!='\n');
        if (isValidBloodGroup(p.bloodGroup))
            break;
        printf("Invalid blood group.\n");
    }

    // Vaccination
    while (true)
    {
        printf("Have you received both doses of the COVID-19 vaccine? (y/n): ");
        scanf(" %c", &p.vaccinesDone);
        while(getchar()!='\n');
        p.vaccinesDone = tolower(p.vaccinesDone);
        if (p.vaccinesDone == 'y' || p.vaccinesDone == 'n')
            break;
        printf("Enter 'y' or 'n' only.\n");
    }

    // Area of Treatment
    while (true)
    {
        printf("Area of Treatment required: ");
        fgets(p.areaOfTreatment, sizeof(p.areaOfTreatment), stdin);
        p.areaOfTreatment[strcspn(p.areaOfTreatment, "\n")] = 0;
        if (strlen(p.areaOfTreatment) > 0)
            break;
        printf("Area of treatment cannot be empty.\n");
    }

    // Insurance
    while (true)
    {
        printf("Insurance? (yes/no): ");
        scanf("%s", p.insurance);
        while(getchar()!='\n');
        if (strcmp(p.insurance, "yes") == 0 || strcmp(p.insurance, "no") == 0)
            break;
        printf("Please enter 'yes' or 'no'.\n");
    }

    // Phone Number
    while (true) {
        printf("Phone Number: ");
        fgets(p.phoneNumber, sizeof(p.phoneNumber), stdin);
        p.phoneNumber[strcspn(p.phoneNumber, "\n")] = 0; // Remove newline
        // Basic validation: check if not empty and contains only digits (and possibly '+', '(', ')', '-')
        // For simplicity, just checking if not empty here. More robust validation can be added.
        if (strlen(p.phoneNumber) > 0) break;
        printf("Phone number cannot be empty.\n");
    }


    FILE *patientFile = fopen(FILENAME, "a"); // Use FILENAME define
    if (patientFile == NULL)
    {
        printf("Error opening patient details file.\n");
        return false;
    }

    fprintf(patientFile, "Patient ID: %d\n", p.id);
    fprintf(patientFile, "Name: %s\n", p.name);
    fprintf(patientFile, "Age: %d\n", p.age);
    fprintf(patientFile, "Gender: %c\n", p.gender);
    fprintf(patientFile, "Address: %s\n", p.address);
    fprintf(patientFile, "Condition: %s\n", p.condition);
    fprintf(patientFile, "Blood Group: %s\n", p.bloodGroup);
    fprintf(patientFile, "Vaccines Done: %c\n", p.vaccinesDone);
    fprintf(patientFile, "Area of Treatment: %s\n", p.areaOfTreatment);
    fprintf(patientFile, "Insurance: %s\n", p.insurance);
    fprintf(patientFile, "Phone Number: %s\n", p.phoneNumber);
    fprintf(patientFile, "Hospital Assigned: %s\n", hospital_names[assigned_hospital_idx_0_based]); // Use 0-based index
    fprintf(patientFile, "Optimal Cost: %.2lf INR\n", (double)averageWeight * moneyFactor);
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

// Structure for ambulance (from ieee-fix-issue40)
struct Ambulance
{
    int id;
    int location;    // 1-based hospital index
    char status[16]; // e.g., "available", "busy"
    char type[20];   // e.g., "Basic", "ICU", "Ventilator"
};
#define MAX_AMBULANCES 20

// Read ambulances from file (from ieee-fix-issue40)
int readAmbulances(struct Ambulance ambulances[], int maxAmb, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error: Could not open ambulance file: %s\n", filename);
        return 0;
    }
    char line[128];
    int count = 0;
    // Skip header line if present
    if (fgets(line, sizeof(line), f) && (strstr(line, "// Format:") == NULL && strstr(line, "Format:") == NULL)) {
        // If the first line is not a comment/header, rewind to process it as data
        fseek(f, 0, SEEK_SET);
    } else if (feof(f) || ferror(f)) { // Handle empty or error on first read
        fclose(f);
        return 0;
    }


    while (fgets(line, sizeof(line), f) && count < maxAmb)
    {
        // More robust comment/empty line skipping
        char first_char = ' ';
        sscanf(line, " %c", &first_char); // Read first non-whitespace char
        if (first_char == '/' || first_char == '\0' || first_char == '\n') 
            continue; 

        int id, loc;
        char status_temp[16]; 
        char type_temp[20];   

        // Format: AmbulanceID,CurrentHospitalIndex,Status,Type
        if (sscanf(line, "%d,%d,%15[^,],%19s", &id, &loc, status_temp, type_temp) == 4)
        {
            ambulances[count].id = id;
            ambulances[count].location = loc;

            strncpy(ambulances[count].status, status_temp, 15);
            ambulances[count].status[15] = '\0'; 

            strncpy(ambulances[count].type, type_temp, 19);
            ambulances[count].type[19] = '\0'; 
            
            count++;
        } else {
            // Optionally print a warning for malformed lines
            // printf("Warning: Malformed line in ambulance file: %s", line);
        }
    }
    fclose(f);
    return count;
}

// Find nearest available ambulance of a specific type to a given hospital location (from ieee-fix-issue40)
int findNearestAmbulance(struct Ambulance ambulances[], int ambCount, int src_patient_loc_1_based, int weights[15][15], const char* requiredType) {
    int minDist = INT_MAX, ambIdx = -1;
    for (int i = 0; i < ambCount; ++i) {
        if (strcmp(ambulances[i].status, "available") != 0) {
            continue;
        }
        if (requiredType != NULL && strcmp(ambulances[i].type, requiredType) != 0) { // Allow NULL for any type
            continue;
        }
            
        if (ambulances[i].location == src_patient_loc_1_based) {
            if (minDist > 0 || ambIdx == -1) { 
                minDist = 0;
                ambIdx = i;
            }
        } else { 
            // ambulance location and patient location are 1-based, weights array is 0-based
            if (ambulances[i].location -1 < 0 || ambulances[i].location -1 >=15 || src_patient_loc_1_based -1 <0 || src_patient_loc_1_based -1 >=15){
                // Invalid index, skip
                continue;
            }
            int dist = weights[ambulances[i].location - 1][src_patient_loc_1_based - 1];
            
            if (dist >= 0 && dist < minDist) { // dist can be 0 if already at location (handled above) or if path is 0
                minDist = dist;
                ambIdx = i;
            }
        }
    }
    return ambIdx;
}

// Update ambulance status and location in file (from ieee-fix-issue40)
void updateAmbulanceStatus(int ambId, int newLocation_1_based, const char *newStatus, const char *filename)
{
    struct Ambulance ambulances[MAX_AMBULANCES];
    int count = readAmbulances(ambulances, MAX_AMBULANCES, filename);
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Error: Could not open ambulance file for writing: %s\n", filename);
        return;
    }
    fprintf(f, "// Format: AmbulanceID,CurrentHospitalIndex (1-based),Status,Type\n"); 
    for (int i = 0; i < count; ++i)
    {
        if (ambulances[i].id == ambId)
        {
            ambulances[i].location = newLocation_1_based;
            strncpy(ambulances[i].status, newStatus, 15);
            ambulances[i].status[15] = '\0'; 
        }
        fprintf(f, "%d,%d,%s,%s\n", ambulances[i].id, ambulances[i].location, ambulances[i].status, ambulances[i].type); 
    }
    fclose(f);
}

// Display all ambulances and their locations, status, and estimated time to src (from ieee-fix-issue40)
void displayAllAmbulances(struct Ambulance ambulances[], int ambCount, char hospital_names[15][50], int src_patient_loc_1_based, int weights[15][15])
{
    printf("\nAmbulance Status List (all ambulances):\n");
    if (ambCount == 0) {
        printf("  No ambulances available or loaded.\n");
        return;
    }
    int available_count = 0;
    for (int i = 0; i < ambCount; ++i)
    {
        // Ensure valid indices for hospital_names and weights
        if (ambulances[i].location < 1 || ambulances[i].location > 15 || src_patient_loc_1_based < 1 || src_patient_loc_1_based > 15) {
            printf("  Ambulance %d (%s) - Invalid location data.\n", ambulances[i].id, ambulances[i].type);
            continue;
        }

        int delay = weights[ambulances[i].location - 1][src_patient_loc_1_based - 1]; 
        printf("  Ambulance %d (%s) at %s | Status: %s | ", 
               ambulances[i].id, 
               ambulances[i].type, 
               hospital_names[ambulances[i].location - 1], 
               ambulances[i].status);

        if (strcmp(ambulances[i].status, "available") == 0) {
            available_count++;
            if (ambulances[i].location == src_patient_loc_1_based) 
            {
                printf("Already at emergency location (0 seconds to reach)\n");
            }
            else if (delay >= 0 && delay < INT_MAX) 
            {
                printf("Estimated time to reach emergency: %d seconds\n", delay);
            }
            else
            {
                printf("Path information unavailable or cannot reach emergency location directly\n");
            }
        } else {
            printf("Currently busy.\n");
        }
    }
    printf("Total ambulances: %d (%d available)\n", ambCount, available_count);
}


// searchPatientById from 'main' branch (enhanced)
struct Patient* searchPatientById(int id) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        perror("Error opening patient_details.txt for search");
        return NULL;
    }

    char line[MAX_FIELD_LEN * 2]; 
    struct Patient *foundPatient = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Patient ID:")) {
            int current_id;
            if (sscanf(line, "Patient ID: %d", &current_id) == 1 && current_id == id) {
                foundPatient = (struct Patient*)malloc(sizeof(struct Patient));
                if (!foundPatient) {
                    perror("Memory allocation failed in searchPatientById");
                    fclose(file);
                    return NULL;
                }
                foundPatient->id = id; 

                // Initialize string fields
                foundPatient->name[0] = '\0'; foundPatient->bloodGroup[0] = '\0';
                foundPatient->address[0] = '\0'; foundPatient->condition[0] = '\0';
                foundPatient->areaOfTreatment[0] = '\0'; foundPatient->insurance[0] = '\0';
                foundPatient->phoneNumber[0] = '\0'; foundPatient->hospitalAssigned[0] = '\0';
                foundPatient->vaccinesDone = ' '; foundPatient->gender = ' ';

                #define READ_FIELD(format_str, field) \
                    if (fgets(line, sizeof(line), file)) { \
                        if (sscanf(line, format_str, field) != 1) { \
                            /* Handle parse error, e.g., set to default or log */ \
                        } \
                    } else { goto cleanup_error; }
                
                #define READ_CHAR_FIELD(format_str, field_ptr) \
                    if (fgets(line, sizeof(line), file)) { \
                        char temp_char_val; \
                        if (sscanf(line, format_str, &temp_char_val) == 1) { \
                            *(field_ptr) = temp_char_val; \
                        } else { /* Handle parse error */ } \
                    } else { goto cleanup_error; }


                READ_FIELD("Name: %[^\n]", foundPatient->name);
                READ_FIELD("Age: %d", &foundPatient->age);
                READ_CHAR_FIELD("Gender: %c", &foundPatient->gender);
                READ_FIELD("Address: %[^\n]", foundPatient->address);
                READ_FIELD("Condition: %[^\n]", foundPatient->condition);
                READ_FIELD("Blood Group: %[^\n]", foundPatient->bloodGroup);
                READ_CHAR_FIELD("Vaccines Done: %c", &foundPatient->vaccinesDone);
                READ_FIELD("Area of Treatment: %[^\n]", foundPatient->areaOfTreatment);
                READ_FIELD("Insurance: %[^\n]", foundPatient->insurance);
                READ_FIELD("Phone Number: %[^\n]", foundPatient->phoneNumber);
                READ_FIELD("Hospital Assigned: %[^\n]", foundPatient->hospitalAssigned);
                READ_FIELD("Optimal Cost: %lf INR", &foundPatient->optimalCost);
                
                if (fgets(line, sizeof(line), file) == NULL || strstr(line, "---") == NULL) {
                     goto cleanup_error;
                }

                fclose(file);
                return foundPatient;

            cleanup_error: 
                if (foundPatient) free(foundPatient);
                foundPatient = NULL;
                fprintf(stderr, "Error reading complete record for ID %d.\n", id);
                break; 
            }
        }
    }
    fclose(file);
    return NULL; 
}

// updatePatientRecord from 'main' branch (enhanced)
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

    char line[MAX_FIELD_LEN * 2]; 
    bool found = false;
    int line_count_in_record = 14; // Patient ID, Name, Age, Gender, Address, Condition, Blood Group, Vaccines, Area, Insurance, Phone, Hospital, Cost, Separator

    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Patient ID:"))
        {
            int id_from_file;
            sscanf(line, "Patient ID: %d", &id_from_file);

            if (id_from_file == patient->id)
            {
                found = true;
                fprintf(temp, "Patient ID: %d\n", patient->id);
                fprintf(temp, "Name: %s\n", patient->name);
                fprintf(temp, "Age: %d\n", patient->age);
                fprintf(temp, "Gender: %c\n", patient->gender);
                fprintf(temp, "Address: %s\n", patient->address);
                fprintf(temp, "Condition: %s\n", patient->condition);
                fprintf(temp, "Blood Group: %s\n", patient->bloodGroup);
                fprintf(temp, "Vaccines Done: %c\n", patient->vaccinesDone);
                fprintf(temp, "Area of Treatment: %s\n", patient->areaOfTreatment);
                fprintf(temp, "Insurance: %s\n", patient->insurance);
                fprintf(temp, "Phone Number: %s\n", patient->phoneNumber);
                fprintf(temp, "Hospital Assigned: %s\n", patient->hospitalAssigned);
                fprintf(temp, "Optimal Cost: %.2lf INR\n", patient->optimalCost);
                fprintf(temp, "-----------------\n"); 

                for (int i = 0; i < (line_count_in_record - 1); i++) { // Skip old record lines
                    if (fgets(line, sizeof(line), file) == NULL) break;
                }
            }
            else
            {
                fputs(line, temp); // Write the "Patient ID" line
                for (int i = 0; i < (line_count_in_record - 1); i++) { // Write rest of this non-target record
                     if (fgets(line, sizeof(line), file) != NULL) {
                        fputs(line, temp);
                     } else break;
                }
            }
        }
        // This else should ideally not be reached if file format is consistent (starts with Patient ID)
        // else { fputs(line, temp); } 
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
        remove("temp.txt"); 
    }
    return found;
}


// Function to generate statistics
void generatePatientStatistics()
{
    FILE *file = fopen(FILENAME, "r"); // Use FILENAME
    if (!file) {
        printf("Could not open patient details file for statistics.\n");
        return;
    }

    int totalPatients = 0;
    int vaccinatedY = 0; // Count for 'y' or 'Y'
    int withInsuranceYes = 0; // Count for "yes"
    double totalCost = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Patient ID:"))
        {
            totalPatients++;
        }
        else if (strstr(line, "Vaccines Done:"))
        {
            char vaccine_status;
            if (sscanf(line, "Vaccines Done: %c", &vaccine_status) == 1) {
                if (tolower(vaccine_status) == 'y') {
                    vaccinatedY++;
                }
            }
        }
        else if (strstr(line, "Insurance:"))
        {
            char insurance_status[10]; // Increased buffer for "yes" or "no"
            if (sscanf(line, "Insurance: %s", insurance_status) == 1) {
                 // Convert to lowercase for case-insensitive comparison
                for(int i = 0; insurance_status[i]; i++){
                    insurance_status[i] = tolower(insurance_status[i]);
                }
                if (strcmp(insurance_status, "yes") == 0) {
                    withInsuranceYes++;
                }
            }
        }
        else if (strstr(line, "Optimal Cost:"))
        {
            double cost;
            if (sscanf(line, "Optimal Cost: %lf INR", &cost) == 1) { // Match " INR"
                 totalCost += cost;
            }
        }
    }
    fclose(file);

    printf("\n=== Patient Statistics ===\n");
    printf("Total Patients: %d\n", totalPatients);
    printf("Vaccinated Patients (Y): %d (%.1f%%)\n", vaccinatedY,
           totalPatients > 0 ? (vaccinatedY * 100.0 / totalPatients) : 0);
    printf("Patients with Insurance (yes): %d (%.1f%%)\n", withInsuranceYes,
           totalPatients > 0 ? (withInsuranceYes * 100.0 / totalPatients) : 0);
    printf("Average Optimal Cost per Patient: %.2f INR\n",
           totalPatients > 0 ? (totalCost / totalPatients) : 0.0); // Handle division by zero
    printf("=======================\n");
}

// Feedback functions from 'main' branch
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

double calculateHospitalScore(int distance, double rating) { // rating is double now
    // Lower score is better. Rating reduces score. Distance increases it.
    // Adjust weighting as needed. Example: rating 0-5.
    // If rating is 0 (not rated), don't let it overly influence.
    if (rating == 0.0) return (double)distance; // Or treat as average, e.g., distance - (2.5 * X)
    return (double)distance - (rating * 10); // Example: Each star reduces effective distance by 10 units
}


void saveFeedback(int hospitalNum_1_based, const char *feedback, int rating, char hospital_names[][50]) {
    // Check if hospital_feedback.txt exists, if not, create it with headers
    FILE *checkFile = fopen("hospital_feedback.txt", "r");
    if (!checkFile) {
        // File doesn't exist, create it and add all hospital names as headers
        FILE *initFile = fopen("hospital_feedback.txt", "w");
        if (!initFile) {
            printf("Error creating hospital_feedback.txt.\n");
            return;
        }
        for (int i = 0; i < 15; ++i) { // Assuming 15 hospitals
            fprintf(initFile, "Hospital: %s\n", hospital_names[i]);
            fprintf(initFile, "(No feedback yet)\n");
            fprintf(initFile, "-----------------------------------------------------\n");
        }
        fclose(initFile);
    } else {
        fclose(checkFile);
    }

    FILE *file = fopen("hospital_feedback.txt", "r");
    FILE *temp = fopen("temp_feedback.txt", "w");
    if (!file || !temp) {
        printf("Error opening feedback files for saving.\n");
        if(file) fclose(file);
        if(temp) fclose(temp);
        return;
    }

    char line[1000];
    bool inTargetHospitalSection = false;
    bool feedbackAdded = false;
    char targetHospitalHeader[100];
    sprintf(targetHospitalHeader, "Hospital: %s", hospital_names[hospitalNum_1_based-1]);

    while (fgets(line, sizeof(line), file)) {
        fputs(line, temp); // Write the line first

        if (strstr(line, targetHospitalHeader)) {
            inTargetHospitalSection = true;
            // Check if the next line is "(No feedback yet)"
            long currentPos = ftell(file);
            char nextLine[1000];
            if (fgets(nextLine, sizeof(nextLine), file)) {
                if (strstr(nextLine, "(No feedback yet)")) {
                    // Skip writing "(No feedback yet)" as we are adding new feedback
                } else {
                    fputs(nextLine, temp); // Write it back if it's not the placeholder
                }
            } else {
                 // Reached EOF after header, means no placeholder was there.
            }
            // Add new feedback
            fprintf(temp, "Rating: %d stars\n", rating);
            fprintf(temp, "Feedback: \"%s\"\n", feedback);
            feedbackAdded = true;
        }
         // If we were in the target section and hit the separator, reset flag
        if (inTargetHospitalSection && strstr(line, "-----------------------------------------------------")) {
            inTargetHospitalSection = false;
        }
    }

    fclose(file);
    fclose(temp);

    remove("hospital_feedback.txt");
    rename("temp_feedback.txt", "hospital_feedback.txt");
}


void displayHospitalFeedback(int hospitalNum_1_based, char hospital_names[][50]) {
    FILE *file = fopen("hospital_feedback.txt", "r");
    printf("\n=== Feedback for %s ===\n", hospital_names[hospitalNum_1_based-1]);

    if (!file) {
        printf("No feedback file found. Please add feedback first.\n");
        printf("================================\n");
        return;
    }

    char line[1000];
    bool inTargetHospitalSection = false;
    bool feedbackFoundForHospital = false;
    char targetHospitalHeader[100];
    sprintf(targetHospitalHeader, "Hospital: %s", hospital_names[hospitalNum_1_based-1]);

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, targetHospitalHeader)) {
            inTargetHospitalSection = true;
            continue; // Don't print the header itself, already printed
        }

        if (inTargetHospitalSection) {
            if (strstr(line, "-----------------------------------------------------")) {
                break; // End of section for this hospital
            }
            if (!strstr(line, "(No feedback yet)")) { // Don't print placeholder
                 printf("%s", line); // Print the feedback line (includes newline)
                 feedbackFoundForHospital = true;
            }
        }
    }

    if (!feedbackFoundForHospital) {
        printf("No specific feedback entries available yet for this hospital.\n");
    }
    printf("================================\n");
    fclose(file);
}


double calculateAverageFeedbackRating(int hospitalNum_1_based, char hospital_names[][50]) {
    FILE *file = fopen("hospital_feedback.txt", "r");
    if (!file) {
        return 0.0; // No file, no rating
    }

    char line[1000];
    bool inTargetHospitalSection = false;
    int totalRatingSum = 0;
    int ratingCount = 0;
    char targetHospitalHeader[100];
    sprintf(targetHospitalHeader, "Hospital: %s", hospital_names[hospitalNum_1_based-1]);

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, targetHospitalHeader)) {
            inTargetHospitalSection = true;
            continue;
        }

        if (inTargetHospitalSection) {
            if (strstr(line, "-----------------------------------------------------")) {
                break; 
            }
            if (strstr(line, "Rating:")) {
                int rating;
                if (sscanf(line, "Rating: %d stars", &rating) == 1) {
                    totalRatingSum += rating;
                    ratingCount++;
                }
            }
        }
    }
    fclose(file);
    return ratingCount > 0 ? (double)totalRatingSum / ratingCount : 0.0;
}


int main()
{
    int hospitals = 15;
    int choice;
    int near_hosp_1_based, src_1_based, dest_1_based; // Clarified 1-based
    char input1_emergency_yn;
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

    readMatrixFromFile(matrix, matrix_file);
    readMatrixFromFile(casualtiesMatrix, casualties_file);
    readMatrixFromFile(weights, weights_file);
    readHospitalNamesFromFile(hospital_names, hospital_names_file);

    NODE *adjList = createAdjacencyList(hospitals, matrix, casualtiesMatrix, weights, hospital_names);

    for (;;)
    {
        printf("\nEnter your choice\n");
        printf("1. Finding Hospital\n");
        printf("2. Print Hospital Name & Feedback\n"); // Updated menu item
        printf("3. Display Hospitals List\n");
        printf("4. Search Patient Record\n");
        printf("5. Update Patient Record\n");
        printf("6. Generate Statistics\n");
        printf("7. Give Feedback\n");
        printf("8. Exit\n");

        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 8) {
            clear_input_buffer(); // Clear buffer after successful scanf
        } else {
            printf("Invalid choice. Please select a valid option (1-8).\n");
            clear_input_buffer(); // Clear buffer on invalid input
            continue;
        }
        printf("\n");

        switch (choice)
        {
        case 1:
            while (1)
            {
                printf("Is it a case of emergency?(y/n): \n");
                scanf(" %c", &input1_emergency_yn);
                clear_input_buffer();
                input1_emergency_yn = tolower(input1_emergency_yn);
                if (input1_emergency_yn == 'y' || input1_emergency_yn == 'n')
                    break;
                printf("Invalid input. Please enter 'y' or 'n'.\n");
            }

            if (input1_emergency_yn == 'y')
            {
                while (1)
                {
                    printf("\nSelect a number corresponding to your nearest location.\n");
                    printf("1.Rajaji nagar  2.Sahakar Nagar  3.Sanjaynagar  4.Yeshwanthpur  5.Nagarbavi\n");
                    printf("6.Bannerghatta  7.Shanti Nagar  8.Marathahalli  9.Sarjapur  10.Jayanagar\n");
                    printf("11.Bommasandra  12.Whitefield  13.Krishnarajapuram  14.Yelahanka  15.Kengeri: \n");
                    if (scanf("%d", &src_1_based) == 1 && src_1_based >= 1 && src_1_based <= hospitals) {
                        clear_input_buffer();
                        break;
                    }
                    printf("Invalid nearest location hospital number. Please enter a number between 1 and 15.\n");
                    clear_input_buffer();
                }

                struct Ambulance ambulances[MAX_AMBULANCES];
                int ambCount = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");

                char requiredType[20];
                printf("\nEnter required ambulance type (e.g., Basic, ALS, ICU): "); 
                fgets(requiredType, sizeof(requiredType), stdin);
                requiredType[strcspn(requiredType, "\n")] = 0; 

                printf("\n=== AMBULANCE LIST (before hospital selection) ===\n");
                displayAllAmbulances(ambulances, ambCount, hospital_names, src_1_based, weights);
                printf("=== END OF AMBULANCE LIST ===\n\n");
                fflush(stdout);

                printf("\nFinding the nearest hospital possible...\n");

                NODE cur = adjList[src_1_based - 1]; 
                int minWeight = INT_MAX;
                int nearestHospital_idx_0_based = -1; 

                while (cur != NULL)
                {
                    int currentDestHospital_idx_0_based = -1;
                    for (int k = 0; k < hospitals; ++k) { 
                        if (strcmp(hospital_names[k], cur->hospital_name) == 0) {
                            currentDestHospital_idx_0_based = k;
                            break;
                        }
                    }

                    if (currentDestHospital_idx_0_based != -1) {
                        if (cur->weight < minWeight) {
                            minWeight = cur->weight;
                            nearestHospital_idx_0_based = currentDestHospital_idx_0_based; 
                        }
                        printf("\nCasualty Level at Hospital %s (%d): %d - ", hospital_names[currentDestHospital_idx_0_based], currentDestHospital_idx_0_based + 1, cur->casualtiesPresent);
                        printAdmissionDifficulty(cur->casualtiesPresent);
                    } else {
                        printf("\nError: Could not find hospital index for connected hospital: %s\n", cur->hospital_name);
                    }
                    cur = cur->link;
                }
                
                // Check self-loop
                if (weights[src_1_based - 1][src_1_based - 1] < minWeight && weights[src_1_based-1][src_1_based-1] != INT_MAX && weights[src_1_based-1][src_1_based-1] >=0)
                {
                    minWeight = weights[src_1_based - 1][src_1_based - 1];
                    nearestHospital_idx_0_based = src_1_based - 1; 
                }

                int averageWeightForCost = (minWeight == INT_MAX) ? 0 : minWeight; // Use 0 cost if no path
                double optimalCost = (double)averageWeightForCost * moneyFactor;

                if (nearestHospital_idx_0_based != -1)
                {
                    // Display feedback for the initially found nearest hospital
                    printf("\n=== Initial Nearest Hospital: %s ===\n", hospital_names[nearestHospital_idx_0_based]);
                    double avgRatingNearest = calculateAverageFeedbackRating(nearestHospital_idx_0_based + 1, hospital_names);
                    if (avgRatingNearest > 0) {
                        printf("Average User Rating: %.1f stars\n", avgRatingNearest);
                    } else {
                        printf("Status: Not Rated\n");
                    }
                    // displayHospitalFeedback(nearestHospital_idx_0_based + 1, hospital_names); // Optionally display full feedback here

                    printf("\nNearest hospital to Region %s (%d) is Hospital %s (%d) with a road rating of %d\n", hospital_names[src_1_based-1], src_1_based, hospital_names[nearestHospital_idx_0_based], nearestHospital_idx_0_based + 1, minWeight == INT_MAX ? -1 : minWeight);
                    printf("\nOptimal Cost to %s: %.2lf INR\n", hospital_names[nearestHospital_idx_0_based], optimalCost);
                    
                    int ambIdx = findNearestAmbulance(ambulances, ambCount, src_1_based, weights, requiredType); 

                    if (ambIdx == -1 && strcmp(requiredType, "Basic") != 0) { 
                        printf("\nNo ambulance of type '%s' found. Searching for a 'Basic' ambulance...\n", requiredType);
                        ambIdx = findNearestAmbulance(ambulances, ambCount, src_1_based, weights, "Basic");
                        if (ambIdx != -1) {
                            printf("Found a 'Basic' ambulance instead.\n");
                        }
                    }
                    
                    if (ambIdx != -1)
                    {
                        printf("\nDispatching Ambulance %d (%s) from %s to %s...\n", ambulances[ambIdx].id, ambulances[ambIdx].type, hospital_names[ambulances[ambIdx].location - 1], hospital_names[nearestHospital_idx_0_based]);
                        simulateAmbulanceMovement(hospital_names[src_1_based - 1], hospital_names[nearestHospital_idx_0_based], 30, 80);
                        updateAmbulanceStatus(ambulances[ambIdx].id, nearestHospital_idx_0_based + 1, "busy", "ambulance_locations.txt");
                    }
                    else
                    {
                        printf("No available ambulance of type '%s' or 'Basic' could be dispatched to %s!\n", requiredType, hospital_names[nearestHospital_idx_0_based]);
                    }
                    fflush(stdout);

                    int ambCountAfter = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
                    printf("\n=== AMBULANCE LIST (after dispatch) ===\n");
                    displayAllAmbulances(ambulances, ambCountAfter, hospital_names, src_1_based, weights); 
                    printf("=== END OF AMBULANCE LIST ===\n\n");
                    fflush(stdout);

                    if (handlePatientDetails(nearestHospital_idx_0_based, averageWeightForCost, hospital_names, moneyFactor))
                    {
                        printf("Patient can be admitted to the hospital.\n\n");
                    }
                    else
                    {
                        printf("Patient could not be admitted (e.g. ID exists or invalid input).\n\n");
                    }
                }
                else
                {
                    printf("\nNo adjacent hospitals found or path to self for %s (%d)\n", hospital_names[src_1_based-1], src_1_based);
                }
            }
            else if (input1_emergency_yn == 'n') // Non-emergency
            {
                while (1)
                {
                    printf("\nSelect the number corresponding to your nearest location: \n");
                    // ... (location list) ...
                    printf("1.Rajaji nagar  2.Sahakar Nagar  3.Sanjaynagar  4.Yeshwanthpur  5.Nagarbavi\n");
                    printf("6.Bannerghatta  7.Shanti Nagar  8.Marathahalli  9.Sarjapur  10.Jayanagar\n");
                    printf("11.Bommasandra  12.Whitefield  13.Krishnarajapuram  14.Yelahanka  15.Kengeri: \n");
                    if (scanf("%d", &src_1_based) == 1 && src_1_based >= 1 && src_1_based <= 15) {
                        clear_input_buffer();
                        break;
                    }
                    printf("Invalid nearest location hospital number. Please enter a number between 1 and 15.\n");
                    clear_input_buffer();
                }

                struct Ambulance ambulances[MAX_AMBULANCES];
                int ambCount = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
                char requiredType[20];
                printf("\nEnter required ambulance type (e.g., Basic, ALS, ICU): ");
                fgets(requiredType, sizeof(requiredType), stdin);
                requiredType[strcspn(requiredType, "\n")] = 0;

                printf("\n=== AMBULANCE LIST (before hospital selection) ===\n");
                displayAllAmbulances(ambulances, ambCount, hospital_names, src_1_based, weights);
                printf("=== END OF AMBULANCE LIST ===\n\n");
                fflush(stdout);

                while (1)
                {
                    printf("\nSelect the hospital you want to go: \n");
                    // ... (hospital list) ...
                    printf("1.Suguna_Hospital(Rajajinagar)\t\t2.Aster_CMI_Hospital(Sahakarnagar)\t3.MS_Ramaiah_Hospital(Sanjaynagar)\n");
                    printf("4.People's_Tree_Hospital(Yeshwanthpur) \t5.Fortis_Hospital(Nagarbhavi)\t\t6.Appolo_Hospital(Bannerghatta)\n");
                    printf("7.HCG_Hospital(Shantinagar)\t\t8.Cloudnine_Hospital(Marathahalli)\t9.Columbia_Asia(Sarjapur)\n");
                    printf("10.Sagar_Hospital(Jayanagar)\t\t11.Narayana_Hrudayalaya(Bommasandra)\t12.Manipal_Hospital(Whitefield)\n");
                    printf("13.Koshys_Hospital(Krishnarajapuram)\t14.Sparsh_Hospital(Yelahanka)\t\t15.BGS_Gleneagles_Hospital(Kengeri)\n");

                    if (scanf("%d", &dest_1_based) == 1 && dest_1_based >= 1 && dest_1_based <= 15) {
                        clear_input_buffer();
                        break;
                    }
                    printf("Invalid destination hospital number. Please enter a number between 1 and 15.\n");
                    clear_input_buffer();
                }

                if (src_1_based == dest_1_based) {
                    printf("\nSource and destination are the same hospital: %s.\n", hospital_names[src_1_based-1]);
                    printf("No travel required - Cost: 0 INR\n\n");
                    if (handlePatientDetails(src_1_based -1, 0, hospital_names, moneyFactor)) { // Pass 0-based index
                        printf("Patient can be processed at the current hospital.\n\n");
                    } else {
                        printf("Patient cannot be processed due to invalid input or other issues.\n\n");
                    }
                } else {
                    printf("\nFinding the optimal route to %s: \n", hospital_names[dest_1_based-1]);

                    int distanceDijkstra[hospitals]; // Renamed to avoid conflict if weights is global/passed around
                    int previousDijkstra[hospitals];
                    bool visitedDijkstra[hospitals];
                    
                    for (int i = 0; i < hospitals; i++) {
                        distanceDijkstra[i] = INT_MAX;
                        previousDijkstra[i] = -1;
                        visitedDijkstra[i] = false;
                    }
                    distanceDijkstra[src_1_based - 1] = 0;

                    for (int count = 0; count < hospitals - 1; count++) {
                        int u_idx = -1;
                        int minCurrentDistance = INT_MAX;
                        for (int v_idx = 0; v_idx < hospitals; v_idx++) {
                            if (!visitedDijkstra[v_idx] && distanceDijkstra[v_idx] < minCurrentDistance) {
                                u_idx = v_idx;
                                minCurrentDistance = distanceDijkstra[v_idx];
                            }
                        }
                        if (u_idx == -1) break; // No path to remaining nodes
                        visitedDijkstra[u_idx] = true;

                        NODE current_node_in_adj = adjList[u_idx];
                        while (current_node_in_adj != NULL) {
                            int v_neighbor_idx = -1;
                            for(int k=0; k<hospitals; ++k){ // Find index of neighbor
                                if(strcmp(hospital_names[k], current_node_in_adj->hospital_name) == 0){
                                    v_neighbor_idx = k;
                                    break;
                                }
                            }
                            if(v_neighbor_idx != -1 && !visitedDijkstra[v_neighbor_idx] && 
                               distanceDijkstra[u_idx] != INT_MAX && /* Check u_idx is reachable */
                               distanceDijkstra[u_idx] + current_node_in_adj->weight < distanceDijkstra[v_neighbor_idx]) {
                                distanceDijkstra[v_neighbor_idx] = distanceDijkstra[u_idx] + current_node_in_adj->weight;
                                previousDijkstra[v_neighbor_idx] = u_idx;
                            }
                            current_node_in_adj = current_node_in_adj->link;
                        }
                    }

                    if (distanceDijkstra[dest_1_based - 1] == INT_MAX) {
                        printf("No route found from %s to %s.\n", hospital_names[src_1_based - 1], hospital_names[dest_1_based - 1]);
                    } else {
                        printf("\nOptimal route from %s to %s (Distance: %d): \n", hospital_names[src_1_based - 1], hospital_names[dest_1_based - 1], distanceDijkstra[dest_1_based - 1]);
                        int route[20];
                        int routeLen = 0;
                        int current_path_idx = dest_1_based - 1;
                        while (current_path_idx != -1) {
                            route[routeLen++] = current_path_idx;
                            current_path_idx = previousDijkstra[current_path_idx];
                        }
                        for (int i = routeLen - 1; i >= 0; i--) {
                            printf("%s", hospital_names[route[i]]);
                            if (i > 0) printf(" -> ");
                        }
                        printf("\n");
                        
                        double pathDistanceForCost = distanceDijkstra[dest_1_based-1];
                        
                        // Hospital Recommendation Logic from 'main' branch
                        double threshold = pathDistanceForCost * 1.2; // 20% longer
                        int bestHospital_1_based = dest_1_based; // Start with user's choice
                        double bestScore = calculateHospitalScore(pathDistanceForCost, calculateAverageFeedbackRating(dest_1_based, hospital_names));
                        
                        printf("\nChecking for alternative hospitals...\n");
                        for (int alt_idx_0 = 0; alt_idx_0 < hospitals; alt_idx_0++) {
                            if (alt_idx_0 != (dest_1_based - 1) && distanceDijkstra[alt_idx_0] != INT_MAX && distanceDijkstra[alt_idx_0] <= threshold) {
                                double altScore = calculateHospitalScore(distanceDijkstra[alt_idx_0], calculateAverageFeedbackRating(alt_idx_0 + 1, hospital_names));
                                printf("Considering %s (Dist: %d, Rating: %.1f, Score: %.1f)\n", hospital_names[alt_idx_0], distanceDijkstra[alt_idx_0], calculateAverageFeedbackRating(alt_idx_0 + 1, hospital_names), altScore);
                                if (altScore < bestScore) {
                                    bestScore = altScore;
                                    bestHospital_1_based = alt_idx_0 + 1;
                                }
                            }
                        }
                        
                        if (bestHospital_1_based != dest_1_based) {
                            printf("\n--- Recommendation --- \n");
                            printf("Original choice: %s (Distance: %d, Avg Rating: %.1f)\n", hospital_names[dest_1_based-1], (int)pathDistanceForCost, calculateAverageFeedbackRating(dest_1_based, hospital_names));
                            printf("Recommended: %s (Distance: %d, Avg Rating: %.1f)\n", hospital_names[bestHospital_1_based-1], distanceDijkstra[bestHospital_1_based-1], calculateAverageFeedbackRating(bestHospital_1_based, hospital_names));
                            printf("Switch to recommended hospital %s? (y/n): ", hospital_names[bestHospital_1_based-1]);
                            char switchChoice;
                            scanf(" %c", &switchChoice);
                            clear_input_buffer();
                            if (tolower(switchChoice) == 'y') {
                                dest_1_based = bestHospital_1_based;
                                pathDistanceForCost = distanceDijkstra[dest_1_based-1]; // Update path distance
                                printf("Destination updated to %s.\n", hospital_names[dest_1_based-1]);
                                // Re-calculate route for display if destination changed (simplified: just update dest_1_based and pathDistanceForCost)
                                routeLen = 0; // Recalculate route for simulation
                                current_path_idx = dest_1_based - 1;
                                 while (current_path_idx != -1) {
                                    route[routeLen++] = current_path_idx;
                                    current_path_idx = previousDijkstra[current_path_idx];
                                }
                            }
                        }
                        
                        printf("\n=== Current Feedback for Chosen Destination: %s ===\n", hospital_names[dest_1_based-1]);
                        displayHospitalFeedback(dest_1_based, hospital_names);

                        double optimalCostForChosenDest = pathDistanceForCost * moneyFactor;
                        printf("Optimal Cost to %s: %.2lf INR\n", hospital_names[dest_1_based-1], optimalCostForChosenDest);

                        int ambIdx = findNearestAmbulance(ambulances, ambCount, src_1_based, weights, requiredType);
                        if (ambIdx == -1 && strcmp(requiredType, "Basic") != 0) {
                            printf("\nNo ambulance of type '%s' found. Searching for a 'Basic' ambulance...\n", requiredType);
                            ambIdx = findNearestAmbulance(ambulances, ambCount, src_1_based, weights, "Basic");
                            if (ambIdx != -1) printf("Found a 'Basic' ambulance instead.\n");
                        }

                        if (ambIdx != -1) {
                            printf("\nDispatching Ambulance %d (%s) from %s to %s...\n", ambulances[ambIdx].id, ambulances[ambIdx].type, hospital_names[ambulances[ambIdx].location - 1], hospital_names[dest_1_based - 1]);
                            // Simulate movement along the calculated route
                            for (int i = routeLen - 1; i > 0; --i) {
                                simulateAmbulanceMovement(hospital_names[route[i]], hospital_names[route[i-1]], 15, 60);
                            }
                            updateAmbulanceStatus(ambulances[ambIdx].id, dest_1_based, "busy", "ambulance_locations.txt");
                        } else {
                            printf("No available ambulance of type '%s' or 'Basic' could be dispatched for the route to %s!\n", requiredType, hospital_names[dest_1_based-1]);
                        }
                        
                        int ambCountAfter = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
                        printf("\n=== AMBULANCE LIST (after dispatch) ===\n");
                        displayAllAmbulances(ambulances, ambCountAfter, hospital_names, src_1_based, weights);
                        printf("=== END OF AMBULANCE LIST ===\n\n");
                        fflush(stdout);

                        if (handlePatientDetails(dest_1_based - 1, pathDistanceForCost, hospital_names, moneyFactor)) { // Pass 0-based index
                            printf("Patient can be admitted to the hospital.\n\n");
                        } else {
                            printf("Patient cannot be admitted due to invalid input.\n\n");
                        }
                    }
                }
            }
            break;
            
        case 2: // Print Hospital Name & Feedback
            while (1) {
                printf("Enter the hospital number (1-15) to print its name and feedback: ");
                if (scanf("%d", &near_hosp_1_based) == 1 && near_hosp_1_based >= 1 && near_hosp_1_based <= hospitals) {
                    clear_input_buffer();
                    break;
                }
                printf("Invalid hospital number. Please enter a number between 1 and %d.\n", hospitals);
                clear_input_buffer();
            }
            printHospitalName(near_hosp_1_based, hospital_names);
            double avgRating = calculateAverageFeedbackRating(near_hosp_1_based, hospital_names);
            if (avgRating > 0) {
                printf("Average User Rating: %.1f stars\n", avgRating);
            } else {
                printf("Status: Not Rated (No feedback entries with ratings yet)\n");
            }
            displayHospitalFeedback(near_hosp_1_based, hospital_names);
            break;

        case 3:
            displayAdjacencyList(adjList, hospitals, hospital_names);
            break;

        case 4: // Search Patient Record
        {
            int searchId;
            printf("Enter Patient ID to search: ");
            if(scanf("%d", &searchId) != 1) {
                printf("Invalid ID format.\n");
                clear_input_buffer();
                break;
            }
            clear_input_buffer();

            struct Patient *patient = searchPatientById(searchId);
            if (patient) {
                printf("\nPatient Found:\n");
                printf("Patient ID: %d\n", patient->id);
                printf("Name: %s\n", patient->name);
                printf("Age: %d\n", patient->age);
                printf("Gender: %c\n", patient->gender);
                printf("Address: %s\n", patient->address);
                printf("Condition: %s\n", patient->condition);
                printf("Blood Group: %s\n", patient->bloodGroup);
                printf("Vaccines Done: %c\n", patient->vaccinesDone);
                printf("Area of Treatment: %s\n", patient->areaOfTreatment);
                printf("Insurance: %s\n", patient->insurance);
                printf("Phone Number: %s\n", patient->phoneNumber);
                printf("Hospital Assigned: %s\n", patient->hospitalAssigned);
                printf("Optimal Cost: %.2f INR\n", patient->optimalCost);
                printf("-----------------\n");
                free(patient);
            } else {
                printf("Patient with ID %d not found.\n", searchId);
            }
            break;
        }

         case 5: // Update Patient Record
        {
           int updateId;
            printf("Enter Patient ID to update: ");
            if (scanf("%d", &updateId) != 1) {
                printf("Invalid input for Patient ID.\n");
                clear_input_buffer();
                break; 
            }
            clear_input_buffer(); 

            struct Patient *patient_to_update = searchPatientById(updateId);
            if (patient_to_update) {
                char input_buffer[MAX_FIELD_LEN]; 
                char char_input;
                int int_input;
                //double double_input; // Not used here, but kept if needed for other fields
                char num_str[30]; 

                printf("\n--- Updating Patient ID: %d ---\n", patient_to_update->id);
                printf("Current Name: %s. New: ", patient_to_update->name);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0) strcpy(patient_to_update->name, input_buffer);

                printf("Current Age: %d. New: ", patient_to_update->age);
                fgets(num_str, sizeof(num_str), stdin); num_str[strcspn(num_str, "\n")] = 0;
                if (strlen(num_str) > 0 && sscanf(num_str, "%d", &int_input) == 1) patient_to_update->age = int_input;
                
                printf("Current Gender: %c. New (M/F/O): ", patient_to_update->gender);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0 && sscanf(input_buffer, " %c", &char_input) == 1 && (toupper(char_input)=='M' || toupper(char_input)=='F' || toupper(char_input)=='O')) patient_to_update->gender = toupper(char_input);

                printf("Current Address: %s. New: ", patient_to_update->address);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0) strcpy(patient_to_update->address, input_buffer);
                
                printf("Current Condition: %s. New: ", patient_to_update->condition);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0) strcpy(patient_to_update->condition, input_buffer);

                printf("Current Blood Group: %s. New: ", patient_to_update->bloodGroup);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0 && isValidBloodGroup(input_buffer)) strcpy(patient_to_update->bloodGroup, input_buffer);
                
                printf("Current Vaccines Done (y/n): %c. New: ", patient_to_update->vaccinesDone);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0 && sscanf(input_buffer, " %c", &char_input) == 1 && (tolower(char_input)=='y' || tolower(char_input)=='n')) patient_to_update->vaccinesDone = tolower(char_input);

                printf("Current Area of Treatment: %s. New: ", patient_to_update->areaOfTreatment);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0) strcpy(patient_to_update->areaOfTreatment, input_buffer);

                printf("Current Insurance (yes/no): %s. New: ", patient_to_update->insurance);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0 && (strcmp(input_buffer, "yes")==0 || strcmp(input_buffer, "no")==0) ) strcpy(patient_to_update->insurance, input_buffer);
                
                printf("Current Phone Number: %s. New: ", patient_to_update->phoneNumber);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0) strcpy(patient_to_update->phoneNumber, input_buffer);
                
                // Hospital Assigned & Optimal Cost are usually system-determined, but allow update if needed.
                printf("Current Hospital Assigned: %s. New: ", patient_to_update->hospitalAssigned);
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                if (strlen(input_buffer) > 0) strcpy(patient_to_update->hospitalAssigned, input_buffer);

                // Example for Optimal Cost update, though it's usually calculated
                // printf("Current Optimal Cost: %.2lf. New: ", patient_to_update->optimalCost);
                // fgets(num_str, sizeof(num_str), stdin); num_str[strcspn(num_str, "\n")] = 0;
                // if (strlen(num_str) > 0 && sscanf(num_str, "%lf", &double_input) == 1) patient_to_update->optimalCost = double_input;


                if (updatePatientRecord(patient_to_update)) {
                    printf("Record updated successfully.\n");
                } else {
                    printf("Error updating record.\n");
                }
                free(patient_to_update); 
            } else {
                printf("Patient with ID %d not found.\n", updateId);
            }
            break;
        }        

        case 6:
            generatePatientStatistics();
            break;
        
        case 7: // Give Feedback
        {
            int feedbackHospital_1_based;
            char feedback_text[500];
            int rating_stars;

            while (1) {
                printf("\nEnter hospital number (1-15) to provide feedback: ");
                if (scanf("%d", &feedbackHospital_1_based) == 1 && feedbackHospital_1_based >= 1 && feedbackHospital_1_based <= 15) {
                    clear_input_buffer();
                    break;
                }
                printf("Invalid hospital number. Please enter a number between 1 and 15.\n");
                clear_input_buffer();
            }
            
            while (1) {
                printf("Enter rating (1-5 stars): ");
                if (scanf("%d", &rating_stars) == 1 && rating_stars >= 1 && rating_stars <= 5) {
                    clear_input_buffer();
                    break;
                }
                printf("Invalid rating. Please enter a number between 1 and 5.\n");
                clear_input_buffer();
            }
            
            printf("Enter your feedback (max 500 characters):\n");
            fgets(feedback_text, sizeof(feedback_text), stdin);
            feedback_text[strcspn(feedback_text, "\n")] = 0; 

            saveFeedback(feedbackHospital_1_based, feedback_text, rating_stars, hospital_names);
            printf("\nThank you for your feedback!\n");
            break;
        }

        case 8: // Exit
            printf("Exiting system.\n");
            // Free adjacency list memory before exiting
            for(int i=0; i<hospitals; ++i) {
                NODE current = adjList[i];
                NODE next_node;
                while(current != NULL) {
                    next_node = current->link;
                    free(current);
                    current = next_node;
                }
            }
            free(adjList);
            return 0;

        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }
    }
    // Should not be reached if exit is handled properly
    return 0; 
}
