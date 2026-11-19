#ifndef HOSPITAL_H
#define HOSPITAL_H

#include "graph.h"

#define MAX_HOSPITAL_NAME_LEN 64
#define MAX_CAPACITY_STR_LEN 16

typedef enum {
    HOSP_OPEN = 0,
    HOSP_CLOSED = 1
} HospitalStatus;

typedef struct Hospital {
    int id;                                /* Unique Hospital ID (1, 2, ...) */
    char name[MAX_HOSPITAL_NAME_LEN];      /* Hospital facility name */
    int locationNode;                      /* Graph vertex index corresponding to location */
    int emergencyFacility;                 /* 1 = Yes, 0 = No */
    int icuAvailability;                   /* 1 = Yes, 0 = No */
    int totalBeds;                         /* Total bed capacity */
    int availableBeds;                     /* Current available beds */
    HospitalStatus status;                 /* HOSP_OPEN or HOSP_CLOSED */
    char capacityLevel[MAX_CAPACITY_STR_LEN]; /* "HIGH", "MEDIUM", "LOW" */
    int selectionCount;                    /* Statistics counter */
} Hospital;

typedef struct HospitalList {
    Hospital *hospitals;
    int count;
    int capacity;
} HospitalList;

/* Function prototypes */
HospitalList* createHospitalList(int capacity);
int addHospital(HospitalList *list, Hospital h);
void displayHospitals(const HospitalList *list, const Graph *g);
Hospital* findHospitalById(HospitalList *list, int id);
Hospital* findHospitalByNode(HospitalList *list, int nodeIndex);
int selectBestHospital(HospitalList *hList, const Graph *g, int patientNode, int severity, int preferredId, char *reasonBuffer, int reasonBufSize);
const char* getHospitalStatusStr(HospitalStatus status);
void freeHospitalList(HospitalList *list);

#endif /* HOSPITAL_H */
