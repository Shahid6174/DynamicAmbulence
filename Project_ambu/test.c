#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int main() {
    // Update cost for patient 1234
    if (set_patient_param("1234", "Current_Treatment_Cost(INR)", 4021.23f)) {
        printf("Parameter updated successfully.\n");
    } else {
        printf("Failed to update parameter.\n");
    }
    
    float cost = get_patient_param("1234", "Current_Treatment_Cost(INR)");
    printf("Cost: %.2f\n", cost);

    float optimal_cost = get_patient_param("9798", "Optimal Cost");
    printf("Optimal Cost: %.2f\n", optimal_cost);

    return 0;
}

