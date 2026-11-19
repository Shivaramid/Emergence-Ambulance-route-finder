#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ui.h"

void clearScreen(void) {
    /* Portable terminal clear */
    #ifdef _WIN32
        /* Windows console */
        system("cls");
    #else
        /* ANSI escape clear */
        printf("\033[H\033[J");
    #endif
}

void pauseKey(void) {
    printf("\nPress [Enter] to return to the menu...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void printDivider(void) {
    printf("========================================================================================\n");
}

void printHeader(const char *title) {
    printf("\n========================================================================================\n");
    int len = (int)strlen(title);
    int pad = (88 - len) / 2;
    if (pad < 0) pad = 0;
    for (int i = 0; i < pad; i++) putchar(' ');
    printf("%s\n", title);
    printf("========================================================================================\n");
}

void printSubHeader(const char *title) {
    printf("\n------------------------------------------------------------\n");
    printf(" %s\n", title);
    printf("------------------------------------------------------------\n");
}

void printSuccess(const char *msg) {
    printf("[/] SUCCESS: %s\n", msg);
}

void printWarning(const char *msg) {
    printf("[!] WARNING: %s\n", msg);
}

void printError(const char *msg) {
    printf("[X] ERROR: %s\n", msg);
}

void printInfo(const char *msg) {
    printf("[*] INFO: %s\n", msg);
}

/* ASCII representation of the simulated geographic road network */
void displayLocationMap(void) {
    printHeader("NARSINGI & SURROUNDING LOCALITIES - ROAD NETWORK TOPOLOGY MAP");
    printf("                                                                                        \n");
    printf("                              [19] GACHIBOWLI JUNCTION <---> [31] Gachibowli Hosp       \n");
    printf("                                         |                                              \n");
    printf("                              [18] NANAKRAMGUDA CIRCLE                                  \n");
    printf("                                 /       |         \\                                    \n");
    printf("        [10] MANIKONDA MAIN     /        |          \\      [14] KOKAPET JUNCTION        \n");
    printf("               |               /         |           \\        |            \\            \n");
    printf("        [27] DVR Hosp         /          |            \\       |       [15] Golden Mile  \n");
    printf("               |             /           |             \\      |             |           \n");
    printf("        [9] PUPPALAGUDA JCT /       [8] ALKAPUR         [7] REGALIA   [30] Kokapet Hosp \n");
    printf("               |           /             |                    |             |           \n");
    printf("        [26] Spring Field /         [28] Jasmin Hosp          |       [17] CBIT Gandipet\n");
    printf("               |         /               |                    |            /            \n");
    printf("        [11] NEKNAMPUR <-----------------+--------------------+--> [16] GANDIPET LAKE   \n");
    printf("               |                         |                                  |           \n");
    printf("        [20] MEHDIPATNAM                 |                                  |           \n");
    printf("               |                  [6] MN CIRCLE                             |           \n");
    printf("               |                  /      |      \\                           |           \n");
    printf("               |          [5] RATNADEEP  |   [0] DMART NARSINGI             |           \n");
    printf("               |                \\        |       /                          |           \n");
    printf("               |             [1] NARSINGI X ROADS                           |           \n");
    printf("               |             /   |          \\                               |           \n");
    printf("               |   [24] PULSE    |          [12] MANCHIREVULA               |           \n");
    printf("               |                 |                 |                        |           \n");
    printf("               |   [3] BUS STOP  | [2] POLICE      |                        |           \n");
    printf("               |        |        |      |          |                        |           \n");
    printf("               |   [25] Delight  |   [4] LAKE      |                        |           \n");
    printf("               |                 |      |          |                        |           \n");
    printf("               |                 +-> [29] Care     |                        |           \n");
    printf("               |                                   |                        |           \n");
    printf("               |                      [13] BRIDGE -+                        |           \n");
    printf("               |                             |                              |           \n");
    printf("               +-----> [21] RAJENDRANAGAR <--+                              |           \n");
    printf("                              |                                             |           \n");
    printf("                       [22] KISMATPUR <-------------------------------------+           \n");
    printf("                              |                                                         \n");
    printf("                       [23] SHAMSHABAD AIRPORT APPROACH                                 \n");
    printf("                                                                                        \n");
    printDivider();
    printf("  Legend:                                                                               \n");
    printf("    [0]-[23] : Public Landmarks, Junctions, and Residential Nodes                      \n");
    printf("    [24]-[31]: Dedicated Hospital & Emergency Care Nodes                                \n");
    printf("    Lines (-, |, /, \\): Bidirectional roads with simulated distance weights            \n");
    printDivider();
}

/* Formatted Main Menu */
void displayMainMenu(void) {
    printHeader("EMERGENCY AMBULANCE ROUTE FINDER - NARSINGI DISPATCH COMMAND");
    printf("  [1]  View Location Map (ASCII Road Topology)                                          \n");
    printf("  [2]  View All Locations & Landmarks (Vertex Directory)                                \n");
    printf("  [3]  View Road Network (Graph Adjacency List)                                         \n");
    printf("  [4]  View Hospitals (Capacity, ICU, & Availability Status)                            \n");
    printf("  [5]  View Ambulances (Fleet Status, Equipment, & Stations)                            \n");
    printf("  ------------------------------------------------------------------------------------\n");
    printf("  [6]  Report Emergency (Register Incident & Enqueue in Priority Queue)                 \n");
    printf("  [7]  View Emergency Queue (Priority Order: Critical > High > Medium > Low)           \n");
    printf("  [8]  Process Next Emergency (Full Dispatch Pipeline: Ambulance + Route + Hospital)    \n");
    printf("  ------------------------------------------------------------------------------------\n");
    printf("  [9]  Find Shortest Route (Manual Dijkstra Algorithm between any 2 Nodes)             \n");
    printf("  [10] Find Nearest Hospital (Dijkstra Hospital Suitability Evaluator)                 \n");
    printf("  [11] Assign Ambulance (Nearest Available Unit Dispatcher)                             \n");
    printf("  ------------------------------------------------------------------------------------\n");
    printf("  [12] View Active Emergencies (Dispatched / En Route Missions)                         \n");
    printf("  [13] Complete Emergency (Patient Delivered, Restock & Free Ambulance)                \n");
    printf("  [14] View Emergency History (Full Incident Audit Log)                                 \n");
    printf("  ------------------------------------------------------------------------------------\n");
    printf("  [15] Search Emergency Record (By ID, Patient Name, or Category)                      \n");
    printf("  [16] Sort Emergency Records (Bubble, Selection, or Insertion Sort)                    \n");
    printf("  [17] System Statistics & Operations Analytics                                         \n");
    printf("  [18] Exit System (Save & Clean Shutdown)                                             \n");
    printDivider();
}

/* Safe integer reader with range validation */
int getIntInput(const char *prompt, int min, int max) {
    char buffer[64];
    int value;

    while (1) {
        printf("%s (%d - %d): ", prompt, min, max);
        fflush(stdout);

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            clearerr(stdin);
            continue;
        }

        /* Check if line has any digits */
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        if (endptr == buffer) {
            printf("[!] Invalid input. Please enter a valid number.\n");
            continue;
        }

        if (value < min || value > max) {
            printf("[!] Value out of bounds. Please enter a number between %d and %d.\n", min, max);
            continue;
        }

        return value;
    }
}

