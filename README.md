ChillGrid – Downtown Cooling Energy Simulation System


ITM Skills University | Problem 89
An interactive, menu-driven C++ simulation system for managing downtown district cooling infrastructure.




2.1 Project Title

ChillGrid – Downtown Cooling Energy Simulation System


2.2 Problem Statement

Modern downtown districts rely on centralized chilled-water networks to cool multiple buildings simultaneously. Managing such a network requires tracking coolant velocity across pipe loops, enforcing and rolling back pressure overrides on compressors, queuing buildings for cooling restart after outages, cataloguing physical pipe parts by serial number, prioritizing high-heat junctions, finding energy-efficient routing paths through the pipe network, and allocating electrical power to cooling motors on demand.

Without a unified system, operators must manage these tasks manually, leading to errors, inefficiencies, and delayed response during faults. ChillGrid addresses this by consolidating all these operations into a single interactive C++ application backed by carefully chosen data structures.


2.3 Objectives


Record and monitor coolant velocity readings per pipe loop with a rolling history window.
Apply, log, and rollback compressor pressure override actions.
Maintain a FIFO restart queue for buildings awaiting cooling restoration.
Provide O(1) average-case lookup of pipe parts by serial number.
Sort cooling junctions by heat draw using an in-place Merge Sort.
Model the pipe network as a weighted undirected graph and compute the minimum-friction path between any two junctions using Dijkstra's algorithm.
Allocate available electrical power to cooling motors in highest-demand-first order using a max-heap priority queue.



2.4 System Overview / Architecture

┌─────────────────────────────────────────────────────────────┐
│                        main()                               │
│              Interactive Menu (choices 0–21)                │
└────────────┬────────────────────────────────────────────────┘
             │ delegates to
   ┌──────────▼──────────────────────────────────────────┐
   │              Seven Subsystem Classes                 │
   ├──────────────────────┬──────────────────────────────┤
   │  DataRegistry        │  deque<double> per loop      │
   │  PressureOverride    │  stack<PressureAction>        │
   │  RestartQueue        │  queue<string>                │
   │  SerialCodeLookup    │  unordered_map<string,Part>   │
   │  JunctionSorter      │  vector<Junction> + MergeSort │
   │  TopologyAndRouting  │  adj-list graph + Dijkstra    │
   │  ElectricalAllocator │  priority_queue<CoolingMotor> │
   └──────────────────────┴──────────────────────────────┘

Each class is self-contained with its own data store and public interface. main() instantiates one object per class and routes user input to the appropriate method.


2.5 Data Structures and Algorithms Used

#SubsystemData StructureAlgorithm / Operation1DataRegistryunordered_map + dequeSliding-window velocity history (max 10 readings); O(1) push/pop2PressureOverridestackLIFO action log; rollback in O(1)3RestartQueuequeueFIFO building restart; enqueue/dequeue O(1)4SerialCodeLookupunordered_mapHash-based part lookup; O(1) average5JunctionSortervectorMerge Sort (descending heat draw); O(n log n)6TopologyAndRoutingAdjacency list (unordered_map<string, vector<Edge>>)Dijkstra's shortest path; O((V+E) log V)7ElectricalAllocatorpriority_queue (max-heap)Greedy highest-demand-first allocation; O(log n) push/pop


2.6 Implementation Approach

DataRegistry

A deque<double> is maintained per loop ID inside an unordered_map. When the deque reaches 10 entries the oldest is evicted (pop_front) before appending the new reading. This gives a constant-size sliding window with O(1) amortised operations.

PressureOverride

Every applyAction() call pushes a PressureAction struct onto a stack. rollback() pops the top element, naturally reversing the most recent action. The full log is inspected by copying the stack into a temporary.

RestartQueue

A plain std::queue<string> provides strict FIFO ordering. Buildings are enqueued by name and processed (dequeued) one at a time, mirroring real-world restart sequencing.

SerialCodeLookup

PipePart structs are stored in an unordered_map keyed by serial number, yielding expected O(1) insert and lookup without any sorting requirement.

JunctionSorter

Junctions are stored in a vector<Junction> and sorted in-place using a custom Merge Sort (descending heatDraw). Merge Sort was chosen for its guaranteed O(n log n) worst-case and stable ordering.

TopologyAndRouting

The pipe network is an undirected weighted graph stored as an adjacency list. Pipes are added bidirectionally. Dijkstra's algorithm with a min-heap (priority_queue with greater<>) finds the minimum-friction-energy path between any two junctions.

ElectricalAllocator

CoolingMotor objects implement operator< (by demand) and are stored in a std::priority_queue (max-heap). allocate() always serves the highest-demand motor first, capping allocation at available power.


2.7 Time and Space Complexity Analysis

