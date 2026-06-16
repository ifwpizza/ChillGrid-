/*
 * ChillGrid - Downtown Cooling Energy Simulation System
 * ITM Skills University - Problem 89
 * INTERACTIVE MENU-DRIVEN VERSION
 */

#include <iostream>
#include <unordered_map>
#include <vector>
#include <deque>
#include <stack>
#include <queue>
#include <string>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <stdexcept>

using namespace std;

// ============================================================
//  SHARED DATA TYPES
// ============================================================
struct PipePart {
    string serialNumber, partName, location;
    double pressureRating;
    bool   inService;
};

struct Junction {
    string id, zone;
    double heatDraw;
};

struct Edge {
    string to;
    double frictionEnergy;
};

struct PressureAction {
    string compressorId, actionType, timestamp;
    double pressureBar;
};

struct CoolingMotor {
    string motorId, building;
    double demand;
    bool operator<(const CoolingMotor& o) const { return demand < o.demand; }
};

// ============================================================
//  1. DATA REGISTRY
// ============================================================
class DataRegistry {
    static const int MAX_READINGS = 10;
    unordered_map<string, deque<double>> registry;
public:
    void recordVelocity(const string& loopId, double v) {
        auto& dq = registry[loopId];
        if ((int)dq.size() >= MAX_READINGS) dq.pop_front();
        dq.push_back(v);
        cout << "  [OK] Velocity " << v << " m/s recorded for loop " << loopId << "\n";
    }
    void showLoop(const string& loopId) {
        auto it = registry.find(loopId);
        if (it == registry.end() || it->second.empty()) { cout << "  Loop not found.\n"; return; }
        cout << "  Loop " << loopId << " -> Live: " << it->second.back() << " m/s | History: ";
        for (double x : it->second) cout << x << " ";
        double sum = 0; for (double x : it->second) sum += x;
        cout << "| Avg: " << sum/it->second.size() << " m/s\n";
    }
    void listLoops() {
        if (registry.empty()) { cout << "  No loops registered.\n"; return; }
        for (auto& [id, dq] : registry)
            cout << "  " << id << " -> " << (dq.empty() ? 0 : dq.back()) << " m/s\n";
    }
};

// ============================================================
//  2. PRESSURE OVERRIDE
// ============================================================
class PressureOverride {
    stack<PressureAction> log;
public:
    void applyAction(const PressureAction& a) {
        log.push(a);
        cout << "  [OK] " << a.actionType << " applied on " << a.compressorId << " -> " << a.pressureBar << " bar\n";
    }
    void rollback() {
        if (log.empty()) { cout << "  Nothing to rollback.\n"; return; }
        auto a = log.top(); log.pop();
        cout << "  [ROLLED BACK] " << a.actionType << " on " << a.compressorId << " (" << a.pressureBar << " bar)\n";
    }
    void showLog() {
        if (log.empty()) { cout << "  Log is empty.\n"; return; }
        stack<PressureAction> tmp = log;
        int i = 1;
        while (!tmp.empty()) {
            auto& a = tmp.top();
            cout << "  " << i++ << ". " << a.compressorId << " | " << a.actionType
                 << " | " << a.pressureBar << " bar | " << a.timestamp << "\n";
            tmp.pop();
        }
    }
};

// ============================================================
//  3. RESTART QUEUE
// ============================================================
class RestartQueue {
    queue<string> q;
public:
    void enqueue(const string& b) { q.push(b); cout << "  [OK] " << b << " added to restart queue.\n"; }
    void processNext() {
        if (q.empty()) { cout << "  Queue is empty.\n"; return; }
        cout << "  [PROCESSED] Cooling restarted for: " << q.front() << "\n";
        q.pop();
    }
    void showQueue() {
        if (q.empty()) { cout << "  Queue is empty.\n"; return; }
        queue<string> tmp = q;
        cout << "  Pending (" << q.size() << "): ";
        while (!tmp.empty()) { cout << tmp.front() << " "; tmp.pop(); }
        cout << "\n";
    }
};

