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
    char vaccinesDone;
    char areaOfTreatment[50];
    char insurance[5];
    long long phoneNumber;
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
    char type[20];   // e.g., "Basic", "ICU", "Ventilator"
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
        char status_temp[16]; // Temporary buffer for status
        char type_temp[20];   // Temporary buffer for type

        // Format: AmbulanceID,CurrentHospitalIndex,Status,Type
        if (sscanf(line, "%d,%d,%15[^,],%19s", &id, &loc, status_temp, type_temp) == 4)
        {
            ambulances[count].id = id;
            ambulances[count].location = loc;

            strncpy(ambulances[count].status, status_temp, 15);
            ambulances[count].status[15] = '\0'; // Ensure null termination

            strncpy(ambulances[count].type, type_temp, 19);
            ambulances[count].type[19] = '\0'; // Ensure null termination

            count++;
        }
    }
    fclose(f);
    return count;
}
// Find nearest available ambulance of a specific type to a given hospital location
int findNearestAmbulance(struct Ambulance ambulances[], int ambCount, int src, int weights[15][15], const char* requiredType) {
    int minDist = INT_MAX, ambIdx = -1;
    for (int i = 0; i < ambCount; ++i) {
        // Check availability
        if (strcmp(ambulances[i].status, "available") != 0) {
            continue;
        }
        // Check type
        if (strcmp(ambulances[i].type, requiredType) != 0) {
            continue;
        }

        // If ambulance is already at the source location (src is 1-based for location parameters)
        if (ambulances[i].location == src) {
            // If this ambulance is at the source and meets criteria, it's the best option (distance 0)
            // We only update if this is better than a previously found non-zero distance,
            // or if no ambulance has been found yet (ambIdx == -1).
            if (minDist > 0 || ambIdx == -1) { // Check if current minDist is not already 0
                minDist = 0;
                ambIdx = i;
            }
        } else { // If ambulance is at a different location
            // Calculate distance from ambulance's location to src (both are 1-based for user context, so convert to 0-based for array access)
            int dist = weights[ambulances[i].location - 1][src - 1];

            // Ensure it's a valid path (dist > 0, assuming 0 or less means no direct path or error)
            // And it's shorter than current minDist
            if (dist > 0 && dist < minDist) {
                minDist = dist;
                ambIdx = i;
            }
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
    fprintf(f, "// Format: AmbulanceID,CurrentHospitalIndex (1-based),Status,Type\n"); // Updated comment
    for (int i = 0; i < count; ++i)
    {
        if (ambulances[i].id == ambId)
        {
            ambulances[i].location = newLocation;
            strncpy(ambulances[i].status, newStatus, 15);
            ambulances[i].status[15] = '\0'; // Ensure null termination
            // The type of the ambulance being updated remains unchanged by this function
        }
        fprintf(f, "%d,%d,%s,%s\n", ambulances[i].id, ambulances[i].location, ambulances[i].status, ambulances[i].type); // Added type
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
        int delay = weights[ambulances[i].location - 1][src - 1]; // Assuming src is 1-based for hospital_names and weights access
        printf("  Ambulance %d (%s) at %s | Status: %s | ",
               ambulances[i].id,
               ambulances[i].type,
               hospital_names[ambulances[i].location - 1], // Assuming location is 1-based
               ambulances[i].status);

        if (ambulances[i].location == src) // src is the emergency location (1-based)
        {
            printf("Already at emergency location (0 seconds to reach)\n");
        }
        // Check if ambulance can reach the source (src-1 for 0-based weights array)
        else if (delay > 0 && delay < INT_MAX) // Check for valid path; delay > 0 means it's not the same location
        {
            printf("Estimated time to reach emergency: %d seconds\n", delay);
        }
        else
        {
            // If delay is INT_MAX or 0 (if not src itself), it implies no path or error.
            printf("Path information unavailable or cannot reach emergency location directly\n");
        }
        count++;
    }
    printf("Total ambulances: %d\n", count);
}

// New function to search patient by ID
struct Patient *searchPatientById(int patientId)
{
    FILE *file = fopen("patient_details.txt", "r");
    if (!file)
        return NULL;

    struct Patient *patient = malloc(sizeof(struct Patient));
    char line[256];
    int found = 0;

    // Initialize patient structure
    memset(patient, 0, sizeof(struct Patient));

    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Patient ID:"))
        {
            int id;
            if (sscanf(line, "Patient ID: %d", &id) == 1 && id == patientId)
            {
                found = 1;
                patient->id = id;

                // Go back to start of patient record
                long pos = ftell(file);
                fseek(file, 0, SEEK_SET);
                int inRecord = 0;

                // Read file until we find our record
                while (fgets(line, sizeof(line), file))
                {
                    if (strstr(line, "Patient ID:"))
                    {
                        int currentId;
                        sscanf(line, "Patient ID: %d", &currentId);
                        if (currentId == patientId)
                        {
                            inRecord = 1;
                            continue;
                        }
                    }

                    if (inRecord)
                    {
                        if (strstr(line, "-----------------"))
                        {
                            break; // End of record
                        }

                        char field[50];
                        if (sscanf(line, "Name: %[^\n]", patient->name) == 1)
                            continue;
                        if (sscanf(line, "Age: %d", &patient->age) == 1)
                            continue;
                        if (sscanf(line, "Blood Group: %[^\n]", patient->bloodGroup) == 1)
                            continue;
                        if (sscanf(line, "Vaccines Done: %c", &patient->vaccinesDone) == 1)
                            continue;
                        if (sscanf(line, "Area of Treatment: %[^\n]", patient->areaOfTreatment) == 1)
                            continue;
                        if (sscanf(line, "Insurance: %[^\n]", patient->insurance) == 1)
                            continue;
                        if (sscanf(line, "Phone Number: %lld", &patient->phoneNumber) == 1)
                            continue;
                        if (sscanf(line, "Hospital Assigned: %[^\n]", patient->hospitalAssigned) == 1)
                            continue;
                        if (sscanf(line, "Optimal Cost: %lf", &patient->optimalCost) == 1)
                            continue;
                    }
                }
                break;
            }
        }
    }

    fclose(file);
    if (!found)
    {
        free(patient);
        return NULL;
    }
    return patient;
}

// Function to update patient records
bool updatePatientRecord(struct Patient *patient)
{
    FILE *file = fopen("patient_details.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp)
        return false;

    char line[256];
    bool found = false;
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Patient ID:"))
        {
            int id;
            sscanf(line, "Patient ID: %d", &id);
            if (id == patient->id)
            {
                found = true;
                fprintf(temp, "Name: %s\n", patient->name);
                fprintf(temp, "Age: %d\n", patient->age);
                fprintf(temp, "Blood Group: %s\n", patient->bloodGroup);
                fprintf(temp, "Patient ID: %d\n", patient->id);
                fprintf(temp, "Vaccines Done: %c\n", patient->vaccinesDone);
                fprintf(temp, "Area of Treatment: %s\n", patient->areaOfTreatment);
                fprintf(temp, "Insurance: %s\n", patient->insurance);
                fprintf(temp, "Phone Number: %lld\n", patient->phoneNumber);
                fprintf(temp, "Hospital Assigned: %s\n", patient->hospitalAssigned);
                fprintf(temp, "Optimal Cost: %.2lf\n", patient->optimalCost);
                fprintf(temp, "-----------------\n");

                // Skip original record
                for (int i = 0; i < 11; i++)
                {
                    fgets(line, sizeof(line), file);
                }
            }
            else
            {
                fputs(line, temp);
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
        remove("patient_details.txt");
        rename("temp.txt", "patient_details.txt");
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

                // ADD PROMPT FOR AMBULANCE TYPE HERE
                char requiredType[20];
                printf("\nEnter required ambulance type (e.g., BLS, ALS): "); // Updated prompt text
                scanf("%19s", requiredType);
                // Clear input buffer
                while (getchar() != '\n');

                printf("\n=== AMBULANCE LIST (before hospital selection) ===\n");
                displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights);
                printf("=== END OF AMBULANCE LIST ===\n\n");
                fflush(stdout);

                printf("\nFinding the nearest hospital possible...\n");

                // Traverse the adjacency list of the input hospital (src)
                NODE cur = adjList[src - 1]; // src is 1-based for user, adjList is 0-indexed
                int minWeight = INT_MAX;
                int nearestHospital_idx = -1; // Will store the 0-based index of the nearest hospital

                while (cur != NULL)
                {
                    int currentDestHospital_idx = -1;
                    // Find the 0-based index of the hospital name in cur->hospital_name
                    for (int k = 0; k < hospitals; ++k) { // 'hospitals' var is 15
                        if (strcmp(hospital_names[k], cur->hospital_name) == 0) {
                            currentDestHospital_idx = k;
                            break;
                        }
                    }

                    if (currentDestHospital_idx != -1) {
                        if (cur->weight < minWeight) {
                            minWeight = cur->weight;
                            nearestHospital_idx = currentDestHospital_idx; // Store 0-based index
                        }
                        printf("\nCasualty Level at Hospital %s (%d): %d - ", hospital_names[currentDestHospital_idx], currentDestHospital_idx + 1, cur->casualtiesPresent);
                        printAdmissionDifficulty(cur->casualtiesPresent);
                    } else {
                        // This case should ideally not happen if hospital_names and adjList are consistent
                        printf("\nError: Could not find hospital index for connected hospital: %s\n", cur->hospital_name);
                    }
                    cur = cur->link;
                }

                // Check if there is a self-loop with a smaller weight (i.e., staying at src is better)
                // weights is 0-indexed, src is 1-indexed
                if (weights[src - 1][src - 1] < minWeight && weights[src-1][src-1] != INT_MAX && weights[src-1][src-1] >=0) // Ensure self-loop is valid path
                {
                    minWeight = weights[src - 1][src - 1];
                    nearestHospital_idx = src - 1; // 0-based index for src hospital
                }

                int averageWeight = minWeight;
                double optimalCost = (averageWeight == INT_MAX) ? -1 : averageWeight * moneyFactor; // Handle no path found

                if (nearestHospital_idx != -1)
                {
                    // Show ambulance list again BEFORE dispatch
                    printf("\n=== AMBULANCE LIST (before dispatch) ===\n");
                    displayAllAmbulances(ambulances, ambCount, hospital_names, src, weights); // src is patient's location (1-based)
                    printf("=== END OF AMBULANCE LIST ===\n\n");
                    fflush(stdout);
                    // src is 1-based, nearestHospital_idx is 0-based
                    printf("\nNearest hospital to Region %s (%d) is Hospital %s (%d) with a road rating of %d\n", hospital_names[src-1], src, hospital_names[nearestHospital_idx], nearestHospital_idx + 1, averageWeight);
                    if(optimalCost != -1) {
                        printf("\nOptimal Cost: %.2lf INR\n", optimalCost);
                    } else {
                        printf("\nOptimal Cost: N/A (No path found or excessive weight)\n");
                    }


                    // findNearestAmbulance expects src as 1-based patient location
                    int ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights, requiredType);

                    // If specific type not found, try to find a "Basic" ambulance as fallback
                    if (ambIdx == -1 && strcmp(requiredType, "Basic") != 0) { // Avoid re-searching if "Basic" was already the request
                        printf("\nNo ambulance of type '%s' found. Searching for a 'Basic' ambulance...\n", requiredType);
                        ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights, "Basic");
                        if (ambIdx != -1) {
                            printf("Found a 'Basic' ambulance instead.\n");
                        }
                    }

                    if (ambIdx != -1)
                    {
                        // Dispatch message: ambulance location (1-based), destination hospital (0-based for name)
                        printf("\nDispatching Ambulance %d (%s) from %s to %s...\n", ambulances[ambIdx].id, ambulances[ambIdx].type, hospital_names[ambulances[ambIdx].location - 1], hospital_names[nearestHospital_idx]);
                        // simulateAmbulanceMovement: patient location (0-based name) to destination hospital (0-based name)
                        simulateAmbulanceMovement(hospital_names[src - 1], hospital_names[nearestHospital_idx], 30, 80);
                        // updateAmbulanceStatus: newLocation is 1-based
                        updateAmbulanceStatus(ambulances[ambIdx].id, nearestHospital_idx + 1, "busy", "ambulance_locations.txt");
                    }
                    else
                    {
                        // HANDLE NO MATCHING AMBULANCE: destination hospital (0-based for name)
                        printf("No available ambulance of type '%s' or 'Basic' could be dispatched to %s!\n", requiredType, hospital_names[nearestHospital_idx]);
                    }
                    fflush(stdout);

                    // Show all ambulances and their time delays AFTER dispatch
                    int ambCountAfter = readAmbulances(ambulances, MAX_AMBULANCES, "ambulance_locations.txt");
                    printf("\n=== AMBULANCE LIST (after dispatch) ===\n");
                    displayAllAmbulances(ambulances, ambCountAfter, hospital_names, src, weights); // src is patient's location
                    printf("=== END OF AMBULANCE LIST ===\n\n");
                    fflush(stdout);

                    // handlePatientDetails expects nearestHospital as 1-based.
                    if (handlePatientDetails(src, nearestHospital_idx + 1, averageWeight, hospital_names, moneyFactor))
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
                    printf("\nNo adjacent hospitals found or path to self for %s (%d)\n", hospital_names[src-1], src);
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

                // ADD PROMPT FOR AMBULANCE TYPE HERE
                char requiredType[20];
                printf("\nEnter required ambulance type (e.g., BLS, ALS): "); // Updated prompt text
                scanf("%19s", requiredType);
                // Clear input buffer
                while (getchar() != '\n');

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

                    // UPDATE CALL TO findNearestAmbulance
                    int ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights, requiredType);

                    // If specific type not found, try to find a "Basic" ambulance as fallback
                    if (ambIdx == -1 && strcmp(requiredType, "Basic") != 0) { // Avoid re-searching if "Basic" was already the request
                        printf("\nNo ambulance of type '%s' found. Searching for a 'Basic' ambulance...\n", requiredType);
                        ambIdx = findNearestAmbulance(ambulances, ambCount, src, weights, "Basic");
                        if (ambIdx != -1) {
                            printf("Found a 'Basic' ambulance instead.\n");
                        }
                    }

                    if (ambIdx != -1)
                    {
                        printf("\nDispatching Ambulance %d (%s) from %s to %s...\n", ambulances[ambIdx].id, ambulances[ambIdx].type, hospital_names[ambulances[ambIdx].location - 1], hospital_names[dest - 1]);
                        for (int i = routeLen - 1; i > 0; --i)
                        {
                            // Moving from current step in route to next step in route towards destination
                            simulateAmbulanceMovement(hospital_names[route[i]], hospital_names[route[i - 1]], 15, 60);
                        }
                         // Final leg: ambulance current location to patient (src), then patient (src) to hospital (dest)
                        // The loop above handles patient (src) to hospital (dest) if src is part of route.
                        // However, the ambulance is dispatched from its current location.
                        // The simulation should ideally be: amb_loc -> src_loc -> dest_loc (hospital)
                        // For simplicity, current simulation in loop is parts of src_loc -> dest_loc.
                        // The printed dispatch message correctly states amb_loc.
                        updateAmbulanceStatus(ambulances[ambIdx].id, dest, "busy", "ambulance_locations.txt");
                    }
                    else
                    {
                        // HANDLE NO MATCHING AMBULANCE
                        printf("No available ambulance of type '%s' or 'Basic' could be dispatched for the route to %s!\n", requiredType, hospital_names[dest-1]);
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
                printf("Phone Number: %lld\n", patient->phoneNumber);
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
            scanf("%d", &updateId);

            struct Patient *patient = searchPatientById(updateId);
            if (patient)
            {
                printf("Enter new phone number: ");
                scanf("%lld", &patient->phoneNumber);

                if (updatePatientRecord(patient))
                {
                    printf("Record updated successfully.\n");
                }
                else
                {
                    printf("Error updating record.\n");
                }
                free(patient);
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
