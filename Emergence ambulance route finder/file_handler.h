#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "graph.h"
#include "hospital.h"
#include "ambulance.h"
#include "emergency.h"
#include "priority_queue.h"

#define DATA_DIR "data"
#define LOCATIONS_FILE   "data/locations.dat"
#define ROADS_FILE       "data/roads.dat"
#define HOSPITALS_FILE   "data/hospitals.dat"
#define AMBULANCES_FILE  "data/ambulances.dat"
#define EMERGENCIES_FILE "data/emergencies.dat"

/* Directory management */
int ensureDataDirectoryExists(void);

/* File I/O operations */
int saveLocationsToFile(const Graph *g, const char *filepath);
int loadLocationsFromFile(Graph *g, const char *filepath);

int saveRoadsToFile(const Graph *g, const char *filepath);
int loadRoadsFromFile(Graph *g, const char *filepath);

int saveHospitalsToFile(const HospitalList *list, const char *filepath);
int loadHospitalsFromFile(HospitalList *list, const char *filepath);

int saveAmbulancesToFile(const AmbulanceList *list, const char *filepath);
int loadAmbulancesFromFile(AmbulanceList *list, const char *filepath);

int saveEmergenciesToFile(const EmergencyList *list, const char *filepath);
int loadEmergenciesFromFile(EmergencyList *list, const char *filepath);

/* Seed initial simulated dataset if data files do not exist */
void initializeDefaultDatasetIfEmpty(Graph *g, HospitalList *hList, AmbulanceList *aList, EmergencyList *eList, PriorityQueue *pq);

#endif /* FILE_HANDLER_H */
