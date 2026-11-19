#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "graph.h"
#include "dijkstra.h"
#include "hospital.h"
#include "ambulance.h"
#include "emergency.h"
#include "priority_queue.h"
#include "file_handler.h"
#include "search_sort.h"
#include "ui.h"
#include "controller.h"

/* Global tracking counters */
static int g_nextTicketId = 1001;
static int g_nextSequenceNumber = 1;

/* Helper to get current timestamp string */
static void getCurrentTimeStr(char *buffer, size_t bufSize) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if (timeinfo) {
        strftime(buffer, bufSize, "%I:%M %p", timeinfo);
    } else {
        snprintf(buffer, bufSize, "12:00 PM");
    }
}

/* Scans existing records to initialize next IDs */
static void syncTrackingCounters(const EmergencyList *list) {
    if (!list) return;
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].id >= g_nextTicketId) {
            g_nextTicketId = list->records[i].id + 1;
        }
        if (list->records[i].sequenceNumber >= g_nextSequenceNumber) {
            g_nextSequenceNumber = list->records[i].sequenceNumber + 1;
        }
    }
}

/* ============================================================================
 * DISPATCH WORKFLOW HANDLERS
 * ============================================================================ */

/* Handles emergency reporting */
static void handleReportEmergency(EmergencyList *eList, PriorityQueue *pq, const Graph *g, const HospitalList *hList) {
    printHeader("REPORT NEW EMERGENCY INCIDENT");

    EmergencyRequest req;
    memset(&req, 0, sizeof(req));

    req.id = g_nextTicketId++;
    req.sequenceNumber = g_nextSequenceNumber++;
    getCurrentTimeStr(req.registeredTime, sizeof(req.registeredTime));

    printf("Incident Ticket Number : #%d\n", req.id);
    printf("Registration Sequence  : #%d\n", req.sequenceNumber);
    printf("Time Logged            : %s\n\n", req.registeredTime);

    getStringInput("Enter Patient / Caller Name", req.patientName, sizeof(req.patientName));
    getStringInput("Enter Patient Contact Phone", req.contactNumber, sizeof(req.contactNumber));

    printf("\nSelect Incident Category:\n");
    printf("  1. Accident / Severe Trauma\n");
    printf("  2. Chest Pain / Cardiac Emergency\n");
    printf("  3. Severe Breathing Distress\n");
    printf("  4. Physical Trauma / Fall Injury\n");
    printf("  5. Unconscious / Non-Responsive Person\n");
    printf("  6. Maternity / Pregnancy Emergency\n");
    printf("  7. Other Acute Medical Condition\n");
    int typeChoice = getIntInput("Select Incident Type", 1, 7);
    req.type = (EmergencyType)typeChoice;

    printf("\nSelect Triage Clinical Severity Level:\n");
    printf("  1. CRITICAL (Life-threatening, Immediate Priority)\n");
    printf("  2. HIGH     (Severe condition, Urgent Dispatch)\n");
    printf("  3. MEDIUM   (Stable but urgent transport needed)\n");
    printf("  4. LOW      (Non-critical transport)\n");
    int sevChoice = getIntInput("Select Severity Level", 1, 4);
    req.severity = (EmergencySeverity)sevChoice;

    printf("\nSelect Patient Landmark / Current Location:\n");
    /* Show every graph node so the selected input always maps to a visible landmark/hospital node. */
    for (int i = 0; i < g->numVertices; i++) {
        printf("  %2d. %-32s [%s]\n", i, g->vertices[i].name, g->vertices[i].locality);
    }
    req.landmarkNode = getIntInput("Select Patient Location Node", 0, g->numVertices - 1);

    printf("\nPreferred Destination Hospital:\n");
    printf("  0. Automatic Best-Match Selection (Recommended)\n");
    if (hList) {
        for (int i = 0; i < hList->count; i++) {
            printf("  %d. %s\n", hList->hospitals[i].id, hList->hospitals[i].name);
        }
    }
    int hospChoice = getIntInput("Select Preferred Hospital (0 for Auto)", 0, hList ? hList->count : 0);
    req.preferredHospitalId = (hospChoice == 0) ? -1 : hospChoice;

    req.status = EM_QUEUED;
    strncpy(req.dispatchNotes, "Queued for priority ambulance dispatch", sizeof(req.dispatchNotes) - 1);

    /* Enqueue into manual Priority Queue */
    enqueueEmergency(pq, req);
    /* Append to system Emergency list */
    addEmergencyRecord(eList, req);

    /* Persist to storage */
    saveEmergenciesToFile(eList, EMERGENCIES_FILE);

    printf("\n");
    printSuccess("Emergency ticket successfully registered and queued!");
    printf("  Ticket #%d placed in Priority Queue.\n", req.id);
    printf("  Priority Ranking: %s (Severity %d) | FIFO Order: #%d\n",
           getSeverityStr(req.severity), (int)req.severity, req.sequenceNumber);
}

