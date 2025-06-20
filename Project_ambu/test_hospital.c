// Simple assert-based tests for shortest path and hospital selection logic
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#define TEST_ENV_MAX_HOSPITALS 15 // Changed from static const int to #define

#define HOSPITALS 3

// Minimal test data for 3 hospitals
int test_weights[HOSPITALS][HOSPITALS] = {
    {0, 2, 5},
    {2, 0, 1},
    {5, 1, 0}
};

char test_hospital_names[HOSPITALS][50] = {
    "1.Suguna_Hospital(rajajinagar)",
    "2.Aster_CMI_Hospital(sahakarnagar)",
    "3.MS_Ramaiah_Hospital(sanjaynagar)"
};

// static const int TEST_ENV_MAX_HOSPITALS = 15; // Removed

struct Ambulance {
    int id;
    int location;    // 1-based hospital index
    char status[16]; // e.g., "available", "busy"
    char type[20];   // e.g., "BLS", "ALS"
};

// Sample Ambulance Data for Testing findNearestAmbulance
// Covers various types, statuses, and locations.
// Assume a small world of 3-5 hospitals for these specific test data points
// to align with manageable weight matrices for tests.
// Locations are 1-based.
struct Ambulance test_ambulances[] = {
    {1, 1, "available", "BLS"},      // At hospital 1, BLS, available
    {2, 2, "available", "ALS"},      // At hospital 2, ALS, available
    {3, 1, "busy",      "BLS"},      // At hospital 1, BLS, busy
    {4, 3, "available", "BLS"},      // At hospital 3, BLS, available
    {5, 2, "available", "BLS"},      // At hospital 2, BLS, available (another BLS)
    {6, 3, "available", "ALS"},      // At hospital 3, ALS, available (another ALS)
    {7, 4, "available", "BLS"},      // At hospital 4, BLS, available
    {8, 5, "available", "Unknown"}   // At hospital 5, Unknown type, available
};
int num_test_ambulances = sizeof(test_ambulances) / sizeof(test_ambulances[0]);

// Placeholder for a sample 15x15 weights matrix for testing.
// We'll only populate parts of it for specific tests.
// Initialize with INT_MAX for no path, 0 for self-loops.
int sample_test_weights[TEST_ENV_MAX_HOSPITALS][TEST_ENV_MAX_HOSPITALS];

// Helper function to initialize sample_test_weights for tests
void init_sample_test_weights() {
    for (int i = 0; i < TEST_ENV_MAX_HOSPITALS; ++i) {
        for (int j = 0; j < TEST_ENV_MAX_HOSPITALS; ++j) {
            if (i == j) {
                sample_test_weights[i][j] = 0; // Distance to self is 0
            } else {
                sample_test_weights[i][j] = INT_MAX; // Default to no path
            }
        }
    }
}

// Declaration for the function in hospital_final.c
int findNearestAmbulance(struct Ambulance ambulances[], int ambCount, int src, int weights[15][15], const char* requiredType);

// Test Case 1: Correct Type Found
void test_find_correct_type() {
    init_sample_test_weights();
    // Patient at/near hospital 1 (src=1)
    // Ambulance 1: id=1, loc=1, available, BLS
    // Ambulance 2: id=2, loc=2, available, ALS
    // Ambulance 4: id=4, loc=3, available, BLS
    // Weights: H1-H2 = 5, H1-H3 = 10
    sample_test_weights[0][1] = 5; sample_test_weights[1][0] = 5; // H1 to H2
    sample_test_weights[0][2] = 10; sample_test_weights[2][0] = 10; // H1 to H3

    // Request BLS, patient at H1. Ambulance 1 (id=1) is at H1.
    int ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "BLS");
    assert(ambIdx != -1 && test_ambulances[ambIdx].id == 1); // Ambulance 1 (id=1)

    // Request ALS, patient at H1. Ambulance 2 (id=2) is at H2 (dist 5). Amb 6 (id=6) at H3 (dist 10)
    ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "ALS");
    assert(ambIdx != -1 && test_ambulances[ambIdx].id == 2); // Ambulance 2 (id=2)
    printf("test_find_correct_type passed!\n");
}

// Test Case 2: No Ambulance of Required Type
void test_no_ambulance_of_type() {
    init_sample_test_weights();
    // Request "ICU" type which doesn't exist in test_ambulances
    int ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "ICU");
    assert(ambIdx == -1);
    printf("test_no_ambulance_of_type passed!\n");
}

