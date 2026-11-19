# EMERGENCY AMBULANCE ROUTE FINDER
### Simulation System for Narsingi & Surrounding Areas (Hyderabad, Telangana)
**A Terminal-Based Data Structures & Algorithms Course Project in Standard C**

---

## ⚠️ ACADEMIC SIMULATION DISCLAIMER
> **IMPORTANT NOTICE:**  
> This project is an **academic simulation** developed solely for demonstrating Data Structures and Algorithms (DSA) in standard C. All road distances, hospital availability metrics, ICU statuses, bed counts, ambulance positioning, and estimated travel times are **simulated demo parameters** and **do NOT represent real-time emergency dispatch or live hospital data**. This software must not be used as an actual emergency dispatch system.

---

## 1. PROBLEM STATEMENT
During medical emergencies, selecting a suitable ambulance and hospital quickly is critical. In a geographically distributed area such as Narsingi and its surrounding localities, patients may report emergencies from specific landmarks such as shopping centers, junctions, bus stops, residential communities, and public locations. A system is required to identify the emergency location, prioritize emergency requests, locate an available ambulance, determine a suitable hospital, and calculate an efficient route between locations.

The **Emergency Ambulance Route Finder** addresses this problem by representing the road network as a weighted graph and using **Dijkstra's shortest path algorithm** to calculate routes. A **manual priority queue** is used to process emergencies according to clinical severity with FIFO tie-breaking, while **structures, searching, sorting, dynamic memory allocation, and persistent file handling** are used for managing system data.

---

## 2. PROJECT OBJECTIVES
1. **Represent a real-world road network using a weighted graph** with an adjacency list, capturing landmarks, junctions, residential colonies, and hospitals across Narsingi, Kokapet, Puppalaguda, Manikonda, Gandipet, Gachibowli, and Shamshabad.
2. **Implement Dijkstra's shortest path algorithm manually** without external libraries, using dynamic distance, visited, and parent tracking arrays to reconstruct and display complete step-by-step route turnarounds.
3. **Implement priority-based emergency processing** using a manual Binary Min-Heap Priority Queue adhering to clinical triage levels (`CRITICAL` > `HIGH` > `MEDIUM` > `LOW`) with strict FIFO tie-breaking for equal severities.
4. **Find the nearest available ambulance** by dynamically running Dijkstra from all idle fleet units to the incident landmark.
5. **Find a suitable hospital** using a multi-criteria clinical suitability scoring algorithm factoring in emergency facilities, ICU readiness, bed capacity, open/closed status, and road distance.
6. **Display complete shortest routes** with intermediate landmark turnarounds, road distances in kilometers, and estimated transit times.
7. **Store emergency and system state using persistent file handling** (`.dat` files) with automatic creation and seeding of default datasets.
8. **Demonstrate classical searching and sorting algorithms** (Bubble Sort, Selection Sort, Insertion Sort, and substring-matching Linear Search).
9. **Apply advanced C programming fundamentals**: dynamic memory allocation (`malloc`, `calloc`, `realloc`, `free`), pointers, composite structures, modular compilation, and robust input validation.
10. **Build an interactive, crash-proof menu-driven terminal interface** with ASCII network maps, clear tables, and operational statistics.

---

## 3. GEOGRAPHIC AREA & GRAPH TOPOLOGY
The road network models **Narsingi** as the central hub and extends into adjacent corridors of Western and Southern Hyderabad:
- **Central Narsingi Core:** DMart Narsingi, Narsingi X Roads, Narsingi Police Station, Narsingi Bus Stop, Narsingi Lake, Ratnadeep Supermarket, MN Circle.
- **Kokapet & Gandipet Corridor:** Rajapushpa Regalia, Kokapet Junction, Kokapet Golden Mile, Gandipet Lake, CBIT Gandipet.
- **Puppalaguda & Manikonda Corridor:** Puppalaguda Junction, Manikonda Main Road, Neknampur Junction, Alkapur Township.
- **IT & Financial District Corridor:** Nanakramguda Circle, Gachibowli Junction.
- **Southern Corridors:** Manchirevula Junction, Manchirevula Bridge, Rajendranagar (Agri Univ), Kismatpur Junction, Mehdipatnam, Shamshabad Airport Road.
- **Dedicated Hospital Nodes:** PULSE Hospital, Delight Hospital, Spring Field Hospital, DVR Hospital & Diagnostics, Jasmin Multispeciality Hospital, Narsingi Emergency Care Center, Kokapet Emergency Hospital, Gachibowli Emergency Hospital.

