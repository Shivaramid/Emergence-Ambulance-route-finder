#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "emergency.h"

/* Manual Binary Min-Heap Priority Queue for Emergency Requests */
typedef struct PriorityQueue {
    EmergencyRequest *heap;    /* Dynamically allocated array storing the heap elements */
    int size;                  /* Number of items currently queued */
    int capacity;              /* Allocated memory capacity */
} PriorityQueue;

/* Function prototypes */
PriorityQueue* createPriorityQueue(int initialCapacity);
int isHigherPriority(const EmergencyRequest *a, const EmergencyRequest *b);
int enqueueEmergency(PriorityQueue *pq, EmergencyRequest req);
int dequeueEmergency(PriorityQueue *pq, EmergencyRequest *outReq);
int peekEmergency(const PriorityQueue *pq, EmergencyRequest *outReq);
int isPQEmpty(const PriorityQueue *pq);
int getPQSize(const PriorityQueue *pq);
void displayPriorityQueue(const PriorityQueue *pq, const Graph *g);
void freePriorityQueue(PriorityQueue *pq);

#endif /* PRIORITY_QUEUE_H */
