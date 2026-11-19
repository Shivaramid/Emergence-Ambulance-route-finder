#ifndef EMERGENCY_H
#define EMERGENCY_H

#include "graph.h"

#define MAX_PATIENT_NAME_LEN 64
#define MAX_CONTACT_LEN 20
#define MAX_TIME_STR_LEN 32
#define MAX_NOTES_LEN 128

typedef enum {
    SEV_CRITICAL = 1,
    SEV_HIGH = 2,
    SEV_MEDIUM = 3,
    SEV_LOW = 4
} EmergencySeverity;

typedef enum {
    ET_ACCIDENT = 1,
    ET_CHEST_PAIN,
    ET_BREATHING,
    ET_INJURY,
    ET_UNCONSCIOUS,
    ET_PREGNANCY,
    ET_OTHER
} EmergencyType;

typedef enum {
    EM_REGISTERED = 0,
    EM_QUEUED,
    EM_PROCESSING,
    EM_AMBULANCE_ASSIGNED,
    EM_EN_ROUTE,
    EM_ARRIVED,
    EM_COMPLETED,
    EM_CANCELLED
} EmergencyStatus;

typedef struct EmergencyRequest {
    int id;                                /* Unique Ticket ID (e.g. 1001) */
    int sequenceNumber;                    /* Monotonic registration order counter for tie-breaking */
    char patientName[MAX_PATIENT_NAME_LEN];/* Patient / caller full name */
    char contactNumber[MAX_CONTACT_LEN];   /* Emergency contact phone */
    EmergencyType type;                    /* Medical incident category */
    EmergencySeverity severity;            /* CRITICAL, HIGH, MEDIUM, LOW */
    int landmarkNode;                      /* Graph node index where patient is located */
    int preferredHospitalId;               /* Caller preferred hospital ID (-1 if none) */
    char assignedAmbulanceId[16];          /* Assigned vehicle code (e.g. "AMB001") */
    int assignedHospitalId;                /* Assigned destination hospital ID */
    float ambulanceToPatientDist;          /* Dispatch to pickup road distance */
    float patientToHospitalDist;           /* Pickup to emergency room road distance */
    float totalDistance;                   /* Total mission distance */
    EmergencyStatus status;                /* Lifecycle status */
    char registeredTime[MAX_TIME_STR_LEN]; /* Registration timestamp */
    char dispatchNotes[MAX_NOTES_LEN];     /* Dispatcher and algorithm routing notes */
} EmergencyRequest;

typedef struct EmergencyList {
    EmergencyRequest *records;
    int count;
    int capacity;
} EmergencyList;

/* Function prototypes */
EmergencyList* createEmergencyList(int capacity);
int addEmergencyRecord(EmergencyList *list, EmergencyRequest req);
EmergencyRequest* findEmergencyById(EmergencyList *list, int id);
const char* getSeverityStr(EmergencySeverity sev);
const char* getEmergencyTypeStr(EmergencyType type);
const char* getEmergencyStatusStr(EmergencyStatus status);
void displayEmergencyTable(const EmergencyList *list, const Graph *g);
void displaySingleEmergencyDetail(const EmergencyRequest *req, const Graph *g);
void freeEmergencyList(EmergencyList *list);

#endif /* EMERGENCY_H */