SubsystemOperationTime ComplexitySpace ComplexityDataRegistryRecord velocityO(1) amortisedO(L × W) — L loops, W=10 windowDataRegistryShow / ListO(W) / O(L)O(1) extraPressureOverrideApply / RollbackO(1)O(A) — A actionsRestartQueueEnqueue / DequeueO(1)O(B) — B buildingsSerialCodeLookupAdd / FindO(1) avgO(P) — P partsJunctionSorterSortO(n log n)O(n) merge bufferTopologyAndRoutingAdd pipeO(1)O(V + E)TopologyAndRoutingDijkstraO((V+E) log V)O(V) dist + prev mapsElectricalAllocatorAdd motorO(log n)O(M) — M motorsElectricalAllocatorAllocateO(log n)O(1)


2.8 Execution Steps

Prerequisites


A C++17-compatible compiler: g++ 9+, clang++ 10+, or MSVC 2019+.
No external libraries — standard library only.


Compile

bashg++ -std=c++17 -O2 -o chillgrid chillgrid.cpp

Run

bash./chillgrid          # Linux / macOS
chillgrid.exe        # Windows

Navigate the Menu

The program presents a numbered menu on every iteration. Enter the number corresponding to the desired operation and press Enter. Follow the on-screen prompts to supply required values. Enter 0 to exit.


2.9 Sample Inputs and Outputs

Record & View Loop Velocity (Options 1 → 2)

Enter choice: 1
  Enter Loop ID: LOOP-A
  Enter velocity (m/s): 3.5
  [OK] Velocity 3.5 m/s recorded for loop LOOP-A

Enter choice: 2
  Enter Loop ID: LOOP-A
  Loop LOOP-A -> Live: 3.5 m/s | History: 3.5 | Avg: 3.5 m/s

Apply & Rollback Pressure (Options 4 → 6 → 5)

Enter choice: 4
  Compressor ID: COMP-01
  Action type: INCREASE
  Pressure (bar): 12.5
  Timestamp: 2024-06-01T08:00
  [OK] INCREASE applied on COMP-01 -> 12.5 bar

Enter choice: 6
  1. COMP-01 | INCREASE | 12.5 bar | 2024-06-01T08:00

Enter choice: 5
  [ROLLED BACK] INCREASE on COMP-01 (12.5 bar)

Find Minimum Friction Path (Options 15 → 17)

Enter choice: 15
  From Junction ID: J1
  To Junction ID: J2
  Friction energy (kJ): 4.0
  [OK] Pipe added: J1 <-> J2 (friction: 4.0 kJ)

Enter choice: 15
  From Junction ID: J1
  To Junction ID: J3
  Friction energy (kJ): 1.5
  [OK] Pipe added: J1 <-> J3 (friction: 1.5 kJ)

Enter choice: 15
  From Junction ID: J3
  To Junction ID: J2
  Friction energy (kJ): 1.0
  [OK] Pipe added: J3 <-> J2 (friction: 1.0 kJ)

Enter choice: 17
  Source Junction ID: J1
  Destination Junction ID: J2
  Path: J1 -> J3 -> J2
  Total friction energy: 2.50 kJ

Electrical Allocation (Options 18 → 19)

Enter choice: 18
  Motor ID: MTR-01
  Building name: TowerA
  Immediate demand (kW): 150.0
  [OK] Motor MTR-01 (TowerA) added, demand: 150 kW

Enter choice: 18
  Motor ID: MTR-02
  Building name: PlazaB
  Immediate demand (kW): 220.0
  [OK] Motor MTR-02 (PlazaB) added, demand: 220 kW

Enter choice: 19
  Available power (kW): 300.0
  [ALLOCATED] 220 kW -> Motor MTR-02 | Building: PlazaB | Demand: 220 kW


2.11 Results and Observations


DataRegistry correctly evicts the oldest reading once the 10-entry window is full, keeping memory bounded regardless of runtime duration.
PressureOverride demonstrates true LIFO semantics; multiple consecutive rollbacks correctly reverse actions in reverse-application order.
RestartQueue enforces strict FIFO, ensuring buildings that experienced outages earliest are restored first — matching operational priority.
SerialCodeLookup delivers instant retrieval even with a large catalog, thanks to hash-table O(1) average lookup.
JunctionSorter produces a correctly descending ranked list, enabling operators to immediately direct attention to the highest heat-draw zones.
TopologyAndRouting (Dijkstra) consistently identifies the globally optimal low-friction path, even when a direct link exists but carries higher friction than a two-hop alternative (as shown in Sample I/O above).
ElectricalAllocator always serves the motor with the greatest instantaneous demand first, preventing high-load motors from being starved while low-demand motors consume available power.



2.12 Conclusion

ChillGrid demonstrates how classical data structures — hash maps, stacks, queues, deques, vectors, adjacency lists, and heaps — can be combined in a single cohesive C++ application to solve a realistic engineering domain problem. Each subsystem is independently testable and encapsulated within its own class, making the codebase easy to extend (e.g., adding persistent storage, a GUI, or network telemetry) without disturbing existing logic. The project confirms that thoughtful data-structure selection directly translates to both algorithmic efficiency and code clarity in real-world simulation systems.


ChillGrid — ITM Skills University | Problem 89