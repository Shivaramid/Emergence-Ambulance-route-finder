#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NAME_LEN 64
#define MAX_LOCALITY_LEN 64

/* Node types representing different categories of graph vertices */
typedef enum {
    NODE_LANDMARK = 0,
    NODE_JUNCTION,
    NODE_RESIDENTIAL,
    NODE_HOSPITAL
} NodeType;

/* Edge in the adjacency list representing a road */
typedef struct Edge {
    int destination;           /* Vertex index of destination */
    float distance;            /* Simulated road distance in kilometers */
    struct Edge *next;         /* Pointer to next edge in adjacency list */
} Edge;

/* Vertex representing a landmark, junction, or hospital */
typedef struct Vertex {
    int id;                    /* Unique node identifier (0-based index) */
    char name[MAX_NAME_LEN];   /* Specific landmark/facility name */
    char locality[MAX_LOCALITY_LEN]; /* General area/locality */
    NodeType type;             /* Type of node */
    Edge *head;                /* Head of linked list of adjacent edges */
} Vertex;

/* Graph structure with dynamic vertex capacity */
typedef struct Graph {
    int numVertices;           /* Current number of vertices */
    int capacity;              /* Allocated capacity */
    Vertex *vertices;          /* Dynamically allocated array of vertices */
} Graph;

/* Function prototypes */
Graph* createGraph(int initialCapacity);
int addLocation(Graph *g, const char *name, const char *locality, NodeType type);
int addRoad(Graph *g, int src, int dest, float distance);
void displayGraph(const Graph *g);
void displayAllLocations(const Graph *g);
int findLocationByName(const Graph *g, const char *name);
const char* getNodeTypeName(NodeType type);
void freeGraph(Graph *g);

#endif /* GRAPH_H */