// ============================================================
//  4. SERIAL CODE LOOKUP
// ============================================================
class SerialCodeLookup {
    unordered_map<string, PipePart> catalog;
public:
    void addPart(const PipePart& p) {
        catalog[p.serialNumber] = p;
        cout << "  [OK] Part " << p.serialNumber << " (" << p.partName << ") registered.\n";
    }
    void findPart(const string& serial) {
        auto it = catalog.find(serial);
        if (it == catalog.end()) { cout << "  Part not found: " << serial << "\n"; return; }
        auto& p = it->second;
        cout << "  Serial  : " << p.serialNumber << "\n"
             << "  Name    : " << p.partName     << "\n"
             << "  Location: " << p.location     << "\n"
             << "  Pressure: " << p.pressureRating << " bar\n"
             << "  Status  : " << (p.inService ? "In Service" : "Out of Service") << "\n";
    }
    void listParts() {
        if (catalog.empty()) { cout << "  No parts registered.\n"; return; }
        for (auto& [sn, p] : catalog)
            cout << "  " << sn << " | " << p.partName << " | " << p.location << "\n";
    }
};

// ============================================================
//  5. JUNCTION SORTER
// ============================================================
class JunctionSorter {
    vector<Junction> junctions;
    void mergeSort(vector<Junction>& arr, int l, int r) {
        if (l >= r) return;
        int m = l + (r-l)/2;
        mergeSort(arr, l, m); mergeSort(arr, m+1, r);
        vector<Junction> tmp;
        int i = l, j = m+1;
        while (i<=m && j<=r) { if (arr[i].heatDraw >= arr[j].heatDraw) tmp.push_back(arr[i++]); else tmp.push_back(arr[j++]); }
        while (i<=m) tmp.push_back(arr[i++]);
        while (j<=r) tmp.push_back(arr[j++]);
        for (int k=l; k<=r; k++) arr[k] = tmp[k-l];
    }
public:
    void addJunction(const Junction& j) {
        junctions.push_back(j);
        cout << "  [OK] Junction " << j.id << " added (heat draw: " << j.heatDraw << " kW)\n";
    }
    void sortAndShow() {
        if (junctions.empty()) { cout << "  No junctions added.\n"; return; }
        mergeSort(junctions, 0, (int)junctions.size()-1);
        cout << "  Junctions ranked by heat draw:\n";
        for (int i=0; i<(int)junctions.size(); i++)
            cout << "  " << (i+1) << ". " << junctions[i].id
                 << " | Zone: " << junctions[i].zone
                 << " | Heat: " << junctions[i].heatDraw << " kW\n";
    }
};

// ============================================================
//  6 & 7. TOPOLOGY LOOP + ROUTING PATH
// ============================================================
class TopologyAndRouting {
    unordered_map<string, vector<Edge>> graph;
public:
    void addPipe(const string& a, const string& b, double friction) {
        graph[a].push_back({b, friction});
        graph[b].push_back({a, friction});
        cout << "  [OK] Pipe added: " << a << " <-> " << b << " (friction: " << friction << " kJ)\n";
    }
    void showNetwork() {
        if (graph.empty()) { cout << "  No pipes added.\n"; return; }
        for (auto& [node, edges] : graph) {
            cout << "  " << node << " -> ";
            for (auto& e : edges) cout << e.to << "(" << e.frictionEnergy << "kJ) ";
            cout << "\n";
        }
    }
    void findPath(const string& src, const string& dst) {
        if (!graph.count(src) || !graph.count(dst)) { cout << "  Node not found in network.\n"; return; }
        unordered_map<string,double> dist;
        unordered_map<string,string> prev;
        for (auto& [n,_] : graph) dist[n] = numeric_limits<double>::infinity();
        dist[src] = 0;
        priority_queue<pair<double,string>, vector<pair<double,string>>, greater<>> pq;
        pq.push({0, src});
        while (!pq.empty()) {
            auto [cost, u] = pq.top(); pq.pop();
            if (cost > dist[u]) continue;
            if (u == dst) break;
            for (auto& e : graph[u]) {
                double nc = dist[u] + e.frictionEnergy;
                if (nc < dist[e.to]) { dist[e.to] = nc; prev[e.to] = u; pq.push({nc, e.to}); }
            }
        }
        if (dist[dst] == numeric_limits<double>::infinity()) { cout << "  No path found.\n"; return; }
        vector<string> path;
        for (string at = dst; at != src; at = prev[at]) { path.push_back(at); if (!prev.count(at)) break; }
        path.push_back(src);
        reverse(path.begin(), path.end());
        cout << "  Path: ";
        for (int i=0; i<(int)path.size(); i++) { cout << path[i]; if (i+1<(int)path.size()) cout << " -> "; }
        cout << "\n  Total friction energy: " << fixed << setprecision(2) << dist[dst] << " kJ\n";
    }
};

