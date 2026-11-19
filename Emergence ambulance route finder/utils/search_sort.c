#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "search_sort.h"

/* Case-insensitive string search helper */
static int containsCaseInsensitive(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    if (strlen(needle) == 0) return 1;

    char lowerHaystack[128];
    char lowerNeedle[128];

    size_t i = 0;
    for (; haystack[i] && i < sizeof(lowerHaystack) - 1; i++) {
        lowerHaystack[i] = (char)tolower((unsigned char)haystack[i]);
    }
    lowerHaystack[i] = '\0';

    size_t j = 0;
    for (; needle[j] && j < sizeof(lowerNeedle) - 1; j++) {
        lowerNeedle[j] = (char)tolower((unsigned char)needle[j]);
    }
    lowerNeedle[j] = '\0';

    return (strstr(lowerHaystack, lowerNeedle) != NULL);
}

/* Linear search for emergency ticket by ID */
int searchEmergencyById(const EmergencyList *list, int id) {
    if (!list) return -1;
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Search and display emergencies matching patient name substring */
void searchEmergencyByName(const EmergencyList *list, const char *nameSubstr, const Graph *g) {
    if (!list || !nameSubstr) return;

    int matchCount = 0;
    printf("\n--- Search Results for Patient Name matching '%s' ---\n", nameSubstr);
    for (int i = 0; i < list->count; i++) {
        if (containsCaseInsensitive(list->records[i].patientName, nameSubstr)) {
            matchCount++;
            displaySingleEmergencyDetail(&list->records[i], g);
        }
    }

    if (matchCount == 0) {
        printf("No emergency records matched the name query '%s'.\n", nameSubstr);
    } else {
        printf("Found %d matching record(s).\n", matchCount);
    }
}

/* Search and display emergencies by incident type */
void searchEmergencyByType(const EmergencyList *list, EmergencyType type, const Graph *g) {
    if (!list) return;

    int matchCount = 0;
    printf("\n--- Emergencies of Category: %s ---\n", getEmergencyTypeStr(type));
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].type == type) {
            matchCount++;
            displaySingleEmergencyDetail(&list->records[i], g);
        }
    }

    if (matchCount == 0) {
        printf("No records found under category: %s\n", getEmergencyTypeStr(type));
    } else {
        printf("Found %d matching incident(s).\n", matchCount);
    }
}

/* Linear search for ambulance by ID */
int searchAmbulanceById(const AmbulanceList *list, const char *id) {
    if (!list || !id) return -1;
    for (int i = 0; i < list->count; i++) {
        if (containsCaseInsensitive(list->ambulances[i].id, id)) {
            return i;
        }
    }
    return -1;
}

/* Search location in the graph by name query */
int searchLocationByName(const Graph *g, const char *query) {
    if (!g || !query) return -1;
    for (int i = 0; i < g->numVertices; i++) {
        if (containsCaseInsensitive(g->vertices[i].name, query) ||
            containsCaseInsensitive(g->vertices[i].locality, query)) {
            return i;
        }
    }
    return -1;
}

/* Swap helper for sorting algorithms */
static void swapEmergencies(EmergencyRequest *a, EmergencyRequest *b) {
    EmergencyRequest temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * ============================================================================
 * SORTING ALGORITHM 1: SELECTION SORT
 * Sorts emergency records by clinical priority (Severity 1 to 4).
 * Time Complexity: O(n^2), In-place sorting.
 * ============================================================================
 */
void sortEmergenciesByPriority(EmergencyRequest *arr, int n) {
    if (!arr || n <= 1) return;

    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            /* Priority 1 (CRITICAL) comes before Priority 4 (LOW) */
            if (arr[j].severity < arr[minIdx].severity) {
                minIdx = j;
            } else if (arr[j].severity == arr[minIdx].severity) {
                /* FIFO tie-breaker */
                if (arr[j].sequenceNumber < arr[minIdx].sequenceNumber) {
                    minIdx = j;
                }
            }
        }
        if (minIdx != i) {
            swapEmergencies(&arr[i], &arr[minIdx]);
        }
    }
}

/*
 * ============================================================================
 * SORTING ALGORITHM 2: BUBBLE SORT
 * Sorts emergency records alphabetically by Patient Name (A-Z).
 * Time Complexity: O(n^2), In-place sorting.
 * ============================================================================
 */
void sortEmergenciesByName(EmergencyRequest *arr, int n) {
    if (!arr || n <= 1) return;

    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - i - 1; j++) {
            if (strcmp(arr[j].patientName, arr[j + 1].patientName) > 0) {
                swapEmergencies(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break; /* Optimized early exit */
    }
}

/*
 * ============================================================================
 * SORTING ALGORITHM 3: INSERTION SORT
 * Sorts emergency records chronologically by Registration Sequence (ID/time).
 * Time Complexity: O(n^2), In-place and stable.
 * ============================================================================
 */
void sortEmergenciesByTime(EmergencyRequest *arr, int n) {
    if (!arr || n <= 1) return;

    for (int i = 1; i < n; i++) {
        EmergencyRequest key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].sequenceNumber > key.sequenceNumber) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/*
 * ============================================================================
 * SORTING ALGORITHM 4: SELECTION SORT (BY DISTANCE)
 * Sorts emergency records in ascending order of calculated route distance.
 * Time Complexity: O(n^2), In-place sorting.
 * ============================================================================
 */
void sortEmergenciesByDistance(EmergencyRequest *arr, int n) {
    if (!arr || n <= 1) return;

    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j].totalDistance < arr[minIdx].totalDistance) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            swapEmergencies(&arr[i], &arr[minIdx]);
        }
    }
}

/* Displays sorted emergency records in a clear table */
void displaySortedEmergencies(const EmergencyRequest *arr, int n, const Graph *g, const char *sortCriteria) {
    if (!arr || n == 0) {
        printf("No records available to display.\n");
        return;
    }

    printf("\n========================================================================================\n");
    printf("            SORTED EMERGENCY RECORDS (%s)             \n", sortCriteria);
    printf("========================================================================================\n");
    printf("+------+----------------------+----------------------+----------+------------------------+------------+--------------+\n");
    printf("| ID   | Patient Name         | Emergency Type       | Severity | Patient Location       | Total Dist | Status       |\n");
    printf("+------+----------------------+----------------------+----------+------------------------+------------+--------------+\n");
    for (int i = 0; i < n; i++) {
        const EmergencyRequest *req = &arr[i];
        const char *locName = (g && req->landmarkNode >= 0 && req->landmarkNode < g->numVertices)
                              ? g->vertices[req->landmarkNode].name : "Unknown";

        printf("| %-4d | %-20s | %-20s | %-8s | %-22s | %6.2f km | %-12s |\n",
               req->id,
               req->patientName,
               getEmergencyTypeStr(req->type),
               getSeverityStr(req->severity),
               locName,
               req->totalDistance,
               getEmergencyStatusStr(req->status));
    }
    printf("+------+----------------------+----------------------+----------+------------------------+------------+--------------+\n");
    printf("Total Sorted Records: %d\n", n);
}
