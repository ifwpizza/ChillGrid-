# ❄️ ChillGrid — Downtown Cooling Energy Simulation System

> 🏫 **ITM Skills University** &nbsp;|&nbsp; 📌 Problem 89  
> An interactive, menu-driven **C++ simulation system** for managing downtown district cooling infrastructure.

---

## 📋 Table of Contents

1. [Problem Statement](#-problem-statement)
2. [Objectives](#-objectives)
3. [System Overview / Architecture](#-system-overview--architecture)
4. [Data Structures and Algorithms Used](#-data-structures-and-algorithms-used)
5. [Implementation Approach](#-implementation-approach)
6. [Time and Space Complexity Analysis](#-time-and-space-complexity-analysis)
7. [Execution Steps](#-execution-steps)
8. [Sample Inputs and Outputs](#-sample-inputs-and-outputs)
9. [Results and Observations](#-results-and-observations)
10. [Conclusion](#-conclusion)

---

## 2.1 📛 Project Title

> **ChillGrid — Downtown Cooling Energy Simulation System**

---

## 2.2 ❗ Problem Statement

Modern downtown districts rely on **centralized chilled-water networks** to cool multiple buildings simultaneously. Managing such a network requires:

- 📊 Tracking coolant **velocity** across pipe loops
- 🔧 Enforcing and **rolling back** pressure overrides on compressors
- 🏢 Queuing buildings for **cooling restart** after outages
- 🔍 Cataloguing physical **pipe parts** by serial number
- 🌡️ Prioritizing **high-heat junctions**
- 🗺️ Finding **energy-efficient routing paths** through the pipe network
- ⚡ Allocating **electrical power** to cooling motors on demand

Without a unified system, operators must manage these tasks manually — leading to errors, inefficiencies, and delayed response during faults.

> **ChillGrid** addresses this by consolidating all these operations into a single interactive C++ application backed by carefully chosen data structures.

---

## 2.3 🎯 Objectives

| # | Objective |
|---|---|
| 1 | 📈 Record and monitor **coolant velocity** readings per pipe loop with a rolling history window |
| 2 | ↩️ Apply, log, and **rollback** compressor pressure override actions |
| 3 | 🔄 Maintain a **FIFO restart queue** for buildings awaiting cooling restoration |
| 4 | ⚡ Provide **O(1) average-case lookup** of pipe parts by serial number |
| 5 | 📉 Sort cooling junctions by heat draw using **in-place Merge Sort** |
| 6 | 🗺️ Model the pipe network as a weighted graph and compute **minimum-friction paths** (Dijkstra's algorithm) |
| 7 | 🔋 Allocate available electrical power to cooling motors in **highest-demand-first** order (max-heap) |

---

## 2.4 🏗️ System Overview / Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                         main()                               │
│               Interactive Menu (choices 0–21)                │
└──────────────────┬───────────────────────────────────────────┘
                   │ delegates to
      ┌────────────▼────────────────────────────────────┐
      │           Seven Subsystem Classes               │
      ├─────────────────────────┬───────────────────────┤
      │  📦 DataRegistry        │  deque<double>         │
      │  🔧 PressureOverride    │  stack<PressureAction> │
      │  🏢 RestartQueue        │  queue<string>         │
      │  🔍 SerialCodeLookup    │  unordered_map<>       │
      │  🌡️  JunctionSorter      │  vector + MergeSort    │
      │  🗺️  TopologyAndRouting  │  adj-list + Dijkstra   │
      │  ⚡ ElectricalAllocator │  priority_queue (heap) │
      └─────────────────────────┴───────────────────────┘
```

> Each class is **self-contained** with its own data store and public interface. `main()` instantiates one object per class and routes user input to the appropriate method.

---

## 2.5 🧠 Data Structures and Algorithms Used

| # | 📦 Subsystem | 🗄️ Data Structure | ⚙️ Algorithm / Operation |
|---|---|---|---|
| 1 | DataRegistry | `unordered_map` + `deque` | Sliding-window velocity history (max 10 readings); O(1) push/pop |
| 2 | PressureOverride | `stack` | LIFO action log; rollback in O(1) |
| 3 | RestartQueue | `queue` | FIFO building restart; enqueue/dequeue O(1) |
| 4 | SerialCodeLookup | `unordered_map` | Hash-based part lookup; O(1) average |
| 5 | JunctionSorter | `vector` | Merge Sort (descending heat draw); O(n log n) |
| 6 | TopologyAndRouting | Adjacency list (`unordered_map<string, vector<Edge>>`) | Dijkstra's shortest path; O((V+E) log V) |
| 7 | ElectricalAllocator | `priority_queue` (max-heap) | Greedy highest-demand-first allocation; O(log n) push/pop |

---

## 2.6 🔧 Implementation Approach

### 📦 DataRegistry
A `deque<double>` is maintained **per loop ID** inside an `unordered_map`. When the deque reaches 10 entries, the oldest is evicted (`pop_front`) before appending the new reading. This gives a constant-size sliding window with **O(1) amortised** operations.

---

### 🔧 PressureOverride
Every `applyAction()` call pushes a `PressureAction` struct onto a **stack**. `rollback()` pops the top element, naturally reversing the most recent action. The full log is inspected by copying the stack into a temporary.

---

### 🏢 RestartQueue
A plain `std::queue<string>` provides **strict FIFO** ordering. Buildings are enqueued by name and processed (dequeued) one at a time, mirroring real-world restart sequencing.

---

### 🔍 SerialCodeLookup
`PipePart` structs are stored in an `unordered_map` keyed by serial number, yielding expected **O(1) insert and lookup** without any sorting requirement.

---

### 🌡️ JunctionSorter
Junctions are stored in a `vector<Junction>` and sorted in-place using a **custom Merge Sort** (descending `heatDraw`). Merge Sort was chosen for its guaranteed **O(n log n) worst-case** and stable ordering.

---

### 🗺️ TopologyAndRouting
The pipe network is an **undirected weighted graph** stored as an adjacency list. Pipes are added bidirectionally. **Dijkstra's algorithm** with a min-heap (`priority_queue` with `greater<>`) finds the minimum-friction-energy path between any two junctions.

---

### ⚡ ElectricalAllocator
`CoolingMotor` objects implement `operator<` (by demand) and are stored in a `std::priority_queue` (**max-heap**). `allocate()` always serves the **highest-demand motor first**, capping allocation at available power.

---

## 2.7 ⏱️ Time and Space Complexity Analysis

| 📦 Subsystem | ⚙️ Operation | 🕐 Time Complexity | 💾 Space Complexity |
|---|---|---|---|
| DataRegistry | Record velocity | O(1) amortised | O(L × W) — L loops, W=10 window |
| DataRegistry | Show / List | O(W) / O(L) | O(1) extra |
| PressureOverride | Apply / Rollback | O(1) | O(A) — A actions |
| RestartQueue | Enqueue / Dequeue | O(1) | O(B) — B buildings |
| SerialCodeLookup | Add / Find | O(1) avg | O(P) — P parts |
| JunctionSorter | Sort | O(n log n) | O(n) merge buffer |
| TopologyAndRouting | Add pipe | O(1) | O(V + E) |
| TopologyAndRouting | Dijkstra | O((V+E) log V) | O(V) dist + prev maps |
| ElectricalAllocator | Add motor | O(log n) | O(M) — M motors |
| ElectricalAllocator | Allocate | O(log n) | O(1) |

---

## 2.8 🚀 Execution Steps

### 🛠️ Prerequisites

> ✅ A **C++17-compatible compiler**: `g++ 9+`, `clang++ 10+`, or `MSVC 2019+`  
> ✅ **No external libraries** — standard library only

---

### ⚙️ Compile

```bash
g++ -std=c++17 -O2 -o chillgrid chillgrid.cpp
```

### ▶️ Run

```bash
./chillgrid        # Linux / macOS
chillgrid.exe      # Windows
```

### 🧭 Navigate the Menu

The program presents a **numbered menu** on every iteration:
1. Enter the number for the desired operation and press **Enter**
2. Follow the on-screen prompts to supply required values
3. Enter **`0`** to exit

---

## 2.9 📊 Sample Inputs and Outputs

### 🌊 Record & View Loop Velocity `(Options 1 → 2)`

```
Enter choice: 1
  Enter Loop ID: LOOP-A
  Enter velocity (m/s): 3.5
  [OK] Velocity 3.5 m/s recorded for loop LOOP-A

Enter choice: 2
  Enter Loop ID: LOOP-A
  Loop LOOP-A -> Live: 3.5 m/s | History: 3.5 | Avg: 3.5 m/s
```

---

### 🔧 Apply & Rollback Pressure `(Options 4 → 6 → 5)`

```
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
```

---

### 🗺️ Find Minimum Friction Path `(Options 15 → 17)`

```
Enter choice: 15
  From Junction ID: J1  |  To Junction ID: J2  |  Friction: 4.0 kJ
  [OK] Pipe added: J1 <-> J2 (friction: 4.0 kJ)

Enter choice: 15
  From Junction ID: J1  |  To Junction ID: J3  |  Friction: 1.5 kJ
  [OK] Pipe added: J1 <-> J3 (friction: 1.5 kJ)

Enter choice: 15
  From Junction ID: J3  |  To Junction ID: J2  |  Friction: 1.0 kJ
  [OK] Pipe added: J3 <-> J2 (friction: 1.0 kJ)

Enter choice: 17
  Source: J1  |  Destination: J2
  Path: J1 -> J3 -> J2
  Total friction energy: 2.50 kJ   ✅ (vs direct J1->J2 at 4.0 kJ)
```

---

### ⚡ Electrical Allocation `(Options 18 → 19)`

```
Enter choice: 18
  Motor ID: MTR-01 | Building: TowerA | Demand: 150.0 kW
  [OK] Motor MTR-01 (TowerA) added, demand: 150 kW

Enter choice: 18
  Motor ID: MTR-02 | Building: PlazaB | Demand: 220.0 kW
  [OK] Motor MTR-02 (PlazaB) added, demand: 220 kW

Enter choice: 19
  Available power (kW): 300.0
  [ALLOCATED] 220 kW -> Motor MTR-02 | Building: PlazaB | Demand: 220 kW
```

---

## 2.11 📈 Results and Observations

| 📦 Subsystem | ✅ Observation |
|---|---|
| 📦 DataRegistry | Correctly evicts the oldest reading once the 10-entry window is full, keeping memory **bounded** regardless of runtime duration |
| 🔧 PressureOverride | Demonstrates true **LIFO** semantics; multiple consecutive rollbacks correctly reverse actions in reverse-application order |
| 🏢 RestartQueue | Enforces strict **FIFO** — buildings that experienced outages earliest are restored first, matching operational priority |
| 🔍 SerialCodeLookup | Delivers **instant retrieval** even with a large catalog, thanks to hash-table O(1) average lookup |
| 🌡️ JunctionSorter | Produces a correctly **descending ranked list**, enabling operators to immediately direct attention to the highest heat-draw zones |
| 🗺️ TopologyAndRouting | **Dijkstra** consistently identifies the globally optimal low-friction path, even when a direct link exists but carries higher friction than a two-hop alternative |
| ⚡ ElectricalAllocator | Always serves the motor with the **greatest instantaneous demand first**, preventing high-load motors from being starved by low-demand ones |

---

## 2.12 ✅ Conclusion

**ChillGrid** demonstrates how classical data structures — hash maps, stacks, queues, deques, vectors, adjacency lists, and heaps — can be **combined in a single cohesive C++ application** to solve a realistic engineering domain problem.

### 🔑 Key Takeaways

- 🧩 Each subsystem is **independently testable** and encapsulated within its own class
- 🔌 The codebase is easy to **extend** (e.g., adding persistent storage, a GUI, or network telemetry) without disturbing existing logic
- ⚡ Thoughtful **data-structure selection** directly translates to both algorithmic efficiency and code clarity in real-world simulation systems

---

*❄️ ChillGrid — ITM Skills University | Problem 89*

ChillGrid — ITM Skills University | Problem 89