// Test Case 3: Type Available, but Not Closest Overall
void test_typed_ambulance_not_closest_overall() {
    init_sample_test_weights();
    // Patient at H1 (src=1)
    // Amb1 (id=1, BLS, loc=1)
    // Amb2 (id=2, ALS, loc=2) dist H1-H2 = 5 (and H2-H1 = 5)
    // Amb5 (id=5, BLS, loc=2) dist H1-H2 = 5
    sample_test_weights[0][1] = 5; // H1 (idx 0) to H2 (idx 1)
    sample_test_weights[1][0] = 5; // H2 (idx 1) to H1 (idx 0)


    // Request ALS. Amb2 (id=2, loc=2) is ALS. Amb5 (id=5, loc=2) is BLS and also at loc=2.
    // Amb1 (id=1, loc=1) is BLS.
    // findNearestAmbulance should pick Amb2 (ALS) over Amb1 (BLS) even though Amb1 is closer if type wasn't a factor.
    int ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "ALS");
    assert(ambIdx != -1 && test_ambulances[ambIdx].id == 2); // Should be Amb2 (id=2, ALS)
    printf("test_typed_ambulance_not_closest_overall passed!\n");
}

// Test Case 4: Ambulance at Source
void test_ambulance_at_source() {
    init_sample_test_weights();
    int ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "BLS");
    assert(ambIdx != -1 && test_ambulances[ambIdx].id == 1);
    
    // Verify no ALS at H1 directly from test_ambulances
    int alsAtH1 = -1;
    for(int i=0; i < num_test_ambulances; ++i) {
        if(test_ambulances[i].location == 1 && strcmp(test_ambulances[i].type, "ALS") == 0 && strcmp(test_ambulances[i].status, "available") == 0) {
            alsAtH1 = i;
            break;
        }
    }
    assert(alsAtH1 == -1); // Double check test data assumption

    ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "ALS");
    assert(ambIdx == -1); // No ALS at H1 available
    printf("test_ambulance_at_source passed!\n");
}

// Test Case 5: Ambulance Type Mismatch (requesting a type not in test_ambulances)
void test_ambulance_type_mismatch() {
    init_sample_test_weights();
    int ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "NonExistentType");
    assert(ambIdx == -1);
    printf("test_ambulance_type_mismatch passed!\n");
}

// Test Case 6: Unavailable Typed Ambulance
void test_unavailable_typed_ambulance() {
    init_sample_test_weights();
    // test_ambulances[2] is Amb3 {3, 1, "busy", "BLS"}
    // Test if requesting BLS at src=1, ignores Amb3 (busy) and finds Amb1 (available)
    int ambIdx = findNearestAmbulance(test_ambulances, num_test_ambulances, 1, sample_test_weights, "BLS");
    assert(ambIdx != -1 && test_ambulances[ambIdx].id == 1); // Should find Amb1
    printf("test_unavailable_typed_ambulance passed!\n");
}

// Simple Floyd-Warshall for test
void floyd_warshall(int n, int w[n][n], int out[n][n]) {
    memcpy(out, w, sizeof(int)*n*n);
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (out[i][k] + out[k][j] < out[i][j])
                    out[i][j] = out[i][k] + out[k][j];
}

void test_shortest_path() {
    int result[HOSPITALS][HOSPITALS];
    floyd_warshall(HOSPITALS, test_weights, result);
    // Path from 0 to 2 should be 3 (0->1->2)
    assert(result[0][2] == 3);
    // Path from 2 to 0 should be 3 (2->1->0)
    assert(result[2][0] == 3);
    // Path from 1 to 2 should be 1 (direct)
    assert(result[1][2] == 1);
    printf("test_shortest_path passed!\n");
}

void test_hospital_selection() {
    // Simulate selection: from 0, nearest is 1 (weight 2)
    int min = INT_MAX, idx = -1;
    for (int j = 0; j < HOSPITALS; ++j) {
        if (j != 0 && test_weights[0][j] < min) {
            min = test_weights[0][j];
            idx = j;
        }
    }
    assert(idx == 1);
    assert(min == 2);
    printf("test_hospital_selection passed!\n");
}

int main() {
    // test_shortest_path(); // Can be kept if desired
    // test_hospital_selection(); // Can be kept if desired

    printf("Starting findNearestAmbulance tests...\n");
    test_find_correct_type();
    test_no_ambulance_of_type();
    test_typed_ambulance_not_closest_overall();
    test_ambulance_at_source();
    test_ambulance_type_mismatch();
    test_unavailable_typed_ambulance();
    
    printf("\nAll findNearestAmbulance tests were called.\n");
    return 0;
}
