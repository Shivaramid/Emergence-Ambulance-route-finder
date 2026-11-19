#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

#define MAX_PATH_NODES 128
#define INF_DISTANCE 1e9f
#define AVERAGE_AMBULANCE_SPEED_KMPH 40.0f

/* Structure holding shortest route calculation results */
typedef struct {
    int path[MAX_PATH_NODES];     /* Array representing sequence of vertex indices */
    int pathLength;              /* Number of vertices along the path */
    float totalDistance;         /* Sum of edge weights in km */
    float estimatedTimeMinutes;  /* Simulated transit time */
    int found;                   /* 1 if path exists, 0 otherwise */
} ShortestRoute;

/* Function prototypes */
ShortestRoute dijkstraShortestPath(const Graph *g, int source, int destination);
void printRoute(const Graph *g, const ShortestRoute *route);
float calculateEstimatedTime(float distanceKm);

#endif /* DIJKSTRA_H */
