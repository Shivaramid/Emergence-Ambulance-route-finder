#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define CREATE_DIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define CREATE_DIR(d) mkdir(d, 0755)
#endif

#include "file_handler.h"

/* Ensures the 'data' directory exists */
int ensureDataDirectoryExists(void) {
    CREATE_DIR(DATA_DIR);
    return 1;
}

/* Save locations to formatted text file */
int saveLocationsToFile(const Graph *g, const char *filepath) {
    if (!g || !filepath) return 0;
    ensureDataDirectoryExists();

    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[Warning] Could not open %s for writing.\n", filepath);
        return 0;
    }

    fprintf(fp, "# Locations Dataset: ID|Name|Locality|Type\n");
    fprintf(fp, "%d\n", g->numVertices);
    for (int i = 0; i < g->numVertices; i++) {
        fprintf(fp, "%d|%s|%s|%d\n",
                g->vertices[i].id,
                g->vertices[i].name,
                g->vertices[i].locality,
                (int)g->vertices[i].type);
    }

    fclose(fp);
    return 1;
}

/* Load locations from file into graph */
int loadLocationsFromFile(Graph *g, const char *filepath) {
    if (!g || !filepath) return 0;

    FILE *fp = fopen(filepath, "r");
    if (!fp) return 0;

    char line[256];
    int count = 0;

    /* Read header and count */
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (sscanf(line, "%d", &count) == 1) break;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), fp)) break;
        int typeVal = 0;
        char name[MAX_NAME_LEN] = {0};
        char locality[MAX_LOCALITY_LEN] = {0};

        /* Parse pipe-delimited format */
        char *token = strtok(line, "|\n");
        /* Skip ID token (assigned sequentially by graph) */
        (void)token;

        token = strtok(NULL, "|\n");
        if (token) strncpy(name, token, sizeof(name) - 1);

        token = strtok(NULL, "|\n");
        if (token) strncpy(locality, token, sizeof(locality) - 1);

        token = strtok(NULL, "|\n");
        if (token) typeVal = atoi(token);

        addLocation(g, name, locality, (NodeType)typeVal);
    }

    fclose(fp);
    return (g->numVertices > 0);
}

/* Save roads (edges) to formatted file */
int saveRoadsToFile(const Graph *g, const char *filepath) {
    if (!g || !filepath) return 0;
    ensureDataDirectoryExists();

    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    fprintf(fp, "# Roads Dataset: Src|Dest|Distance\n");
    for (int i = 0; i < g->numVertices; i++) {
        Edge *e = g->vertices[i].head;
        while (e) {
            /* To avoid writing duplicate bidirectional edges, write only src < dest */
            if (i < e->destination) {
                fprintf(fp, "%d|%d|%.2f\n", i, e->destination, e->distance);
            }
            e = e->next;
        }
    }

    fclose(fp);
    return 1;
}

/* Load roads from file */
int loadRoadsFromFile(Graph *g, const char *filepath) {
    if (!g || !filepath) return 0;

    FILE *fp = fopen(filepath, "r");
    if (!fp) return 0;

    char line[128];
    int loaded = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        int src = -1, dest = -1;
        float dist = 0.0f;
        if (sscanf(line, "%d|%d|%f", &src, &dest, &dist) == 3) {
            addRoad(g, src, dest, dist);
            loaded++;
        }
    }

    fclose(fp);
    return (loaded > 0);
}

/* Save hospitals */
int saveHospitalsToFile(const HospitalList *list, const char *filepath) {
    if (!list || !filepath) return 0;
    ensureDataDirectoryExists();

    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    fprintf(fp, "# Hospitals Dataset: ID|Name|Node|Emergency|ICU|TotalBeds|AvailBeds|Status|Capacity|SelectionCount\n");
    fprintf(fp, "%d\n", list->count);
    for (int i = 0; i < list->count; i++) {
        Hospital *h = &list->hospitals[i];
        fprintf(fp, "%d|%s|%d|%d|%d|%d|%d|%d|%s|%d\n",
                h->id, h->name, h->locationNode, h->emergencyFacility,
                h->icuAvailability, h->totalBeds, h->availableBeds,
                (int)h->status, h->capacityLevel, h->selectionCount);
    }

    fclose(fp);
    return 1;
}