/* Processes the highest priority emergency in the queue */
static void handleProcessNextEmergency(EmergencyList *eList, PriorityQueue *pq, Graph *g, HospitalList *hList, AmbulanceList *aList) {
    printHeader("PROCESS NEXT EMERGENCY - DISPATCH PIPELINE");

    if (isPQEmpty(pq)) {
        printWarning("The Emergency Priority Queue is currently empty. No pending calls to process.");
        return;
    }

    EmergencyRequest pendingReq;
    if (!dequeueEmergency(pq, &pendingReq)) {
        printError("Failed to dequeue from priority queue.");
        return;
    }

    /* Locate in primary emergency list */
    EmergencyRequest *req = findEmergencyById(eList, pendingReq.id);
    if (!req) {
        printError("Emergency record not found in system storage.");
        return;
    }

    req->status = EM_PROCESSING;

    printf("\n[Step 1/4] Processing Highest Priority Call:\n");
    printf("  Incident Ticket     : #%d\n", req->id);
    printf("  Patient Name        : %s\n", req->patientName);
    printf("  Incident Severity   : %s (Priority %d)\n", getSeverityStr(req->severity), (int)req->severity);
    printf("  Patient Landmark    : %s [%s]\n", g->vertices[req->landmarkNode].name, g->vertices[req->landmarkNode].locality);
    printf("  Registration FIFO   : #%d\n", req->sequenceNumber);

    /* Step 2: Locate nearest suitable available ambulance */
    printf("\n[Step 2/4] Locating Closest Available Ambulance Fleet Unit...\n");
    float ambDist = 0.0f;
    int ambIdx = selectNearestAmbulance(aList, g, req->landmarkNode, (int)req->severity, &ambDist);

    if (ambIdx == -1) {
        printError("No ambulances are currently AVAILABLE! All units are Busy or under Maintenance.");
        printWarning("Re-queuing emergency call at highest priority for immediate dispatch upon fleet return.");
        req->status = EM_QUEUED;
        enqueueEmergency(pq, *req);
        return;
    }

    Ambulance *amb = &aList->ambulances[ambIdx];
    printf("  Found Nearest Unit  : %s (%s)\n", amb->id, amb->vehicleNumber);
    printf("  Equipment Standard  : %s\n", amb->equipment);
    printf("  Current Station     : %s\n", g->vertices[amb->currentLocationNode].name);
    printf("  Calculated Distance : %.2f km\n", ambDist);

    /* Step 3: Compute route: Ambulance -> Patient */
    ShortestRoute routeAmbToPatient = dijkstraShortestPath(g, amb->currentLocationNode, req->landmarkNode);

    /* Step 4: Determine suitable hospital */
    printf("\n[Step 3/4] Evaluating Hospital Suitability via Dijkstra Algorithm...\n");
    char reason[256] = "";
    int hospId = selectBestHospital(hList, g, req->landmarkNode, (int)req->severity, req->preferredHospitalId, reason, sizeof(reason));

    if (hospId == -1) {
        printError("No reachable hospital with available emergency beds found!");
        req->status = EM_QUEUED;
        enqueueEmergency(pq, *req);
        return;
    }

    Hospital *hosp = findHospitalById(hList, hospId);
    printf("  Selected Facility   : %s\n", hosp ? hosp->name : "Unknown");
    printf("  Decision Basis      : %s\n", reason);

    /* Compute route: Patient -> Hospital */
    ShortestRoute routePatientToHosp = dijkstraShortestPath(g, req->landmarkNode, hosp->locationNode);

    /* Step 5: Assign ambulance & update mission metrics */
    amb->status = AMB_BUSY;
    amb->assignedEmergencyId = req->id;

    strncpy(req->assignedAmbulanceId, amb->id, sizeof(req->assignedAmbulanceId) - 1);
    req->assignedHospitalId = hosp->id;
    req->ambulanceToPatientDist = routeAmbToPatient.totalDistance;
    req->patientToHospitalDist = routePatientToHosp.totalDistance;
    req->totalDistance = routeAmbToPatient.totalDistance + routePatientToHosp.totalDistance;
    req->status = EM_AMBULANCE_ASSIGNED;
    snprintf(req->dispatchNotes, sizeof(req->dispatchNotes), "Dispatched %s -> Deliver to %s", amb->id, hosp->name);

    /* Save updated records */
    saveAmbulancesToFile(aList, AMBULANCES_FILE);
    saveHospitalsToFile(hList, HOSPITALS_FILE);
    saveEmergenciesToFile(eList, EMERGENCIES_FILE);

    /* Step 6: Display comprehensive dispatch summary */
    printf("\n[Step 4/4] DISPATCH SUMMARY & NAVIGATION ROUTES\n");
    printDivider();
    printf("  AMBULANCE DISPATCH ROUTE (Unit %s -> Patient):\n", amb->id);
    printRoute(g, &routeAmbToPatient);

    printf("\n  PATIENT TRANSPORT ROUTE (Patient -> Hospital %s):\n", hosp->name);
    printRoute(g, &routePatientToHosp);

    printf("\n============================================================\n");
    printf("                  OVERALL MISSION METRICS                   \n");
    printf("============================================================\n");
    printf("  Assigned Ambulance    : %s (%s)\n", amb->id, amb->equipment);
    printf("  Pickup Location       : %s\n", g->vertices[req->landmarkNode].name);
    printf("  Destination Hospital  : %s\n", hosp->name);
    printf("  Ambulance -> Patient  : %.2f km (ETA ~%.1f mins)\n",
           routeAmbToPatient.totalDistance, routeAmbToPatient.estimatedTimeMinutes);
    printf("  Patient -> Hospital   : %.2f km (ETA ~%.1f mins)\n",
           routePatientToHosp.totalDistance, routePatientToHosp.estimatedTimeMinutes);
    printf("  Total Mission Road    : %.2f km\n", req->totalDistance);
    printf("  Combined Transit Time : ~%.1f minutes\n",
           routeAmbToPatient.estimatedTimeMinutes + routePatientToHosp.estimatedTimeMinutes);
    printf("  Status Indicator      : [✓] AMBULANCE ASSIGNED & EN ROUTE\n");
    printDivider();
}

