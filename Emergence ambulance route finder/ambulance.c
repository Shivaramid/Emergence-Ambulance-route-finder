#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ambulance.h"
#include "dijkstra.h"

AmbulanceList* createAmbulanceList(int capacity) {
    if (capacity <= 0) capacity = 8;
    AmbulanceList *list = (AmbulanceList*)malloc(sizeof(AmbulanceList));
    if (!list) {
        fprintf(stderr, "[Error] Memory allocation failed for AmbulanceList.\n");
        return NULL;
    }
    list->count = 0;
    list->capacity = capacity;
    list->ambulances = (Ambulance*)calloc((size_t)capacity, sizeof(Ambulance));
    if (!list->ambulances) {
        fprintf(stderr, "[Error] Memory allocation failed for Ambulance array.\n");
        free(list);
        return NULL;
    }
    return list;
}

int addAmbulance(AmbulanceList *list, Ambulance a) {
    if (!list) return 0;
    if (list->count >= list->capacity) {
        int newCap = list->capacity * 2;
        Ambulance *newAmb = (Ambulance*)realloc(list->ambulances, (size_t)newCap * sizeof(Ambulance));
        if (!newAmb) return 0;
        list->ambulances = newAmb;
        list->capacity = newCap;
    }
    list->ambulances[list->count++] = a;
    return 1;
}

const char* getAmbulanceStatusStr(AmbulanceStatus status) {
    switch (status) {
        case AMB_AVAILABLE:   return "AVAILABLE";
        case AMB_BUSY:        return "BUSY";
        case AMB_MAINTENANCE: return "MAINTENANCE";
        default:              return "UNKNOWN";
    }
}

void displayAmbulances(const AmbulanceList *list, const Graph *g) {
    if (!list || list->count == 0) {
        printf("No ambulances registered in the system.\n");
        return;
    }

    printf("+--------+----------------+-------------------------------+-------------+------------------------------+------------+\n");
    printf("| ID     | Vehicle Reg No | Current Station / Location    | Status      | Emergency Equipment          | Total Dist |\n");
    printf("+--------+----------------+-------------------------------+-------------+------------------------------+------------+\n");
    for (int i = 0; i < list->count; i++) {
        Ambulance *a = &list->ambulances[i];
        const char *locName = (g && a->currentLocationNode >= 0 && a->currentLocationNode < g->numVertices)
                              ? g->vertices[a->currentLocationNode].name : "Unknown Location";
        printf("| %-6s | %-14s | %-29s | %-11s | %-28s | %6.1f km  |\n",
               a->id,
               a->vehicleNumber,
               locName,
               getAmbulanceStatusStr(a->status),
               a->equipment,
               a->totalKmRun);
    }
    printf("+--------+----------------+-------------------------------+-------------+------------------------------+------------+\n");
    printf("Total Ambulances: %d\n", list->count);
}

Ambulance* findAmbulanceById(AmbulanceList *list, const char *id) {
    if (!list || !id) return NULL;
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->ambulances[i].id, id) == 0) {
            return &list->ambulances[i];
        }
    }
    return NULL;
}

/*
 * Selects the nearest available ambulance to the patient location.
 * For Critical emergencies, prioritizes Advanced Life Support (ALS) units if available nearby.
 */
int selectNearestAmbulance(AmbulanceList *aList, const Graph *g, int patientNode, int severity, float *outDistance) {
    if (!aList || aList->count == 0 || !g) {
        return -1;
    }

    int bestIdx = -1;
    float minDistance = INF_DISTANCE;

    /* For critical cases, first attempt to find nearest ALS unit */
    if (severity == 1) {
        for (int i = 0; i < aList->count; i++) {
            Ambulance *a = &aList->ambulances[i];
            if (a->status == AMB_AVAILABLE && strstr(a->equipment, "ALS") != NULL) {
                ShortestRoute route = dijkstraShortestPath(g, a->currentLocationNode, patientNode);
                if (route.found && route.totalDistance < minDistance) {
                    minDistance = route.totalDistance;
                    bestIdx = i;
                }
            }
        }
    }

    /* If no ALS unit found or not critical, find any nearest available unit */
    if (bestIdx == -1) {
        minDistance = INF_DISTANCE;
        for (int i = 0; i < aList->count; i++) {
            Ambulance *a = &aList->ambulances[i];
            if (a->status == AMB_AVAILABLE) {
                ShortestRoute route = dijkstraShortestPath(g, a->currentLocationNode, patientNode);
                if (route.found && route.totalDistance < minDistance) {
                    minDistance = route.totalDistance;
                    bestIdx = i;
                }
            }
        }
    }

    if (bestIdx != -1 && outDistance) {
        *outDistance = minDistance;
    }

    return bestIdx;
}

void freeAmbulanceList(AmbulanceList *list) {
    if (!list) return;
    free(list->ambulances);
    free(list);
}