/* Load hospitals */
int loadHospitalsFromFile(HospitalList *list, const char *filepath) {
    if (!list || !filepath) return 0;

    FILE *fp = fopen(filepath, "r");
    if (!fp) return 0;

    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (sscanf(line, "%d", &count) == 1) break;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), fp)) break;
        Hospital h;
        memset(&h, 0, sizeof(h));

        char *token = strtok(line, "|\n");
        if (token) h.id = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) strncpy(h.name, token, sizeof(h.name) - 1);

        token = strtok(NULL, "|\n");
        if (token) h.locationNode = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) h.emergencyFacility = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) h.icuAvailability = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) h.totalBeds = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) h.availableBeds = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) h.status = (HospitalStatus)atoi(token);

        token = strtok(NULL, "|\n");
        if (token) strncpy(h.capacityLevel, token, sizeof(h.capacityLevel) - 1);

        token = strtok(NULL, "|\n");
        if (token) h.selectionCount = atoi(token);

        addHospital(list, h);
    }

    fclose(fp);
    return (list->count > 0);
}

/* Save ambulances */
int saveAmbulancesToFile(const AmbulanceList *list, const char *filepath) {
    if (!list || !filepath) return 0;
    ensureDataDirectoryExists();

    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    fprintf(fp, "# Ambulances Dataset: ID|VehicleNo|Node|Status|Equipment|AssignedEmergId|TotalKm\n");
    fprintf(fp, "%d\n", list->count);
    for (int i = 0; i < list->count; i++) {
        Ambulance *a = &list->ambulances[i];
        fprintf(fp, "%s|%s|%d|%d|%s|%d|%.2f\n",
                a->id, a->vehicleNumber, a->currentLocationNode,
                (int)a->status, a->equipment, a->assignedEmergencyId, a->totalKmRun);
    }

    fclose(fp);
    return 1;
}

/* Load ambulances */
int loadAmbulancesFromFile(AmbulanceList *list, const char *filepath) {
    if (!list || !filepath) return 0;

    FILE *fp = fopen(filepath, "r");
    if (!fp) return 0;

    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (sscanf(line, "%d", &count) == 1) break;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), fp)) break;
        Ambulance a;
        memset(&a, 0, sizeof(a));

        char *token = strtok(line, "|\n");
        if (token) strncpy(a.id, token, sizeof(a.id) - 1);

        token = strtok(NULL, "|\n");
        if (token) strncpy(a.vehicleNumber, token, sizeof(a.vehicleNumber) - 1);

        token = strtok(NULL, "|\n");
        if (token) a.currentLocationNode = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) a.status = (AmbulanceStatus)atoi(token);

        token = strtok(NULL, "|\n");
        if (token) strncpy(a.equipment, token, sizeof(a.equipment) - 1);

        token = strtok(NULL, "|\n");
        if (token) a.assignedEmergencyId = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) a.totalKmRun = (float)atof(token);

        addAmbulance(list, a);
    }

    fclose(fp);
    return (list->count > 0);
}

/* Save emergency records */
int saveEmergenciesToFile(const EmergencyList *list, const char *filepath) {
    if (!list || !filepath) return 0;
    ensureDataDirectoryExists();

    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    fprintf(fp, "# Emergencies Dataset: ID|Seq|Patient|Contact|Type|Severity|Node|PrefHosp|AmbID|HospID|AmbDist|HospDist|TotalDist|Status|Time|Notes\n");
    fprintf(fp, "%d\n", list->count);
    for (int i = 0; i < list->count; i++) {
        EmergencyRequest *r = &list->records[i];
        fprintf(fp, "%d|%d|%s|%s|%d|%d|%d|%d|%s|%d|%.2f|%.2f|%.2f|%d|%s|%s\n",
                r->id, r->sequenceNumber, r->patientName, r->contactNumber,
                (int)r->type, (int)r->severity, r->landmarkNode, r->preferredHospitalId,
                strlen(r->assignedAmbulanceId) > 0 ? r->assignedAmbulanceId : "None",
                r->assignedHospitalId, r->ambulanceToPatientDist,
                r->patientToHospitalDist, r->totalDistance, (int)r->status,
                r->registeredTime, r->dispatchNotes);
    }

    fclose(fp);
    return 1;
}