/* Manually compute Dijkstra route between any 2 arbitrary nodes */
static void handleFindShortestRoute(const Graph *g) {
    printHeader("MANUAL DIJKSTRA SHORTEST PATH FINDER");
    printf("Demonstrating Dijkstra's Shortest Path Algorithm on Road Network Graph.\n\n");

    displayAllLocations(g);

    int src = getIntInput("\nEnter Source Location ID", 0, g->numVertices - 1);
    int dest = getIntInput("Enter Destination Location ID", 0, g->numVertices - 1);

    ShortestRoute route = dijkstraShortestPath(g, src, dest);
    printRoute(g, &route);
}

/* Finds nearest hospital for any arbitrary landmark */
static void handleFindNearestHospital(HospitalList *hList, const Graph *g) {
    printHeader("FIND NEAREST SUITABLE HOSPITAL");

    displayAllLocations(g);

    int loc = getIntInput("\nSelect Starting Location ID", 0, g->numVertices - 1);

    printf("\nSelect Triage Condition / Severity:\n");
    printf("  1. CRITICAL (Requires 24/7 Emergency & ICU)\n");
    printf("  2. HIGH     (Requires Emergency Department)\n");
    printf("  3. MEDIUM   (General Inpatient / Beds)\n");
    printf("  4. LOW      (General Outpatient)\n");
    int sev = getIntInput("Select Severity Level", 1, 4);

    char reason[256] = "";
    int hospId = selectBestHospital(hList, g, loc, sev, -1, reason, sizeof(reason));

    if (hospId == -1) {
        printError("No open hospital with available beds found.");
        return;
    }

    Hospital *hosp = findHospitalById(hList, hospId);
    if (!hosp) return;

    printf("\n============================================================\n");
    printf("               RECOMMENDED HOSPITAL FACILITY                \n");
    printf("============================================================\n");
    printf("Recommended Hospital : %s [ID: %d]\n", hosp->name, hosp->id);
    printf("Hospital Category    : %s\n", hosp->emergencyFacility ? "24/7 Emergency Care" : "General Healthcare");
    printf("ICU Beds Available   : %s\n", hosp->icuAvailability ? "YES" : "NO");
    printf("Available Beds       : %d / %d\n", hosp->availableBeds, hosp->totalBeds);
    printf("Suitability Reason   : %s\n", reason);
    printf("------------------------------------------------------------\n");

    ShortestRoute route = dijkstraShortestPath(g, loc, hosp->locationNode);
    printRoute(g, &route);
}