// ============================================================
//  8. ELECTRICAL ALLOCATOR
// ============================================================
class ElectricalAllocator {
    priority_queue<CoolingMotor> pq;
public:
    void addMotor(const CoolingMotor& m) {
        pq.push(m);
        cout << "  [OK] Motor " << m.motorId << " (" << m.building << ") added, demand: " << m.demand << " kW\n";
    }
    void allocate(double availPower) {
        if (pq.empty()) { cout << "  No motors in queue.\n"; return; }
        auto m = pq.top(); pq.pop();
        double given = min(m.demand, availPower);
        cout << "  [ALLOCATED] " << given << " kW -> Motor " << m.motorId
             << " | Building: " << m.building << " | Demand: " << m.demand << " kW\n";
    }
    void allocateAll(double availPower) {
        if (pq.empty()) { cout << "  No motors in queue.\n"; return; }
        while (!pq.empty()) allocate(availPower);
    }
    void showQueue() {
        if (pq.empty()) { cout << "  No motors in queue.\n"; return; }
        priority_queue<CoolingMotor> tmp = pq;
        cout << "  Motors (highest demand first):\n";
        while (!tmp.empty()) {
            auto& m = tmp.top();
            cout << "  " << m.motorId << " | " << m.building << " | " << m.demand << " kW\n";
            tmp.pop();
        }
    }
};

// ============================================================
//  HELPER
// ============================================================
void clearInput() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