/* Safe float reader */
float getFloatInput(const char *prompt, float min, float max) {
    char buffer[64];
    float value;

    while (1) {
        printf("%s (%.1f - %.1f): ", prompt, min, max);
        fflush(stdout);

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            clearerr(stdin);
            continue;
        }

        char *endptr;
        value = (float)strtod(buffer, &endptr);

        if (endptr == buffer) {
            printf("[!] Invalid numeric input.\n");
            continue;
        }

        if (value < min || value > max) {
            printf("[!] Value out of valid range (%.1f to %.1f).\n", min, max);
            continue;
        }

        return value;
    }
}

/* Safe string reader that trims newline */
void getStringInput(const char *prompt, char *buffer, int maxLen) {
    while (1) {
        printf("%s: ", prompt);
        fflush(stdout);

        if (!fgets(buffer, maxLen, stdin)) {
            clearerr(stdin);
            continue;
        }

        /* Remove trailing newline or carriage return */
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }

        /* Skip leading whitespace */
        char *start = buffer;
        while (*start && isspace((unsigned char)*start)) start++;

        if (strlen(start) == 0) {
            printf("[!] Field cannot be empty. Please enter a value.\n");
            continue;
        }

        if (start != buffer) {
            memmove(buffer, start, strlen(start) + 1);
        }

        return;
    }
}