/* Manually dispatches or assigns an ambulance */
static void handleAssignAmbulance(AmbulanceList *aList, const Graph *g) {
    printHeader("MANUAL AMBULANCE DISPATCH ASSIGNMENT");
    displayAmbulances(aList, g);

    displayAllLocations(g);
    int targetLoc = getIntInput("\nEnter Target Incident Location ID", 0, g->numVertices - 1);

    float dist = 0.0f;
    int bestIdx = selectNearestAmbulance(aList, g, targetLoc, 2, &dist);

    if (bestIdx == -1) {
        printError("No ambulances are currently available in the fleet.");
        return;
    }

    Ambulance *amb = &aList->ambulances[bestIdx];
    printf("\nAuto-Identified Nearest Ambulance:\n");
    printf("  Vehicle Code   : %s (%s)\n", amb->id, amb->vehicleNumber);
    printf("  Current Station: %s\n", g->vertices[amb->currentLocationNode].name);
    printf("  Route Distance : %.2f km\n\n", dist);

    ShortestRoute route = dijkstraShortestPath(g, amb->currentLocationNode, targetLoc);
    printRoute(g, &route);

    printf("\nConfirm Dispatch of %s? (1 = Confirm Dispatch, 0 = Cancel): ", amb->id);
    int confirm = getIntInput("Choice", 0, 1);
    if (confirm == 1) {
        amb->status = AMB_BUSY;
        saveAmbulancesToFile(aList, AMBULANCES_FILE);
        printSuccess("Ambulance dispatched! Fleet status updated to BUSY.");
    } else {
        printInfo("Dispatch operation cancelled.");
    }
}

/* Displays all active (in-progress) emergencies */
static void handleViewActiveEmergencies(const EmergencyList *list, const Graph *g) {
    printHeader("CURRENTLY ACTIVE EMERGENCY MISSIONS");

    int activeCount = 0;
    printf("+------+----------------------+----------------------+----------+------------------------+--------+--------+--------------+\n");
    printf("| ID   | Patient Name         | Emergency Type       | Severity | Patient Location       | Amb ID | HospID | Status       |\n");
    printf("+------+----------------------+----------------------+----------+------------------------+--------+--------+--------------+\n");

    for (int i = 0; i < list->count; i++) {
        EmergencyRequest *r = &list->records[i];
        if (r->status == EM_AMBULANCE_ASSIGNED || r->status == EM_EN_ROUTE || r->status == EM_ARRIVED) {
            activeCount++;
            const char *locName = (g && r->landmarkNode >= 0 && r->landmarkNode < g->numVertices)
                                  ? g->vertices[r->landmarkNode].name : "Unknown";

            printf("| %-4d | %-20s | %-20s | %-8s | %-22s | %-6s | HOSP-%-2d | %-12s |\n",
                   r->id, r->patientName, getEmergencyTypeStr(r->type),
                   getSeverityStr(r->severity), locName,
                   r->assignedAmbulanceId, r->assignedHospitalId,
                   getEmergencyStatusStr(r->status));
        }
    }
    printf("+------+----------------------+----------------------+----------+------------------------+--------+--------+--------------+\n");
    if (activeCount == 0) {
        printf("No active emergency dispatches currently in progress.\n");
    } else {
        printf("Total Active Missions: %d\n", activeCount);
    }
}