/* Load emergency records */
int loadEmergenciesFromFile(EmergencyList *list, const char *filepath) {
    if (!list || !filepath) return 0;

    FILE *fp = fopen(filepath, "r");
    if (!fp) return 0;

    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (sscanf(line, "%d", &count) == 1) break;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), fp)) break;
        EmergencyRequest r;
        memset(&r, 0, sizeof(r));

        char *token = strtok(line, "|\n");
        if (token) r.id = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) r.sequenceNumber = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) strncpy(r.patientName, token, sizeof(r.patientName) - 1);

        token = strtok(NULL, "|\n");
        if (token) strncpy(r.contactNumber, token, sizeof(r.contactNumber) - 1);

        token = strtok(NULL, "|\n");
        if (token) r.type = (EmergencyType)atoi(token);

        token = strtok(NULL, "|\n");
        if (token) r.severity = (EmergencySeverity)atoi(token);

        token = strtok(NULL, "|\n");
        if (token) r.landmarkNode = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) r.preferredHospitalId = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) {
            if (strcmp(token, "None") != 0) {
                strncpy(r.assignedAmbulanceId, token, sizeof(r.assignedAmbulanceId) - 1);
            }
        }

        token = strtok(NULL, "|\n");
        if (token) r.assignedHospitalId = atoi(token);

        token = strtok(NULL, "|\n");
        if (token) r.ambulanceToPatientDist = (float)atof(token);

        token = strtok(NULL, "|\n");
        if (token) r.patientToHospitalDist = (float)atof(token);

        token = strtok(NULL, "|\n");
        if (token) r.totalDistance = (float)atof(token);

        token = strtok(NULL, "|\n");
        if (token) r.status = (EmergencyStatus)atoi(token);

        token = strtok(NULL, "|\n");
        if (token) strncpy(r.registeredTime, token, sizeof(r.registeredTime) - 1);

        token = strtok(NULL, "|\n");
        if (token) strncpy(r.dispatchNotes, token, sizeof(r.dispatchNotes) - 1);

        addEmergencyRecord(list, r);
    }

    fclose(fp);
    return (list->count > 0);
}

