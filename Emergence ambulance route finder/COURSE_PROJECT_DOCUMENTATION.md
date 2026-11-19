# Course-Based Project Documentation
## Emergency Ambulance Route Finder

### Course
Data Structures

### Application
Terminal-based C simulation for Narsingi and surrounding areas, Hyderabad, Telangana.

---

## 1. Abstract

Emergency Ambulance Route Finder is a terminal-based academic simulation that models emergency ambulance dispatch using Data Structures and Algorithms. The system accepts an emergency from a specific landmark, prioritizes requests according to severity, identifies an available ambulance, selects a suitable hospital, and calculates routes using Dijkstra's shortest-path algorithm.

The road network is represented as a weighted graph using an adjacency list. Emergency requests are maintained using a Binary Min-Heap Priority Queue. Structures, pointers, dynamic memory allocation, searching, sorting, and file handling are also used.

The project is designed as a C implementation and does not provide real-time emergency dispatch, GPS, live traffic, or live hospital information.

## 2. Problem Statement

During an emergency, selecting an ambulance and suitable hospital quickly can be difficult when several requests are active. A locality such as Narsingi is also too broad to represent a patient's exact position: treating the whole locality as one graph vertex can make a nearby hospital appear to have zero travel distance.

This project models specific landmarks and junctions such as DMart Narsingi, Narsingi X Roads, Narsingi Police Station, Narsingi Bus Stop, Narsingi Lake, Puppalaguda Junction, Kokapet Junction, Gachibowli Junction, and other surrounding locations. Hospitals are represented as separate graph vertices.

The system then uses a priority queue for emergency severity and Dijkstra's algorithm for shortest routes.

## 3. Objectives

1. Represent a road network as a weighted graph.
2. Implement Dijkstra's shortest-path algorithm.
3. Implement a Binary Min-Heap Priority Queue.
4. Process emergency requests by severity and FIFO order.
5. Find an available ambulance for an incident.
6. Find a suitable reachable hospital.
7. Display the ambulance-to-patient and patient-to-hospital routes.
8. Store system records using file handling.
9. Demonstrate searching and sorting algorithms.
10. Demonstrate C structures, pointers and dynamic memory.
11. Implement a clear MVC architecture in C.

## 4. Use Cases

### Use Case 1 — Report Emergency
The user registers a patient/caller, emergency type, severity, and specific landmark.

### Use Case 2 — Prioritize Emergency
The request enters a Binary Min-Heap Priority Queue. CRITICAL requests are processed before HIGH, MEDIUM and LOW requests. Equal priorities use FIFO order.

### Use Case 3 — Find Available Ambulance
The system compares available ambulance locations using graph shortest-path distance and selects the closest suitable unit.

### Use Case 4 — Find Suitable Hospital
The system evaluates emergency capability, ICU readiness, bed availability and hospital status before comparing route distance.

### Use Case 5 — Find Shortest Route
Dijkstra's algorithm calculates the minimum-distance route between two graph vertices.

### Additional Use Cases
- Assign ambulance
- Complete emergency
- Search records
- Sort records
- View emergency history
- View system statistics

## 5. Data Structures

| Data Structure | Usage |
|---|---|
| Weighted Graph | Road network |
| Adjacency List | Graph representation |
| Linked List | Adjacency-list edges |
| Binary Min-Heap | Emergency priority queue |
| Array / Stack-like path buffer | Dijkstra path reconstruction |
| Structures | Locations, ambulances, hospitals and emergencies |
| Dynamic arrays | Runtime storage |
| File records | Persistent datasets |

## 6. Algorithms

### Dijkstra's Algorithm

Input: source vertex and destination vertex.

Steps:
1. Set every distance to infinity.
2. Set source distance to zero.
3. Select the unvisited vertex with the smallest distance.
4. Relax all adjacent edges.
5. Store the predecessor of every improved vertex.
6. Repeat until the destination is reached or no reachable vertex remains.
7. Follow predecessor links backwards to reconstruct the route.
8. Reverse the route for display.

For the current implementation, minimum-distance vertex selection is performed linearly, giving approximately O(V^2 + E) time.