**Network Size:** 32 Graph Vertices, 56 Bidirectional Roads (112 Directed Edges).

---

## 4. DATA STRUCTURES IMPLEMENTED

| Data Structure | Implementation Details | Purpose in Project |
| :--- | :--- | :--- |
| **Weighted Graph** | Adjacency List using dynamically allocated vertices array and linked lists of `Edge` structures (`destination`, `distance`, `next`). | Models the road network connections and simulated road distances between landmarks. |
| **Priority Queue** | Manual Binary Min-Heap with dynamic array reallocation (`EmergencyRequest* heap`). | Prioritizes emergency calls (`CRITICAL` > `HIGH` > `MEDIUM` > `LOW`) with FIFO tie-breaking. |
| **Stack / Path Array** | Array and stack-based backtracking using `parent[]` pointers. | Reconstructs the turn-by-turn path from destination back to source in Dijkstra's algorithm. |
| **Composite Structures** | `Vertex`, `Edge`, `Graph`, `Hospital`, `Ambulance`, `EmergencyRequest`, `ShortestRoute`. | Encapsulates system entities, medical metadata, and dispatch state cleanly. |
| **Dynamic Memory** | `malloc()`, `calloc()`, `realloc()`, `free()`. | Dynamic graph resizing, heap resizing, temporary sorting buffers, and clean memory deallocation on exit. |

---

## 5. ALGORITHMS IMPLEMENTED

### A. Dijkstra's Shortest Path Algorithm
- **Time Complexity:** $O(V^2 + E)$ using adjacency lists and linear minimum distance vertex selection.
- **Procedure:**
  1. Allocate and initialize distance array `dist[]` to $\infty$ (`1e9`), `visited[]` to 0, and `parent[]` to -1.
  2. Set `dist[source] = 0.0`.
  3. In each iteration, select the unvisited vertex $u$ with minimum tentative distance.
  4. Terminate early if $u = \text{destination}$ or if no reachable unvisited vertices remain.
  5. Mark $u$ as visited.
  6. For each adjacent road edge $(u, v)$ with weight $w$:
     $$\text{If } \text{dist}[u] + w < \text{dist}[v] \implies \text{dist}[v] = \text{dist}[u] + w, \quad \text{parent}[v] = u$$
  7. Trace the parent chain from `destination` to `source` using a temporary stack, invert into the forward path array, and calculate simulated travel time ($40 \text{ km/h}$ simulated city emergency speed).

### B. Manual Binary Min-Heap Priority Queue
- **Operations:** `enqueueEmergency()` $O(\log N)$, `dequeueEmergency()` $O(\log N)$, `peekEmergency()` $O(1)$.
- **Comparator (`isHigherPriority`):**
  - Clinical Severity Priority: $\text{CRITICAL (1)} > \text{HIGH (2)} > \text{MEDIUM (3)} > \text{LOW (4)}$.
  - FIFO Tie-Breaking: When $\text{severity}_A = \text{severity}_B$, the item with earlier registration ($\text{sequenceNumber}_A < \text{sequenceNumber}_B$) takes precedence.
- Bubble-up (heapify-up) and Bubble-down (heapify-down) ensure complete adherence to heap properties without external libraries.

### C. Classical Sorting Algorithms
1. **Selection Sort:**
   - Used for sorting emergency records by **Clinical Severity** (Critical $\to$ Low).
   - Used for sorting records by **Total Route Distance** (Shortest $\to$ Longest).
2. **Bubble Sort:**
   - Used for sorting emergency records alphabetically by **Patient Name** ($A \to Z$) with early-termination flag.
3. **Insertion Sort:**
   - Used for sorting emergency records chronologically by **Registration Sequence / Time**.

### D. Searching Algorithms
- **Linear Search:** ID lookup for emergency tickets and ambulance units.
- **Case-Insensitive Substring Search:** Querying patient names and landmark names across the graph.

