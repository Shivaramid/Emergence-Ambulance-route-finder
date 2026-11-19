#ifndef AMBULANCE_H
#define AMBULANCE_H

#include "graph.h"

#define MAX_AMB_ID_LEN 16
#define MAX_REG_NO_LEN 20
#define MAX_EQUIPMENT_LEN 48

typedef enum {
    AMB_AVAILABLE = 0,
    AMB_BUSY = 1,
    AMB_MAINTENANCE = 2
} AmbulanceStatus;

typedef struct Ambulance {
    char id[MAX_AMB_ID_LEN];                  /* Unique ambulance code (e.g. AMB001) */
    char vehicleNumber[MAX_REG_NO_LEN];      /* RTO registration (e.g. TS-09-EM-1001) */
    int currentLocationNode;                  /* Graph vertex index of current location */
    AmbulanceStatus status;                  /* AVAILABLE, BUSY, MAINTENANCE */
    char equipment[MAX_EQUIPMENT_LEN];        /* "Advanced Life Support (ALS)" or "Basic (BLS)" */
    int assignedEmergencyId;                  /* Associated emergency request ID, or -1 */
    float totalKmRun;                         /* Cumulative km driven */
} Ambulance;

typedef struct AmbulanceList {
    Ambulance *ambulances;
    int count;
    int capacity;
} AmbulanceList;

/* Function prototypes */
AmbulanceList* createAmbulanceList(int capacity);
int addAmbulance(AmbulanceList *list, Ambulance a);
void displayAmbulances(const AmbulanceList *list, const Graph *g);
Ambulance* findAmbulanceById(AmbulanceList *list, const char *id);
int selectNearestAmbulance(AmbulanceList *aList, const Graph *g, int patientNode, int severity, float *outDistance);
const char* getAmbulanceStatusStr(AmbulanceStatus status);
void freeAmbulanceList(AmbulanceList *list);

#endif /* AMBULANCE_H */