### Priority Queue
The emergency queue is a Binary Min-Heap.

Priority:
1. CRITICAL
2. HIGH
3. MEDIUM
4. LOW

If severity is equal, the earlier registration sequence is processed first.

- Enqueue: O(log n)
- Dequeue: O(log n)
- Peek: O(1)

### Searching
The project demonstrates linear search by emergency ticket, case-insensitive patient-name search, ambulance ID search, and landmark search.

### Sorting
The project demonstrates Selection Sort by severity, Bubble Sort by patient name, Insertion Sort by registration sequence, and Selection Sort by route distance.

## 7. MVC Architecture

### Model
- graph.c/.h
- dijkstra.c/.h
- priority_queue.c/.h
- ambulance.c/.h
- hospital.c/.h
- emergency.c/.h
- file_handler.c/.h
- search_sort.c/.h
- data/*.dat

### View
- ui.c/.h

The View provides menus, prompts, formatted tables, ASCII maps, route output, status messages and statistics.

### Controller
- controller.c/.h

The Controller coordinates the complete workflow between the View and Model: emergency registration, queue processing, ambulance selection, hospital selection, routing, completion, search/sort, and persistence.

### Entry Point
main.c only starts the controller:

    #include "controller.h"
    int main(void) {
        runApplication();
        return 0;
    }

## 8. Implementation

### Graph
The graph contains 32 vertices and 56 bidirectional simulated roads. Each road is stored as an adjacency-list edge with a distance weight.

### Landmark-Based Location
The system does not use Narsingi as one single patient vertex. It uses specific locations such as DMart Narsingi and Narsingi X Roads. Hospitals are separate nodes.

### Emergency Processing
A reported incident is stored in the emergency list and inserted into the priority queue. The next request is selected according to severity and registration sequence.

### Ambulance Dispatch
Dijkstra is run from available ambulance locations to the emergency landmark. The nearest suitable ambulance is selected.

### Hospital Selection
Hospitals are evaluated using simulated readiness information and route distance. The selected hospital is then reached through another Dijkstra calculation.

### File Handling
Location, road, hospital, ambulance and emergency datasets are persisted under data/. The program creates/seeds the datasets automatically when needed.

## 9. Example Terminal Demonstration

    REPORT NEW EMERGENCY INCIDENT
    Patient / Caller: Rahul
    Incident Type: Accident / Severe Trauma
    Severity: CRITICAL
    Patient Landmark: DMart Narsingi
    Emergency ticket successfully registered and queued.

    PROCESS NEXT EMERGENCY
    Processing: CRITICAL
    Patient Landmark: DMart Narsingi
    Locating Closest Available Ambulance...
    Selected Ambulance: AMB001
    Evaluating Hospital Suitability...
    Selected Hospital: PULSE Hospital
    Ambulance Dispatch Route: [calculated by Dijkstra]
    Patient Transport Route: [calculated by Dijkstra]
    Status: AMBULANCE ASSIGNED & EN ROUTE

All distances and ETAs shown by the application are simulated academic values.

## 10. Future Scope

1. GPS-based patient location.
2. Real road-network data.
3. Real-time traffic information.
4. Live ambulance tracking.
5. Real hospital bed and ICU availability.
6. Mobile application interface.
7. Database instead of flat data files.
8. Authentication for dispatch operators.
9. Dynamic road weights based on traffic.
10. Integration with official emergency services.

## 11. Learning / New Things

Through this project, the team learns:
- How graphs model real-world road networks.
- How adjacency lists represent sparse graphs.
- How Dijkstra's algorithm finds shortest paths.
- How priority queues can model emergency triage.
- How structures organize related data in C.
- How pointers and dynamic memory support linked structures.
- How searching and sorting algorithms are applied to application data.
- How file handling provides persistent storage.
- How a C program can be separated into Model, View and Controller responsibilities.
- How multiple algorithms can work together in a single application.

## 12. Academic Disclaimer

This is an academic simulation. Road distances, ambulance positions, hospital capacity, hospital status, ICU availability and travel times are simulated project data. The program must not be used as a real emergency dispatch system.