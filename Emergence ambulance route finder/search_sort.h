#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include "graph.h"
#include "emergency.h"
#include "ambulance.h"

/* Search function prototypes */
int searchEmergencyById(const EmergencyList *list, int id);
void searchEmergencyByName(const EmergencyList *list, const char *nameSubstr, const Graph *g);
void searchEmergencyByType(const EmergencyList *list, EmergencyType type, const Graph *g);
int searchAmbulanceById(const AmbulanceList *list, const char *id);
int searchLocationByName(const Graph *g, const char *query);

/* Sorting function prototypes */
void sortEmergenciesByPriority(EmergencyRequest *arr, int n);  /* Demonstrates Selection Sort */
void sortEmergenciesByName(EmergencyRequest *arr, int n);      /* Demonstrates Bubble Sort */
void sortEmergenciesByTime(EmergencyRequest *arr, int n);      /* Demonstrates Insertion Sort */
void sortEmergenciesByDistance(EmergencyRequest *arr, int n);  /* Demonstrates Selection Sort */

/* Display sorted results */
void displaySortedEmergencies(const EmergencyRequest *arr, int n, const Graph *g, const char *sortCriteria);

#endif /* SEARCH_SORT_H */
