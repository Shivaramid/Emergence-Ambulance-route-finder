#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"

/* Allocates and initializes the manual priority queue */
PriorityQueue* createPriorityQueue(int initialCapacity) {
    if (initialCapacity <= 0) initialCapacity = 16;
    PriorityQueue *pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        fprintf(stderr, "[Error] Failed to allocate PriorityQueue structure.\n");
        return NULL;
    }
    pq->size = 0;
    pq->capacity = initialCapacity;
    pq->heap = (EmergencyRequest*)calloc((size_t)initialCapacity, sizeof(EmergencyRequest));
    if (!pq->heap) {
        fprintf(stderr, "[Error] Failed to allocate PriorityQueue heap array.\n");
        free(pq);
        return NULL;
    }
    return pq;
}

/*
 * Priority comparison:
 * 1. Severity: SEV_CRITICAL (1) > SEV_HIGH (2) > SEV_MEDIUM (3) > SEV_LOW (4)
 * 2. FIFO Tie-breaker: If severities match, earlier registration (smaller sequenceNumber) wins.
 * Returns 1 if 'a' has higher priority than 'b', 0 otherwise.
 */
int isHigherPriority(const EmergencyRequest *a, const EmergencyRequest *b) {
    if (!a || !b) return 0;
    if (a->severity < b->severity) {
        return 1; /* Numerically smaller severity is higher clinical priority */
    }
    if (a->severity > b->severity) {
        return 0;
    }
    /* Tie-breaker: FIFO order */
    return (a->sequenceNumber < b->sequenceNumber) ? 1 : 0;
}

/* Helper to swap two requests in the heap */
static void swapRequests(EmergencyRequest *a, EmergencyRequest *b) {
    EmergencyRequest temp = *a;
    *a = *b;
    *b = temp;
}

/* Enqueues a new emergency request and maintains heap property (bubble up) */
int enqueueEmergency(PriorityQueue *pq, EmergencyRequest req) {
    if (!pq) return 0;

    /* Expand capacity if full */
    if (pq->size >= pq->capacity) {
        int newCap = pq->capacity * 2;
        EmergencyRequest *newHeap = (EmergencyRequest*)realloc(pq->heap, (size_t)newCap * sizeof(EmergencyRequest));
        if (!newHeap) {
            fprintf(stderr, "[Error] Failed to reallocate PriorityQueue heap.\n");
            return 0;
        }
        pq->heap = newHeap;
        pq->capacity = newCap;
    }

    int curr = pq->size;
    pq->heap[curr] = req;
    pq->size++;

    /* Bubble up (Heapify Up) */
    while (curr > 0) {
        int parent = (curr - 1) / 2;
        if (isHigherPriority(&pq->heap[curr], &pq->heap[parent])) {
            swapRequests(&pq->heap[curr], &pq->heap[parent]);
            curr = parent;
        } else {
            break;
        }
    }

    return 1;
}

/* Dequeues the highest priority emergency request (bubble down) */
int dequeueEmergency(PriorityQueue *pq, EmergencyRequest *outReq) {
    if (!pq || pq->size == 0) return 0;

    if (outReq) {
        *outReq = pq->heap[0];
    }

    /* Move last element to root and bubble down */
    pq->heap[0] = pq->heap[pq->size - 1];
    pq->size--;

    int curr = 0;
    while (1) {
        int left = 2 * curr + 1;
        int right = 2 * curr + 2;
        int highest = curr;

        if (left < pq->size && isHigherPriority(&pq->heap[left], &pq->heap[highest])) {
            highest = left;
        }
        if (right < pq->size && isHigherPriority(&pq->heap[right], &pq->heap[highest])) {
            highest = right;
        }

        if (highest != curr) {
            swapRequests(&pq->heap[curr], &pq->heap[highest]);
            curr = highest;
        } else {
            break;
        }
    }

    return 1;
}

/* Peeks at the highest priority item without removing it */
int peekEmergency(const PriorityQueue *pq, EmergencyRequest *outReq) {
    if (!pq || pq->size == 0) return 0;
    if (outReq) {
        *outReq = pq->heap[0];
    }
    return 1;
}

int isPQEmpty(const PriorityQueue *pq) {
    return (!pq || pq->size == 0);
}

int getPQSize(const PriorityQueue *pq) {
    return pq ? pq->size : 0;
}

/* Displays the priority queue in exact dispatch order by duplicating the heap */
void displayPriorityQueue(const PriorityQueue *pq, const Graph *g) {
    if (!pq || pq->size == 0) {
        printf("\n============================================================\n");
        printf("               EMERGENCY DISPATCH QUEUE: EMPTY              \n");
        printf("============================================================\n");
        printf("There are currently no pending emergency calls in queue.\n");
        return;
    }

    /* Create temporary copy of PriorityQueue to simulate dequeueing in order */
    PriorityQueue *tempPQ = createPriorityQueue(pq->size + 2);
    if (!tempPQ) return;

    for (int i = 0; i < pq->size; i++) {
        enqueueEmergency(tempPQ, pq->heap[i]);
    }

    printf("\n========================================================================================\n");
    printf("                  PENDING EMERGENCY DISPATCH QUEUE (PRIORITY ORDER)                     \n");
    printf("========================================================================================\n");
    printf("+-------+------+----------------------+----------------------+----------+-------------+-------------------------+\n");
    printf("| Order | ID   | Patient Name         | Incident Type        | Severity | Seq (FIFO)  | Patient Landmark        |\n");
    printf("+-------+------+----------------------+----------------------+----------+-------------+-------------------------+\n");

    int rank = 1;
    EmergencyRequest req;
    while (dequeueEmergency(tempPQ, &req)) {
        const char *locName = (g && req.landmarkNode >= 0 && req.landmarkNode < g->numVertices)
                              ? g->vertices[req.landmarkNode].name : "Unknown";

        printf("| %-5d | %-4d | %-20s | %-20s | %-8s | #%-10d | %-23s |\n",
               rank++,
               req.id,
               req.patientName,
               getEmergencyTypeStr(req.type),
               getSeverityStr(req.severity),
               req.sequenceNumber,
               locName);
    }
    printf("+-------+------+----------------------+----------------------+----------+-------------+-------------------------+\n");
    printf("Total Waiting in Queue: %d\n", pq->size);

    freePriorityQueue(tempPQ);
}

void freePriorityQueue(PriorityQueue *pq) {
    if (!pq) return;
    free(pq->heap);
    free(pq);
}
