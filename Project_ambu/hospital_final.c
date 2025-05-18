#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

struct node
{
    char hospital_name[50];
    int casualtiesPresent;
    int weight;
    struct node *link;
};

typedef struct node * NODE;

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
    NODE cur;
    if (first == NULL)
    {
        printf("List is empty.\n");
        return;
    }

    cur = first;
    while (cur != NULL)
    {
        printf("%s(Casualties:%d, Road_Weight:%d) -> ", cur->hospital_name, cur->casualtiesPresent, cur->weight);
        cur = cur->link;
    }

    printf("NULL\n");
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
    for (int i = 0; i < hospitals; i++)
    {
        printf("Adjacency list for Hospital %s => \n", hospital_names[i]);
        display(adjList[i]);
        printf("\n");
    }
}

// Function to read a matrix from a file
void readMatrixFromFile(int matrix[15][15], const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (fscanf(file, "%d", &matrix[i][j]) != 1)
            {
                printf("Error reading matrix from file %s.\n", filename);
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
}

// Function to read hospital names from a file
void readHospitalNamesFromFile(char hospital_names[15][50], const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 15; i++)
    {
        if (fgets(hospital_names[i], 50, file) == NULL)
        {
            printf("Error reading hospital names from file %s.\n", filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // Remove newline character if present
        hospital_names[i][strcspn(hospital_names[i], "\n")] = '\0';
    }

    fclose(file);
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

// Function to check if a hospital ID already exists
bool isHospitalIdExist(int hospitalId)
{
    FILE *patientFile = fopen("patient_details.txt", "r");
    if (patientFile == NULL)
    {
        return false; // File doesn't exist, so the ID is not found
    }

    char line[256];
    while (fgets(line, sizeof(line), patientFile) != NULL)
    {
        int existingHospitalId;
        if (sscanf(line, "Hospital ID: %d", &existingHospitalId) == 1)
        {
            if (existingHospitalId == hospitalId)
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
void handlePatientDetails(int src, int nearestHospital, int averageWeight, char hospital_names[15][50], int moneyFactor)
{
    // Existing emergency or non-emergency case logic

    // Additional logic for gathering patient details
    char name[50], bloodGroup[5], insurance[5];
    int age, hospitalId;
    char areaOfTreatment[50];
    long long phoneNumber;

    printf("Enter unique 4-digit Hospital ID: ");
    scanf("%d", &hospitalId);
    if (hospitalId < 1000 || hospitalId > 9999) {
        printf("Invalid ID. Please enter a 4-digit number.\n");
        return; // or loop back to re-enter
    }

    if (isHospitalIdExist(hospitalId))
    {
        printf("\nPatient record with Hospital ID %d already exists. Not writing to the file.\n\n", hospitalId);
        return;
    }

    printf("\nEnter patient details:\n");
    printf("Name: ");
    scanf("%s", name);

    printf("Age: ");
    scanf("%d", &age);

    printf("Blood Group: ");
    scanf("%s", bloodGroup);

    printf("Have both vaccines been done? (y/n): ");
    char vaccinesDone;
    scanf(" %c", &vaccinesDone);

    printf("Area of Treatment required: ");
    scanf("%s", areaOfTreatment);

    printf("Insurance? (yes/no): ");
    scanf("%s", insurance);

    printf("Phone Number: ");
    scanf("%lld", &phoneNumber);

    // Write patient details to a file
    FILE *patientFile = fopen("patient_details.txt", "a");
    if (patientFile == NULL)
    {
        printf("Error opening patient details file for writing.\n");
    }
    else
    {
        fprintf(patientFile, "Name: %s\n", name);
        fprintf(patientFile, "Age: %d\n", age);
        fprintf(patientFile, "Blood Group: %s\n", bloodGroup);
        fprintf(patientFile, "Hospital ID: %d\n", hospitalId);
        fprintf(patientFile, "Vaccines Done: %c\n", vaccinesDone);
        fprintf(patientFile, "Area of Treatment: %s\n", areaOfTreatment);
        fprintf(patientFile, "Insurance: %s\n", insurance);
        fprintf(patientFile, "Phone Number: %lld\n", phoneNumber);
        fprintf(patientFile, "Hospital Assigned: %s\n", hospital_names[nearestHospital - 1]);
        fprintf(patientFile, "Optimal Cost: %.2lf INR\n", (double)averageWeight * moneyFactor);
        fprintf(patientFile, "-----------------\n");
        fclose(patientFile);

        printf("\nPatient details successfully recorded.\n\n");
    }
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

    int matrix[15][15];
    int casualtiesMatrix[15][15];
    int weights[15][15];
    char hospital_names[15][50];
    int moneyFactor = 500;

    // Read matrices from files
    readMatrixFromFile(matrix, "matrix.txt");
    readMatrixFromFile(casualtiesMatrix, "casualtiesMatrix.txt");
    readMatrixFromFile(weights, "weights.txt");
    readHospitalNamesFromFile(hospital_names, "hospital_names.txt");

    NODE *adjList = createAdjacencyList(hospitals, matrix, casualtiesMatrix, weights, hospital_names);

    for (;;)
    {
        printf("\nEnter your choice\n1.Finding_Hospital\n2.Print_Hospital_Name\n3.Display_Hospitals_List\n4:Exit: ");
        scanf("%d", &choice);
        printf("\n");
        if (choice < 1 || choice > 4) {
            printf("Invalid choice. Please select a valid option.\n");
            continue; // go back to the start of the loop
        }

        switch (choice)
        {
        case 1:
            printf("Is it a case of emergency?(y/n): \n");
            scanf(" %c", &input1);

            if (input1 == 'y')
            {
                printf("\nSelect a number corresponding to your nearest location.\n");
                printf("1.Rajaji nagar  2.Sahakar Nagar  3.Sanjaynagar  4.Yeshwanthpur  5.Nagarbavi\n");
                printf("6.Bannerghatta  7.Shanti Nagar  8.Marathahalli  9.Sarjapur  10.Jayanagar\n");
                printf("11.Bommasandra  12.Whitefield  13.Krishnarajapuram  14.Yelahanka  15.Kengeri: \n");
                scanf("%d", &src);

                // Check if the input hospital number is valid
                if (src >= 1 && src <= hospitals)
                {
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
                        printf("\nNearest hospital to Region %d is Hospital %s with a road rating of %d\n", src, hospital_names[nearestHospital - 1], averageWeight);
                        printf("\nOptimal Cost: %.2lf INR\n", optimalCost);
                        handlePatientDetails(src, nearestHospital, averageWeight, hospital_names, moneyFactor);
                        printf("Patient will be admitted to the hospital.\n\n");
                        exit(0);
                    }
                    else
                    {
                        printf("\nNo adjacent hospitals found for Hospital %d\n", src);
                    }
                }
                else
                {
                    printf("\nInvalid input. Enter a valid hospital number.\n");
                }
            }
            else if (input1 == 'n')
            {
                printf("\nSelect the number corresponding to your nearest location: \n");
                printf("1.Rajaji nagar   2.Sahakar Nagar   3.Sanjaynagar         4.Yeshwanthpur   5.Nagarbavi\n");
                printf("6.Bannerghatta   7.Shanti Nagar    8.Marathahalli        9.Sarjapur       10.Jayanagar\n");
                printf("11.Bommasandra   12.Whitefield     13.Krishnarajapuram   14.Yelahanka     15.Kengeri: \n");
                scanf("%d", &src);
                if(src < 1 || src > 15){
                    printf("invalid nearest location hospital number.Please Enter between 1 and 15.\n");
                    continue;
                }
                printf("\nSelect the hospital you want to go: \n");
                printf("1.Suguna_Hospital(Rajajinagar)\t\t2.Aster_CMI_Hospital(Sahakarnagar)\t3.MS_Ramaiah_Hospital(Sanjaynagar)\n");
                printf("4.People's_Tree_Hospital(Yeshwanthpur) \t5.Fortis_Hospital(Nagarbhavi)\t\t6.Appolo_Hospital(Bannerghatta)\n");
                printf("7.HCG_Hospital(Shantinagar)\t\t8.Cloudnine_Hospital(Marathahalli)\t9.Columbia_Asia(Sarjapur)\n");
                printf("10.Sagar_Hospital(Jayanagar)\t\t11.Narayana_Hrudayalaya(Bommasandra)\t12.Manipal_Hospital(Whitefield)\n");
                printf("13.Koshys_Hospital(Krishnarajapuram)\t14.Sparsh_Hospital(Yelahanka)\t\t15.BGS_Gleneagles_Hospital(Kengeri)\n");
                scanf("%d", &dest);
                if(dest < 1 || dest > 15){
                    printf("invalid destination hospital number.Please Enter between 0 and 14.\n");
                    continue;
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

                while (current != -1)
                {
                    printf("%s", hospital_names[current]);
                    int prev = previous[current];

                    if (prev != -1)
                    {
                        // Update total weight and edge count
                        totalWeight += weights[prev][current];
                        edgeCount++;

                        printf(" <- ");
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
                    handlePatientDetails(src, dest, averageWeight, hospital_names, moneyFactor);
                    printf("Patient will be admitted to the hospital.\n\n");
                    exit(0);
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
            printf("Enter the hospital number to print its name: ");
            scanf("%d", &near_hosp);
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