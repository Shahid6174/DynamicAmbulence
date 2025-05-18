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
    long long phoneNumber; // changed from long to long long
    char hospitalAssigned[50];
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

int isValidHospitalID(int id) {
    return (id > 0);
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

            if (!isValidHospitalID(entries[entryCount].patientID)) {
                printf("Invalid patient ID at entry %d: %d\n", entryCount + 1, entries[entryCount].patientID);
                continue;
            }

            sscanf(fgets(line, sizeof(line), file), "Vaccines Done: %c", &entries[entryCount].vaccinesDone);
            sscanf(fgets(line, sizeof(line), file), "Area of Treatment: %[^\n]", entries[entryCount].areaOfTreatment);
            sscanf(fgets(line, sizeof(line), file), "Insurance: %[^\n]", entries[entryCount].insurance);
            sscanf(fgets(line, sizeof(line), file), "Phone Number: %lld", &entries[entryCount].phoneNumber); // use %lld

            if (!isValidPhoneNumber(entries[entryCount].phoneNumber)) {
                printf("Invalid phone number at entry %d: %lld\n", entryCount + 1, entries[entryCount].phoneNumber);
                continue;
            }

            sscanf(fgets(line, sizeof(line), file), "Hospital Assigned: %[^\n]", entries[entryCount].hospitalAssigned);
            sscanf(fgets(line, sizeof(line), file), "Optimal Cost: %f", &entries[entryCount].optimalCost);

            entryCount++;
        }
    }

    fclose(file);

    for (int i = 0; i < entryCount; i++) {
        printf("Name: %s\n", entries[i].name);
        printf("Age: %d\n", entries[i].age);
        printf("Blood Group: %s\n", entries[i].bloodGroup);
        printf("Hospital ID: %d\n", entries[i].patientID);
        printf("Vaccines Done: %c\n", entries[i].vaccinesDone);
        printf("Area of Treatment: %s\n", entries[i].areaOfTreatment);
        printf("Insurance: %s\n", entries[i].insurance);
        printf("Phone Number: %lld\n", entries[i].phoneNumber); // use %lld for printing
        printf("Hospital Assigned: %s\n", entries[i].hospitalAssigned);
        printf("Optimal Cost: %.2f\n", entries[i].optimalCost);
        printf("-----------------\n");
    }

    return 0;
}
