#include <iostream>
#include <vector>
#include <queue>
#include <cmath>    // std::abs
#include <algorithm>

struct Node {
    int id;
    int row, col;
    int parentId = -1;

    int g = 0;   // cost so far
    int h = 0;   // heuristic to goal
    int f = 0;   // g + h

    std::vector<int> neighbors;
};

// Comparator: make a min-heap by f
struct CompareF {
    bool operator()(const Node& a, const Node& b) const {
        return a.f > b.f;
    }
};

// Manhattan distance
int heuristic(const Node& a, const Node& b) {
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}

int main() {
    // --- Minimal demo graph (3x3 grid nodes 0..8) ---
    std::vector<Node> nodes(9);
    int idx = 0;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            nodes[idx] = Node{ idx, r, c, -1, 0, 0, 0, {} };
            ++idx;
        }
    }
    auto id_of = [](int r, int c) { return r * 3 + c; };
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            int me = id_of(r, c);
            if (r > 0) nodes[me].neighbors.push_back(id_of(r - 1, c));
            if (r < 2) nodes[me].neighbors.push_back(id_of(r + 1, c));
            if (c > 0) nodes[me].neighbors.push_back(id_of(r, c - 1));
            if (c < 2) nodes[me].neighbors.push_back(id_of(r, c + 1));
        }
    }
    // -----------------------------------------------

    int startId = 0;  // top-left (0,0)
    int goalId  = 8;  // bottom-right (2,2)

    std::priority_queue<Node, std::vector<Node>, CompareF> openSet;
    std::vector<bool> closed(nodes.size(), false);

    // Init start
    nodes[startId].g = 0;
    nodes[startId].h = heuristic(nodes[startId], nodes[goalId]);
    nodes[startId].f = nodes[startId].g + nodes[startId].h;
    openSet.push(nodes[startId]);

    // TODO: A* main loop
    while (!openSet.empty()) {
        // Pop the node with lowest f
        Node current = openSet.top();
        openSet.pop();

        // Check if goal reached
        if (current.id == goalId) {
            std::cout << "Goal reached!" << std::endl;
            break;
        }

        // Skip if already closed
        if (closed[current.id]) {
            continue;
        }

        // Mark current as closed
        closed[current.id] = true;

        // Explore neighbors
        for (int neighborId : current.neighbors) {
            // Skip if neighbor is closed
            if (closed[neighborId]) {
                continue;
            }

            // Compute tentative g cost
            int tentativeG = current.g + 1;

            // If this is an improvement (or neighbor not yet initialized)
            if (nodes[neighborId].parentId == -1 || tentativeG < nodes[neighborId].g) {
                // Update neighbor
                nodes[neighborId].g = tentativeG;
                nodes[neighborId].h = heuristic(nodes[neighborId], nodes[goalId]);
                nodes[neighborId].f = nodes[neighborId].g + nodes[neighborId].h;
                nodes[neighborId].parentId = current.id;

                // Push updated neighbor into openSet
                openSet.push(nodes[neighborId]);
            }
        }
    }

    return 0;
}