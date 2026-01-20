#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>

class Graph {
private:
    std::vector<std::string> nodes;              // Node names
    std::vector<std::vector<int>> adjList;       // Adjacency list
public:
    void addNode(const std::string& name);
    void addEdge(int u, int v);                  // Connect node indices
    void printGraph() const;
};

#endif