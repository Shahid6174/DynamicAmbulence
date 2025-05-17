#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    long phoneNumber;
    char hospitalAssigned[50];
    float optimalCost;
};

int main() {
    FILE *file = fopen("your_data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int entryCount = 0;

    struct Patient entries[MAX_ENTRIES];

    while (fgets(line, sizeof(line), file) != NULL) {
        // Assuming each entry has a fixed structure
        if (sscanf(line, "Name: %[^\n]", entries[entryCount].name) == 1) {
            sscanf(fgets(line, sizeof(line), file), "Age: %d", &entries[entryCount].age);
            sscanf(fgets(line, sizeof(line), file), "Blood Group: %[^\n]", entries[entryCount].bloodGroup);
            sscanf(fgets(line, sizeof(line), file), "Hospital ID: %d", &entries[entryCount].patientID);
            sscanf(fgets(line, sizeof(line), file), "Vaccines Done: %c", &entries[entryCount].vaccinesDone);
            sscanf(fgets(line, sizeof(line), file), "Area of Treatment: %[^\n]", entries[entryCount].areaOfTreatment);
            sscanf(fgets(line, sizeof(line), file), "Insurance: %[^\n]", entries[entryCount].insurance);
            sscanf(fgets(line, sizeof(line), file), "Phone Number: %ld", &entries[entryCount].phoneNumber);
            sscanf(fgets(line, sizeof(line), file), "Hospital Assigned: %[^\n]", entries[entryCount].hospitalAssigned);
            sscanf(fgets(line, sizeof(line), file), "Optimal Cost: %f", &entries[entryCount].optimalCost);

            // Move to the next entry
            entryCount++;
        }
    }

    fclose(file);

    // Now you can use the 'entries' array to integrate with a Google Sheets API using another language.

    // Example: Printing the parsed data
    for (int i = 0; i < entryCount; i++) {
        printf("Name: %s\n", entries[i].name);
        printf("Age: %d\n", entries[i].age);
        printf("Blood Group: %s\n", entries[i].bloodGroup);
        printf("Hospital ID: %d\n", entries[i].patientID);
        printf("Vaccines Done: %c\n", entries[i].vaccinesDone);
        printf("Area of Treatment: %s\n", entries[i].areaOfTreatment);
        printf("Insurance: %s\n", entries[i].insurance);
        printf("Phone Number: %ld\n", entries[i].phoneNumber);
        printf("Hospital Assigned: %s\n", entries[i].hospitalAssigned);
        printf("Optimal Cost: %.2f\n", entries[i].optimalCost);
        printf("-----------------\n");
    }

    return 0;
}