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

// Function to display the adjacency list
void displayAdjacencyList(NODE *adjList, int hospitals, char hospital_names[15][50])
{
    printf("Displaying the adjacency lists for all hospitals:\n\n");

    for (int i = 0; i < hospitals; i++)
    {
        printf("Hospital: %s\n", hospital_names[i]);
        printf("Connected Hospitals / Neighbors:\n");
        display(adjList[i]); // Assuming this prints the adjacency list of the ith hospital
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

bool handlePatientDetails(int src, int nearestHospital, int averageWeight, char hospital_names[15][50], int moneyFactor)
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
};
#define MAX_AMBULANCES 20

// Read ambulances from file
int readAmbulances(struct Ambulance ambulances[], int maxAmb, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;
    char line[128];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < maxAmb)
    {
        if (line[0] == '/' || line[0] == '\\' || line[0] == '\n')
            continue; // skip comments/empty
        int id, loc;
        char status[16];
        if (sscanf(line, "%d,%d,%15s", &id, &loc, status) == 3)
        {
            ambulances[count].id = id;
            ambulances[count].location = loc;
            strncpy(ambulances[count].status, status, 15);
            ambulances[count].status[15] = '\0';
            count++;
        }
    }
    fclose(f);
    return count;
}
// Find nearest available ambulance to a given hospital
int findNearestAmbulance(struct Ambulance ambulances[], int ambCount, int src, int weights[15][15])
{
    int minDist = INT_MAX, ambIdx = -1;
    for (int i = 0; i < ambCount; ++i)
    {
        if (strcmp(ambulances[i].status, "available") != 0)
            continue;
        int dist = weights[ambulances[i].location - 1][src - 1];
        if (dist > 0 && dist < minDist)
        {
            minDist = dist;
            ambIdx = i;
        }
    }
    return ambIdx;
}

// Update ambulance status and location in file
void updateAmbulanceStatus(int ambId, int newLocation, const char *newStatus, const char *filename)
{
    struct Ambulance ambulances[MAX_AMBULANCES];
    int count = readAmbulances(ambulances, MAX_AMBULANCES, filename);
    FILE *f = fopen(filename, "w");
    if (!f)
        return;
    fprintf(f, "// Format: AmbulanceID,CurrentHospitalIndex (1-based),Status\n");
    for (int i = 0; i < count; ++i)
    {
        if (ambulances[i].id == ambId)
        {
            ambulances[i].location = newLocation;
            strncpy(ambulances[i].status, newStatus, 15);
            ambulances[i].status[15] = '\0';
        }
        fprintf(f, "%d,%d,%s\n", ambulances[i].id, ambulances[i].location, ambulances[i].status);
    }
    fclose(f);
}

// Display all ambulances and their locations, status, and estimated time to src
void displayAllAmbulances(struct Ambulance ambulances[], int ambCount, char hospital_names[15][50], int src, int weights[15][15])
{
    printf("\nAmbulance Status List (all ambulances):\n");
    int count = 0;
    for (int i = 0; i < ambCount; ++i)
    {
        int delay = weights[ambulances[i].location - 1][src - 1];
        printf("  Ambulance %d at %s | Status: %s | ", ambulances[i].id, hospital_names[ambulances[i].location - 1], ambulances[i].status);
        if (ambulances[i].location == src)
        {
            printf("Already at emergency location (0 seconds to reach)\n");
        }
        else if (delay >= 0 && delay < 10000)
        {
            printf("Estimated time to reach emergency: %d seconds\n", delay);
        }
        else
        {
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

                // Skip Patient ID line (if your file includes it again after blood group, usually not)
                // Based on your updatePatientRecord, Patient ID is WRITTEN first.
                // In searchPatientById, it's the first thing you read. So, you don't skip it here
                // unless it appears a second time in the record (unlikely/bad format).
                // Let's assume your file format means after Blood Group comes Vaccines Done, not ID again.

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
                    if (sscanf(line, "Optimal Cost: %lf", &foundPatient->optimalCost) != 1) {
                        foundPatient->optimalCost = 0.0; // Default or error value
                        fprintf(stderr, "Warning: Could not parse Optimal Cost for ID %d. Setting to 0.0.\n", id);
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
// Your existing updatePatientRecord function (adjusted for consistency)
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
                fprintf(temp, "Optimal Cost: %.2lf\n", patient->optimalCost);
                fprintf(temp, "-----------------\n"); // Separator line

                // Skip the original record lines from the input file
                // Start from line 1 (Name) as we already read "Patient ID"
                // So, skip (line_count_in_record - 1) more lines
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
            // This case should ideally not be reached if the file format is strict
            // (i.e., every record starts with "Patient ID:").
            // If it can happen, you might need more robust parsing.
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
            printf("7. Exit\n");

            if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 7)
                break;
            printf("Invalid choice. Please select a valid option (1-7).\n");
            while (getchar() != '\n')
                ;
        }

        printf("\n");

        if (choice < 1 || choice > 7)
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

                if (nearestHospital != -1)
                {
                    // Show ambulance list again BEFORE dispatch
                    printf("\n=== AMBULANCE LIST (before dispatch) ===\n");
                    displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights);
                    printf("=== END OF AMBULANCE LIST ===\n\n");
                    fflush(stdout);

                    printf("\nNearest hospital to Region %d is Hospital %s with a road rating of %d\n", src, hospital_names[nearestHospital - 1], averageWeight);
                    printf("\nOptimal Cost: %.2lf INR\n", optimalCost);

                    // Find and dispatch the nearest available ambulance
                    int ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights);
                    if (ambIdx != -1)
                    {
                        printf("\nDispatching Ambulance %d from %s...\n", ambulances[ambIdx].id, hospital_names[ambulances[ambIdx].location - 1]);
                        simulateAmbulanceMovement(hospital_names[src - 1], hospital_names[nearestHospital - 1], 30, 80);
                        updateAmbulanceStatus(ambulances[ambIdx].id, nearestHospital, "busy", "ambulance_locations.txt");
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

                    if (handlePatientDetails(src, nearestHospital, averageWeight, hospital_names, moneyFactor))
                    {
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

                printf("\nFinding the optimal route: \n");

                // Implement Dijkstra's algorithm to find the optimal route
                int distance[hospitals];
                int previous[hospitals];
                bool visited[hospitals];

                // Initialize distances and previous nodes
                for (int i = 0; i < hospitals; i++)
                {
                    distance[i] = INT_MAX; // INFINITY
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
                    printf("\nAverage Edge Weight: %.2lf\n", averageWeight);
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
                        updateAmbulanceStatus(ambulances[ambIdx].id, dest, "busy", "ambulance_locations.txt");
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

                    if (handlePatientDetails(src, dest, averageWeight, hospital_names, moneyFactor))
                    {
                        printf("Patient can be admitted to the hospital.\n\n");
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
            else
            {
                printf("Enter valid input...\n");
            }
            break;

        case 2:
            // Validate near_hosp input
            while (1)
            {
                printf("Enter the hospital number to print its name: ");
                if (scanf("%d", &near_hosp) == 1 && near_hosp >= 1 && near_hosp <= hospitals)
                    break;
                printf("Invalid hospital number. Please enter a number between 1 and %d.\n", hospitals);
                while (getchar() != '\n')
                    ;
            }
            printHospitalName(near_hosp, hospital_names);
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
            generatePatientStatistics();
            break;

        case 7:
            return 0;

        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }
    }
}