/* Initializes default seed data if files don't exist */
void initializeDefaultDatasetIfEmpty(Graph *g, HospitalList *hList, AmbulanceList *aList, EmergencyList *eList, PriorityQueue *pq) {
    if (!g || !hList || !aList || !eList) return;

    /* Try loading from file first */
    int locLoaded = loadLocationsFromFile(g, LOCATIONS_FILE);
    int roadLoaded = loadRoadsFromFile(g, ROADS_FILE);
    int hospLoaded = loadHospitalsFromFile(hList, HOSPITALS_FILE);
    int ambLoaded = loadAmbulancesFromFile(aList, AMBULANCES_FILE);
    int emergLoaded = loadEmergenciesFromFile(eList, EMERGENCIES_FILE);

    /* If locations not loaded, build full seed dataset */
    if (!locLoaded || g->numVertices == 0) {
        printf("[System] Seeding default graph locations for Narsingi & surrounding zones...\n");

        /* Nodes 0 to 23: Landmarks & Junctions */
        addLocation(g, "DMart Narsingi", "Narsingi", NODE_LANDMARK);            /* 0 */
        addLocation(g, "Narsingi X Roads", "Narsingi", NODE_JUNCTION);           /* 1 */
        addLocation(g, "Narsingi Police Station", "Narsingi", NODE_LANDMARK);    /* 2 */
        addLocation(g, "Narsingi Bus Stop", "Narsingi", NODE_LANDMARK);          /* 3 */
        addLocation(g, "Narsingi Lake", "Narsingi", NODE_LANDMARK);              /* 4 */
        addLocation(g, "Ratnadeep Supermarket", "Narsingi", NODE_LANDMARK);      /* 5 */
        addLocation(g, "MN Circle", "Narsingi", NODE_JUNCTION);                  /* 6 */
        addLocation(g, "Rajapushpa Regalia", "Kokapet", NODE_RESIDENTIAL);       /* 7 */
        addLocation(g, "Alkapur Township", "Neknampur", NODE_RESIDENTIAL);       /* 8 */
        addLocation(g, "Puppalaguda Junction", "Puppalaguda", NODE_JUNCTION);    /* 9 */
        addLocation(g, "Manikonda Main Road", "Manikonda", NODE_JUNCTION);       /* 10 */
        addLocation(g, "Neknampur Junction", "Neknampur", NODE_JUNCTION);        /* 11 */
        addLocation(g, "Manchirevula Junction", "Manchirevula", NODE_JUNCTION);  /* 12 */
        addLocation(g, "Manchirevula Bridge", "Manchirevula", NODE_LANDMARK);    /* 13 */
        addLocation(g, "Kokapet Junction", "Kokapet", NODE_JUNCTION);            /* 14 */
        addLocation(g, "Kokapet Golden Mile", "Kokapet", NODE_LANDMARK);         /* 15 */
        addLocation(g, "Gandipet Lake", "Gandipet", NODE_LANDMARK);              /* 16 */
        addLocation(g, "CBIT Gandipet", "Gandipet", NODE_LANDMARK);              /* 17 */
        addLocation(g, "Nanakramguda Circle", "Nanakramguda", NODE_JUNCTION);    /* 18 */
        addLocation(g, "Gachibowli Junction", "Gachibowli", NODE_JUNCTION);      /* 19 */
        addLocation(g, "Mehdipatnam Rythu Bazar", "Mehdipatnam", NODE_JUNCTION); /* 20 */
        addLocation(g, "Rajendranagar Agri Univ", "Rajendranagar", NODE_LANDMARK);/* 21 */
        addLocation(g, "Kismatpur Junction", "Kismatpur", NODE_JUNCTION);        /* 22 */
        addLocation(g, "Shamshabad Airport Road", "Shamshabad", NODE_JUNCTION);  /* 23 */

        /* Nodes 24 to 31: Hospitals */
        addLocation(g, "PULSE Hospital", "Narsingi", NODE_HOSPITAL);             /* 24 */
        addLocation(g, "Delight Hospital", "Narsingi", NODE_HOSPITAL);           /* 25 */
        addLocation(g, "Spring Field Hospital", "Puppalaguda", NODE_HOSPITAL);   /* 26 */
        addLocation(g, "DVR Hospital & Diagnostics", "Manikonda", NODE_HOSPITAL);/* 27 */
        addLocation(g, "Jasmin Multispeciality Hospital", "Alkapur", NODE_HOSPITAL);/* 28 */
        addLocation(g, "Narsingi Emergency Care", "Narsingi", NODE_HOSPITAL);    /* 29 */
        addLocation(g, "Kokapet Emergency Hospital", "Kokapet", NODE_HOSPITAL);  /* 30 */
        addLocation(g, "Gachibowli Emergency Hospital", "Gachibowli", NODE_HOSPITAL);/* 31 */

        saveLocationsToFile(g, LOCATIONS_FILE);
    }

    if (!roadLoaded) {
        printf("[System] Seeding default road graph connections...\n");
        /* Narsingi Core Roads */
        addRoad(g, 0, 1, 1.20f);
        addRoad(g, 0, 6, 2.00f);
        addRoad(g, 0, 5, 0.80f);
        addRoad(g, 1, 2, 0.90f);
        addRoad(g, 1, 3, 0.70f);
        addRoad(g, 1, 24, 0.60f); /* To PULSE Hospital */
        addRoad(g, 1, 12, 2.80f);
        addRoad(g, 1, 14, 3.20f);
        addRoad(g, 1, 21, 5.80f);
        addRoad(g, 2, 4, 1.10f);
        addRoad(g, 2, 29, 0.70f); /* To Narsingi Emergency Care */
        addRoad(g, 2, 25, 1.00f); /* To Delight Hospital */
        addRoad(g, 3, 5, 0.60f);
        addRoad(g, 3, 25, 0.50f); /* To Delight Hospital */
        addRoad(g, 4, 29, 0.40f);
        addRoad(g, 4, 12, 2.30f);
        addRoad(g, 5, 6, 1.40f);
        addRoad(g, 5, 24, 1.30f);
        addRoad(g, 6, 7, 1.50f);
        addRoad(g, 6, 8, 1.80f);
        addRoad(g, 6, 28, 0.80f); /* To Jasmin Hospital */
        addRoad(g, 7, 14, 2.20f);
        addRoad(g, 8, 9, 2.10f);
        addRoad(g, 8, 11, 2.40f);
        addRoad(g, 8, 28, 1.10f);

        /* Puppalaguda & Manikonda */
        addRoad(g, 9, 10, 1.70f);
        addRoad(g, 9, 26, 0.50f); /* To Spring Field Hospital */
        addRoad(g, 9, 18, 3.00f);
        addRoad(g, 10, 11, 2.50f);
        addRoad(g, 10, 27, 0.60f); /* To DVR Hospital */
        addRoad(g, 10, 18, 2.80f);
        addRoad(g, 10, 26, 1.90f);
        addRoad(g, 11, 20, 5.50f);
        addRoad(g, 11, 16, 4.20f);

        /* Manchirevula & Gandipet */
        addRoad(g, 12, 13, 1.50f);
        addRoad(g, 12, 21, 4.00f);
        addRoad(g, 13, 21, 3.20f);
        addRoad(g, 13, 22, 2.70f);

        /* Kokapet & Financial District */
        addRoad(g, 14, 15, 1.60f);
        addRoad(g, 14, 18, 3.50f);
        addRoad(g, 14, 16, 3.80f);
        addRoad(g, 15, 30, 0.60f); /* To Kokapet Emergency Hospital */
        addRoad(g, 15, 17, 3.10f);
        addRoad(g, 16, 17, 2.40f);
        addRoad(g, 16, 22, 4.50f);
        addRoad(g, 17, 14, 4.10f);

        /* Gachibowli & Highways */
        addRoad(g, 18, 19, 3.20f);
        addRoad(g, 18, 27, 2.90f);
        addRoad(g, 18, 30, 3.60f);
        addRoad(g, 18, 31, 3.40f);
        addRoad(g, 19, 31, 0.70f); /* To Gachibowli Emergency Hospital */
        addRoad(g, 19, 20, 9.00f);
        addRoad(g, 20, 21, 6.00f);
        addRoad(g, 21, 22, 3.40f);
        addRoad(g, 21, 23, 8.50f);
        addRoad(g, 22, 23, 7.20f);

        saveRoadsToFile(g, ROADS_FILE);
    }

    if (!hospLoaded || hList->count == 0) {
        printf("[System] Seeding default hospital directory...\n");
        Hospital h1 = {1, "PULSE Hospital", 24, 1, 1, 45, 12, HOSP_OPEN, "HIGH", 5};
        Hospital h2 = {2, "Delight Hospital", 25, 1, 0, 30, 8, HOSP_OPEN, "MEDIUM", 2};
        Hospital h3 = {3, "Spring Field Hospital", 26, 1, 1, 50, 15, HOSP_OPEN, "HIGH", 4};
        Hospital h4 = {4, "DVR Hospital & Diagnostics", 27, 1, 0, 35, 6, HOSP_OPEN, "MEDIUM", 3};
        Hospital h5 = {5, "Jasmin Multispeciality Hospital", 28, 1, 1, 60, 18, HOSP_OPEN, "HIGH", 6};
        Hospital h6 = {6, "Narsingi Emergency Care", 29, 1, 1, 25, 5, HOSP_OPEN, "LOW", 4};
        Hospital h7 = {7, "Kokapet Emergency Hospital", 30, 1, 1, 40, 10, HOSP_OPEN, "MEDIUM", 3};
        Hospital h8 = {8, "Gachibowli Emergency Hospital", 31, 1, 1, 75, 22, HOSP_OPEN, "HIGH", 7};

        addHospital(hList, h1);
        addHospital(hList, h2);
        addHospital(hList, h3);
        addHospital(hList, h4);
        addHospital(hList, h5);
        addHospital(hList, h6);
        addHospital(hList, h7);
        addHospital(hList, h8);

        saveHospitalsToFile(hList, HOSPITALS_FILE);
    }

    if (!ambLoaded || aList->count == 0) {
        printf("[System] Seeding default ambulance fleet...\n");
        Ambulance a1 = {"AMB001", "TS-09-EM-1001", 1, AMB_AVAILABLE, "ALS (Advanced Life Support)", -1, 42.5f};
        Ambulance a2 = {"AMB002", "TS-09-EM-1002", 10, AMB_AVAILABLE, "ALS (Advanced Life Support)", -1, 58.0f};
        Ambulance a3 = {"AMB003", "TS-09-EM-1003", 14, AMB_AVAILABLE, "BLS (Basic Life Support)", -1, 31.2f};
        Ambulance a4 = {"AMB004", "TS-09-EM-1004", 19, AMB_AVAILABLE, "ALS (Advanced Life Support)", -1, 89.4f};
        Ambulance a5 = {"AMB005", "TS-09-EM-1005", 9, AMB_AVAILABLE, "BLS (Basic Life Support)", -1, 25.0f};
        Ambulance a6 = {"AMB006", "TS-09-EM-1006", 21, AMB_AVAILABLE, "BLS (Basic Life Support)", -1, 18.6f};
        Ambulance a7 = {"AMB007", "TS-09-EM-1007", 6, AMB_AVAILABLE, "ALS (Advanced Life Support)", -1, 64.3f};

        addAmbulance(aList, a1);
        addAmbulance(aList, a2);
        addAmbulance(aList, a3);
        addAmbulance(aList, a4);
        addAmbulance(aList, a5);
        addAmbulance(aList, a6);
        addAmbulance(aList, a7);

        saveAmbulancesToFile(aList, AMBULANCES_FILE);
    }

    if (!emergLoaded || eList->count == 0) {
        printf("[System] Seeding default sample emergency records...\n");
        EmergencyRequest r1 = {1001, 1, "Ramesh Rao", "9849012345", ET_CHEST_PAIN, SEV_CRITICAL, 0, -1, "", 0, 0.0f, 0.0f, 0.0f, EM_QUEUED, "10:15 AM", "Pending dispatch"};
        EmergencyRequest r2 = {1002, 2, "Sneha Reddy", "9849023456", ET_INJURY, SEV_LOW, 7, -1, "", 0, 0.0f, 0.0f, 0.0f, EM_QUEUED, "10:18 AM", "Pending dispatch"};
        EmergencyRequest r3 = {1003, 3, "Vikram Kumar", "9849034567", ET_ACCIDENT, SEV_HIGH, 9, -1, "", 0, 0.0f, 0.0f, 0.0f, EM_QUEUED, "10:22 AM", "Pending dispatch"};
        EmergencyRequest r4 = {1004, 4, "Ananya Sharma", "9849045678", ET_BREATHING, SEV_CRITICAL, 10, -1, "", 0, 0.0f, 0.0f, 0.0f, EM_QUEUED, "10:25 AM", "Pending dispatch"};

        addEmergencyRecord(eList, r1);
        addEmergencyRecord(eList, r2);
        addEmergencyRecord(eList, r3);
        addEmergencyRecord(eList, r4);

        saveEmergenciesToFile(eList, EMERGENCIES_FILE);
    }

    /* Enqueue any QUEUED or REGISTERED records into the Priority Queue */
    if (pq) {
        for (int i = 0; i < eList->count; i++) {
            if (eList->records[i].status == EM_QUEUED || eList->records[i].status == EM_REGISTERED) {
                enqueueEmergency(pq, eList->records[i]);
            }
        }
    }
}