string getLine(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

double getDouble(const string& prompt) {
    double v;
    cout << prompt;
    cin >> v;
    clearInput();
    return v;
}

int getInt(const string& prompt) {
    int v;
    cout << prompt;
    cin >> v;
    clearInput();
    return v;
}

// ============================================================
//  MAIN MENU
// ============================================================
int main() {
    DataRegistry      registry;
    PressureOverride  pressure;
    RestartQueue      restartQ;
    SerialCodeLookup  catalog;
    JunctionSorter    sorter;
    TopologyAndRouting topo;
    ElectricalAllocator allocator;

    int choice;
    while (true) {
        cout << "\n╔══════════════════════════════════════╗\n";
        cout << "║     ChillGrid - Cooling System        ║\n";
        cout << "╠══════════════════════════════════════╣\n";
        cout << "║  1. Record pipe velocity              ║\n";
        cout << "║  2. View loop velocity                ║\n";
        cout << "║  3. List all loops                    ║\n";
        cout << "║  4. Apply pressure action             ║\n";
        cout << "║  5. Rollback pressure action          ║\n";
        cout << "║  6. View pressure log                 ║\n";
        cout << "║  7. Add building to restart queue     ║\n";
        cout << "║  8. Process next restart              ║\n";
        cout << "║  9. View restart queue                ║\n";
        cout << "║ 10. Add pipe part (serial)            ║\n";
        cout << "║ 11. Lookup pipe part by serial        ║\n";
        cout << "║ 12. List all pipe parts               ║\n";
        cout << "║ 13. Add junction                      ║\n";
        cout << "║ 14. Sort & view junctions by heat     ║\n";
        cout << "║ 15. Add pipe to network               ║\n";
        cout << "║ 16. View pipe network                 ║\n";
        cout << "║ 17. Find min friction path            ║\n";
        cout << "║ 18. Add cooling motor                 ║\n";
        cout << "║ 19. Allocate power to next motor      ║\n";
        cout << "║ 20. Allocate power to all motors      ║\n";
        cout << "║ 21. View motor queue                  ║\n";
        cout << "║  0. Exit                              ║\n";
        cout << "╚══════════════════════════════════════╝\n";
        choice = getInt("Enter choice: ");

        cout << "\n";

        if (choice == 0) {
            cout << "  Goodbye! ChillGrid shutting down.\n";
            break;
        }

        switch (choice) {

        // ---- DATA REGISTRY ----
        case 1: {
            string id = getLine("  Enter Loop ID: ");
            double v  = getDouble("  Enter velocity (m/s): ");
            registry.recordVelocity(id, v);
            break;
        }
        case 2: {
            string id = getLine("  Enter Loop ID: ");
            registry.showLoop(id);
            break;
        }
        case 3:
            registry.listLoops();
            break;

        // ---- PRESSURE OVERRIDE ----
        case 4: {
            PressureAction a;
            a.compressorId = getLine("  Compressor ID: ");
            cout << "  Action type (INCREASE / DECREASE / OVERRIDE): ";
            a.actionType   = getLine("");
            a.pressureBar  = getDouble("  Pressure (bar): ");
            a.timestamp    = getLine("  Timestamp (e.g. 2024-06-01T08:00): ");
            pressure.applyAction(a);
            break;
        }
        case 5:
            pressure.rollback();
            break;
        case 6:
            pressure.showLog();
            break;

        // ---- RESTART QUEUE ----
        case 7: {
            string b = getLine("  Building name: ");
            restartQ.enqueue(b);
            break;
        }
        case 8:
            restartQ.processNext();
            break;
        case 9:
            restartQ.showQueue();
            break;

        // ---- SERIAL CODE ----
        case 10: {
            PipePart p;
            p.serialNumber   = getLine("  Serial Number: ");
            p.partName       = getLine("  Part Name: ");
            p.location       = getLine("  Location: ");
            p.pressureRating = getDouble("  Pressure Rating (bar): ");
            int s = getInt("  In Service? (1=Yes 0=No): ");
            p.inService = (s == 1);
            catalog.addPart(p);
            break;
        }
        case 11: {
            string sn = getLine("  Enter Serial Number: ");
            catalog.findPart(sn);
            break;
        }
        case 12:
            catalog.listParts();
            break;

        // ---- JUNCTION SORTER ----
        case 13: {
            Junction j;
            j.id       = getLine("  Junction ID: ");
            j.zone     = getLine("  Zone name: ");
            j.heatDraw = getDouble("  Heat Draw (kW): ");
            sorter.addJunction(j);
            break;
        }
        case 14:
            sorter.sortAndShow();
            break;

        // ---- TOPOLOGY + ROUTING ----
        case 15: {
            string a = getLine("  From Junction ID: ");
            string b = getLine("  To Junction ID: ");
            double f = getDouble("  Friction energy (kJ): ");
            topo.addPipe(a, b, f);
            break;
        }
        case 16:
            topo.showNetwork();
            break;
        case 17: {
            string src = getLine("  Source Junction ID: ");
            string dst = getLine("  Destination Junction ID: ");
            topo.findPath(src, dst);
            break;
        }

        // ---- ELECTRICAL ALLOCATOR ----
        case 18: {
            CoolingMotor m;
            m.motorId  = getLine("  Motor ID: ");
            m.building = getLine("  Building name: ");
            m.demand   = getDouble("  Immediate demand (kW): ");
            allocator.addMotor(m);
            break;
        }
        case 19: {
            double p = getDouble("  Available power (kW): ");
            allocator.allocate(p);
            break;
        }
        case 20: {
            double p = getDouble("  Available power per motor (kW): ");
            allocator.allocateAll(p);
            break;
        }
        case 21:
            allocator.showQueue();
            break;

        default:
            cout << "  Invalid choice. Try again.\n";
        }
    }
    return 0;
}