/* Computes and prints system statistics */
void displaySystemStatistics(const EmergencyList *eList, const AmbulanceList *aList, const HospitalList *hList, const Graph *g) {
    printHeader("EMERGENCY SYSTEM OPERATIONAL STATISTICS & ANALYTICS");

    int totalEmerg = eList ? eList->count : 0;
    int criticalCount = 0, highCount = 0, medCount = 0, lowCount = 0;
    int completedCount = 0, activeCount = 0, queuedCount = 0;
    float sumDist = 0.0f;
    int distRecordCount = 0;

    /* Frequency array for locations */
    int *locFreq = NULL;
    if (g && g->numVertices > 0) {
        locFreq = (int*)calloc((size_t)g->numVertices, sizeof(int));
    }

    if (eList) {
        for (int i = 0; i < eList->count; i++) {
            EmergencyRequest *r = &eList->records[i];
            if (r->severity == SEV_CRITICAL) criticalCount++;
            else if (r->severity == SEV_HIGH) highCount++;
            else if (r->severity == SEV_MEDIUM) medCount++;
            else if (r->severity == SEV_LOW) lowCount++;

            if (r->status == EM_COMPLETED) completedCount++;
            else if (r->status == EM_AMBULANCE_ASSIGNED || r->status == EM_EN_ROUTE || r->status == EM_ARRIVED) activeCount++;
            else if (r->status == EM_QUEUED || r->status == EM_REGISTERED) queuedCount++;

            if (r->totalDistance > 0.0f) {
                sumDist += r->totalDistance;
                distRecordCount++;
            }

            if (locFreq && r->landmarkNode >= 0 && r->landmarkNode < g->numVertices) {
                locFreq[r->landmarkNode]++;
            }
        }
    }

    int availAmb = 0, busyAmb = 0, maintAmb = 0;
    if (aList) {
        for (int i = 0; i < aList->count; i++) {
            if (aList->ambulances[i].status == AMB_AVAILABLE) availAmb++;
            else if (aList->ambulances[i].status == AMB_BUSY) busyAmb++;
            else if (aList->ambulances[i].status == AMB_MAINTENANCE) maintAmb++;
        }
    }

    /* Find most frequent location */
    int maxLocIdx = -1, maxLocCount = 0;
    if (locFreq && g) {
        for (int i = 0; i < g->numVertices; i++) {
            if (locFreq[i] > maxLocCount) {
                maxLocCount = locFreq[i];
                maxLocIdx = i;
            }
        }
    }

    /* Find most frequently selected hospital */
    int maxHospIdx = -1, maxHospCount = 0;
    if (hList) {
        for (int i = 0; i < hList->count; i++) {
            if (hList->hospitals[i].selectionCount > maxHospCount) {
                maxHospCount = hList->hospitals[i].selectionCount;
                maxHospIdx = i;
            }
        }
    }

    float avgDist = (distRecordCount > 0) ? (sumDist / (float)distRecordCount) : 0.0f;

    printf("  Total Emergencies Logged   : %d\n", totalEmerg);
    printf("    - Critical (Priority 1)  : %d\n", criticalCount);
    printf("    - High     (Priority 2)  : %d\n", highCount);
    printf("    - Medium   (Priority 3)  : %d\n", medCount);
    printf("    - Low      (Priority 4)  : %d\n", lowCount);
    printf("\n");
    printf("  Incident Resolution:\n");
    printf("    - Completed Missions     : %d\n", completedCount);
    printf("    - Active Dispatches      : %d\n", activeCount);
    printf("    - Waiting in Queue       : %d\n", queuedCount);
    printf("\n");
    printf("  Fleet Readiness:\n");
    printf("    - Total Fleet Size       : %d\n", aList ? aList->count : 0);
    printf("    - Available Ambulances   : %d\n", availAmb);
    printf("    - Dispatched (Busy)      : %d\n", busyAmb);
    printf("    - Maintenance Status     : %d\n", maintAmb);
    printf("\n");
    printf("  Routing Analytics:\n");
    printf("    - Average Route Distance : %.2f km (Simulated)\n", avgDist);
    if (maxLocIdx != -1 && g) {
        printf("    - Top Incident Location  : %s (%d incidents)\n", g->vertices[maxLocIdx].name, maxLocCount);
    } else {
        printf("    - Top Incident Location  : N/A\n");
    }

    if (maxHospIdx != -1 && hList) {
        printf("    - Most Selected Hospital : %s (%d dispatches)\n",
               hList->hospitals[maxHospIdx].name, maxHospCount);
    } else {
        printf("    - Most Selected Hospital : N/A\n");
    }

    printDivider();

    if (locFreq) free(locFreq);
}
