#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/* Creates a new graph with dynamic memory allocation */
Graph* createGraph(int initialCapacity) {
    if (initialCapacity <= 0) {
        initialCapacity = 16;
    }
    Graph *g = (Graph*)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "[Error] Memory allocation failed for Graph structure.\n");
        return NULL;
    }
    g->numVertices = 0;
    g->capacity = initialCapacity;
    g->vertices = (Vertex*)calloc((size_t)initialCapacity, sizeof(Vertex));
    if (!g->vertices) {
        fprintf(stderr, "[Error] Memory allocation failed for Graph vertices array.\n");
        free(g);
        return NULL;
    }
    return g;
}

/* Returns human-readable label for node type */
const char* getNodeTypeName(NodeType type) {
    switch (type) {
        case NODE_LANDMARK:    return "Landmark";
        case NODE_JUNCTION:    return "Junction";
        case NODE_RESIDENTIAL: return "Residential";
        case NODE_HOSPITAL:    return "Hospital";
        default:               return "General";
    }
}

/* Adds a new location node to the graph */
int addLocation(Graph *g, const char *name, const char *locality, NodeType type) {
    if (!g) return -1;

    /* Expand capacity dynamically if needed */
    if (g->numVertices >= g->capacity) {
        int newCapacity = g->capacity * 2;
        Vertex *newVertices = (Vertex*)realloc(g->vertices, (size_t)newCapacity * sizeof(Vertex));
        if (!newVertices) {
            fprintf(stderr, "[Error] Failed to expand Graph capacity.\n");
            return -1;
        }
        g->vertices = newVertices;
        /* Zero out newly allocated vertices */
        for (int i = g->capacity; i < newCapacity; i++) {
            memset(&g->vertices[i], 0, sizeof(Vertex));
        }
        g->capacity = newCapacity;
    }

    int id = g->numVertices;
    g->vertices[id].id = id;
    strncpy(g->vertices[id].name, name, MAX_NAME_LEN - 1);
    g->vertices[id].name[MAX_NAME_LEN - 1] = '\0';
    strncpy(g->vertices[id].locality, locality, MAX_LOCALITY_LEN - 1);
    g->vertices[id].locality[MAX_LOCALITY_LEN - 1] = '\0';
    g->vertices[id].type = type;
    g->vertices[id].head = NULL;

    g->numVertices++;
    return id;
}

/* Internal helper: appends a directed edge to vertex's linked list */
static void addEdgeInternal(Graph *g, int src, int dest, float distance) {
    Edge *newEdge = (Edge*)malloc(sizeof(Edge));
    if (!newEdge) {
        fprintf(stderr, "[Error] Failed to allocate memory for road edge.\n");
        return;
    }
    newEdge->destination = dest;
    newEdge->distance = distance;
    newEdge->next = g->vertices[src].head;
    g->vertices[src].head = newEdge;
}

/* Adds a bidirectional road between two locations */
int addRoad(Graph *g, int src, int dest, float distance) {
    if (!g) return 0;
    if (src < 0 || src >= g->numVertices || dest < 0 || dest >= g->numVertices) {
        return 0;
    }
    if (src == dest || distance <= 0.0f) {
        return 0;
    }

    /* Check if edge already exists */
    Edge *curr = g->vertices[src].head;
    while (curr) {
        if (curr->destination == dest) {
            curr->distance = distance; /* Update weight */
            /* Update reverse edge too */
            Edge *rev = g->vertices[dest].head;
            while (rev) {
                if (rev->destination == src) {
                    rev->distance = distance;
                    break;
                }
                rev = rev->next;
            }
            return 1;
        }
        curr = curr->next;
    }

    /* Add bidirectional edges */
    addEdgeInternal(g, src, dest, distance);
    addEdgeInternal(g, dest, src, distance);
    return 1;
}

/* Displays all vertices in a structured table */
void displayAllLocations(const Graph *g) {
    if (!g || g->numVertices == 0) {
        printf("No locations registered in the system.\n");
        return;
    }

    printf("+-----+--------------------------------------+--------------------+---------------+\n");
    printf("| ID  | Specific Landmark / Facility         | Locality / Area    | Category      |\n");
    printf("+-----+--------------------------------------+--------------------+---------------+\n");
    for (int i = 0; i < g->numVertices; i++) {
        printf("| %-3d | %-36s | %-18s | %-13s |\n",
               g->vertices[i].id,
               g->vertices[i].name,
               g->vertices[i].locality,
               getNodeTypeName(g->vertices[i].type));
    }
    printf("+-----+--------------------------------------+--------------------+---------------+\n");
    printf("Total Locations: %d\n", g->numVertices);
}

/* Displays the complete graph as an adjacency list */
void displayGraph(const Graph *g) {
    if (!g || g->numVertices == 0) {
        printf("The road network graph is empty.\n");
        return;
    }

    printf("\n========================================================================================\n");
    printf("                       ROAD NETWORK ADJACENCY LIST REPRESENTATION                       \n");
    printf("========================================================================================\n");
    for (int i = 0; i < g->numVertices; i++) {
        printf("[%2d] %-30s -> ", g->vertices[i].id, g->vertices[i].name);
        Edge *curr = g->vertices[i].head;
        if (!curr) {
            printf("(No connected roads)");
        } else {
            while (curr) {
                printf("[%d: %s (%.1f km)]",
                       curr->destination,
                       g->vertices[curr->destination].name,
                       curr->distance);
                if (curr->next) {
                    printf(" -> ");
                }
                curr = curr->next;
            }
        }
        printf("\n");
    }
    printf("========================================================================================\n");
}

/* Case-insensitive search for a location by name substring */
int findLocationByName(const Graph *g, const char *name) {
    if (!g || !name) return -1;
    for (int i = 0; i < g->numVertices; i++) {
        if (strstr(g->vertices[i].name, name) != NULL) {
            return i;
        }
    }
    return -1;
}

/* Deallocates the entire graph and its linked lists */
void freeGraph(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->numVertices; i++) {
        Edge *curr = g->vertices[i].head;
        while (curr) {
            Edge *temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(g->vertices);
    free(g);
}
