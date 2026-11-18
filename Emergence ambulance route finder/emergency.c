#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emergency.h"

EmergencyList* createEmergencyList(int capacity) {
    if (capacity <= 0) capacity = 16;
    EmergencyList *list = (EmergencyList*)malloc(sizeof(EmergencyList));
    if (!list) {
        fprintf(stderr, "[Error] Memory allocation failed for EmergencyList.\n");
        return NULL;
    }
    list->count = 0;
    list->capacity = capacity;
    list->records = (EmergencyRequest*)calloc((size_t)capacity, sizeof(EmergencyRequest));
    if (!list->records) {
        fprintf(stderr, "[Error] Memory allocation failed for Emergency records.\n");
        free(list);
        return NULL;
    }
    return list;
}

int addEmergencyRecord(EmergencyList *list, EmergencyRequest req) {
    if (!list) return 0;
    if (list->count >= list->capacity) {
        int newCap = list->capacity * 2;
        EmergencyRequest *newRecs = (EmergencyRequest*)realloc(list->records, (size_t)newCap * sizeof(EmergencyRequest));
        if (!newRecs) return 0;
        list->records = newRecs;
        list->capacity = newCap;
    }
    list->records[list->count++] = req;
    return 1;
}

EmergencyRequest* findEmergencyById(EmergencyList *list, int id) {
    if (!list) return NULL;
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].id == id) {
            return &list->records[i];
        }
    }
    return NULL;
}

const char* getSeverityStr(EmergencySeverity sev) {
    switch (sev) {
        case SEV_CRITICAL: return "CRITICAL";
        case SEV_HIGH:     return "HIGH";
        case SEV_MEDIUM:   return "MEDIUM";
        case SEV_LOW:      return "LOW";
        default:           return "UNKNOWN";
    }
}

const char* getEmergencyTypeStr(EmergencyType type) {
    switch (type) {
        case ET_ACCIDENT:   return "Accident / Trauma";
        case ET_CHEST_PAIN: return "Chest Pain / Cardiac";
        case ET_BREATHING:  return "Breathing Distress";
        case ET_INJURY:     return "Physical Injury";
        case ET_UNCONSCIOUS:return "Unconscious Person";
        case ET_PREGNANCY:  return "Maternity / Pregnancy";
        case ET_OTHER:      return "Other Medical Urgent";
        default:            return "General Emergency";
    }
}

const char* getEmergencyStatusStr(EmergencyStatus status) {
    switch (status) {
        case EM_REGISTERED:         return "REGISTERED";
        case EM_QUEUED:             return "QUEUED";
        case EM_PROCESSING:         return "PROCESSING";
        case EM_AMBULANCE_ASSIGNED: return "AMB_ASSIGNED";
        case EM_EN_ROUTE:           return "EN_ROUTE";
        case EM_ARRIVED:            return "ARRIVED";
        case EM_COMPLETED:          return "COMPLETED";
        case EM_CANCELLED:          return "CANCELLED";
        default:                    return "UNKNOWN";
    }
}

void displayEmergencyTable(const EmergencyList *list, const Graph *g) {
    if (!list || list->count == 0) {
        printf("No emergency records logged in the system.\n");
        return;
    }

    printf("+------+----------------------+----------------------+----------+------------------------+--------+--------+--------------+\n");
    printf("| ID   | Patient Name         | Emergency Type       | Severity | Patient Location       | Amb ID | HospID | Status       |\n");
    printf("+------+----------------------+----------------------+----------+------------------------+--------+--------+--------------+\n");
    for (int i = 0; i < list->count; i++) {
        EmergencyRequest *req = &list->records[i];
        const char *locName = (g && req->landmarkNode >= 0 && req->landmarkNode < g->numVertices)
                              ? g->vertices[req->landmarkNode].name : "Unknown";

        char hospStr[12];
        if (req->assignedHospitalId > 0) {
            snprintf(hospStr, sizeof(hospStr), "HOSP-%d", req->assignedHospitalId);
        } else {
            strncpy(hospStr, "None", sizeof(hospStr));
        }

        const char *ambStr = (strlen(req->assignedAmbulanceId) > 0) ? req->assignedAmbulanceId : "None";

        printf("| %-4d | %-20s | %-20s | %-8s | %-22s | %-6s | %-6s | %-12s |\n",
               req->id,
               req->patientName,
               getEmergencyTypeStr(req->type),
               getSeverityStr(req->severity),
               locName,
               ambStr,
               hospStr,
               getEmergencyStatusStr(req->status));
    }
    printf("+------+----------------------+----------------------+----------+------------------------+--------+--------+--------------+\n");
    printf("Total Records: %d\n", list->count);
}

void displaySingleEmergencyDetail(const EmergencyRequest *req, const Graph *g) {
    if (!req) return;
    const char *locName = (g && req->landmarkNode >= 0 && req->landmarkNode < g->numVertices)
                          ? g->vertices[req->landmarkNode].name : "Unknown Location";

    printf("\n============================================================\n");
    printf("               EMERGENCY INCIDENT TICKET #%d                \n", req->id);
    printf("============================================================\n");
    printf("Registration Time   : %s\n", req->registeredTime);
    printf("Patient Name        : %s\n", req->patientName);
    printf("Contact Number      : %s\n", req->contactNumber);
    printf("Incident Category   : %s\n", getEmergencyTypeStr(req->type));
    printf("Triage Severity     : %s (Priority %d)\n", getSeverityStr(req->severity), (int)req->severity);
    printf("Patient Landmark    : %s\n", locName);
    printf("Assigned Ambulance  : %s\n", strlen(req->assignedAmbulanceId) > 0 ? req->assignedAmbulanceId : "Pending Assignment");
    printf("Assigned Hospital   : %s\n", req->assignedHospitalId > 0 ? "Assigned" : "Pending Selection");
    printf("Ambulance -> Patient: %.2f km\n", req->ambulanceToPatientDist);
    printf("Patient -> Hospital : %.2f km\n", req->patientToHospitalDist);
    printf("Total Mission Dist  : %.2f km\n", req->totalDistance);
    printf("Current Status      : %s\n", getEmergencyStatusStr(req->status));
    printf("Dispatch Notes      : %s\n", req->dispatchNotes);
    printf("============================================================\n");
}

void freeEmergencyList(EmergencyList *list) {
    if (!list) return;
    free(list->records);
    free(list);
}