---

## 6. SYSTEM ARCHITECTURE & WORKFLOW
```
                          +-------------------------+
                          |   Patient / Emergency   |
                          |      Call Received      |
                          +------------+------------+
                                       |
                                       v
                          +-------------------------+
                          |  Select Landmark Node   |
                          |  & Medical Severity     |
                          +------------+------------+
                                       |
                                       v
                          +-------------------------+
                          | Enqueue in Priority Q   |
                          | (Severity + FIFO order) |
                          +------------+------------+
                                       |
                                       v
                          +-------------------------+
                          |  Process Next Incident  |
                          | (Dequeue Root of Heap)  |
                          +------------+------------+
                                       |
                   +-------------------+-------------------+
                   |                                       |
                   v                                       v
      +-------------------------+             +-------------------------+
      | Find Nearest Available  |             |  Evaluate Suitable Hosp |
      |  Ambulance via Dijkstra |             |  (ICU, Beds, Facility)  |
      +------------+------------+             +------------+------------+
                   |                                       |
                   v                                       v
      +-------------------------+             +-------------------------+
      |  Compute Dijkstra Route |             |  Compute Dijkstra Route |
      |  Ambulance -> Patient   |             |    Patient -> Hospital  |
      +------------+------------+             +------------+------------+
                   |                                       |
                   +-------------------+-------------------+
                                       |
                                       v
                          +-------------------------+
                          |  Assign Ambulance Unit  |
                          |  (Update Status: BUSY)  |
                          +------------+------------+
                                       |
                                       v
                          +-------------------------+
                          | Output Turnaround Route |
                          | & Transit Time to Driver|
                          +------------+------------+
                                       |
                                       v
                          +-------------------------+
                          | Save State to File      |
                          | (Persistent .dat Store) |
                          +-------------------------+
```

---

## 7. MODULAR FILE STRUCTURE
```
Emergency-Ambulance-Route-Finder/
│
├── main.c              # Main application entry point, menu loop, and workflow orchestration
│
├── graph.h             # Vertex, Edge, and Graph data structure declarations
├── graph.c             # Adjacency list creation, dynamic expansion, edge insertion, display
│
├── dijkstra.h          # ShortestRoute structure and routing prototypes
├── dijkstra.c          # Manual Dijkstra algorithm and route turnaround printer
│
├── priority_queue.h    # Binary Min-Heap Priority Queue declarations
├── priority_queue.c    # Manual heapify-up, heapify-down, enqueue, dequeue, peek, and display
│
├── ambulance.h         # Ambulance structure, fleet list, and dispatch prototypes
├── ambulance.c         # Fleet management, nearest ambulance selector, and restock logic
│
├── hospital.h          # Hospital facility structure, status, and suitability prototypes
├── hospital.c          # Multi-criteria hospital evaluator and bed tracker
│
├── emergency.h         # Incident request model, severity enum, and triage tables
├── emergency.c         # Incident records management and formatted table displays
│
├── search_sort.h       # Search and sort function prototypes
├── search_sort.c       # Selection Sort, Bubble Sort, Insertion Sort, and substring search
│
├── file_handler.h      # File persistence prototypes and dataset paths
├── file_handler.c      # Auto-creation of data/ directory, .dat load/save, and seed dataset
│
├── ui.h                # Terminal interface, headers, input validators, and statistics
├── ui.c                # ASCII topology map, safe input readers, and analytics dashboard
│
├── data/               # Persistent file storage (auto-generated if missing)
│   ├── locations.dat   # 32 landmark and hospital vertices
│   ├── roads.dat       # 56 bidirectional road edges with simulated distances
│   ├── hospitals.dat   # Hospital directory, ICU status, bed counts, selection metrics
│   ├── ambulances.dat  # Ambulance fleet positioning, statuses, and cumulative distance
│   └── emergencies.dat # Incident history audit log
│
├── Makefile            # Standard GCC compilation build script
└── README.md           # Project documentation, DSA guide, and viva Q&A
```

---

## 8. COMPILATION & EXECUTION INSTRUCTIONS

### Prerequisites
- GCC Compiler (MinGW on Windows, GCC on Linux/macOS).
- C99 Standard support (`-std=c99`).