/* Completes an active emergency mission and returns ambulance to service */
static void handleCompleteEmergency(EmergencyList *eList, AmbulanceList *aList, HospitalList *hList, const Graph *g) {
    printHeader("COMPLETE EMERGENCY MISSION & RESTOCK AMBULANCE");
    handleViewActiveEmergencies(eList, g);

    int ticketId = getIntInput("\nEnter Ticket ID of emergency to mark COMPLETED (0 to Cancel)", 0, 99999);
    if (ticketId == 0) return;

    EmergencyRequest *req = findEmergencyById(eList, ticketId);
    if (!req) {
        printError("Emergency ticket not found.");
        return;
    }

    if (req->status == EM_COMPLETED) {
        printWarning("This emergency is already marked as COMPLETED.");
        return;
    }

    req->status = EM_COMPLETED;
    snprintf(req->dispatchNotes, sizeof(req->dispatchNotes), "Patient safely admitted; mission completed");

    /* Free assigned ambulance if any */
    if (strlen(req->assignedAmbulanceId) > 0) {
        Ambulance *amb = findAmbulanceById(aList, req->assignedAmbulanceId);
        if (amb) {
            amb->status = AMB_AVAILABLE;
            amb->assignedEmergencyId = -1;
            amb->totalKmRun += req->totalDistance;

            /* Relocate ambulance to the hospital where patient was delivered */
            Hospital *h = findHospitalById(hList, req->assignedHospitalId);
            if (h) {
                amb->currentLocationNode = h->locationNode;
            }

            printf("\nAmbulance %s has been restocked and returned to AVAILABLE status at %s.\n",
                   amb->id, (h && g) ? g->vertices[h->locationNode].name : "Base Station");
        }
    }

    saveEmergenciesToFile(eList, EMERGENCIES_FILE);
    saveAmbulancesToFile(aList, AMBULANCES_FILE);
    printSuccess("Emergency ticket marked as COMPLETED. Records updated.");
}

/* Searches emergency records */
static void handleSearch(const EmergencyList *eList, const AmbulanceList *aList, const Graph *g) {
    printHeader("SEARCH SYSTEM RECORDS");
    printf("  1. Search Emergency Ticket by ID\n");
    printf("  2. Search Emergency by Patient Name (Substring)\n");
    printf("  3. Search Emergency by Medical Incident Category\n");
    printf("  4. Search Ambulance by Vehicle ID\n");
    printf("  5. Search Location / Landmark in Road Network\n");
    int choice = getIntInput("Select Search Option", 1, 5);

    if (choice == 1) {
        int id = getIntInput("Enter Ticket ID", 1, 99999);
        int idx = searchEmergencyById(eList, id);
        if (idx != -1) {
            displaySingleEmergencyDetail(&eList->records[idx], g);
        } else {
            printError("No emergency record found with specified ID.");
        }
    } else if (choice == 2) {
        char query[64];
        getStringInput("Enter Patient Name (or partial name)", query, sizeof(query));
        searchEmergencyByName(eList, query, g);
    } else if (choice == 3) {
        printf("\nIncident Categories:\n");
        printf("  1. Accident / Severe Trauma\n");
        printf("  2. Chest Pain / Cardiac Emergency\n");
        printf("  3. Severe Breathing Distress\n");
        printf("  4. Physical Trauma / Fall Injury\n");
        printf("  5. Unconscious / Non-Responsive Person\n");
        printf("  6. Maternity / Pregnancy Emergency\n");
        printf("  7. Other Acute Medical Condition\n");
        int typeChoice = getIntInput("Select Category", 1, 7);
        searchEmergencyByType(eList, (EmergencyType)typeChoice, g);
    } else if (choice == 4) {
        char ambId[16];
        getStringInput("Enter Ambulance Code (e.g. AMB001)", ambId, sizeof(ambId));
        int idx = searchAmbulanceById(aList, ambId);
        if (idx != -1) {
            Ambulance *a = &aList->ambulances[idx];
            printf("\nAmbulance Found:\n");
            printf("  ID           : %s\n", a->id);
            printf("  Vehicle No   : %s\n", a->vehicleNumber);
            printf("  Station Node : %s\n", g->vertices[a->currentLocationNode].name);
            printf("  Status       : %s\n", getAmbulanceStatusStr(a->status));
            printf("  Equipment    : %s\n", a->equipment);
            printf("  Total Run    : %.1f km\n", a->totalKmRun);
        } else {
            printError("Ambulance ID not found in fleet records.");
        }
    } else if (choice == 5) {
        char query[64];
        getStringInput("Enter Landmark / Area Name Query", query, sizeof(query));
        int nodeIdx = searchLocationByName(g, query);
        if (nodeIdx != -1) {
            printf("\nLocation Found in Network:\n");
            printf("  ID       : %d\n", g->vertices[nodeIdx].id);
            printf("  Landmark : %s\n", g->vertices[nodeIdx].name);
            printf("  Locality : %s\n", g->vertices[nodeIdx].locality);
            printf("  Category : %s\n", getNodeTypeName(g->vertices[nodeIdx].type));
        } else {
            printError("No landmark matching the query was found in the graph.");
        }
    }
}

