#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dijkstra.h"

/* Computes estimated travel time assuming average city emergency speed */
float calculateEstimatedTime(float distanceKm) {
    if (distanceKm <= 0.0f) return 0.0f;
    /* (distance / speed) * 60 minutes + 1.0 min base traffic/mobilization buffer */
    float time = (distanceKm / AVERAGE_AMBULANCE_SPEED_KMPH) * 60.0f;
    if (time < 1.0f) time = 1.0f;
    return time;
}

/* Dijkstra's Shortest Path Algorithm implementation */
ShortestRoute dijkstraShortestPath(const Graph *g, int source, int destination) {
    ShortestRoute route;
    route.pathLength = 0;
    route.totalDistance = 0.0f;
    route.estimatedTimeMinutes = 0.0f;
    route.found = 0;

    if (!g || g->numVertices == 0) return route;
    if (source < 0 || source >= g->numVertices || destination < 0 || destination >= g->numVertices) {
        return route;
    }

    int n = g->numVertices;

    /* Allocate dynamic tracking arrays */
    float *dist = (float*)malloc((size_t)n * sizeof(float));
    int *visited = (int*)calloc((size_t)n, sizeof(int));
    int *parent = (int*)malloc((size_t)n * sizeof(int));

    if (!dist || !visited || !parent) {
        fprintf(stderr, "[Error] Memory allocation failed in Dijkstra computation.\n");
        if (dist) free(dist);
        if (visited) free(visited);
        if (parent) free(parent);
        return route;
    }

    /* Step 1: Initialize all distances to infinity, source to 0 */
    for (int i = 0; i < n; i++) {
        dist[i] = INF_DISTANCE;
        parent[i] = -1;
    }
    dist[source] = 0.0f;

    /* Step 2: Dijkstra relaxation loop */
    for (int i = 0; i < n; i++) {
        int u = -1;
        float minDist = INF_DISTANCE;

        /* Step 3: Select unvisited vertex with minimum distance */
        for (int v = 0; v < n; v++) {
            if (!visited[v] && dist[v] < minDist) {
                minDist = dist[v];
                u = v;
            }
        }

        /* If no reachable unvisited node remains or target reached */
        if (u == -1 || minDist >= INF_DISTANCE) {
            break;
        }

        visited[u] = 1;

        if (u == destination) {
            break; /* Early termination once target is settled */
        }

        /* Step 4: Relax all adjacent edges of vertex u */
        Edge *edge = g->vertices[u].head;
        while (edge) {
            int v = edge->destination;
            if (!visited[v]) {
                float newDist = dist[u] + edge->distance;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    parent[v] = u; /* Step 5: Store parent node */
                }
            }
            edge = edge->next;
        }
    }

    /* Step 6 & 7: Check if destination was reached and reconstruct path */
    if (dist[destination] < INF_DISTANCE) {
        route.found = 1;
        route.totalDistance = dist[destination];
        route.estimatedTimeMinutes = calculateEstimatedTime(dist[destination]);

        /* Reconstruct path from destination to source using a stack */
        int tempStack[MAX_PATH_NODES];
        int stackTop = 0;
        int curr = destination;

        while (curr != -1 && stackTop < MAX_PATH_NODES) {
            tempStack[stackTop++] = curr;
            if (curr == source) break;
            curr = parent[curr];
        }

        /* Invert stack into path array */
        route.pathLength = 0;
        for (int i = stackTop - 1; i >= 0; i--) {
            route.path[route.pathLength++] = tempStack[i];
        }
    }

    /* Free dynamically allocated tracking arrays */
    free(dist);
    free(visited);
    free(parent);

    return route;
}

/* Displays the calculated shortest route with visual formatting */
void printRoute(const Graph *g, const ShortestRoute *route) {
    if (!g || !route || !route->found || route->pathLength == 0) {
        printf("\n============================================================\n");
        printf("               NO ROUTE FOUND BETWEEN LOCATIONS             \n");
        printf("============================================================\n");
        printf("A connected road route could not be established.\n");
        return;
    }

    int src = route->path[0];
    int dest = route->path[route->pathLength - 1];

    printf("\n============================================================\n");
    printf("                  CALCULATED SHORTEST ROUTE                 \n");
    printf("============================================================\n");
    printf("Source Location      : %s [%s]\n", g->vertices[src].name, g->vertices[src].locality);
    printf("Destination Location : %s [%s]\n", g->vertices[dest].name, g->vertices[dest].locality);
    printf("------------------------------------------------------------\n");
    printf("Route Progression:\n\n");

    for (int i = 0; i < route->pathLength; i++) {
        int nodeIdx = route->path[i];
        printf("  [%02d] %s (%s)", nodeIdx, g->vertices[nodeIdx].name, getNodeTypeName(g->vertices[nodeIdx].type));
        if (i < route->pathLength - 1) {
            /* Find distance to next step */
            float stepDist = 0.0f;
            Edge *e = g->vertices[nodeIdx].head;
            while (e) {
                if (e->destination == route->path[i + 1]) {
                    stepDist = e->distance;
                    break;
                }
                e = e->next;
            }
            printf("\n         | (%.2f km)\n         v\n", stepDist);
        } else {
            printf("\n");
        }
    }

    printf("------------------------------------------------------------\n");
    printf("Total Route Distance : %.2f km (Simulated Road Distance)\n", route->totalDistance);
    printf("Estimated Transit    : %.1f minutes (~%.0f min)\n",
           route->estimatedTimeMinutes, ceil(route->estimatedTimeMinutes));
    printf("============================================================\n");
}