### Compilation Command (Direct GCC)
```bash
gcc -Wall -Wextra -std=c99 main.c graph.c dijkstra.c priority_queue.c ambulance.c hospital.c emergency.c file_handler.c search_sort.c ui.c -o ambulance_system -lm
```

### Using Makefile
```bash
# Compile
make

# Run
make run

# Clean build artifacts
make clean
```

### Running the Executable
**On Windows:**
```cmd
ambulance_system.exe
```

**On Linux / macOS:**
```bash
./ambulance_system
```

---

## 9. SAMPLE RUN & EXPECTED OUTPUTS

### A. Priority Queue Inspection (Menu Option 7)
Demonstrates clinical prioritization and FIFO tie-breaking:
```text
========================================================================================
                  PENDING EMERGENCY DISPATCH QUEUE (PRIORITY ORDER)                     
========================================================================================
+-------+------+----------------------+----------------------+----------+-------------+-------------------------+
| Order | ID   | Patient Name         | Incident Type        | Severity | Seq (FIFO)  | Patient Landmark        |
+-------+------+----------------------+----------------------+----------+-------------+-------------------------+
| 1     | 1001 | Ramesh Rao           | Chest Pain / Cardiac | CRITICAL | #1          | DMart Narsingi          |
| 2     | 1004 | Ananya Sharma        | Breathing Distress   | CRITICAL | #4          | Manikonda Main Road     |
| 3     | 1003 | Vikram Kumar         | Accident / Trauma    | HIGH     | #3          | Puppalaguda Junction    |
| 4     | 1002 | Sneha Reddy          | Physical Injury      | LOW      | #2          | Rajapushpa Regalia      |
+-------+------+----------------------+----------------------+----------+-------------+-------------------------+
Total Waiting in Queue: 4
```

### B. Automated Emergency Dispatch Pipeline (Menu Option 8)
Processing Ticket #1001:
```text
========================================================================================
                       PROCESS NEXT EMERGENCY - DISPATCH PIPELINE
========================================================================================

[Step 1/4] Processing Highest Priority Call:
  Incident Ticket     : #1001
  Patient Name        : Ramesh Rao
  Incident Severity   : CRITICAL (Priority 1)
  Patient Landmark    : DMart Narsingi [Narsingi]
  Registration FIFO   : #1

[Step 2/4] Locating Closest Available Ambulance Fleet Unit...
  Found Nearest Unit  : AMB001 (TS-09-EM-1001)
  Equipment Standard  : ALS (Advanced Life Support)
  Current Station     : Narsingi X Roads
  Calculated Distance : 1.20 km

[Step 3/4] Evaluating Hospital Suitability via Dijkstra Algorithm...
  Selected Facility   : PULSE Hospital
  Decision Basis      : Nearest suitable facility (1.80 km) with 24/7 Emergency Care & ICU capacity (ICU Available).

[Step 4/4] DISPATCH SUMMARY & NAVIGATION ROUTES
========================================================================================
  AMBULANCE DISPATCH ROUTE (Unit AMB001 -> Patient):

============================================================
                  CALCULATED SHORTEST ROUTE                 
============================================================
Source Location      : Narsingi X Roads [Narsingi]
Destination Location : DMart Narsingi [Narsingi]
------------------------------------------------------------
Route Progression:

  [01] Narsingi X Roads (Junction)
         | (1.20 km)
         v
  [00] DMart Narsingi (Landmark)
------------------------------------------------------------
Total Route Distance : 1.20 km (Simulated Road Distance)
Estimated Transit    : 1.8 minutes (~2 min)
============================================================

  PATIENT TRANSPORT ROUTE (Patient -> Hospital PULSE Hospital):

============================================================
                  CALCULATED SHORTEST ROUTE                 
============================================================
Source Location      : DMart Narsingi [Narsingi]
Destination Location : PULSE Hospital [Narsingi]
------------------------------------------------------------
Route Progression:

  [00] DMart Narsingi (Landmark)
         | (1.20 km)
         v
  [01] Narsingi X Roads (Junction)
         | (0.60 km)
         v
  [24] PULSE Hospital (Hospital)
------------------------------------------------------------
Total Route Distance : 1.80 km (Simulated Road Distance)
Estimated Transit    : 2.7 minutes (~3 min)
============================================================

============================================================
                  OVERALL MISSION METRICS                   
============================================================
  Assigned Ambulance    : AMB001 (ALS (Advanced Life Support))
  Pickup Location       : DMart Narsingi
  Destination Hospital  : PULSE Hospital
  Ambulance -> Patient  : 1.20 km (ETA ~1.8 mins)
  Patient -> Hospital   : 1.80 km (ETA ~2.7 mins)
  Total Mission Road    : 3.00 km
  Combined Transit Time : ~4.5 minutes
  Status Indicator      : [✓] AMBULANCE ASSIGNED & EN ROUTE
========================================================================================
```