/* Sorts emergency records using course DS sorting algorithms */
static void handleSort(const EmergencyList *eList, const Graph *g) {
    printHeader("SORT EMERGENCY INCIDENT RECORDS");

    if (!eList || eList->count == 0) {
        printWarning("No records available to sort.");
        return;
    }

    printf("Choose Sorting Dimension & DSA Algorithm:\n");
    printf("  1. Sort by Clinical Severity (Highest Priority First) - [SELECTION SORT]\n");
    printf("  2. Sort Alphabetically by Patient Name (A to Z)      - [BUBBLE SORT]\n");
    printf("  3. Sort Chronologically by Sequence / Log Time       - [INSERTION SORT]\n");
    printf("  4. Sort by Route Distance (Shortest to Longest)      - [SELECTION SORT]\n");
    int choice = getIntInput("Select Sorting Method", 1, 4);

    /* Allocate dynamic temporary array to avoid mutating original file storage */
    EmergencyRequest *copyArr = (EmergencyRequest*)malloc((size_t)eList->count * sizeof(EmergencyRequest));
    if (!copyArr) {
        printError("Memory allocation failed for sorting buffer.");
        return;
    }
    memcpy(copyArr, eList->records, (size_t)eList->count * sizeof(EmergencyRequest));

    if (choice == 1) {
        sortEmergenciesByPriority(copyArr, eList->count);
        displaySortedEmergencies(copyArr, eList->count, g, "Selection Sort by Clinical Severity");
    } else if (choice == 2) {
        sortEmergenciesByName(copyArr, eList->count);
        displaySortedEmergencies(copyArr, eList->count, g, "Bubble Sort by Patient Name");
    } else if (choice == 3) {
        sortEmergenciesByTime(copyArr, eList->count);
        displaySortedEmergencies(copyArr, eList->count, g, "Insertion Sort by Registration Sequence");
    } else if (choice == 4) {
        sortEmergenciesByDistance(copyArr, eList->count);
        displaySortedEmergencies(copyArr, eList->count, g, "Selection Sort by Total Route Distance");
    }

    free(copyArr);
}

/* ============================================================================
 * MAIN ENTRY POINT
 * ============================================================================ */
