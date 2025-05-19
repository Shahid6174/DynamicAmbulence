#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ENTRIES 100
#define MAX_LINE_LENGTH 100

struct Patient {
    char name[50];
    int age;
    char bloodGroup[10];
    int patientID;
    char vaccinesDone;
    char areaOfTreatment[20];
    char insurance[5];
    long long phoneNumber;
    char hospitalAssigned[50];
    char severity[10]; //  Added severity field
    float optimalCost;
};

int isValidName(const char *name) {
    if (strlen(name) == 0) return 0;
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && name[i] != ' ') return 0;
    }
    return 1;
}

int isValidPhoneNumber(long long phone) {
    return (phone >= 1000000000LL && phone <= 9999999999LL); // ensure 10-digit number
}

int isValidPatientID(int id) {
    return (id > 0);
}

//  compare severity priority for sorting (Red < Yellow < Green)
int getSeverityPriority(const char *severity) {
    if (strcmp(severity, "Red") == 0) return 0;
    if (strcmp(severity, "Yellow") == 0) return 1;
    if (strcmp(severity, "Green") == 0) return 2;
    return 3; // unknown
}

int compareSeverity(const void *a, const void *b) {
    struct Patient *p1 = (struct Patient *)a;
    struct Patient *p2 = (struct Patient *)b;
    return getSeverityPriority(p1->severity) - getSeverityPriority(p2->severity);
}

int main() {
    FILE *file = fopen("patient_details.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int entryCount = 0;

    struct Patient entries[MAX_ENTRIES];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "Name: %[^\n]", entries[entryCount].name) == 1) {
            if (!isValidName(entries[entryCount].name)) {
                printf("Invalid name at entry %d: %s\n", entryCount + 1, entries[entryCount].name);
                continue;
            }

            sscanf(fgets(line, sizeof(line), file), "Age: %d", &entries[entryCount].age);
            sscanf(fgets(line, sizeof(line), file), "Blood Group: %[^\n]", entries[entryCount].bloodGroup);
            sscanf(fgets(line, sizeof(line), file), "Patient ID: %d", &entries[entryCount].patientID);

            if (!isValidPatientID(entries[entryCount].patientID)) {
                printf("Invalid patient ID at entry %d: %d\n", entryCount + 1, entries[entryCount].patientID);
                continue;
            }

            sscanf(fgets(line, sizeof(line), file), "Vaccines Done: %c", &entries[entryCount].vaccinesDone);
            sscanf(fgets(line, sizeof(line), file), "Area of Treatment: %[^\n]", entries[entryCount].areaOfTreatment);
            sscanf(fgets(line, sizeof(line), file), "Insurance: %[^\n]", entries[entryCount].insurance);
            sscanf(fgets(line, sizeof(line), file), "Phone Number: %lld", &entries[entryCount].phoneNumber);

            if (!isValidPhoneNumber(entries[entryCount].phoneNumber)) {
                printf("Invalid phone number at entry %d: %lld\n", entryCount + 1, entries[entryCount].phoneNumber);
                continue;
            }

            sscanf(fgets(line, sizeof(line), file), "Hospital Assigned: %[^\n]", entries[entryCount].hospitalAssigned);
            sscanf(fgets(line, sizeof(line), file), "Severity: %[^\n]", entries[entryCount].severity);
            sscanf(fgets(line, sizeof(line), file), "Optimal Cost: %f", &entries[entryCount].optimalCost);

            entryCount++;
        }
    }

    fclose(file);

    // Sort by severity if needed
    qsort(entries, entryCount, sizeof(struct Patient), compareSeverity);

    for (int i = 0; i < entryCount; i++) {
        printf("Name: %s\n", entries[i].name);
        printf("Age: %d\n", entries[i].age);
        printf("Blood Group: %s\n", entries[i].bloodGroup);
        printf("Patient ID: %d\n", entries[i].patientID);
        printf("Vaccines Done: %c\n", entries[i].vaccinesDone);
        printf("Area of Treatment: %s\n", entries[i].areaOfTreatment);
        printf("Insurance: %s\n", entries[i].insurance);
        printf("Phone Number: %lld\n", entries[i].phoneNumber);
        printf("Hospital Assigned: %s\n", entries[i].hospitalAssigned);
        printf("Severity: %s\n", entries[i].severity); // Show severity
        printf("Optimal Cost: %.2f\n", entries[i].optimalCost);
        printf("-------------------------\n");
    }

    return 0;
}
