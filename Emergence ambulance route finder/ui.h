#ifndef UI_H
#define UI_H

#include "graph.h"
#include "hospital.h"
#include "ambulance.h"
#include "emergency.h"

/* UI rendering utilities */
void clearScreen(void);
void pauseKey(void);
void printHeader(const char *title);
void printSubHeader(const char *title);
void printDivider(void);
void printSuccess(const char *msg);
void printWarning(const char *msg);
void printError(const char *msg);
void printInfo(const char *msg);

/* Interactive menus and maps */
void displayLocationMap(void);
void displayMainMenu(void);

/* Safe user input helpers */
int getIntInput(const char *prompt, int min, int max);
float getFloatInput(const char *prompt, float min, float max);
void getStringInput(const char *prompt, char *buffer, int maxLen);

/* System analytics display */
void displaySystemStatistics(const EmergencyList *eList, const AmbulanceList *aList, const HospitalList *hList, const Graph *g);

#endif /* UI_H */