void runApplication(void) {
    /* Initialize Data Structures dynamically */
    Graph *graph = createGraph(32);
    HospitalList *hospitals = createHospitalList(10);
    AmbulanceList *ambulances = createAmbulanceList(10);
    EmergencyList *emergencies = createEmergencyList(32);
    PriorityQueue *priorityQueue = createPriorityQueue(32);

    if (!graph || !hospitals || !ambulances || !emergencies || !priorityQueue) {
        fprintf(stderr, "[Fatal Error] Failed to initialize core data structures. Exiting.\n");
        return 1;
    }

    /* Academic disclaimer banner */
    printf("\n");
    printDivider();
    printf("                     EMERGENCY AMBULANCE ROUTE FINDER                     \n");
    printf("            Simulation System for Narsingi & Surrounding Localities       \n");
    printf("                 Hyderabad, Telangana - Data Structures Project           \n");
    printDivider();
    printf(" ACADEMIC DISCLAIMER:\n");
    printf(" This project is an academic simulation. Location distances, hospital\n");
    printf(" availability, ambulance availability, and travel times are simulated values\n");
    printf(" and are not real-time emergency-service information.\n");
    printDivider();

    /* Automatically ensure data directory and seed datasets if empty */
    ensureDataDirectoryExists();
    initializeDefaultDatasetIfEmpty(graph, hospitals, ambulances, emergencies, priorityQueue);
    syncTrackingCounters(emergencies);

    printf("\n[System Initialization Complete] Road Network Vertices: %d | Fleet: %d | Hospitals: %d\n",
           graph->numVertices, ambulances->count, hospitals->count);

    int running = 1;
    while (running) {
        displayMainMenu();
        int choice = getIntInput("Enter menu selection", 1, 18);

        switch (choice) {
            case 1:  /* View Location Map */
                displayLocationMap();
                pauseKey();
                break;

            case 2:  /* View All Locations */
                printHeader("REGISTERED LANDMARKS & JUNCTIONS DIRECTORY");
                displayAllLocations(graph);
                pauseKey();
                break;

            case 3:  /* View Road Network */
                displayGraph(graph);
                pauseKey();
                break;

            case 4:  /* View Hospitals */
                printHeader("HOSPITAL CAPACITIES & READINESS DIRECTORY");
                displayHospitals(hospitals, graph);
                pauseKey();
                break;

            case 5:  /* View Ambulances */
                printHeader("AMBULANCE FLEET STATUS & POSITIONING DIRECTORY");
                displayAmbulances(ambulances, graph);
                pauseKey();
                break;

            case 6:  /* Report Emergency */
                handleReportEmergency(emergencies, priorityQueue, graph, hospitals);
                pauseKey();
                break;

            case 7:  /* View Emergency Queue */
                displayPriorityQueue(priorityQueue, graph);
                pauseKey();
                break;

            case 8:  /* Process Next Emergency */
                handleProcessNextEmergency(emergencies, priorityQueue, graph, hospitals, ambulances);
                pauseKey();
                break;

            case 9:  /* Find Shortest Route */
                handleFindShortestRoute(graph);
                pauseKey();
                break;

            case 10: /* Find Nearest Hospital */
                handleFindNearestHospital(hospitals, graph);
                pauseKey();
                break;

            case 11: /* Assign Ambulance */
                handleAssignAmbulance(ambulances, graph);
                pauseKey();
                break;

            case 12: /* View Active Emergencies */
                handleViewActiveEmergencies(emergencies, graph);
                pauseKey();
                break;

            case 13: /* Complete Emergency */
                handleCompleteEmergency(emergencies, ambulances, hospitals, graph);
                pauseKey();
                break;

            case 14: /* View Emergency History */
                printHeader("COMPLETE EMERGENCY INCIDENT HISTORY");
                displayEmergencyTable(emergencies, graph);
                pauseKey();
                break;

            case 15: /* Search Emergency Record */
                handleSearch(emergencies, ambulances, graph);
                pauseKey();
                break;

            case 16: /* Sort Emergency Records */
                handleSort(emergencies, graph);
                pauseKey();
                break;

            case 17: /* System Statistics */
                displaySystemStatistics(emergencies, ambulances, hospitals, graph);
                pauseKey();
                break;

            case 18: /* Exit */
                printHeader("SAVING DATA & SHUTTING DOWN SYSTEM");
                saveLocationsToFile(graph, LOCATIONS_FILE);
                saveRoadsToFile(graph, ROADS_FILE);
                saveHospitalsToFile(hospitals, HOSPITALS_FILE);
                saveAmbulancesToFile(ambulances, AMBULANCES_FILE);
                saveEmergenciesToFile(emergencies, EMERGENCIES_FILE);
                printSuccess("All current locations, roads, hospitals, fleet and emergency records saved.");
                printf("Thank you for using Emergency Ambulance Route Finder.\n\n");
                running = 0;
                break;

            default:
                printError("Invalid selection. Please choose an option between 1 and 18.");
                pauseKey();
                break;
        }
    }

    /* Free all dynamically allocated memory */
    freePriorityQueue(priorityQueue);
    freeEmergencyList(emergencies);
    freeAmbulanceList(ambulances);
    freeHospitalList(hospitals);
    freeGraph(graph);

}
