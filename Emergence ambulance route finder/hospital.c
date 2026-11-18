#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hospital.h"
#include "dijkstra.h"

HospitalList* createHospitalList(int capacity) {
    if (capacity <= 0) capacity = 8;
    HospitalList *list = (HospitalList*)malloc(sizeof(HospitalList));
    if (!list) {
        fprintf(stderr, "[Error] Memory allocation failed for HospitalList.\n");
        return NULL;
    }
    list->count = 0;
    list->capacity = capacity;
    list->hospitals = (Hospital*)calloc((size_t)capacity, sizeof(Hospital));
    if (!list->hospitals) {
        fprintf(stderr, "[Error] Memory allocation failed for Hospital array.\n");
        free(list);
        return NULL;
    }
    return list;
}

int addHospital(HospitalList *list, Hospital h) {
    if (!list) return 0;
    if (list->count >= list->capacity) {
        int newCap = list->capacity * 2;
        Hospital *newHosp = (Hospital*)realloc(list->hospitals, (size_t)newCap * sizeof(Hospital));
        if (!newHosp) return 0;
        list->hospitals = newHosp;
        list->capacity = newCap;
    }
    list->hospitals[list->count++] = h;
    return 1;
}

const char* getHospitalStatusStr(HospitalStatus status) {
    return (status == HOSP_OPEN) ? "OPEN" : "CLOSED";
}

void displayHospitals(const HospitalList *list, const Graph *g) {
    if (!list || list->count == 0) {
        printf("No hospitals registered in the system.\n");
        return;
    }

    printf("+----+----------------------------------+-----------------------------+-----------+-----+------------+--------+----------+\n");
    printf("| ID | Hospital Name                    | Location Node               | Emergency | ICU | Beds (Av/T)| Status | Capacity |\n");
    printf("+----+----------------------------------+-----------------------------+-----------+-----+------------+--------+----------+\n");
    for (int i = 0; i < list->count; i++) {
        Hospital *h = &list->hospitals[i];
        const char *locName = (g && h->locationNode >= 0 && h->locationNode < g->numVertices)
                              ? g->vertices[h->locationNode].name : "Unknown";

        char bedStr[16];
        snprintf(bedStr, sizeof(bedStr), "%d/%d", h->availableBeds, h->totalBeds);

        printf("| %-2d | %-32s | %-27s | %-9s | %-3s | %-10s | %-6s | %-8s |\n",
               h->id,
               h->name,
               locName,
               h->emergencyFacility ? "YES (24/7)" : "NO",
               h->icuAvailability ? "YES" : "NO",
               bedStr,
               getHospitalStatusStr(h->status),
               h->capacityLevel);
    }
    printf("+----+----------------------------------+-----------------------------+-----------+-----+------------+--------+----------+\n");
    printf("Total Hospitals: %d (All availability & bed metrics are simulated demo values)\n", list->count);
}

Hospital* findHospitalById(HospitalList *list, int id) {
    if (!list) return NULL;
    for (int i = 0; i < list->count; i++) {
        if (list->hospitals[i].id == id) {
            return &list->hospitals[i];
        }
    }
    return NULL;
}

Hospital* findHospitalByNode(HospitalList *list, int nodeIndex) {
    if (!list) return NULL;
    for (int i = 0; i < list->count; i++) {
        if (list->hospitals[i].locationNode == nodeIndex) {
            return &list->hospitals[i];
        }
    }
    return NULL;
}

/*
 * Evaluates hospital suitability based on:
 * 1. Hospital distance via Dijkstra
 * 2. Status (must be OPEN)
 * 3. Bed availability (> 0)
 * 4. Emergency facility (required for Critical/High)
 * 5. ICU availability (preferred/required for Critical)
 */
int selectBestHospital(HospitalList *hList, const Graph *g, int patientNode, int severity, int preferredId, char *reasonBuffer, int reasonBufSize) {
    if (!hList || hList->count == 0 || !g) {
        if (reasonBuffer) snprintf(reasonBuffer, (size_t)reasonBufSize, "Hospital list or graph not initialized.");
        return -1;
    }

    /* Check if caller specified a preferred hospital */
    if (preferredId > 0) {
        Hospital *pref = findHospitalById(hList, preferredId);
        if (pref && pref->status == HOSP_OPEN && pref->availableBeds > 0) {
            pref->selectionCount++;
            if (pref->availableBeds > 0) pref->availableBeds--;
            if (reasonBuffer) {
                snprintf(reasonBuffer, (size_t)reasonBufSize,
                         "Assigned according to patient's requested hospital preference (%s).", pref->name);
            }
            return pref->id;
        }
    }

    int bestHospitalId = -1;
    float bestDistance = INF_DISTANCE;
    int bestScore = -999999;
    char tempReason[256] = "";

    for (int i = 0; i < hList->count; i++) {
        Hospital *h = &hList->hospitals[i];

        /* Hard constraints: Hospital must be OPEN and have beds */
        if (h->status != HOSP_OPEN || h->availableBeds <= 0) {
            continue;
        }

        /* Calculate Dijkstra shortest path from patient location to hospital */
        ShortestRoute route = dijkstraShortestPath(g, patientNode, h->locationNode);
        if (!route.found) {
            continue;
        }

        /* Compute suitability score */
        int score = 1000 - (int)(route.totalDistance * 50.0f);

        if (severity == 1) { /* CRITICAL */
            if (!h->emergencyFacility) continue; /* Mandatory for critical */
            if (h->icuAvailability) score += 500; else score -= 200;
        } else if (severity == 2) { /* HIGH */
            if (!h->emergencyFacility) score -= 400; else score += 300;
        }

        if (score > bestScore || (score == bestScore && route.totalDistance < bestDistance)) {
            bestScore = score;
            bestDistance = route.totalDistance;
            bestHospitalId = h->id;

            if (severity == 1) {
                snprintf(tempReason, sizeof(tempReason),
                         "Nearest suitable facility (%.2f km) with 24/7 Emergency Care & ICU capacity (%s).",
                         route.totalDistance, h->icuAvailability ? "ICU Available" : "General Emergency");
            } else if (severity == 2) {
                snprintf(tempReason, sizeof(tempReason),
                         "Nearest facility (%.2f km) with active Emergency Department.", route.totalDistance);
            } else {
                snprintf(tempReason, sizeof(tempReason),
                         "Nearest open hospital (%.2f km) with general bed capacity.", route.totalDistance);
            }
        }
    }

    if (bestHospitalId != -1) {
        Hospital *chosen = findHospitalById(hList, bestHospitalId);
        if (chosen) {
            chosen->selectionCount++;
            if (chosen->availableBeds > 0) chosen->availableBeds--;
        }
        if (reasonBuffer) {
            strncpy(reasonBuffer, tempReason, (size_t)reasonBufSize - 1);
            reasonBuffer[reasonBufSize - 1] = '\0';
        }
    } else {
        if (reasonBuffer) {
            snprintf(reasonBuffer, (size_t)reasonBufSize, "No open hospital with available beds and reachable route found.");
        }
    }

    return bestHospitalId;
}

void freeHospitalList(HospitalList *list) {
    if (!list) return;
    free(list->hospitals);
    free(list);
}