---

## 10. SYSTEM LIMITATIONS & FUTURE ENHANCEMENTS

### Academic Limitations
1. **Simulated Weights:** Road distances and transit velocities are static academic simulations rather than live telemetry.
2. **Deterministic Traffic:** Assumes a standard emergency clearance speed without time-of-day dynamic traffic congestion multipliers.
3. **Single Dispatch Assignment:** Ambulances execute one mission at a time before returning to availability.

### Future Enhancements
1. **Time-Dependent Dynamic Weights:** Ingesting simulated peak-hour traffic multipliers (e.g. morning/evening rush hour on Narsingi X Roads).
2. **Multi-Stop Patient Transfer:** Optimization for multi-patient transfer routing during disaster triage scenarios.
3. **A* Search Algorithm:** Adding coordinate-based Euclidean heuristics ($h(n)$) alongside Dijkstra's algorithm.

---

## 11. VIVA VOCE QUESTIONS & ANSWERS (PROJECT DEFENSE GUIDE)

### Q1: Why did you choose an Adjacency List instead of an Adjacency Matrix for representing the road network?
**Answer:**  
Road networks in geographical regions are inherently **sparse graphs**. In our project, there are 32 vertices and 56 undirected roads (average vertex degree $\approx 3.5$).
- An **Adjacency Matrix** requires $O(V^2)$ memory ($32 \times 32 = 1024$ entries), where most entries are zero/infinity.
- An **Adjacency List** consumes only $O(V + E)$ memory, dynamically allocating memory only for connected roads.
- Furthermore, iterating over the neighbors of a vertex during edge relaxation in Dijkstra takes $O(\text{degree}(u))$ in an adjacency list rather than $O(V)$ in a matrix, resulting in superior performance.

### Q2: How does Dijkstra's Algorithm guarantee finding the shortest path?
**Answer:**  
Dijkstra's algorithm operates on a **greedy approach** with the invariant that whenever an unvisited vertex $u$ with the minimum tentative distance is settled (marked visited), its shortest path distance from the source is finalized and optimal. Because all edge weights in our road network represent physical distances ($w(u, v) \ge 0$, no negative edge weights), adding any further edges cannot reduce the distance to an already settled node. Edge relaxation systematically updates neighboring distances:
$$\text{If } \text{dist}[u] + w(u, v) < \text{dist}[v] \implies \text{dist}[v] = \text{dist}[u] + w(u, v)$$

### Q3: How do you reconstruct the actual route sequence from Dijkstra's algorithm?
**Answer:**  
We maintain a `parent[]` array of size $V$, initialized to $-1$. Whenever an edge $(u, v)$ is relaxed, we record `parent[v] = u`. Once the algorithm finishes or settles the destination, we trace backwards starting from `destination` to `source`:
$$\text{destination} \to \text{parent}[\text{destination}] \to \dots \to \text{source}$$
Because this produces the path in reverse order, we push each node onto a temporary stack or array, and then pop/invert it to display the route in forward sequence: $\text{Source} \to \dots \to \text{Destination}$.

### Q4: How is the Priority Queue implemented, and how does it handle equal priorities?
**Answer:**  
The priority queue is implemented manually as a **Binary Min-Heap** stored in a contiguous dynamic array.
- **Priority Logic:** Clinical severity is enumerated numerically: $\text{CRITICAL} = 1$, $\text{HIGH} = 2$, $\text{MEDIUM} = 3$, $\text{LOW} = 4$. Lower numerical value indicates higher medical priority.
- **FIFO Tie-Breaking:** Every emergency request is tagged with an auto-incrementing `sequenceNumber` upon arrival. When two emergencies have the same severity ($\text{sev}_A == \text{sev}_B$), the comparator checks:
  $$\text{sequenceNumber}_A < \text{sequenceNumber}_B$$
  The earlier registered call is treated as higher priority. This satisfies strict FIFO order among identical triage severities.

### Q5: How does the system select the best hospital instead of blindly picking the nearest one?
**Answer:**  
Blindly choosing the nearest hospital can be fatal if the hospital has no ICU or is closed. Our `selectBestHospital()` function computes a multi-factor suitability score:
1. **Hard Constraints:** The hospital must have status `HOSP_OPEN` and `availableBeds > 0`.
2. **Clinical Severity Matching:** If the patient's condition is `CRITICAL`, the hospital **must** possess an active 24/7 Emergency Department (`emergencyFacility == 1`) and preferably an open ICU (`icuAvailability == 1`).
3. **Shortest Road Distance:** Among all eligible facilities satisfying the medical constraints, Dijkstra's algorithm calculates the road distance from the patient's landmark to the hospital, and the facility with minimal road distance is selected.

### Q6: Where is Dynamic Memory Allocation used in the codebase, and how are leaks prevented?
**Answer:**  
Dynamic memory allocation is used purposefully:
1. `createGraph()`: Uses `calloc` for the `Vertex` array and `malloc` for each `Edge` in the adjacency list. Dynamic resizing with `realloc` is supported in `addLocation()`.
2. `createPriorityQueue()`: Dynamically allocates the heap array and doubles capacity via `realloc` when the queue expands.
3. `dijkstraShortestPath()`: Dynamically allocates the `dist`, `visited`, and `parent` tracking arrays and deallocates (`free`) them immediately before returning the route.
4. `freeGraph()`, `freeHospitalList()`, `freeAmbulanceList()`, `freeEmergencyList()`, and `freePriorityQueue()` are explicitly invoked upon system shutdown (Option 18), ensuring all heap allocations are safely freed.

### Q7: Why did you choose formatted text files instead of binary struct dumps (`fwrite`) for persistent storage?
**Answer:**  
1. **Cross-Platform Portability:** Using binary dumps with `fwrite(&struct, sizeof(struct), ...)` is sensitive to compiler structure padding, byte alignment, and endianness differences between 32-bit and 64-bit systems.
2. **Transparency and Verifiability:** Formatted text files (`.dat` with delimiter tokens) allow instructors, evaluators, and testers to inspect and verify graph topologies and emergency audit logs using standard text editors.
3. **Resilience:** Parsing with `fgets` and string tokenization avoids binary buffer overruns.

### Q8: What sorting algorithms did you implement and why?
**Answer:**  
We implemented three classical course algorithms in `search_sort.c`:
1. **Selection Sort:** Used for sorting by Clinical Severity and by Route Distance. Demonstrates selection of minimum element per pass ($O(N^2)$).
2. **Bubble Sort:** Used for sorting patient names alphabetically ($A \to Z$). Includes an optimized early-termination flag `swapped`.
3. **Insertion Sort:** Used for sorting requests chronologically by registration sequence. Demonstrates adaptive insertion into a sorted sub-array.

---

## 12. LICENSE & ACADEMIC INTEGRITY
Developed as an academic course project for Data Structures and Algorithms in Standard C. All source code is modular, fully commented, GCC-compatible, and adheres to academic standards.


---

## 7. COURSE USE CASES

The application contains substantially more than the required five use cases:

1. **Report Emergency** — register a patient incident from a specific landmark such as DMart Narsingi.
2. **Prioritize Emergency** — place the incident into a Binary Min-Heap Priority Queue based on severity with FIFO tie-breaking.
3. **Find Available Ambulance** — compare available ambulances using Dijkstra distance to the incident.
4. **Find Suitable Hospital** — evaluate hospital readiness and choose a suitable reachable facility.
5. **Find Shortest Route** — calculate and display a shortest route using Dijkstra's algorithm.
6. **Assign Ambulance** — dispatch an available unit and update its status.
7. **Complete Emergency** — mark the mission completed and return the ambulance to service.
8. **Search Records** — search emergencies, ambulances, and landmarks.
9. **Sort Records** — demonstrate Selection, Bubble, and Insertion Sort.
10. **View Statistics** — display emergency, fleet, hospital, and routing statistics.

### Recommended Review-1 demonstration

For Review 1, demonstrate this complete scenario:

```
Patient reports emergency
        ↓
Select "DMart Narsingi"
        ↓
Select "CRITICAL"
        ↓
Priority Queue
        ↓
Find nearest available ambulance
        ↓
Dijkstra: Ambulance → DMart Narsingi
        ↓
Evaluate suitable hospital
        ↓
Dijkstra: DMart Narsingi → Hospital
        ↓
Display both routes + distance + ETA
```

This demonstrates the project's main Data Structures and Algorithms in one workflow.


---

## 8. LANDMARK-BASED LOCATION MODEL

A locality such as **Narsingi is not treated as one single graph vertex**. This avoids the unrealistic situation in which a patient reports an emergency "in Narsingi" and a hospital located in Narsingi appears to be at zero distance.

Instead, the graph uses specific landmarks and junctions, including:

- DMart Narsingi
- Narsingi X Roads
- Narsingi Police Station
- Narsingi Bus Stop
- Narsingi Lake
- Ratnadeep Supermarket
- MN Circle
- Rajapushpa Regalia
- Alkapur Township
- Puppalaguda Junction
- Manikonda Main Road
- Neknampur Junction
- Manchirevula Junction
- Kokapet Junction
- Kokapet Golden Mile
- Gandipet Lake
- CBIT Gandipet
- Nanakramguda Circle
- Gachibowli Junction
- Mehdipatnam Rythu Bazar
- Rajendranagar Agri Univ
- Kismatpur Junction
- Shamshabad Airport Road

Hospitals are separate graph vertices. Therefore a report such as **"Emergency at DMart Narsingi"** has a real graph source node and must travel through road edges before reaching a hospital node.

All distances remain **simulated academic project weights**, not live road distances.


---

## 6. MVC ARCHITECTURE (COURSE REQUIREMENT)

The project follows an explicit **Model-View-Controller (MVC)** architecture while remaining a pure terminal-based C application.

### Model
The Model layer contains the application's data structures, algorithms, and persistent data:

- `graph.c / graph.h` — weighted graph and adjacency-list road network
- `dijkstra.c / dijkstra.h` — shortest-path model and route calculation
- `priority_queue.c / priority_queue.h` — emergency priority queue
- `ambulance.c / ambulance.h` — ambulance fleet model
- `hospital.c / hospital.h` — hospital model and suitability logic
- `emergency.c / emergency.h` — emergency records and statuses
- `file_handler.c / file_handler.h` — persistent data storage
- `search_sort.c / search_sort.h` — searching and sorting operations
- `data/*.dat` — persistent simulated datasets

### View
The View layer is implemented by:

- `ui.c / ui.h`

It is responsible for:

- Terminal menus
- Input validation and prompts
- Tables and formatted output
- ASCII road-network map
- Success, warning, and error messages
- Route display and system statistics

### Controller
The Controller layer is implemented by:

- `controller.c / controller.h`

It coordinates user actions between the View and Model. It handles:

- Emergency registration
- Priority-queue processing
- Ambulance selection
- Hospital selection
- Dijkstra route calculation
- Emergency completion
- Searching
- Sorting
- File persistence
- Main application lifecycle

### Entry Point
`main.c` is intentionally kept as a thin entry point:

```c
#include "controller.h"

int main(void) {
    runApplication();
    return 0;
}
```

This makes the MVC separation visible and easy to explain during Review 1 and the final viva.

### MVC Flow

```
+-------------------+
|       VIEW        |
|     ui.c / ui.h   |
+---------+---------+
          |
          v
+-------------------+
|    CONTROLLER     |
| controller.c/.h   |
+---------+---------+
          |
          v
+-------------------+
|      MODEL        |
| Graph / Dijkstra  |
| PQ / Ambulance    |
| Hospital / Files  |
+-------------------+
```

The Controller never directly draws the complete interface; it requests input/output through the View functions and invokes the Model operations for data and algorithms.
